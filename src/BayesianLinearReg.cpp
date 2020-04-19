#include "BayesianLinearReg.h"
#include "stateHelper.h"
#include <cmath>
#include <iostream>
#include <algorithm> 

BayesianLinearReg::BayesianLinearReg(VectorXd& a_m0, MatrixXd& m_S0, double beta) 
: prior_(a_m0, m_S0), posterior_(a_m0, m_S0)
{
	this->v_m0_ = a_m0;
	this->m_S0_ = m_S0;
	this->beta_ = beta;

	this->v_mN_ = this->v_m0_;
	this->m_SN_ = this->m_S0_;
}

BayesianLinearReg::BayesianLinearReg(string saved_state)
{
	stateHelper helper;
	vector<vector<vector<double>>> state = helper.string_to_matrices(saved_state);

	Map<VectorXd> tempVec(&state[0][0][0], state[0][0].size());
	v_m0_ = tempVec;
	Map<VectorXd> tempVec1(&state[0][1][0], state[0][1].size());
	v_mN_ = tempVec1;
	beta_ = state[0][2][0];
	
	MatrixXd temp_m_S0_, temp_m_SN_;
	for (size_t i = 0; i < state[1].size(); ++i){
		temp_m_S0_.row(i) = VectorXd::Map(&state[1][i][0], state[1][i].size());
		temp_m_SN_.row(i) = VectorXd::Map(&state[2][i][0], state[2][i].size());
	}
	m_S0_ = temp_m_S0_;
	m_SN_ = temp_m_SN_;

	
	prior_ = normal_random_variable(v_m0_, m_S0_);
	posterior_ = prior_;
}

BayesianLinearReg::~BayesianLinearReg()
{

}

void BayesianLinearReg::set_posterior(MatrixXd& a_x, VectorXd& a_t)
{
	VectorXd v_t = a_t;

	MatrixXd m_phi = a_x;

	m_SN_ = (m_SN_.inverse() + (m_phi.transpose() * m_phi) * (beta_)).inverse();
	v_mN_ = m_SN_ * (m_S0_.inverse() * v_mN_ + beta_ * m_phi.transpose() * v_t);
	
	for (int i = 0; i < v_mN_.size() - 1; ++i)
		v_mN_(i)= max(0.000001, v_mN_(i));
	
	double factor = 1.0 / (v_mN_.sum() - v_mN_(v_mN_.size() - 1));
	
	for (int i = 0; i < v_mN_.size() - 1; ++i)
		v_mN_(i) = factor * v_mN_(i);

	posterior_ = normal_random_variable(v_mN_, m_SN_);
}

VectorXd BayesianLinearReg::prediction_limit(MatrixXd& a_x, double stdevs)
{
	int N = a_x.rows();

	MatrixXd m_x = a_x;

	VectorXd prediction(N); 
	for (int i = 0; i < N; ++i){
		MatrixXd x = m_x.row(i);
		double sig_sq_x = 1/beta_ + (x * m_SN_ * x.transpose())(0, 0);
		double mean_x = (v_mN_.transpose() * x.transpose())(0, 0);
		prediction(i) = mean_x + stdevs * sqrt(sig_sq_x); 
	}	

	return prediction;
}

string BayesianLinearReg::save_state()
{
	vector<vector<vector<double>>> state;

	vector<double> transformed_v_m0_, transformed_v_mN_, transformed_beta_;
	transformed_v_m0_.resize(v_m0_.size());
	transformed_v_mN_.resize(v_mN_.size());

	VectorXd::Map(&transformed_v_m0_[0], v_m0_.size()) = v_m0_;
	VectorXd::Map(&transformed_v_mN_[0], v_mN_.size()) = v_mN_;
	transformed_beta_.push_back(beta_);
	vector<vector<double>> setting({transformed_v_m0_, transformed_v_mN_, transformed_beta_});
	state.push_back(setting);

	vector<vector<double>> transformed_m_S0_(m_S0_.rows(), vector<double>(m_S0_.cols(), 0.0));
	vector<vector<double>> transformed_m_SN_(m_SN_.rows(), vector<double>(m_SN_.cols(), 0.0));
	for (int i = 0; i < m_S0_.rows(); ++i){
		for (int j = 0; j < m_S0_.cols(); ++j){
			transformed_m_S0_[i][j] = m_S0_.row(i)(j);
			transformed_m_SN_[i][j] = m_SN_.row(i)(j);
		}
	}
	state.push_back(transformed_m_S0_);
	state.push_back(transformed_m_SN_);
	
	stateHelper helper;
	return helper.matrices_to_string(state);
}
