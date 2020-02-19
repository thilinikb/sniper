#ifndef PIECEWISE_LINEAR_BRANCH_PREDICTOR_H
#define PIECEWISE_LINEAR_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include "simulator.h"
#include <vector>

class PiecewiseLinearBranchPredictor : public BranchPredictor
{
public:
   PiecewiseLinearBranchPredictor(String name, core_id_t core_id, UInt32 IN_n, UInt32 IN_h, UInt32 IN_m);
   ~PiecewiseLinearBranchPredictor();

   bool predict(IntPtr ip, IntPtr target);
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target);

private:
   std::vector<bool> ghr;
   std::vector<UInt8> GA;  // needs at most log2n+1 bits
   std::vector<std::vector<std::vector<SInt8>>> W;
   std::vector<std::vector<SInt16>> SR; // Ideally this should be 10/12 bits
   std::vector<std::vector<SInt16>> R;
   
   double theta;
   UInt32 n; 
   UInt32 m;
   UInt32 h;
   SInt32 output;
};

#endif
