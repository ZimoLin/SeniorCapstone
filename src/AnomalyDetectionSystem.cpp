#include "AnomalyDetectionSystem.h"
#include "stacker.h"
#include "BGMM.h"
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
		models_ = {"IFORESTS"};
		prediction_delay_ = 1000;
		barriers_ = vector<vector<double>>();		
		max_stored_data_points_ = 1000;
	} else {
		setting_helper helper;
		Setting user_setting = helper.parse_setting(file_name); 

		anomaly_level_ = user_setting.s_anomaly_level;
		derived_features_ = set<string>(user_setting.s_features.begin(), user_setting.s_features.end());
		models_ = user_setting.s_models;
		prediction_delay_ = user_setting.s_prediction_delay;
		barriers_ = user_setting.s_barriers;
		max_stored_data_points_ = user_setting.s_max_store;

		cout << prediction_delay_ << endl;
	}	
}

AnomalyDetectionSystem::~AnomalyDetectionSystem()
{
	delete stacker_;
}

void AnomalyDetectionSystem::process_input(vector<double> data, anomaly_detected_call_func func)
{
	bool calledFunc = false;
	add_implied_features(data);
	vector<double> next_data = data;
	vector<double> model_results(models_.size(), numeric_limits<double>::quiet_NaN());

	if (ready_for_processing()){

		add_derived_features(data);

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
				stacker_ = new stacker(models_, initial_data_, max_stored_data_points_);
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

// void AnomalyDetectionSystem::initalize_from_saved_state(string saved_state)
// {
// 	//TODO
// }

// string AnomalyDetectionSystem::save_state()
// {
// 	//TODO
// }
