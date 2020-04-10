#ifndef STACKER_H_
#define STACKER_H_

#include <vector>
#include "BayesianLinearReg.h"
#include "model.h"
#include <Eigen/Dense>
#include <string>

class stacker
{
public:
	stacker(vector<string> model_list, vector<vector<double>> initial_data, int max_stored_data_points, int points_to_reconstruct);
	stacker(string saved_state);
	~stacker();

	vector<double> process_input(vector<double> input_data);
	void process_feedback(vector<double> model_outputs, vector<double> input_data, bool is_anamoly);
	string save_state();

private:
	vector<model*> Models_;
	BayesianLinearReg *stacking_model_;
	double logit(bool raw);
	double inverse_logit(double p);	
};

#endif