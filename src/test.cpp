#include "model.h"
#include "BGMM.h"
#include <iostream>

using namespace std;
using namespace Eigen;

int main(int argc, char const *argv[])
{
	(void) argc;
	(void) argv;
	BGMM model1;
	
	model1.pushData(vector<double>({1}));
	model1.pushData(vector<double>({10}));

	for (int i = 0; i < 100; i++){
		model1.process_input(vector<double>({(double)10 * i + i + 1}));	
	}

	// model1.printWeight();

	// cout << model2.process_input(vector<double>({10, 20, 30, 40}));
	// model2.printWeight();
	return 0;
}