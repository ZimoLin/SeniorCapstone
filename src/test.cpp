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
	
	VectorXd a_m0;
	MatrixXd m_s0;
	double beta = 0.0;
	BayesianLinearReg(a_m0, m_s0, beta);
	// BGMM model1;
	
	// VectorXd v(20);
	// v << 1;
	// cout << v.size() << endl;

	// model1.pushData(vector<double>({1}));
	// model1.pushData(vector<double>({10}));

	// for (int i = 0; i < 100; i++){
	// 	model1.process_input(vector<double>({(double)10 * i + i + 1}));	
	// }

	// model1.printWeight();

	// cout << model2.process_input(vector<double>({10, 20, 30, 40}));
	// model2.printWeight();
	return 0;
}