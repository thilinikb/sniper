#include "simulator.h"
#include "perceptron_branch_predictor.h"

PerceptronBranchPredictor::PerceptronBranchPredictor(String name, core_id_t core_id, UInt32 sizeH, UInt32 sizeT)
   : BranchPredictor(name, core_id)
   , ghr(sizeH)
   , p_table(sizeT,std::vector<SInt32>(sizeH))
{
	theta=1.93*sizeH+14;
	table_size=sizeT;
	history_length=sizeH;
}

PerceptronBranchPredictor::~PerceptronBranchPredictor()
{
}

bool PerceptronBranchPredictor::predict(IntPtr ip, IntPtr target)
{
   UInt32 index = ip % table_size;
   sum =0;
      for(UInt32 i=1; i<=history_length ; i++){
	      sum +=  p_table[index][i]*ghr[i-1];
      }

      sum += p_table[index][0];
      return (sum >=0);
}

void PerceptronBranchPredictor::update(bool predicted, bool actual, IntPtr ip, IntPtr target)
{
   updateCounters(predicted, actual);
   UInt32 index = ip % table_size;
   
   if((abs(sum)<= theta) || (predicted!=actual)){
	      for(UInt32 j=1; j<= history_length; j++){
		 if(actual){
	      		p_table[index][j] += (1)*ghr[j-1];
		 }else {
			p_table[index][j] += (-1)*ghr[j-1];
		 }
		   	      }
	if(actual){
	  	p_table[index][0] += (1);
	}else {
		p_table[index][0] += (-1);
	}

      }

  /*Shift global histroy register*/
   for(UInt32 i=(history_length -1);i>0;i--){
		   ghr[i]=ghr[i-1];
		   
	   }

   if(actual){
	   ghr[0]= 1;
   }else {
	   ghr[0]= -1;
   } 
  }
