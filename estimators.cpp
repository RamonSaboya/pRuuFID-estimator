#include "estimators.h"
#include <set>
#include <iostream>
#include <fstream>

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
	this->empty_slots = new int[tags_amount];
	this->success_slots = new int[tags_amount];
	this->collision_slots = new int[tags_amount];
	this->simulation_times = new double[tags_amount];
}

EstimationResult::~EstimationResult() {
	delete[] this->empty_slots;
	delete[] this->success_slots;
	delete[] this->collision_slots;
	delete[] this->simulation_times;
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

LowerBound::LowerBound() : Estimator("lower-bound", "lower_bound.dat", "w lp lw 2 pt 4 ps 2 t 'Lower Bound'") {}
void LowerBound::estimate(const EstimationParameters &parameters) const {
	EstimationResult result(10);
	ofstream datFile(this->get_file_name());
	datFile << "# test" << endl;
}

EomLee::EomLee() : Estimator("eom-lee", "eom_lee.dat", "w lp lw 2 pt 6 ps 2 t 'Eom Lee'") {}
void EomLee::estimate(const EstimationParameters &parameters) const {
	EstimationResult result(10);
}
