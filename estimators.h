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
	int tags_amount;
	int *empty_slots;
	int *success_slots;
	int *collision_slots;
	double *simulation_times;
	EstimationResult(int tags_amount);
	~EstimationResult();
};

class Estimator {
	private:
		string name;
		string file_name;
		string plot_options;
	public:
		Estimator(string name, string file_name, string plot_options);
		virtual void simulate(const EstimationParameters &parameters) const = 0;
		
		string get_name() const;
		string get_file_name() const;
		string get_plot_options() const;
};

class LowerBound : public Estimator {
	public:
		LowerBound();
		void simulate(const EstimationParameters &parameters) const;
};

class EomLee : public Estimator {
	public:
		EomLee();
		void simulate(const EstimationParameters &parameters) const;
};
