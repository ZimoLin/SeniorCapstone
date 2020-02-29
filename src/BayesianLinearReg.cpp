#include "BayesianLinearReg.h"
#include <cmath>
#include <iostream>
#include <algorithm> 

BayesianLinearReg::BayesianLinearReg(VectorXd a_m0, MatrixXd m_S0, double beta) 
: prior_(a_m0, m_S0), posterior_(a_m0, m_S0)
{
	// this->prior_(a_m0, m_S0);
	this->v_m0_ = a_m0;
	this->m_S0_ = m_S0;
	this->beta_ = beta;

	this->v_mN_ = this->v_m0_;
	this->m_SN_ = this->m_S0_;
	this->posterior_ = this->prior_;
}

// BayesianLinearReg::BayesianLinearReg(string saved_state)
// {
// 	//TODO
// }

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

	cout <<v_mN_ << endl;
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

// string BayesianLinearReg::save_state()
// {
// 	//TODO
// }
