#ifndef BC_GSKEW_BRANCH_PREDICTOR_H
#define BC_GSKEW_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include "simple_bimodal_table.h"
#include <vector>

class bcGskewBranchPredictor : public BranchPredictor
{
public:
   bcGskewBranchPredictor(String name, core_id_t core_id, UInt32 size);
   ~bcGskewBranchPredictor();

   bool predict(IntPtr ip, IntPtr target);
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target);

private:
   SimpleBimodalTable bim;
   SimpleBimodalTable g0;
   SimpleBimodalTable g1;
   SimpleBimodalTable meta;
   UInt32 sizeT;
   UInt32 ghr;
   UInt32 m_mask;
   UInt32 nbits;
};

#endif
