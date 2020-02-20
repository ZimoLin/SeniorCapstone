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

	for (int i = 0; i < 5000; ++i)
		initial_data.push_back(vector<double>({distribution1(generator), distribution1(generator)}));

	stacker testStacker(model_list, initial_data);

	vector<double> test_data(2);

	// for (int i = 0; i < 2500; ++i){
	// 	test_data[0] = distribution1(generator);
	// 	test_data[1] = distribution1(generator);
	// 	testStacker.process_input(test_data);
	// }

	testStacker.process_input({100, 100});
	testStacker.process_input({0, 0});
	testStacker.process_input({.5, .5});

	testStacker.process_input({1, 1});

	testStacker.process_feedback({0.767035} ,{0.5, 0.5}, false);

	testStacker.process_input({.5, .5});
	testStacker.process_input({2, 2});


	return 0;
}