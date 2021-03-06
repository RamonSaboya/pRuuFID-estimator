#include "estimators.h"
#include "math_util.h"
#include <bits/stdc++.h>
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

const int INIT_Q = 6;
const double C = 0.1;

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
	this->estimation_times = new int[tag_count]();
	this->simulation_times = new int[tag_count]();
}

Estimator::Estimator(string type, string name, string file_name, string plot_options) {
	this->type = type;
	
	this->name = name;
	this->file_name = file_name;
	this->plot_options = plot_options;
}

string Estimator::get_type() const {
	return type;
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

// Generate data file
void Estimator::write_dat_file(EstimationResult result) const {
	ofstream datFile(this->get_file_name());

	// Set header
	datFile << "# " << setw(3 * W - 2) << this->get_name() << endl;
	datFile << "#" << endl;
	datFile << "#" << endl;
	datFile << "# " << setw(W - 2) << "tags";
	datFile << setw(W) << "slots";
	datFile << setw(W) << "empty";
	datFile << setw(W) << "collision";
	datFile << setw(W) << "efficiency";
	datFile << setw(W) << "error";
	datFile << setw(W) << "simul_time";
	datFile << setw(W) << "estim_time";
	datFile << endl;
	datFile << "#" << endl;
	
	double simulations = result.simulations;
	
	// Print lines corresponding to different tag counts
	for(int i = 0; i < result.tag_count; ++i) {
		int tags = result.tag_amounts[i];
		double errors = result.errors[i] / simulations;
		double empty = result.empty_slots[i] / simulations;
		double success = result.success_slots[i] / simulations;
		double collision = result.collision_slots[i] / simulations;
		double estimation_times = result.estimation_times[i] / simulations / 1e3;
		double simulation_times = result.simulation_times[i] / simulations / 1e3;
		
		double total_slots = empty + success + collision;
		
		double efficiency = success / total_slots;

		datFile << setw(W) << fixed << tags;
		datFile << setw(W) << fixed << total_slots;
		datFile << setw(W) << fixed << empty;
		datFile << setw(W) << fixed << collision;
		datFile << setw(W) << fixed << efficiency;
		datFile << setw(W) << fixed << errors;
		datFile << setw(W) << fixed << simulation_times;
		datFile << setw(W) << fixed << estimation_times;
		datFile << endl;
	}

	datFile.close();
}

SimpleEstimator::SimpleEstimator(string name, string file_name, string plot_options) : Estimator("SIMPLE", name, file_name, plot_options) {}

void SimpleEstimator::simulate(const EstimationParameters &parameters) const {
	// Amount of tag counts
	int n = ((parameters.max_tags - parameters.starting_tags) / parameters.increase_value) + 1;

	EstimationResult result(n, parameters.simulations);

	int tag_count, simulations, frame_count, frame_size;
	int error, slot, idle, success, collision;
	int *frame; // Holds -1 for collision, 0 when empty or 1 otherwise
	high_resolution_clock::time_point start_estimation, end_estimation;
	high_resolution_clock::time_point start_simulation, end_simulation;

	for(int idx = 0; idx < n; ++idx) {
		tag_count = parameters.starting_tags + (parameters.increase_value * idx);
		result.tag_amounts[idx] = tag_count;
		
		start_simulation = high_resolution_clock::now();
		
		simulations = parameters.simulations;
		while(simulations--) {
			frame_count = 0;
			frame_size = parameters.initial_frame;
			error = 0;
	
			while(frame_size > 0 && tag_count > 0) {
				frame_count++;
				
				frame = new int[frame_size]();
				
				success = 0;
				collision = 0;
				for(int i = 0; i < tag_count; ++i) {
					slot = fast_rand() % frame_size;
				
					if(frame[slot] == 0) { // Empty slot
						frame[slot] = 1;

						++success;
					} else if(frame[slot] == 1) { // First collison
						frame[slot] = -1;
					
						--success;
						++collision;
					}
				}
				
				idle = frame_size - (success + collision);
				
				result.empty_slots[idx] += idle;
				result.success_slots[idx] += success;
				result.collision_slots[idx] += collision;
		
				start_estimation = high_resolution_clock::now();
			
				frame_size = calculate_frame_size(idle, success, collision);
		
				end_estimation = high_resolution_clock::now();

				result.estimation_times[idx] += duration_cast<nanoseconds>(end_estimation - start_estimation).count();
				
				tag_count -= success;
				
				error += abs(tag_count - frame_size);
			}
			
			result.errors[idx] += error / frame_count;
		
			tag_count = parameters.starting_tags + (parameters.increase_value * idx);
		}
		
		end_simulation = high_resolution_clock::now();
	
		result.simulation_times[idx] = duration_cast<microseconds>(end_simulation - start_simulation).count();
	}

	write_dat_file(result);
}

ExtendedEstimator::ExtendedEstimator(string name, string file_name, string plot_options) : Estimator("EXTENDED", name, file_name, plot_options) {}

LowerBound::LowerBound() : SimpleEstimator("lower-bound", "lower_bound.dat", "w lp lw 1.75 pt 1 ps 2.75 lt -1 t 'Lower Bound'") {}
int LowerBound::calculate_frame_size(int idle, int success, int collision) const {
	return collision << 1;
}

EomLee::EomLee() : SimpleEstimator("eom-lee", "eom_lee.dat", "w lp lw 1.75 pt 4 dt '_' ps 2.75 lt -1 t 'Eom Lee'") {
	this->threshold = THRESHOLD;
}
double EomLee::get_threshold() const {
	return this->threshold;
}
int EomLee::calculate_frame_size(int idle, int success, int collision) const {
	int l = idle + success + collision;

	double yk = 2, prev_yk = 2;
	double bk, bk_exp;

	while(true) {
		bk = l / (prev_yk * collision + success);
		bk_exp = exp(-1 / bk);
		yk = (1 - bk_exp) / (bk * ((1 - (1 + (1 / bk)) * bk_exp)));
		
		if(fabs(prev_yk - yk) < this->get_threshold()) {
			break;
		}
		
		prev_yk = yk;
	}	

	return ceil(yk * collision);
}

Schoute::Schoute() : SimpleEstimator("schoute", "schoute.dat", "w lp lw 1.75 pt 6 dt 3 ps 2.75 lt -1 t 'Schoute'") {}
int Schoute::calculate_frame_size(int idle, int success, int collision) const {
	return ceil(2.39 * collision);
}

Q::Q() : ExtendedEstimator("Q", "q.dat", "w lp lw 1.75 pt 8 dt '.' ps 2.75 lt -1 t 'Q'") {
	this->q = INIT_Q;
	this->c = C;
	this->current_q = INIT_Q;
}
int Q::get_q() const {
	return this->q;
}
double Q::get_c() const {
	return this->c;
}
void Q::simulate(const EstimationParameters &parameters) const {
	int n = ((parameters.max_tags - parameters.starting_tags) / parameters.increase_value) + 1;

	EstimationResult result(n, parameters.simulations);

	int tag_count, simulations, frame_count, frame_size, next_frame_size, in_slot, last_success;
	int error, silenced, idle, success, collision;
	int *tags;
	double current_q;
	bool all_read, size_changed, frame_collision;
	high_resolution_clock::time_point start_time, end_time;

	for(int idx = 0; idx < n; ++idx) {
		tag_count = parameters.starting_tags + (parameters.increase_value * idx);
		result.tag_amounts[idx] = tag_count;
		
		start_time = high_resolution_clock::now();
		
		simulations = parameters.simulations;
		while(simulations--) {
			current_q = this->get_q();
			
			frame_count = 1;
			error = 0;
			
			tags = new int[tag_count]();
			silenced = 0;
			
			all_read = false;
			while(!all_read) {
				size_changed = true;
				frame_collision = false;
				
				frame_size = 1 << (int) round(current_q);
				for(int slot = 0; slot < frame_size; ++slot) {
					frame_count++;
					in_slot = 0;
					
					for(int i = 0; i < tag_count; ++i) {
						if(tags[i] == INT_MIN) {
							continue;
						}
						
						if(size_changed) {
							tags[i] = fast_rand() % frame_size; // Query Adjust
						} else {
							tags[i]--; // Query Repeat
						}
						
						if(tags[i] == 0) {
							in_slot++;
							last_success = i;
						}
					}
				
					idle = in_slot == 0 ? 1 : 0;
					success = in_slot == 1 ? 1 : 0;
					collision = in_slot >= 2 ? 1 : 0;
				
					result.empty_slots[idx] += idle;
					result.success_slots[idx] += success;
					result.collision_slots[idx] += collision;
					
					if(success) {
						tags[last_success] = INT_MIN;
						++silenced;
					}
					
					if(collision) {
						frame_collision = true;
					}
				
					if(slot == frame_size - 1 && !frame_collision) {
						all_read = true;
					} else {
						if(idle) {
							current_q = max(current_q - this->get_c(), 0.0);
						} else if(collision) {
							current_q = min(current_q + this->get_c(), 15.0);
						}
						
						next_frame_size = 1 << (int) round(current_q);
						
						if(next_frame_size != frame_size) {
							frame_size = next_frame_size;
						
							error += abs((tag_count - silenced) - frame_size);
							break;
						} else {
							size_changed = false;
						}
					}
				}
			}
			
			result.errors[idx] += error / frame_count;
			
			tag_count = parameters.starting_tags + (parameters.increase_value * idx);
		}
		
		end_time = high_resolution_clock::now();
	
		result.simulation_times[idx] = duration_cast<microseconds>(end_time - start_time).count();
	}
	
	write_dat_file(result);
}
