#include "estimators.h"
#include "math_util.h"
#include <stdlib.h>
#include <set>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <string.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int W = 15;

const double THRESHOLD = 0.001;

EstimationParameters::EstimationParameters() {}

EstimationParameters::EstimationParameters(int starting_tags, int increase_value, int max_tags, int simulations, int initial_frame) {
	this->starting_tags = starting_tags;
	this->increase_value = increase_value;
	this->max_tags = max_tags;
	this->simulations = simulations;
	this->initial_frame = initial_frame;
}

EstimationResult::EstimationResult(int tag_count, int simulations) {
	this->tag_count = tag_count;
	this->simulations = simulations;

	this->tag_amounts = new int[tag_count]();
	this->errors = new int[tag_count]();
	this->empty_slots = new int[tag_count]();
	this->success_slots = new int[tag_count]();
	this->collision_slots = new int[tag_count]();
	this->simulation_times = new int[tag_count]();
}

Estimator::Estimator(string name, string file_name, string plot_options) {
	this->name = name;
	this->file_name = file_name;
	this->plot_options = plot_options;
}

string Estimator::get_name() const {
	return name;
}

string Estimator::get_file_name() const {
	return file_name;
}

string Estimator::get_plot_options() const {
	return plot_options;
}

void Estimator::write_dat_file(EstimationResult result) const {
	ofstream datFile(this->get_file_name());

	datFile << "# " << setw(3 * W - 2) << this->get_name() << endl;
	datFile << "#" << endl;
	datFile << "#" << endl;
	datFile << "# " << setw(W - 2) << "tags" << setw(W) << "error" << setw(W) << "slots" << setw(W) << "time" << setw(W) << "empty" << setw(W) << "collision" << endl;
	datFile << "#" << endl;
	
	double simulations = result.simulations;
	
	for(int i = 0; i < result.tag_count; ++i) {
		int tags = result.tag_amounts[i];
		double errors = result.errors[i] / simulations;
		double empty = result.empty_slots[i] / simulations;
		double success = result.success_slots[i] / simulations;
		double collision = result.collision_slots[i] / simulations;
		double simulation_times = result.simulation_times[i] / 1e6;
		
		double total_slots = empty + success + collision;

		datFile << setw(W) << fixed << tags;
		datFile << setw(W) << fixed << errors;
		datFile << setw(W) << fixed << total_slots;
		datFile << setw(W) << fixed << simulation_times;
		datFile << setw(W) << fixed << empty;
		datFile << setw(W) << fixed << collision;
		datFile << endl;
	}

	datFile.close();
}

void Estimator::simulate(const EstimationParameters &parameters) const {
	// Amount of tag counts
	int n = ((parameters.max_tags - parameters.starting_tags) / parameters.increase_value) + 1;

	EstimationResult result(n, parameters.simulations);

	int tag_count, silenced_tags, simulations, frame_count, frame_size;
	int error, slot, idle, success, collision;
	int *frame; // Holds -1 for collision, 0 when empty or 1 otherwise
	high_resolution_clock::time_point start_time, end_time;

	for(int idx = 0; idx < n; ++idx) {
		tag_count = parameters.starting_tags + (parameters.increase_value * idx);
		result.tag_amounts[idx] = tag_count;
		
		start_time = high_resolution_clock::now();
		
		simulations = parameters.simulations;
		while(simulations--) {
			frame_count = 0;
			frame_size = parameters.initial_frame;
			error = 0;
	
			while(frame_size > 0 && tag_count > 0) {
				frame_count++;
				
				frame = new int[frame_size]();

				silenced_tags = 0;
				
				success = 0;
				collision = 0;
				for(int i = 0; i < tag_count; ++i) {
					slot = fast_rand() % frame_size;
				
					if(frame[slot] == -1) { // Collision slot, ignore

					} else if(frame[slot] == 0) { // Empty slot
						++silenced_tags;
						
						frame[slot] = 1;

						++success;
					} else { // First collison
						--silenced_tags;
						
						frame[slot] = -1;
					
						--success;
						++collision;
					}
				}
			
				result.empty_slots[idx] += frame_size - (success + collision);
				result.success_slots[idx] += success;
				result.collision_slots[idx] += collision;
			
				frame_size = this->calculate_frame_size(idle, success, collision);
				
				tag_count -= silenced_tags;
				
				error += abs(tag_count - frame_size);
			}
				
			result.errors[idx] += error / frame_count;
		
			tag_count = parameters.starting_tags + (parameters.increase_value * idx);
		}
		
		end_time = high_resolution_clock::now();
	
		result.simulation_times[idx] = duration_cast<microseconds>(end_time - start_time).count();
	}

	write_dat_file(result);
}

LowerBound::LowerBound() : Estimator("lower-bound", "lower_bound.dat", "w lp lw 2 pt 1 ps 2 t 'Lower Bound'") {}
int LowerBound::calculate_frame_size(int idle, int success, int collision) const {
	return 2 * collision;
}

Schoute::Schoute() : Estimator("schoute", "schoute.dat", "w lp lw 2 pt 6 dt '-' ps 2 t 'Schoute'") {}
int Schoute::calculate_frame_size(int idle, int success, int collision) const {
	return ceil(2.39 * collision);
}

EomLee::EomLee() : Estimator("eom-lee", "eom_lee.dat", "w lp lw 2 pt 4 dt '_' ps 2 t 'Eom Lee'") {}
int EomLee::calculate_frame_size(int idle, int success, int collision) const {
	int l = idle + success + collision;

	double yk = 2, prev_yk = 2;
	double bk, bk_exp;

	while(true) {
		bk = l / (prev_yk * collision + success);
		bk_exp = exp(-1 / bk);
		yk = (1 - bk_exp) / (bk * ((1 - (1 + (1 / bk)) * bk_exp)));
		
		if(fabs(prev_yk - yk) < THRESHOLD) {
			break;
		}
		
		prev_yk = yk;
	}	

	return ceil(yk * collision);
}
