#include "BGMM.h"
#include "libcluster.h"
#include "distributions.h"
#include "stateHelper.h"
#include <iostream>
#include "float.h"
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include <fstream>

using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace distributions;



BGMM::BGMM(vector<vector<double>>& initial_data, int max_stored_data_points, int points_to_reconstruct) : model(initial_data, max_stored_data_points, points_to_reconstruct)
{
        maxSize_ = max_stored_data_points;
        normalized_kept_points_ = true;
        points_to_reconstruct_ = points_to_reconstruct;

        for (vector<double> data : initial_data)
                push_data(data);

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        srand(seed);
}

BGMM::BGMM(string saved_state) : model(saved_state)
{
    vData_.clear();
    stateHelper helper;
    vector<vector<vector<double>>> data = helper.string_to_matrices(saved_state);
    // data[0][0] stores the vector<double> that holds the settings
    dSize_ = (int)data[0][0][0], dNum_ = (int)data[0][0][1], maxSize_ = (int)data[0][0][2];
    normalized_kept_points_ = data[0][0][3] == 1.0 ? true : false;
    points_to_reconstruct_ = (int)data[0][0][4], point_count_ = (int)data[0][0][5];
    for (size_t i = 1; i < data.size(); ++i){
        for (size_t j = 0; j < data[i].size(); ++j)
            push_data(data[i][j]);
    }
    // to reset qZ, weights, and clusters
     MatrixXd curData;
     curData.setZero(vData_.size(), dSize_);
     for (size_t i = 0; i < vData_.size(); ++i)
        curData.block(i, 0, 1, dSize_) = vData_[i];
    learnBGMM(curData, qZ_, weights_, clusters_, PRIORVAL, -1, false, omp_get_max_threads());
}

BGMM::~BGMM(){

}

double BGMM::process_input(vector<double>& input_data){
        push_data(input_data);

        MatrixXd curData;
        curData.setZero(vData_.size(), dSize_);

        for (size_t i = 0; i < vData_.size(); ++i)
                curData.block(i, 0, 1, dSize_) = vData_[i];

        if (point_count_ == 0) {
            learnBGMM(curData, qZ_, weights_, clusters_, PRIORVAL, -1, false, omp_get_max_threads());
            point_count_ = points_to_reconstruct_;
        }
        point_count_--;

        MatrixXd transformed_input = transform_data(input_data);
        int num_clusters = clusters_.size();

        double new_best_likelihood = 0;

        for (size_t i = 0; i < clusters_.size(); ++i){
            double cluster_samples = qZ_.col(i).sum();
                double new_cluster_likelihood = exp((clusters_[i].Eloglike(transformed_input)[0]));
                double anomalous_cluster_measure = (cluster_samples * num_clusters) / vData_.size() / anomaly_level_;
                if (anomalous_cluster_measure < new_cluster_likelihood) {
                    new_cluster_likelihood = anomalous_cluster_measure;
                }
                if (new_cluster_likelihood > new_best_likelihood) {
                        new_best_likelihood = new_cluster_likelihood;
                }
        }

        double worse_values = 0;
        for (size_t j = 0; j < vData_.size(); ++j) {
            double curr_best_likelihood = 0;
            for (size_t i = 0; i < clusters_.size(); ++i){
                double cluster_samples = qZ_.col(i).sum();
                double curr_cluster_likelihood = exp((clusters_[i].Eloglike(vData_[j])[0]));
                double anomalous_cluster_measure = (cluster_samples * num_clusters) / vData_.size() / anomaly_level_;
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

        double proportional_likelihood = worse_values / vData_.size();

        if (proportional_likelihood > 0.999) 
                return 25.0;
        else if (proportional_likelihood < 0.001)
                return -25.0;
        return log(proportional_likelihood/(1-proportional_likelihood));
}

void BGMM::process_feedback(vector<double>& input_data, bool isAnomaly){
        (void) input_data;
        (void) isAnomaly;
        return;
}

void BGMM::push_data(vector<double>& input_data){
    
    if (dSize_ == 0)
            dSize_ = (int)input_data.size();
    else if ((int)input_data.size() != dSize_)
        throw "Input data has wrong dimensionality";
    
    ++dNum_;

    if ((int)vData_.size() == maxSize_) {
        if (normalized_kept_points_) {
            double add_odds = ((double) maxSize_) / dNum_;
            double temp = ((double)rand() / (RAND_MAX));
            if (temp < add_odds) {
                int index_to_drop = rand() % maxSize_;
                vData_[index_to_drop] = transform_data(input_data);
            }
        } else {
            vData_.erase(vData_.begin());
            vData_.push_back(transform_data(input_data));
        }
    } else {
        vData_.push_back(transform_data(input_data));
    }
}


//transform data from vector<double> to MatrixXd

MatrixXd BGMM::transform_data(vector<double>& input_data){
        MatrixXd res(1, dSize_);
        RowVectorXd rowVector(dSize_);

        for (int i = 0; i < dSize_; i++)
                rowVector(i) = input_data[i];
        res.row(0) << rowVector;

        return res;
}

void BGMM::update_setting(bool new_normalized_kept_points)
{
    normalized_kept_points_ = new_normalized_kept_points;
}

vector<vector<vector<double>>> BGMM::matrix_to_vector()
{
    vector<vector<vector<double>>> res;
    // push settings into the first vector<vector<double>>, then push into res
    vector<double> settingVec({(double)dSize_, (double)dNum_, (double)maxSize_});
    settingVec.push_back(normalized_kept_points_? 1.0 : 0.0);
    settingVec.push_back((double)points_to_reconstruct_);
    settingVec.push_back((double)point_count_);
    vector<vector<double>> temp(1, settingVec);
    res.push_back(temp);

    for (size_t i = 0; i < vData_.size(); ++i){
        MatrixXd cur = vData_[i];
        vector<double> temp;
        for (int j = 0; j < dSize_; ++j)
            temp.push_back(cur.row(0)(j));
        vector<vector<double>> temp1(1, temp);
        res.push_back(temp1);
    }
    return res;
}

string BGMM::save_state()
{
    stateHelper helper;
    return helper.matrices_to_string(matrix_to_vector());
}