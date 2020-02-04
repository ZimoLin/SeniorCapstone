#ifndef MODEL_H_
#define MODEL_H_

#include <vector>

using namespace std;

class model
{
public:
	model(vector<vector<double>> initial_input){(void) initial_input;}
	virtual ~model(){};
	virtual double process_input(vector<double> input_data) = 0;
	virtual void process_feedback(vector<double> input_data, bool isAnomaly) = 0;	
};

#endif
