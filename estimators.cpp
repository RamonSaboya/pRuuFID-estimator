#include "estimators.h"
#include <stdlib.h>
#include <set>
#include <iostream>
#include <iomanip>
#include <fstream>

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
		
		int error = abs(tags - final_frames);

		datFile << setw(W) << tags << setw(W) << error << setw(W) << total_slots << setw(W) << simulation_times << setw(W) << empty_slots << setw(W) << collision_slots << endl;
	}

	datFile.close();
}

LowerBound::LowerBound() : Estimator("lower-bound", "lower_bound.dat", "w lp lw 2 pt 4 ps 2 t 'Lower Bound'") {}
void LowerBound::simulate(const EstimationParameters &parameters) const {
	EstimationResult result(10);

	for(int i = 0; i < 10; ++i) {
		result.tags_amounts[i] = 100 * (i + 1);
		result.final_frames[i] = result.tags_amounts[i] + rand() % 140;
		result.empty_slots[i] = rand() % 1100;
		result.collision_slots[i] = rand() % 1800;
		result.success_slots[i] = rand() % (3500 - (result.collision_slots[i] + result.empty_slots[i]));
		result.simulation_times[i] = 10 * (rand() / (RAND_MAX + 1.0));
	}

	write_dat_file(result);
}

EomLee::EomLee() : Estimator("eom-lee", "eom_lee.dat", "w lp lw 2 pt 6 ps 2 t 'Eom Lee'") {}
void EomLee::simulate(const EstimationParameters &parameters) const {
	EstimationResult result(10);

	for(int i = 0; i < 10; ++i) {
		result.tags_amounts[i] = 100 * (i + 1);
		result.final_frames[i] = result.tags_amounts[i] + rand() % 140;
		result.empty_slots[i] = rand() % 1100;
		result.collision_slots[i] = rand() % 1800;
		result.success_slots[i] = rand() % (3500 - (result.collision_slots[i] + result.empty_slots[i]));
		result.simulation_times[i] = 10 * (rand() / (RAND_MAX + 1.0));
	}

	write_dat_file(result);
}
