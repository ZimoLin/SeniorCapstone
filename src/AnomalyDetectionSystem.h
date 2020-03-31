#ifndef ANOMALYDETECTIONSYSTEM_H_
#define ANOMALYDETECTIONSYSTEM_H_

#include <vector>
#include <set>
#include "stacker.h"
#include <sys/time.h>

class AnomalyDetectionSystem
{
public:

    /* 
     * The main reporting mechanism in the library - when an anomaly is detected per process_input,
     * these type of functions are called by ADS where model_results are internal underlying model
     * values (neeeded for process_feedback) and data is a feature-expanded representation of the
     * corresponding data.
     */
	typedef void (*anomaly_detected_call_func)(vector<double> model_results, vector<double> data);

    /*
     * Constructs an anomaly detection system using the settings provided by the file named by
     * file_name. Note that file_name is optional and that default settings will be used if not
     * provided.
     */
	AnomalyDetectionSystem(string file_name = "");


    /*
     * Note that AnomalyDetectionSystem does allocate memory, so the destructor is non-empty.
     */
	~AnomalyDetectionSystem();
	

    /*
     * The anomaly detection system processes the next provided data vector. Note that per temporal
     * information, only one sample can be processed at a time. In lieu of returning a result,
     * the provided anomaly_detected_call_func func will be called with the model information
     * and the processed data vector if and only if an anomaly is detected. 
     */
	void process_input(vector<double> data, anomaly_detected_call_func func);


    /*
     * After process_input has used its provided anomaly_detected_call_func to indicate an anamoly,
     * process_feedback can be called with the provided model_results and data to the 
     * anomaly_detected_call_func once the user has indicated that the underlying data either is
     * or is not anamalous. In cases where model_results are null, one cannot call process_feedback.
     */
	void process_feedback(vector<double> model_results, vector<double> data, bool isAnomaly);

    void initalize_from_saved_state(string saved_state);
    
    string save_state();

private:
	stacker *stacker_;
	
	set<string> derived_features_;

	vector<string> models_;
	vector<vector<double>> barriers_;
	vector<vector<double>> initial_data_;
	vector<double> last_data_;

	int prediction_delay_, max_stored_data_points_, points_to_reconstruct_;
	double anomaly_level_;
	struct timeval last_tp_;

	bool ready_for_processing();
	void add_derived_features(vector<double>& data);
	void add_implied_features(vector<double>& data);
	bool violate_barriers(vector<double>& data);
};

#endif
