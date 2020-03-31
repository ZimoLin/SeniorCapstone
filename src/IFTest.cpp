#include "IsolationForest.h"
#include <iostream>

using namespace std;
using namespace IsolationForest;
int main()
{
	vector<double> temp(2, 0.0);
	vector<vector<double>> data;
	data.push_back(temp);

	Forest testIF(data, 1000);
	return 0;
}