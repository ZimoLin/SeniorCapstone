#ifndef SETTING_HELPER_H
#define SETTING_HELPER_H

#include<iostream>
#include<fstream>
#include <string>
#include <cfloat>
#include <algorithm>
#include <vector>

using namespace std;

typedef struct setting
{
    int s_max_store;
    int s_prediction_delay;
    double s_anomaly_level;
    vector<string> s_models;
    vector<string> s_features;
    vector<vector<double>> s_barriers;
} Setting;

class setting_helper
{
public:
	setting_helper();
	~setting_helper();

	Setting parse_setting(string fname);
	void create_setting();
	
};

#endif