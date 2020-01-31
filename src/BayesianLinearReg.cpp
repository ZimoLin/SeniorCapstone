#include "BayesianLinearReg.h"
#include <cmath>

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

BayesianLinearReg::~BayesianLinearReg()
{
	
}

MatrixXd BayesianLinearReg::get_phi(VectorXd& a_x)
{
	MatrixXd m_phi;
	m_phi.setOnes(2, a_x.size());
	m_phi.row(1) << a_x;
	return m_phi;
}

void BayesianLinearReg::set_posterior(VectorXd& a_x, VectorXd& a_t)
{
	VectorXd v_t = a_t;

	VectorXd m_phi = get_phi(a_x);

	m_SN_ = (m_S0_.inverse() + (m_phi.transpose() * m_phi) * (beta_)).inverse();
	v_mN_ = m_SN_ * (m_S0_.inverse() * v_m0_ + beta_ * m_phi.transpose() * v_t);
	posterior_ = normal_random_variable(v_mN_, m_SN_);
}

VectorXd BayesianLinearReg::prediction_limit(VectorXd& a_x, double stdevs)
{
	int N = a_x.size();

	VectorXd m_x = get_phi(a_x).transpose();

	VectorXd prediction(N); 
	for (int i = 0; i < N; ++i){
		MatrixXd x = m_x.row(i);
		double sig_sq_x = 1/beta_ + (x.transpose() * m_SN_ * x)(0, 0);
		double mean_x = (v_mN_.transpose() * x)(0, 0);
		prediction(i) = mean_x + stdevs * sqrt(sig_sq_x); 
	}	

	return prediction;
}

