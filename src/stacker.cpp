#include "stacker.h"
#include "stateHelper.h"
#include <cmath>
#include <Eigen/Dense>
#include "BGMM.h"
#include "IsolationForest.h"

using namespace std;
using namespace Eigen;
using namespace IsolationForest;

stacker::stacker(vector<string> model_list, vector<vector<double>> initial_data, 
				int max_stored_data_points, int points_to_reconstruct)
{
	for (string model_name : model_list){
		if (model_name == "BGMM_REPRESENTATIVE"){
			BGMM *bgmm = new BGMM(initial_data, max_stored_data_points, points_to_reconstruct);
			Models_.push_back(bgmm);
		} else if (model_name == "BGMM_RECENT"){
			BGMM *bgmm = new BGMM(initial_data, max_stored_data_points, points_to_reconstruct);
			bgmm->updateSetting(false);
			Models_.push_back(bgmm);
		} else if (model_name == "IFORESTS_REPRESENTATIVE"){
			Forest *iforest = new Forest(initial_data, max_stored_data_points, points_to_reconstruct);
			Models_.push_back(iforest);
		} else if (model_name == "IFORESTS_RECENT"){
			Forest *iforest = new Forest(initial_data, max_stored_data_points, points_to_reconstruct);
			iforest->updateSetting(false);
			Models_.push_back(iforest);
		}
	}

	VectorXd a_m0;
	a_m0.setOnes(Models_.size() + 1);
	a_m0 = a_m0 / Models_.size();
	a_m0(Models_.size()) = 0;

	VectorXd temp;
	temp.setOnes(Models_.size() + 1);
	MatrixXd m_S0 = temp.asDiagonal();

	double beta = 1.0;

	stacking_model_ = new BayesianLinearReg(a_m0, m_S0, beta);
}

stacker::stacker(string saved_state)
{
	vector<string> all_saved_states;
	size_t curPos = saved_state.find('\n'), prevPos = 0;
	while (curPos != string::npos){
		all_saved_states.push_back(saved_state.substr(prevPos, curPos - prevPos));
		if (curPos == saved_state.size() - 1) break;
		prevPos = curPos + 1, curPos = saved_state.find('\n', curPos + 1);
	}

	stateHelper helper;
	vector<vector<vector<double>>> model_state = helper.string_to_matrices(all_saved_states[0]);
	for (size_t i = 0; i < model_state[0][0].size(); ++i){
		if (model_state[0][0][i] == 0.0 || model_state[0][0][i] == 1.0){
			BGMM *bgmm = new BGMM(all_saved_states[1 + i]);
			Models_.push_back(bgmm);
		} else {
			Forest *iforest = new Forest(all_saved_states[1 + i]);
			Models_.push_back(iforest);
		}
	}
	stacking_model_ = new BayesianLinearReg(all_saved_states[all_saved_states.size() - 1]);
}

stacker::~stacker()
{
	delete stacking_model_;
}

vector<double> stacker::process_input(vector<double> input_data)
{
	MatrixXd model_predictions(1, Models_.size() + 1);
	vector<double> res;

	for (size_t i = 0; i < Models_.size(); ++i){
		double tempRes = Models_[i]->process_input(input_data);
		model_predictions(i) = tempRes;
		res.push_back(tempRes);
	}
	model_predictions(Models_.size()) = 1.0;

	res.push_back(inverse_logit(stacking_model_->prediction_limit(model_predictions, 0.0)(0)));

	return res;
}

void stacker::process_feedback(vector<double> model_outputs, vector<double> input_data, bool is_anamoly)
{
	for (auto& m : Models_)
		m->process_feedback(input_data, is_anamoly);

	double res = logit(!is_anamoly);

	VectorXd a_t(1);
	a_t(0) = res;

	MatrixXd a_x(1, model_outputs.size() + 1);

	for (size_t i = 0; i < model_outputs.size(); ++i)
		a_x(0, i) =  model_outputs[i];
	a_x(0, model_outputs.size()) = 1.0;

	stacking_model_->set_posterior(a_x, a_t);
}

double stacker::logit(bool raw)
{
	if (raw)
		return 25.0;
	else
		return -25.0;
}

double stacker::inverse_logit(double p)
{
	return exp(p) / (1 + exp(p));
}

string stacker::save_state()
{
	string res = "";
	for (auto model : Models_)
		res += model->save_state() + "\n";
	res += stacking_model_->save_state() + '\n';
	return res;
}



