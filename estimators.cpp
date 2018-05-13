#include "estimators.h"
#include <stdlib.h>
#include <set>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <string.h>

const int W = 15;

EstimationParameters::EstimationParameters() {}

EstimationParameters::EstimationParameters(int starting_tags, int increase_value, int max_tags, int simulations, int initial_frame) {
	this->starting_tags = starting_tags;
	this->increase_value = increase_value;
	this->max_tags = max_tags;
	this->simulations = simulations;
	this->initial_frame = initial_frame;
}

EstimationResult::EstimationResult(int tags_amount) {
	this->tags_amount = tags_amount;
	this->tags_amounts = new int[tags_amount];
	this->final_frames = new int[tags_amount];
	this->empty_slots = new int[tags_amount];
	this->success_slots = new int[tags_amount];
	this->collision_slots = new int[tags_amount];
	this->simulation_times = new double[tags_amount];
	this->abs_errors = new double[tags_amount];
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

	datFile << "# " << this->get_name() << endl;
	datFile << "# " << setw(W - 2) << "tags" << setw(W) << "error" << setw(W) << "slots" << setw(W) << "time" << setw(W) << "empty" << setw(W) << "collision" << endl;
	datFile << "#" << endl;
	
	for(int i = 0; i < result.tags_amount; ++i) {
		int tags = result.tags_amounts[i];
		int final_frames = result.final_frames[i];
		
		int empty_slots = result.empty_slots[i];
		int success_slots = result.success_slots[i];
		int collision_slots = result.collision_slots[i];
		double simulation_times = result.simulation_times[i];
		
		int total_slots = empty_slots + success_slots + collision_slots;
		
		int error = result.abs_errors[i];

		datFile << setw(W) << tags << setw(W) << error << setw(W) << total_slots << setw(W) << simulation_times << setw(W) << empty_slots << setw(W) << collision_slots << endl;
	}

	datFile.close();
}

LowerBound::LowerBound() : Estimator("lower-bound", "lower_bound.dat", "w lp lw 2 pt 4 ps 2 t 'Lower Bound'") {}
void LowerBound::simulate(const EstimationParameters &parameters) const {
	int n = (parameters.max_tags/parameters.increase_value) + 1;

	EstimationResult result(n);

	int tags_left = parameters.starting_tags;

	int initial_size = parameters.initial_frame;
	int current_size = initial_size;
	int* slots = new int[initial_size];
	
	int total_sucess = 0;
	int total_empty = 0;
	int total_collision = 0;
	int total_simulation = 0;
	int total_frames = 1;
	int abs_error = 0;

	for (int i = 0; i < n; i++){
		while(tags_left > 0){
			int sucess = 0;
			int collision = 0;

			for (int tag = 0; tag < tags_left; tag++){
				int random = rand() % current_size;
				slots[random] ++;	
			}

			for (int index = 0; index < current_size; index ++){
				if (slots[index] == 1){
					sucess++;
				}else if (slots[index] > 1){
					collision ++;
				}else if (slots[index] == 0){ 
					total_empty ++;
				}
			}

			abs_error += abs(tags_left - current_size);

			tags_left -= sucess;

			total_sucess += sucess;
			total_collision += collision;
			total_simulation ++;
			total_frames ++;
			
			//lower bound - frame size
			current_size = 2 * collision;
			slots = new int[current_size];

			memset(slots, 0, current_size * sizeof(int));	
		}

		result.tags_amounts[i] = (parameters.starting_tags + (parameters.increase_value * i)); 
		result.final_frames[i] = total_frames;
		result.empty_slots[i] = total_empty;
		result.collision_slots[i] = total_collision;
		result.success_slots[i] = total_sucess;
		result.simulation_times[i] = total_simulation;
		result.abs_errors[i] = (abs_error/total_frames);

		tags_left = parameters.starting_tags + (parameters.increase_value * (i+1));
		total_sucess = 0;
		total_collision = 0;
		total_empty = 0;
		total_simulation = 0;
		total_frames = 1;
		abs_error = 0;
		slots = new int[initial_size];
		current_size = initial_size;
	}

	write_dat_file(result);
}

EomLee::EomLee() : Estimator("eom-lee", "eom_lee.dat", "w lp lw 2 pt 6 ps 2 t 'Eom Lee'") {}
void EomLee::simulate(const EstimationParameters &parameters) const {
	int n = (parameters.max_tags/parameters.increase_value) + 1;

	EstimationResult result(n);

	int tags_left = parameters.starting_tags;

	int initial_size = parameters.initial_frame;
	int current_size = initial_size;
	int* slots = new int[initial_size];

	int total_sucess = 0;
	int total_empty = 0;
	int total_collision = 0;
	int total_simulation = 0;
	int total_frames = 1;
	int abs_error = 0;

	for (int i = 0; i < n; i++){

		while(tags_left != 0){
			int sucess = 0;
			int collision = 0;
			int empty = 0;

			for (int tag = 0; tag < tags_left; tag++){
				int random = rand() % current_size;
				slots[random] ++;
			}

			for (int index = 0; index < current_size; index ++){
				if (slots[index] == 1){
					sucess++;
				}else if (slots[index] > 1){
					collision ++;
				}else{
					empty ++;
				}
			}

			abs_error += 0;
			if(current_size > tags_left){
				abs_error += abs(current_size - tags_left);
			}

			tags_left -= sucess;

			total_sucess += sucess;
			total_collision += collision;
			total_empty += empty;
			total_simulation ++;
			total_frames ++;
			
			//eom lee - frame size

			int L = sucess + collision + empty;			
			double yk = 2;
			double threshold = 0.001;
			int i = 1;
			while(true){
				double last_yk = yk;
				double bk = L/(last_yk * collision * sucess);
				double e = exp(-1 / bk);
            			yk = (1 - e) / (bk * (1 - (1 + 1 / bk) * e));

            			if (abs(last_yk - yk) < threshold) {
					current_size = ceil(yk * collision);
					break;
				}
			}

			slots = new int[current_size];	
			memset(slots, 0, current_size*sizeof(int));	
		}

		result.tags_amounts[i] = (parameters.starting_tags + (parameters.increase_value * i)); 
		result.final_frames[i] = total_frames;
		result.empty_slots[i] = total_empty;
		result.collision_slots[i] = total_collision;
		result.success_slots[i] = total_sucess;
		result.simulation_times[i] = total_simulation;
		result.abs_errors[i] = (abs_error/total_frames);

		tags_left = parameters.starting_tags + (parameters.increase_value * (i+1));
		
		total_sucess = 0;
		total_collision = 0;
		total_empty = 0;
		total_simulation = 0;
		total_frames = 1;
		abs_error = 0;
		slots = new int[initial_size];
		current_size = initial_size;
	}

	write_dat_file(result);
}
