#include "AnomalyDetectionSystem.h"
#include "stacker.h"
#include "BGMM.h"
#include "stateHelper.h"
#include <sys/time.h>
#include<limits>
#include "setting_helper.h"

using namespace std::chrono;
using namespace std;

AnomalyDetectionSystem::AnomalyDetectionSystem(string file_name)
{
	if (file_name.empty()){
		anomaly_level_ = 0.05;
		derived_features_ = {"delta", "weekday", "time", "date", "month"};
		models_ = {"IFORESTS_RECENT"};
		prediction_delay_ = 1000;
		barriers_ = vector<vector<double>>();		
		max_stored_data_points_ = 1000;
		points_to_reconstruct_ = 100;
	} else {
		setting_helper helper;
		Setting user_setting = helper.parse_setting(file_name); 

		anomaly_level_ = user_setting.s_anomaly_level;
		derived_features_ = set<string>(user_setting.s_features.begin(), user_setting.s_features.end());
		models_ = user_setting.s_models;
		prediction_delay_ = user_setting.s_prediction_delay;
		barriers_ = user_setting.s_barriers;
		max_stored_data_points_ = user_setting.s_max_store;
		points_to_reconstruct_ = user_setting.s_points_to_reconstruct;

		cout << prediction_delay_ << endl;
		cout << points_to_reconstruct_ << endl;
	}	
}

AnomalyDetectionSystem::~AnomalyDetectionSystem()
{
	delete stacker_;
}

void AnomalyDetectionSystem::initalize_from_saved_state(string saved_state)
{
	stateHelper helper;
	// find the last of new line character, which marks the end of model data
	// if new line character is not found, then the stacker has not been initialized
	// when save_state() was called.

	size_t last_new_line = saved_state.find_last_of('\n');
	vector<vector<vector<double>>> feature_vec;

	if (last_new_line != string::npos){

		string modelStr = saved_state.substr(0, last_new_line);
		string featureStr = saved_state.substr(last_new_line + 1, saved_state.size() - last_new_line - 1);

		// convert the feature string into feature matrix
		feature_vec = helper.string_to_matrices(featureStr);

		// covert the model list back to string and pass to stacker
		modelStr = helper.matrices_to_string(vector<vector<vector<double>>>(1, feature_vec[1])) + modelStr;
		stacker_ = new stacker(modelStr);
	} else {
		feature_vec = helper.string_to_matrices(saved_state);
	}

	// initiate derived features
	derived_features_.clear();
	for (double feat_id : feature_vec[0][0]){
		if (feat_id == 0.0) derived_features_.insert("delta");
		else if (feat_id == 1.0) derived_features_.insert("weekday");
		else if (feat_id == 2.0) derived_features_.insert("time");
		else if (feat_id == 3.0) derived_features_.insert("date");
		else if (feat_id == 4.0) derived_features_.insert("month");
	}

	// initiate models
	models_.clear();
	for (double model_id : feature_vec[1][0]){
		if (model_id == 0.0) models_.push_back("BGMM_REPRESENTATIVE");
		else if (model_id == 1.0) models_.push_back("BGMM_RECENT");
		else if (model_id == 2.0) models_.push_back("IFORESTS_REPRESENTATIVE");
		else if (model_id == 3.0) models_.push_back("IFORESTS_RECENT");
	}

	// initiate barriers
	barriers_ = feature_vec[2];

	// initiate initial data
	initial_data_ = feature_vec[3];

	// initiate last data
	last_data_ = feature_vec[4][0];

	// initiate various settings
	prediction_delay_ = feature_vec[5][0][0];
	max_stored_data_points_ = feature_vec[5][0][1];
	points_to_reconstruct_ = feature_vec[5][0][2];
	anomaly_level_ = feature_vec[5][0][3];
	
	// initiate last_tp_
	gettimeofday(&last_tp_, 0);
}

void AnomalyDetectionSystem::process_input(vector<double> data, anomaly_detected_call_func func)
{
	bool calledFunc = false;
	// cout << "before implied feat: " << data.size() << endl;
	add_implied_features(data);
	// cout << "after implied feat: " << data.size() << endl;
	vector<double> next_data = data;
	vector<double> model_results(models_.size(), numeric_limits<double>::quiet_NaN());

	if (ready_for_processing()){

		// cout << "before derived feat: " << data.size() << endl;
		add_derived_features(data);
		// cout << "after derived feat: " << data.size() << endl;

		if (stacker_){
			model_results = stacker_->process_input(data);

			double dataAnomalyConf = model_results.back();
			model_results.pop_back();

			if (dataAnomalyConf < anomaly_level_){
				calledFunc = true;
				func(model_results, data);
			}
		} else {
			initial_data_.push_back(data);
			if(initial_data_.size() == (size_t)prediction_delay_)
				stacker_ = new stacker(models_, initial_data_, max_stored_data_points_, points_to_reconstruct_);
		}
	}
	last_data_ = next_data;
	struct timeval tp;
	gettimeofday(&tp, 0);
	last_tp_ = tp;

	if (!calledFunc && violate_barriers(data)){
		func(model_results, data);
	}
}

