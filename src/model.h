#ifndef MODEL_H_
#define MODEL_H_

#include <vector>
#include <string>

using namespace std;

class model
{
public:
	model(vector<vector<double>> initial_input, int max_stored_data_points, int points_to_reconstruct){(void) initial_input; (void) max_stored_data_points; (void) points_to_reconstruct;}
	model(string saved_state){(void)saved_state;};
	virtual ~model(){};
	virtual double process_input(vector<double> input_data) = 0;
	virtual void process_feedback(vector<double> input_data, bool isAnomaly) = 0;
	virtual string save_state() = 0;	
};

#endif
