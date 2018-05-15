#include <bits/stdc++.h>
#include "estimators.h"
#include "math_util.h"
#include "gnuplot.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

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

Gnuplot g;

set<Estimator*> get_estimators();
bool handle_parameters(int argc, char* argv[]);
bool validate_parameters();
void plot_graph(int idx);

int main(int argc, char* argv[]) {
	whitelisted = get_estimators();

	if(!handle_parameters(argc, argv)) {
		return -1;
	}

	if(!validate_parameters()) {
		return -1;
	}
	
	sfast_rand(time(NULL));

	high_resolution_clock::time_point start_time, end_time;
	start_time = high_resolution_clock::now();
	for(Estimator* estimator : whitelisted) {
		estimator->simulate(parameters);
	}
	end_time = high_resolution_clock::now();
	
	cout << "Execution time in ms: " << duration_cast<milliseconds>(end_time - start_time).count() << endl;

	g.cmd("set terminal pngcairo size 1700,900");
	g.cmd("set output 'graph.png'");
	g.cmd("set multiplot");
	g.cmd("set key left top");
	g.cmd("set size 0.3,0.45");
	g.cmd("set style line 10 lc rgb 'black' lt 1 lw 1");
	g.cmd("set grid xtics ytics mxtics mytics ls 10 dt 3");
	g.cmd("set mytics 1");
	g.cmd("set mxtics 1");
	g.cmd("set xr [*:*]");
	g.cmd("set yr [0:*]");
	g.cmd("set xlabel 'Número de Etiquetas' font 'Helvetica,20'");
	g.cmd("set ylabel 'Erro Abs. Médio de Estimação' font 'Helvetica,20'");
	g.cmd("set origin 0.0,0.55");
	plot_graph(2);
	g.cmd("set ylabel 'Número de Slots'");
	g.cmd("set origin 0.35,0.55");
	plot_graph(3);
	g.cmd("set ylabel 'Tempo para Identificação (s)'");
	g.cmd("set origin 0.7,0.55");
	plot_graph(4);
	g.cmd("set ylabel 'Eficiência'");
	g.cmd("set origin 0.0,0.025");
	g.cmd("set key left bottom");
	plot_graph(5);
	g.cmd("set ylabel 'Número de Slots Vazios'");
	g.cmd("set origin 0.35,0.025");
	g.cmd("set key left top");
	plot_graph(6);
	g.cmd("set ylabel 'Número de Slots em Colisão'");
	g.cmd("set origin 0.7,0.025");
	plot_graph(7);
	g.cmd("unset multiplot");
	
	usleep(1000000);

	system("eog --disable-gallery --single-window graph.png &");
}

void plot_graph(int idx) {
	string plot = "plot ";
	
	for(Estimator* estimator : whitelisted) {
		plot += "'" + estimator->get_file_name() + "' u 1:" + to_string(idx) + " " + estimator->get_plot_options() + ", ";
	}
	
	g.cmd(plot.substr(0, plot.size() - 1));
}

set<Estimator*> get_estimators() {
	set<Estimator*> estimators;

	estimators.insert(new LowerBound());
	estimators.insert(new EomLee());
	estimators.insert(new Schoute());
	estimators.insert(new Q());
	
	return estimators;
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
						} else if(value == "schoute") {
							whitelisted.insert(new Schoute());
						} else if(value == "q") {
							whitelisted.insert(new Q());
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

bool validate_parameters() {
	if((max_tags - starting_tags) % increase_values != 0) {
		cerr << "Invalid tags amount" << endl;
		return false;
	}

	EstimationParameters local_parameters(starting_tags, increase_values, max_tags, simulations, initial_frames);

	parameters = local_parameters;

	return true;
}
