#include "BGMM.h"
#include "libcluster.h"
#include "distributions.h"
#include <iostream>
#include "float.h"
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>

using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace distributions;



BGMM::BGMM(vector<vector<double>> initial_data, int max_stored_data_points, int points_to_reconstruct) : model(initial_data, max_stored_data_points, points_to_reconstruct)
{
        maxSize = max_stored_data_points;
        normalized_kept_points = true;
        points_to_reconstruct_ = points_to_reconstruct;

        for (vector<double> data : initial_data)
                pushData(data);

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        srand(seed);
}

// BGMM::BGMM(string saved_state) : model(saved_state)
// {
//     //TODO
// }

BGMM::~BGMM(){

}

double BGMM::process_input(vector<double> input_data){
        pushData(input_data);

        MatrixXd curData;
        curData.setZero(vData.size(), dSize);

        for (size_t i = 0; i < vData.size(); ++i)
                curData.block(i, 0, 1, dSize) = vData[i];

        if (point_count_ == 0) {
            learnBGMM(curData, qZ_, weights_, clusters_, PRIORVAL, -1, false, omp_get_max_threads());
            point_count_ = points_to_reconstruct_;
        }
        point_count_--;
        
        // learnBGMM(curData, qZ_, weights_, clusters_, PRIORVAL, -1, false, omp_get_max_threads());

        MatrixXd transformed_input = transformData(input_data);
        int num_clusters = clusters_.size();

        double new_best_likelihood = 0;

        for (size_t i = 0; i < clusters_.size(); ++i){
            double cluster_samples = qZ_.col(i).sum();
                double new_cluster_likelihood = exp((clusters_[i].Eloglike(transformed_input)[0]));
                double anomalous_cluster_measure = (cluster_samples * num_clusters) / vData.size() / anomaly_level;
                if (anomalous_cluster_measure < new_cluster_likelihood) {
                    new_cluster_likelihood = anomalous_cluster_measure;
                }
                if (new_cluster_likelihood > new_best_likelihood) {
                        new_best_likelihood = new_cluster_likelihood;
                }
        }

        double worse_values = 0;
        for (size_t j = 0; j < vData.size(); ++j) {
            double curr_best_likelihood = 0;
            for (size_t i = 0; i < clusters_.size(); ++i){
                double cluster_samples = qZ_.col(i).sum();
                double curr_cluster_likelihood = exp((clusters_[i].Eloglike(vData[j])[0]));
                double anomalous_cluster_measure = (cluster_samples * num_clusters) / vData.size() / anomaly_level;
                if (anomalous_cluster_measure < curr_cluster_likelihood) {
                    curr_cluster_likelihood = anomalous_cluster_measure;
                }
                if (curr_cluster_likelihood > curr_best_likelihood) {
                        curr_best_likelihood = curr_cluster_likelihood;
                }
            }
            if (new_best_likelihood > curr_best_likelihood) {
                worse_values++;
            }
        }

        double proportional_likelihood = worse_values / vData.size();

        if (proportional_likelihood > 0.999) 
                return 25.0;
        else if (proportional_likelihood < 0.001)
                return -25.0;
        return log(proportional_likelihood/(1-proportional_likelihood));
}

void BGMM::process_feedback(vector<double> input_data, bool isAnomaly){
        (void) input_data;
        (void) isAnomaly;
        return;
}

void BGMM::pushData(vector<double> input_data){
    
    if (dSize == 0)
            dSize = (int)input_data.size();
    else if ((int)input_data.size() != dSize)
        throw "Input data has wrong dimensionality";
    
    ++dNum;

    if ((int)vData.size() == maxSize) {
        if (normalized_kept_points) {
            double add_odds = ((double) maxSize) / dNum;
            double temp = ((double)rand() / (RAND_MAX));
            if (temp < add_odds) {
                int index_to_drop = rand() % maxSize;
                vData[index_to_drop] = transformData(input_data);
            }
        } else {
            vData.erase(vData.begin());
            vData.push_back(transformData(input_data));
        }
    } else {
        vData.push_back(transformData(input_data));
    }
}


//transform data from vector<double> to MatrixXd

MatrixXd BGMM::transformData(vector<double> input_data){
        MatrixXd res(1, dSize);
        RowVectorXd rowVector(dSize);

        for (int i = 0; i < dSize; i++)
                rowVector(i) = input_data[i];
        res.row(0) << rowVector;

        return res;
}

void BGMM::updateSetting(bool new_normalized_kept_points)
{
    normalized_kept_points = new_normalized_kept_points;
}

// string BGMM::save_state()
// {
//     //TODO
// }