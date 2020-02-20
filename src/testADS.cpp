#include "AnomalyDetectionSystem.h"
#include <random>
#include <chrono>
#include <iostream>

using namespace std;

int anomalyCount = 0;

void myFunc(vector<double> model_results, vector<double> data){
	cout << "\nprinting model results:\n" << endl;
	for (double d : model_results)
		cout << d << endl;

	cout << "\nprinting data:\n" << endl;
	for (double d : data)
		cout << d << endl;
	cout << "This is the " << ++anomalyCount << " anomalies count" << endl;
}

int main()
{
	AnomalyDetectionSystem ads1("setting1.txt");

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  	default_random_engine generator (seed);
  	normal_distribution<double> distribution1 (0.0,1);

  	AnomalyDetectionSystem::anomaly_detected_call_func func = myFunc;

	for (int i = 0; i < 1000; ++i)
		ads1.process_input(vector<double>({distribution1(generator), distribution1(generator)}), func);
	// a.process_input(vector<double>({100, 100}), func);

	for (int i = 0; i < 2000; ++i)
		ads1.process_input(vector<double>({distribution1(generator), distribution1(generator)}), func);


	// Barriers tests
	anomalyCount = 0;
	AnomalyDetectionSystem ads2("setting2.txt");
  	vector<double> cand({-0.5,0.5,1.25,1.5,1.75,2.25,12.5,17.5,22.5,27.5});
  	
  	for (size_t i = 0; i < cand.size(); ++i){
  		for (size_t j = 0; j < cand.size(); ++j){
  			ads2.process_input(vector<double>({cand[i], cand[j]}), func);
  		}
  	}
	return 0;
}

