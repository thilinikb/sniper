#ifndef PERCEPTRON_BRANCH_PREDICTOR_H
#define PERCEPTRON_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
//#include <boost/scoped_array.hpp>
#include "simulator.h"
//#include "perceptron_perceptron_table.h"
//#include "perceptron_predictor.h"
#include <vector>
//#include<iostream>
//using namespace std;

class PerceptronBranchPredictor : public BranchPredictor
{
public:
   PerceptronBranchPredictor(String name, core_id_t core_id, UInt32 sizeH, UInt32 sizeT);
   ~PerceptronBranchPredictor();

   bool predict(IntPtr ip, IntPtr target);
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target);

private:
   std::vector<SInt8> ghr;
   //PerceptronPerceptronTable p_table;
   //std::vector<PerceptronPredictor<1024>> p_table;
   std::vector<std::vector<SInt32>> p_table;
   double theta;
   UInt32 table_size;
   UInt32 history_length;
   SInt32 sum;
};

#endif
