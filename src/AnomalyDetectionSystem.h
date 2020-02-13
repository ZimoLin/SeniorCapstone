#ifndef ANOMALYDETECTIONSYSTEM_H_
#define ANOMALYDETECTIONSYSTEM_H_

#include <vector>
#include <set>
#include "stacker.h"

class AnomalyDetectionSystem
{
public:
	typedef void (*anomaly_detected_call_func)(vector<double> model_results, vector<double> data);

	AnomalyDetectionSystem(string filePath = "");
	~AnomalyDetectionSystem();
	
	void process_input(vector<double> data, anomaly_detected_call_func func);
	void process_feedback(vector<double> model_results, vector<double> data, bool isAnomaly);

private:
	stacker *stacker_;
	double anomalyLevel_;
	set<string> derivedFeatures_;
	vector<string> models_;
	vector<vector<double>> Barriers_;
	vector<vector<double>> initialData_;
	vector<double> last_data_;
	int predictionDelay_;

	bool readyForProcessing();
	void add_derived_features(vector<double>& data);
	void add_implied_features(vector<double>& data);
	bool violate_barriers(vector<double>& data);

};

#endif