void AnomalyDetectionSystem::process_feedback(vector<double> model_results, vector<double> data, bool isAnomaly)
{
	if (!stacker_) 
		return; // error maybe
	stacker_->process_feedback(model_results, data, isAnomaly);
}

void AnomalyDetectionSystem::add_derived_features(vector<double>& data)
{
	struct timeval tp;
	gettimeofday(&tp, 0);
	double timestamp = tp.tv_sec + tp.tv_usec / 1000.0 /1000.0;

	double lastStamp = last_tp_.tv_sec + last_tp_.tv_usec / 1000.0 / 1000.0;

	if (derived_features_.find("delta") != derived_features_.end()){
		int len = data.size();
		for (int i = 0; i < len; ++i){
			data.push_back(data[i] - last_data_[i]);
		}
	}
	data.push_back(timestamp - lastStamp);
}

void AnomalyDetectionSystem::add_implied_features(vector<double>& data)
{
	time_t rawtime;
	struct tm *ptm;

	time(&rawtime);
	ptm = gmtime(&rawtime);

	double wday = ptm->tm_wday; 
	double hour = ptm->tm_hour; // not a feature
	double min = hour * 60 + ptm->tm_min;
	double date = ptm->tm_mday;
	double month = ptm->tm_mon;

	

	if (derived_features_.find("weekday") != derived_features_.end())
		data.push_back(wday);
	if (derived_features_.find("time") != derived_features_.end())
		data.push_back(min);
	if (derived_features_.find("date") != derived_features_.end())
		data.push_back(date);
	if (derived_features_.find("month") != derived_features_.end())
		data.push_back(month);
}

bool AnomalyDetectionSystem::ready_for_processing()
{
	if (derived_features_.find("delta") != derived_features_.end()){
		if (last_data_.empty())
			return false;
	}
	return true;
}

bool AnomalyDetectionSystem::violate_barriers(vector<double>& data)
{
	for (size_t i = 0; i < barriers_.size(); ++i){
		double point = data[i];
		for (size_t j = 0; j < barriers_[i].size(); j += 2){
			if (point >= barriers_[i][j] && point <= barriers_[i][j + 1])
				return true;
		}
	}
	return false;
}

string AnomalyDetectionSystem::save_state()
{
	string res = "";

	//saving model states via stacker
	res += stacker_ ? stacker_->save_state() : "";

	// saving features
	vector<vector<vector<double>>> feature_vec;

	// saving derived features
	vector<double> derived_features_vec;
	for (string feat : derived_features_){
		if (feat == "delta") derived_features_vec.push_back(0.0);
		else if (feat == "weekday") derived_features_vec.push_back(1.0);
		else if (feat == "time") derived_features_vec.push_back(2.0);
		else if (feat == "date") derived_features_vec.push_back(3.0);
		else if (feat == "month") derived_features_vec.push_back(4.0);
	}
	vector<vector<double>> temp(1, derived_features_vec);
	feature_vec.push_back(temp);

	// saving models
	vector<double> model_vec;
	for (string model : models_){
		if (model == "BGMM_REPRESENTATIVE")
			model_vec.push_back(0.0);
		else if (model == "BGMM_RECENT")
			model_vec.push_back(1.0);
		else if (model == "IFORESTS_REPRESENTATIVE")
			model_vec.push_back(2.0);
		else if (model == "IFORESTS_RECENT")
			model_vec.push_back(3.0);
	}
	feature_vec.push_back(vector<vector<double>>(1, model_vec));

	//saving barriers

	feature_vec.push_back(barriers_);

	feature_vec.push_back(initial_data_);

	//saving last data
	feature_vec.push_back(vector<vector<double>>(1, last_data_));

	//saving various settings
	vector<double> setting_vec({(double)prediction_delay_, (double)max_stored_data_points_, (double)points_to_reconstruct_, anomaly_level_});
	feature_vec.push_back(vector<vector<double>>(1, setting_vec));
	
	// converting all to string
	stateHelper helper;
	res += helper.matrices_to_string(feature_vec);
	return res;
}
