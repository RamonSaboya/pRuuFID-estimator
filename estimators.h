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
	int *estimation_times;
	int *simulation_times;

	EstimationResult(int tag_count, int simulations);
};

class Estimator {
	private:
		string type;
		
		string name;
		string file_name;
		string plot_options;
	public:
		Estimator(string type, string name, string file_name, string plot_options);
		
		string get_type() const;
		
		string get_name() const;
		string get_file_name() const;
		string get_plot_options() const;
		
		virtual int calculate_frame_size(int idle, int success, int collision) const = 0;
		virtual void simulate(const EstimationParameters &parameters) const = 0;
		
		void write_dat_file(EstimationResult result) const;
};

class SimpleEstimator : public Estimator {
	public:
		SimpleEstimator(string name, string file_name, string plot_options);
		
		void simulate(const EstimationParameters &parameters) const;
};

class ExtendedEstimator : public Estimator {
	public:
		ExtendedEstimator(string name, string file_name, string plot_options);
};

class LowerBound : public SimpleEstimator {
	public:
		LowerBound();
		int calculate_frame_size(int idle, int success, int collision) const;
};

class Schoute : public SimpleEstimator {
	public:
		Schoute();
		int calculate_frame_size(int idle, int success, int collision) const;
};

class EomLee : public SimpleEstimator {
	private:
		double threshold;
	public:
		EomLee();
		
		double get_threshold() const;
		
		int calculate_frame_size(int idle, int success, int collision) const;
};

class Q : public ExtendedEstimator {
	private:
		int q;
		double c;
		
		double current_q;
	public:
		Q();
		
		int get_q() const;
		double get_c() const;

		int calculate_frame_size(int idle, int success, int collision) const {
			return 0;
		}
		void simulate(const EstimationParameters &parameters) const;
};
