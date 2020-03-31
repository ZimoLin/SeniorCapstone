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
	if (argc <= 1 || argc > 3){
		cout << "Usage: ./testADS [dimension] [max store points]" << endl;
		return 0;
	}
	
	int dim = stoi(argv[1]);
	int maxSize = stoi(argv[2]);
	if (dim <= 0 || maxSize <= 0)
		throw("Enter a number larger than 0 for the arguments.");
	
	AnomalyDetectionSystem ads;

	unsigned seed = system_clock::now().time_since_epoch().count();
  	default_random_engine generator (seed);
  	normal_distribution<double> distribution1 (0.0,1);

  	AnomalyDetectionSystem::anomaly_detected_call_func func = myFunc;

  	cout << "Prime the ADS with 1000 samples from a standard normal distribution" << endl;
	for (int i = 0; i < maxSize + 1; ++i){
		vector<double> cur;
		for (int i = 0; i < dim; ++i)
			cur.push_back(distribution1(generator));
		ads.process_input(cur, func);
	}

    normal_distribution<double> distribution2 (0.0,10);

    cout <<  "Now that the variance has expanded, we will start to see anomalous samples appear" << endl;
	for (int i = 0; i < 20; ++i){
		vector<double> cur;
		for (int i = 0; i < dim; ++i)
			cur.push_back(distribution2(generator));
		ads.process_input(cur, func);
	}

    normal_distribution<double> distribution3 (0.0,0.1);

    vector<vector<double>> data;

    for (int i = 0; i < 1000; ++i){
    	vector<double> cur;
    	for (int j = 0; j < dim; ++j)
    		cur.push_back(distribution3(generator));
    	data.push_back(cur);
    }

    cout << "On the other hand, now that the variance has contracted, we will stop seeing anomalous samples." << endl;

    auto start = start_time();
    for (int i = 0; i < 1000; ++i)
        ads.process_input(data[i], func);
    
    double time = delta_usec(start)/1000.0;
    cout << "1000 data points took " << time << " ms." << endl;
	return 0;
}

