#include "model.h"
#include "BGMM.h"
#include <iostream>
#include <random>
#include <chrono>

using namespace std;
using namespace Eigen;

int main()
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  	default_random_engine generator (seed);
  	normal_distribution<double> distribution1 (0.0,1);

  	vector<vector<double>> data;
  	for (int i = 0; i < 100; i++)
  		data.push_back({distribution1(generator), distribution1(generator)});
	BGMM bgmm(data, 50);	
	bgmm.updateSetting(true);	
	bgmm.process_input({1, 1});
}