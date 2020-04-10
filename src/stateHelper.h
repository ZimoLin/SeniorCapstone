#ifndef STATE_HELPER_H
#define STATE_HELPER_H

#include <iostream>
#include <fstream>
#include <string>
#include <cfloat>
#include <algorithm>
#include <vector>

using namespace std;

class stateHelper
{
public:
	stateHelper();
	~stateHelper();

	string matrices_to_string(vector<vector<vector<double>>> data_matrix);
	vector<vector<vector<double>>> string_to_matrices(string line);
	
};

#endif