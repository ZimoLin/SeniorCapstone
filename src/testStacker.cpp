#include "BayesianLinearReg.h"
#include "BGMM.h"
#include "stacker.h"
#include <vector>
#include <random>
#include <chrono>
#include <iostream>

using namespace std;

int main()
{	
	vector<string> model_list;
	model_list.push_back("BGMM");
	vector<vector<double>> initial_data;

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  	default_random_engine generator (seed);
  	normal_distribution<double> distribution1 (0.0,1);

	for (int i = 0; i < 10000; ++i)
		initial_data.push_back(vector<double>({distribution1(generator), distribution1(generator)}));

	stacker testStacker(model_list, 2, initial_data);

	cout << testStacker.process_input({100, 100}) << endl;
	cout << testStacker.process_input({0, 0}) << endl;
	cout << testStacker.process_input({.5, .5}) << endl;

	testStacker.process_feedback({0.767035} ,{0.5, 0.5}, false);

	cout << testStacker.process_input({.5, .5}) << endl;

	return 0;
}