#include "BGMM.h"
#include "libcluster.h"
#include "distributions.h"
#include <iostream>
#include "float.h"

using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace distributions;

BGMM::BGMM(){

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
	cout << num_clusters << endl;

	double best_loglikelihood = -DBL_MIN;
	int best_ll_samples = 0;
	for (GaussWish cluster : clusters){
		double log_likelihood = cluster.Eloglike(transformed_input)[0];
		if (log_likelihood > best_loglikelihood) {
			best_loglikelihood = log_likelihood;
			best_ll_samples = cluster.get_num_samples();
		}
	}
	pushData(input_data);

	double max_likelihood = exp(best_loglikelihood);
	if (best_ll_samples * num_clusters < anomaly_level * vData.size()) {
		return (double) (best_ll_samples * num_clusters) / (double) (vData.size());
	}
	return max_likelihood;
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

void BGMM::printData(){
	cout << vData[0] << endl;
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