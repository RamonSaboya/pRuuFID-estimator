#include "string"
using namespace std;

struct EstimationParameters {
	int starting_tags;
	int increase_value;
	int max_tags;
	int simulations;
	int initial_frame;

	EstimationParameters();
	EstimationParameters(int starting_tags, int increase_value, int max_tags, int simulations, int initial_frame);
};

struct EstimationResult {
	int tag_count;
	int simulations;

	int *tag_amounts;
	int *errors;
	int *empty_slots;
	int *success_slots;
	int *collision_slots;
	int *simulation_times;

	EstimationResult(int tag_count, int simulations);
};

class Estimator {
	private:
		string name;
		string file_name;
		string plot_options;
	public:
		Estimator(string name, string file_name, string plot_options);
		void simulate(const EstimationParameters &parameters) const;
		void write_dat_file(EstimationResult result) const;
		
		virtual int calculate_frame_size(int idle, int success, int collision) const = 0;
		
		string get_name() const;
		string get_file_name() const;
		string get_plot_options() const;
};

class LowerBound : public Estimator {
	public:
		LowerBound();
		int calculate_frame_size(int idle, int success, int collision) const;
};

class Schoute : public Estimator {
	public:
		Schoute();
		int calculate_frame_size(int idle, int success, int collision) const;
};

class EomLee : public Estimator {
	public:
		EomLee();
		int calculate_frame_size(int idle, int success, int collision) const;
};
