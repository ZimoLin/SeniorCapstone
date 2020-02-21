#include "BGMM.h"
#include <random>
#include <vector>
#include <chrono>

using namespace std;

int main()
{
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator (seed);
  normal_distribution<double> distribution1 (0.0,1.0);  
  vector<double> input_data(2);
  vector<vector<double>> initial_data;

  for (int i = 0; i < 500; ++i)
    initial_data.push_back(vector<double>({distribution1(generator), distribution1(generator)}));


  BGMM test(initial_data);

  for (int i = 0; i < 100; ++i){
    input_data[0] = distribution1(generator);
    input_data[1] = distribution1(generator);
    test.process_input(input_data);
  }

  // normal_distribution<double> distribution2 (10.0,1.0); 

  // for (int i = 0; i < 3; ++i){
  //   input_data[0] = distribution2(generator);
  //   test.process_input(input_data);
  // }

  // for (int i = 0; i < 20; ++i){
  //   input_data[0] = distribution1(generator);
  //   test.process_input(input_data);
  // }



  return 0;
}