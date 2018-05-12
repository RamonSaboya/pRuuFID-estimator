#include <bits/stdc++.h>
#include "estimators.h"
#include "gnuplot.h"

using namespace std;

const int STARTING_TAGS = 100;
const int INCREASE_VALUE = 100;
const int MAX_TAGS = 1000;
const int SIMULATIONS = 2000;
const int INITIAL_FRAMES = 64;

int starting_tags;
int increase_values;
int max_tags;
int simulations;
int initial_frames;

set<Estimator*> whitelisted;

EstimationParameters parameters;

bool handle_parameters(int argc, char* argv[]);

string plot_string(int idx);

bool validate_parameters();

set<Estimator*> get_estimators();

int main(int argc, char* argv[]) {
	whitelisted = get_estimators();

	if(!handle_parameters(argc, argv)) {
		return -1;
	}

	if(!validate_parameters()) {
		return -1;
	}

	srand(time(NULL));

	Gnuplot g;
	g.cmd("set terminal pngcairo size 1700,900");
	g.cmd("set output 'graph.png'");
	g.cmd("set multiplot");
	g.cmd("set size 0.3,0.45");
	g.cmd("set style line 10 lc rgb 'black' lt 1 lw 1");
	g.cmd("set grid xtics ytics mxtics mytics ls 10 dt 3");
	g.cmd("set mytics 1");
	g.cmd("set mxtics 1");
	g.cmd("set xr [100:1000]");
	g.cmd("set ytics 20");
	g.cmd("set yr [0:140]");
	g.cmd("set xlabel 'Número de Etiquetas'");
	g.cmd("set ylabel 'Erro Abs. Médio de Estimação'");
	g.cmd("set origin 0.0,0.55");
	g.cmd(plot_string(2));
	g.cmd("set ytics 500");
	g.cmd("set yr [0:3500]");
	g.cmd("set ylabel 'Número de Slots'");
	g.cmd("set origin 0.35,0.55");
	g.cmd(plot_string(3));
	g.cmd("set ytics 1");
	g.cmd("set yr [0:10]");
	g.cmd("set ylabel 'Tempo para Identificação (s)'");
	g.cmd("set origin 0.7,0.55");
	g.cmd(plot_string(4));
	g.cmd("set ytics 100");
	g.cmd("set yr [0:1100]");
	g.cmd("set ylabel 'Número de Slots Vazios'");
	g.cmd("set origin 0.15,0.025");
	g.cmd(plot_string(5));
	g.cmd("set ytics 200");
	g.cmd("set yr [0:1800]");
	g.cmd("set ylabel 'Número de Slots em colisão'");
	g.cmd("set origin 0.55,0.025");
	g.cmd(plot_string(6));
	g.cmd("unset multiplot");

	system("eog --disable-gallery --single-window graph.png &");
}

bool handle_parameters(int argc, char* argv[]) {
	starting_tags = STARTING_TAGS;
	increase_values = INCREASE_VALUE;
	max_tags = MAX_TAGS;
	simulations = SIMULATIONS;
	initial_frames = INITIAL_FRAMES;

	string opt, value;
	try {
		for(int i = 1; i < argc; ++i) {
			opt = string(argv[i]);
		
			if(i < argc - 1) {
				if(opt == "-st" || opt == "--starting-tags") {
					value = string(argv[++i]);
					starting_tags = stoi(value);
				} else if(opt == "-iv" || opt == "--increase-value") {
					value = string(argv[++i]);
					increase_values = stoi(value);
				} else if(opt == "-mt" || opt == "--max-tags") {
					value = string(argv[++i]);
					max_tags = stoi(value);
				} else if(opt == "-s" || opt == "--simulations") {
					value = string(argv[++i]);
					simulations = stoi(value);
				} else if(opt == "-if" || opt == "--initial-frames") {
					value = string(argv[++i]);
					initial_frames = stoi(value);
				} else if(opt == "-w" || opt == "--whitelist") {
					whitelisted.clear();

					while(++i < argc) {
						value = string(argv[i]);
					
						if(value == "lower-bound") {
							whitelisted.insert(new LowerBound());
						} else if(value == "eom-lee") {
							whitelisted.insert(new EomLee());
						} else {
							cerr << "Unknown estimator: " << value << "." << endl;
							return false;
						}
					}
				} else {
					cerr << "Invalid parameter: " << opt << "." << endl;
					return false;
				}
			} else {
				cerr << "Invalid parameter: " << opt << ". No value informed." << endl;
				return false;
			}
		}
	} catch(...) {
		cerr << "Invalid value: " << value << ". Impossible to parse." << endl;
		return false;
	}
	
	return true;
}


string plot_string(int idx) {
	string plot = "plot ";
	
	for(Estimator* estimator : whitelisted) {
		plot += "'" + estimator->get_file_name() + "' u 1:" + to_string(idx) + " " + estimator->get_plot_options() + ", ";
	}
	
	return plot.substr(0, plot.size() - 1);
}

bool validate_parameters() {
	if((max_tags - starting_tags) % increase_values != 0) {
		cerr << "Invalid tags amount" << endl;
		return false;
	}

	EstimationParameters local_parameters(starting_tags, increase_values, max_tags, simulations, initial_frames);

	parameters = local_parameters;

	return true;
}

set<Estimator*> get_estimators() {
	set<Estimator*> estimators;

	estimators.insert(new LowerBound());
	estimators.insert(new EomLee());
	
	return estimators;
}
