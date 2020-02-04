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
	stacker(vector<string> model_list, int input_size, vector<vector<double>> initial_data);
	~stacker();

	double process_input(vector<double> input_data);
	void process_feedback(vector<double> model_outputs, vector<double> input_data, bool is_anamoly);

private:
	vector<model*> Models_;
	BayesianLinearReg *stacking_model_;
	double logit(bool raw);
	double inverse_logit(double p);	
};

#endif