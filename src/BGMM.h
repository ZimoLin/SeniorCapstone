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
	BGMM(vector<vector<double>> initial_data, int max_stored_data_points, int points_to_reconstruct);
	// BGMM(string saved_state);
	~BGMM();
	double process_input(vector<double> input_data);
	void process_feedback(vector<double> input_data, bool isAnomaly);
	void pushData(vector<double> input_data);
	void updateSetting(bool new_normalized_kept_points);
	// string save_state();
private:
	MatrixXd transformData(vector<double> input_data);

	//vars
	vector<MatrixXd> vData;
	int dSize = 0, dNum = 0, maxSize;
	const double anomaly_level = .1;
	bool normalized_kept_points;
	int points_to_reconstruct_;
	int point_count_ = 0;
	MatrixXd qZ_;
    Dirichlet weights_;
    vector<GaussWish> clusters_;
};

#endif 