#ifndef ANOMALYDETECTIONSYSTEM_H_
#define ANOMALYDETECTIONSYSTEM_H_

#include <vector>
#include <set>
#include "stacker.h"
#include <sys/time.h>

class AnomalyDetectionSystem
{
public:
	typedef void (*anomaly_detected_call_func)(vector<double> model_results, vector<double> data);

	AnomalyDetectionSystem(string file_name = "");
	~AnomalyDetectionSystem();
	
	void process_input(vector<double> data, anomaly_detected_call_func func);
	void process_feedback(vector<double> model_results, vector<double> data, bool isAnomaly);

private:
	stacker *stacker_;
	
	set<string> derived_features_;

	vector<string> models_;
	vector<vector<double>> barriers_;
	vector<vector<double>> initial_data_;
	vector<double> last_data_;

	int prediction_delay_, max_stored_data_points_;
	double anomaly_level_;
	struct timeval last_tp_;

	bool ready_for_processing();
	void add_derived_features(vector<double>& data);
	void add_implied_features(vector<double>& data);
	bool violate_barriers(vector<double>& data);
};

#endif
