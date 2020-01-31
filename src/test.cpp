#include "model.h"
#include "BGMM.h"
#include <iostream>
#include "BayesianLinearReg.h"

using namespace std;
using namespace Eigen;

int main(int argc, char const *argv[])
{
	(void) argc;
	(void) argv;
	
	VectorXd a_m0(2);
	a_m0(0) = 0;
	a_m0(1) = 0;

	MatrixXd m_s0(2, 2);
	m_s0(0, 0) = 1;
	m_s0(1, 1) = 1;
	m_s0(0, 1) = 0;
	m_s0(1, 0) = 0;

	double beta = 1.0;
	BayesianLinearReg model(a_m0, m_s0, beta);

	MatrixXd a_x(6, 2);
	
	a_x << 1, -1,
		   2, -1,
		   1, -2,
		   2, 0,
		   4, 2,
		   -3, 5;
	
	

	VectorXd a_t(6);

	a_t << 0, 1, -1, 2, 6, 2;

	model.set_posterior(a_x, a_t);

	MatrixXd a_x1(1, 2);
	a_x1(0, 0) = 1;
	a_x1(0, 1) = 1;

	MatrixXd a_x2(1, 2);
	a_x2(0, 0) = 10;
	a_x2(0, 1) = 10;

	MatrixXd a_x3(1, 2);
	a_x3(0, 0) = 10;
	a_x3(0, 1) = 0;

	MatrixXd a_x4(1, 2);
	a_x4(0, 0) = 0;
	a_x4(0, 1) = 10;

	double stdevs1 = 0.0;
	double stdevs2 = 2.0;

	cout << "ax = {1, 1}, stdev = 0.0: " << model.prediction_limit(a_x1, stdevs1) << endl;
	cout << "ax = {1, 1}, stdev = 2.0: " << model.prediction_limit(a_x1, stdevs2) << endl;
	cout << "ax = {10, 10}, stdev = 0.0: " << model.prediction_limit(a_x2, stdevs1) << endl;
	cout << "ax = {10, 10}, stdev = 2.0: " << model.prediction_limit(a_x2, stdevs2) << endl;
	cout << "ax = {10, 0}, stdev = 0.0: " << model.prediction_limit(a_x3, stdevs1) << endl;
	cout << "ax = {10 , 0}, stdev = 2.0: " << model.prediction_limit(a_x3, stdevs2) << endl;
	cout << "ax = {0, 10}, stdev = 0.0: " << model.prediction_limit(a_x4, stdevs1) << endl;
	cout << "ax = {0, 10}, stdev = 2.0: " << model.prediction_limit(a_x4, stdevs2) << endl;

	return 0;
}