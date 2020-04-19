#ifndef SETTINGHELPER_H
#define SETTINGHELPER_H

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
    int s_points_to_reconstruct;
    double s_anomaly_level;
    vector<string> s_models;
    vector<string> s_features;
    vector<vector<double>> s_barriers;
} Setting;

class settingHelper
{
public:
	settingHelper();
	~settingHelper();

	Setting parse_setting(string fname);
	void create_setting();
	
};

#endif