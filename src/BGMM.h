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
	BGMM(vector<vector<double>>& initial_data, int max_stored_data_points, int points_to_reconstruct);
	BGMM(string saved_state);
	~BGMM();
	double process_input(vector<double>& input_data);
	void process_feedback(vector<double>& input_data, bool isAnomaly);
	void push_data(vector<double>& input_data);
	void update_setting(bool new_normalized_kept_points);
	vector<vector<vector<double>>> matrix_to_vector();
	string save_state();
private:
	MatrixXd transform_data(vector<double>& input_data);

	//vars
	vector<MatrixXd> vData_;
	int dSize_ = 0, dNum_ = 0, maxSize_;
	const double anomaly_level_ = .1;
	bool normalized_kept_points_;
	int points_to_reconstruct_; // new feature, need to be added to setting helper
	int point_count_ = 0;
	MatrixXd qZ_;
    Dirichlet weights_;
    vector<GaussWish> clusters_;
};

#endif 