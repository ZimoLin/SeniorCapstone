#ifndef BGMM_H_
#define BGMM_H_

#include "model.h"
#include "libcluster.h"
#include "distributions.h"
#include <vector>

using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace distributions;

class BGMM : public model
{
public:
	BGMM(vector<vector<double>> initial_data);
	~BGMM();
	double process_input(vector<double> input_data);
	void process_feedback(vector<double> input_data, bool isAnomaly);
	void pushData(vector<double> input_data);
private:
	MatrixXd transformData(vector<double> input_data);

	//vars
	vector<MatrixXd> vData;
	int dSize = 0, dNum = 0;
	double anomaly_level = .01;
};

#endif