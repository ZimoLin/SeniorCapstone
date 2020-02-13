#include "AnomalyDetectionSystem.h"
#include "stacker.h"
#include "BGMM.h"
#include <sys/time.h>
#include<limits>

using namespace std::chrono;
using namespace std;

AnomalyDetectionSystem::AnomalyDetectionSystem(string filePath)
{
	(void) filePath;
	anomalyLevel_ = 0.05;
	derivedFeatures_ = {"delta", "time", "weekday", "date", "month"};
	models_ = {"BGMM"};
	predictionDelay_ = 5000;
}

AnomalyDetectionSystem::~AnomalyDetectionSystem()
{
	delete stacker_;
}

void AnomalyDetectionSystem::process_input(vector<double> data, anomaly_detected_call_func func)
{
	bool calledFunc = false;
	//add_implied_features(data);
	vector<double> next_data = data;
	vector<double> model_results(models_.size(), numeric_limits<double>::quiet_NaN());

	if (readyForProcessing()){
		add_derived_features(data);

		if (stacker_){
			model_results = stacker_->process_input(data);
			double dataAnomalyConf = model_results.back();
			model_results.pop_back();

			if (dataAnomalyConf < anomalyLevel_){
				calledFunc = true;
				func(model_results, data);
			}
		} else {
			initialData_.push_back(data);
			if(initialData_.size() == (size_t)predictionDelay_)
				stacker_ = new stacker(models_, initialData_);
		}
	}
	last_data_ = next_data;
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
	if (derivedFeatures_.find("delta") != derivedFeatures_.end()){
		int len = data.size();
		for (int i = 0; i < len; ++i){
			data.push_back(data[i] - last_data_[i]);
		}
	}

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

	struct timeval tp;
	gettimeofday(&tp, 0);
	double timestamp = tp.tv_sec * 1000 + tp.tv_usec / 1000;

	if (derivedFeatures_.find("weekday") != derivedFeatures_.end())
		data.push_back(wday);
	if (derivedFeatures_.find("time") != derivedFeatures_.end())
		data.push_back(min);
	if (derivedFeatures_.find("date") != derivedFeatures_.end())
		data.push_back(date);
	if (derivedFeatures_.find("month") != derivedFeatures_.end())
		data.push_back(month);

	data.push_back(timestamp);
}

bool AnomalyDetectionSystem::readyForProcessing()
{
	if (derivedFeatures_.find("delta") != derivedFeatures_.end()){
		if (last_data_.empty())
			return false;
	}
	return true;
}

bool AnomalyDetectionSystem::violate_barriers(vector<double>& data)
{
	for (size_t i = 0; i < Barriers_.size(); ++i){
		double point = data[i];
		for (size_t j = 0; j < Barriers_[i].size(); j += 2){
			if (point >= Barriers_[i][j] && point <= Barriers_[i][j + 1])
				return true;
		}
	}
	return false;
}