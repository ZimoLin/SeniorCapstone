#include "AnomalyDetectionSystem.h"
#include <random>
#include <chrono>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

int anomalyCount = 0;

void myFunc(vector<double> model_results, vector<double> data){
    cout << "\n Anomaly Detected\n" << endl;
	cout << "\n Model results:\n" << endl;
	for (double d : model_results)
		cout << d << endl;

	cout << "\n Data:\n" << endl;
	for (double d : data)
		cout << d << endl;
	cout << "This is the " << ++anomalyCount << " anomaly detected" << endl;
}

time_point<high_resolution_clock> start_time () {
    return (high_resolution_clock::now());
}


long int delta_usec
	(time_point<high_resolution_clock> start) {
    time_point<high_resolution_clock>
		end = high_resolution_clock::now();
    duration<double, ratio<1, 1000000> > elapsed_us=end-start;
    long int ticks_us = elapsed_us.count();
    return (ticks_us);
}

int main(int argc, char const *argv[])
{
	if (argc <= 0 || argc > 1)
		throw("Usage: ./testADS [dimension size]");
	
	int dim = stoi(argv[0]);
	if (dim <= 0)
		throw("Enter a number larger than 0;")
	
	AnomalyDetectionSystem ads;

	unsigned seed = system_clock::now().time_since_epoch().count();
  	default_random_engine generator (seed);
  	normal_distribution<double> distribution1 (0.0,1);

  	AnomalyDetectionSystem::anomaly_detected_call_func func = myFunc;

  	 // Prime the ADS with 1000 samples from a standard normal distribution
	for (int i = 0; i < 1000; ++i)
		ads.process_input(vector<double>({distribution1(generator), distribution1(generator), distribution1(generator), distribution1(generator)}), func);

    normal_distribution<double> distribution2 (0.0,10);

    // Now that the variance has expanded, we will start to see anomalous samples appear
	for (int i = 0; i < 0; ++i)
		ads.process_input(vector<double>({distribution2(generator), distribution2(generator), distribution2(generator), distribution2(generator)}), func);

    normal_distribution<double> distribution3 (0.0,10);

    vector<vector<double>> data;

    for (int i = 0; i < 1000; ++i){
    	vector<double> cur;
    	for (int j = 0; j < dim; ++j)
    		cur.push_back(distribution3(generator));
    	data.push_back(cur);
    }





}

int main()
{
	AnomalyDetectionSystem ads;

	unsigned seed = system_clock::now().time_since_epoch().count();
  	default_random_engine generator (seed);
  	normal_distribution<double> distribution1 (0.0,1);

  	AnomalyDetectionSystem::anomaly_detected_call_func func = myFunc;

    // Prime the ADS with 1000 samples from a standard normal distribution
	for (int i = 0; i < 1000; ++i)
		ads.process_input(vector<double>({distribution1(generator), distribution1(generator), distribution1(generator), distribution1(generator)}), func);

    normal_distribution<double> distribution2 (0.0,10);

    // Now that the variance has expanded, we will start to see anomalous samples appear
	for (int i = 0; i < 0; ++i)
		ads.process_input(vector<double>({distribution2(generator), distribution2(generator), distribution2(generator), distribution2(generator)}), func);

    normal_distribution<double> distribution3 (0.0,10);

    auto start = start_time();
    // On the other hand, now that the variance has contracted, we will stop seeing anomalous samples
    for (int i = 0; i < 1000; ++i)
        ads.process_input(vector<double>({distribution3(generator), distribution3(generator), distribution3(generator), distribution3(generator)}), func);
    
    double time = delta_usec(start)/1000.0;
    cout << "10000 data points took " << time << " ms." << endl;
	return 0;
}

