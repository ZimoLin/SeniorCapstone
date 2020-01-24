#include "libcluster.h"
#include "distributions.h"
#include "testdata.h"


//
// Namespaces
//

using namespace std;
using namespace Eigen;
using namespace libcluster;
using namespace distributions;


//
// Functions
//

// Main
int main()
{

  // Populate test data from testdata.h
  MatrixXd Xcat;
  vMatrixXd X;
  makeXdata(Xcat, X);

  // Set up the inputs for the BGMM
  Dirichlet weights;
  vector<GaussWish> clusters;
  MatrixXd qZ;

  // Learn the BGMM
  double F = learnBGMM(Xcat, qZ, weights, clusters, PRIORVAL, true);

  // Print the posterior parameters
  cout << endl << "Cluster Weights:" << endl;
  cout << weights.Elogweight().exp().transpose() << endl;

  cout << endl << "Cluster means:" << endl;
  for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k)
    cout << k->getmean() << endl;

  cout << endl << "Cluster covariances:" << endl;
  for (vector<GaussWish>::iterator k=clusters.begin(); k < clusters.end(); ++k)
    cout << k->getcov() << endl << endl;

  return 0;
}