#ifndef SETTING_HELPER_H
#define SETTING_HELPER_H

#include<iostream>
#include<fstream>
#include <Eigen/Dense>
#include <string>
#include <cfloat>
#include <algorithm>
#include <vector>

using namespace std;
using namespace Eigen;

typedef struct setting
{
    int s_max_store;
    double s_anomaly_level;
    vector<string> s_models;
    vector<vector<double>> s_barriers;
    int s_prediction_delay;
    vector<string> s_features;
} Setting;

class setting_helper
{
public:
	setting_helper();
	~setting_helper();

	Setting parseSetting(string fname);
	void createSetting();
	
};

#endif