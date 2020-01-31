#include <Eigen/Dense>
#include "float.h"
#include <random>

using namespace std;
using namespace Eigen;

struct normal_random_variable
{
    normal_random_variable(Eigen::MatrixXd const& covar)
        : normal_random_variable(Eigen::VectorXd::Zero(covar.rows()), covar)
    {}

    normal_random_variable(Eigen::VectorXd const& mean, Eigen::MatrixXd const& covar)
        : mean(mean)
    {
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
        transform = eigenSolver.eigenvectors() * eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
    }

    Eigen::VectorXd mean;
    Eigen::MatrixXd transform;

    Eigen::VectorXd operator()() const
    {
        static std::mt19937 gen{ std::random_device{}() };
        static std::normal_distribution<> dist;

        return mean + transform * Eigen::VectorXd{ mean.size() }.unaryExpr([&](auto x) { (void)x; return dist(gen); });
    }
};


class BayesianLinearReg
{
public:
	BayesianLinearReg(VectorXd a_m0, MatrixXd m_S0, double beta);
	~BayesianLinearReg();

	
	VectorXd prediction_limit(MatrixXd& a_x, double stdevs);
    void set_posterior(MatrixXd& a_x, VectorXd& a_t);


private:
	
	normal_random_variable prior_;
	normal_random_variable posterior_;

	VectorXd v_m0_;
	VectorXd v_mN_;
	
	MatrixXd m_S0_; 
	MatrixXd m_SN_;

	double beta_;
	
	// MatrixXd get_phi(VectorXd& a_x);
};