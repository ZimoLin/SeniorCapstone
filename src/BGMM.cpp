#include "BGMM.h"
#include "libcluster.h"
#include "distributions.h"
#include <iostream>
#include "float.h"

using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace distributions;

BGMM::BGMM(vector<vector<double>> initial_data) : model(initial_data)
{
        for (vector<double> data : initial_data)
                pushData(data);
}

BGMM::~BGMM(){

}

double BGMM::process_input(vector<double> input_data){
        MatrixXd curData;
        curData.setZero(dNum, dSize);

        for (int i = 0; i < dNum; ++i)
                curData.block(i, 0, 1, dSize) = vData[i];
        MatrixXd qZ;
        Dirichlet weights;
        vector<GaussWish> clusters;
        learnBGMM(curData, qZ, weights, clusters, PRIORVAL, -1, false, omp_get_max_threads());

        MatrixXd transformed_input = transformData(input_data);
        int num_clusters = clusters.size();

        double best_likelihood = 0;

        for (size_t i = 0; i < clusters.size(); ++i){
                double cluster_likelihood = clusters[i].normalized_likelihood(transformed_input)[0];
                double cluster_samples = qZ.col(i).sum();
                double anomalous_cluster_measure = (cluster_samples * num_clusters) / vData.size() / anomaly_level;
                if (anomalous_cluster_measure < cluster_likelihood) {
                    cluster_likelihood = anomalous_cluster_measure;
                }
                if (cluster_likelihood > best_likelihood) {
                        best_likelihood = cluster_likelihood;
                }
        }
        
        pushData(input_data);

        if (best_likelihood > 0.999) 
                return 25.0;
        else if (best_likelihood < 0.001)
                return -25.0;
        return log(best_likelihood/(1-best_likelihood));
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

        vData.push_back(transformData(input_data));
        ++dNum;
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