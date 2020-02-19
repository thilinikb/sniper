#include "simulator.h"
#include "branch_predictor.h"
#include "one_bit_branch_predictor.h"
#include "2bc_gskew_branch_predictor.h"
#include "perceptron_branch_predictor.h"
#include "pentium_m_branch_predictor.h"
#include "piecewise_linear_branch_predictor.h"
#include "config.hpp"
#include "stats.h"

BranchPredictor::BranchPredictor()
   : m_correct_predictions(0)
   , m_incorrect_predictions(0)
{
}

BranchPredictor::BranchPredictor(String name, core_id_t core_id)
   : m_correct_predictions(0)
   , m_incorrect_predictions(0)
{
  registerStatsMetric(name, core_id, "num-correct", &m_correct_predictions);
  registerStatsMetric(name, core_id, "num-incorrect", &m_incorrect_predictions);
}

BranchPredictor::~BranchPredictor()
{ }

UInt64 BranchPredictor::m_mispredict_penalty;

BranchPredictor* BranchPredictor::create(core_id_t core_id)
{
   try
   {
      config::Config *cfg = Sim()->getCfg();
      assert(cfg);

      m_mispredict_penalty = cfg->getIntArray("perf_model/branch_predictor/mispredict_penalty", core_id);

      String type = cfg->getStringArray("perf_model/branch_predictor/type", core_id);
      if (type == "none")
      {
         return 0;
      }
      else if (type == "one_bit")
      {
         UInt32 size = cfg->getIntArray("perf_model/branch_predictor/size", core_id);
         return new OneBitBranchPredictor("branch_predictor", core_id, size);
      } 
      else if (type == "2bc_gskew")
      {
         UInt32 size = cfg->getIntArray("perf_model/branch_predictor/size", core_id);
         return new bcGskewBranchPredictor("branch_predictor", core_id, size);
      }

       else if (type == "perceptron")
      {
         UInt32 sizeH = cfg->getIntArray("perf_model/branch_predictor/sizeH", core_id);
	 UInt32 sizeT = cfg->getIntArray("perf_model/branch_predictor/sizeT", core_id);
	 return new PerceptronBranchPredictor("branch_predictor", core_id, sizeH,sizeT);
      }
       else if (type == "piecewise_linear")
      {
         UInt32 IN_n = cfg->getIntArray("perf_model/branch_predictor/n", core_id);
	 UInt32 IN_m = cfg->getIntArray("perf_model/branch_predictor/m", core_id);
	 UInt32 IN_h = cfg->getIntArray("perf_model/branch_predictor/h", core_id);
	 return new PiecewiseLinearBranchPredictor("branch_predictor", core_id,IN_n,IN_h,IN_m);
      }

      else if (type == "pentium_m")
      {
         return new PentiumMBranchPredictor("branch_predictor", core_id);
      }
      else
      {
         LOG_PRINT_ERROR("Invalid branch predictor type.");
         return 0;
      }
   }
   catch (...)
   {
      LOG_PRINT_ERROR("Config info not available while constructing branch predictor.");
      return 0;
   }
}

UInt64 BranchPredictor::getMispredictPenalty()
{
   return m_mispredict_penalty;
}

void BranchPredictor::resetCounters()
{
  m_correct_predictions = 0;
  m_incorrect_predictions = 0;
}

void BranchPredictor::updateCounters(bool predicted, bool actual)
{
   if (predicted == actual)
      ++m_correct_predictions;
   else
      ++m_incorrect_predictions;
}
