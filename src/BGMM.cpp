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

	double best_loglikelihood = -DBL_MAX;
	int best_cluster_idx = -1;

	for (size_t i = 0; i < clusters.size(); ++i){
		double log_likelihood = clusters[i].Eloglike(transformed_input)[0];
		if (log_likelihood > best_loglikelihood) {
			best_loglikelihood = log_likelihood;
			best_cluster_idx = i;
		}
	}
	
	pushData(input_data);

	double max_likelihood = exp(best_loglikelihood);
	double best_ll_samples = qZ.col(best_cluster_idx).sum();
	double likelihood = max_likelihood;

	if (best_ll_samples * num_clusters < anomaly_level * vData.size()) {
		likelihood = (best_ll_samples * num_clusters) / vData.size();
	}

	// cout << "best ll samples: " << best_ll_samples << endl;
	// cout << "likelihood: " << likelihood << endl;
	// cout << "num_clusters: " << num_clusters << endl;
	// cout << "num samples: " << vData.size() << endl;
	// cout << "best log likelihood: " << best_loglikelihood << endl; 

	if (likelihood > 0.999) 
		return 25.0;
	else if (likelihood < 0.001)
		return -25.0;
	return log(likelihood/(1-likelihood));
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

// void BGMM::printWeight(){
// 	cout << endl << weights.Elogweight().exp().transpose() << endl;
// }