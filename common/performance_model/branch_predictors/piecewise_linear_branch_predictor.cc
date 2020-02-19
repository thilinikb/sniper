#include "simulator.h"
#include "piecewise_linear_branch_predictor.h"
#include <vector>

PiecewiseLinearBranchPredictor::PiecewiseLinearBranchPredictor(String name, core_id_t core_id,  UInt32 IN_n, UInt32 IN_h, UInt32 IN_m)
   : BranchPredictor(name, core_id)
   , ghr(IN_h)
   , GA(IN_h)
   , W(IN_n,std::vector<std::vector<SInt8>>(IN_m,std::vector<SInt8>(IN_h+1)))
   , SR(IN_n,std::vector<SInt16>(IN_h))
   , R(IN_n,std::vector<SInt16>(IN_h))
{
	n=IN_n;
	m=IN_m;
	h=IN_h;
	theta=2.14*(h+1) + 20.58;

	
}

PiecewiseLinearBranchPredictor::~PiecewiseLinearBranchPredictor()
{
}

bool PiecewiseLinearBranchPredictor::predict(IntPtr ip, IntPtr target)
{
   UInt32 i = ip % n;
   UInt32 j = ip % m;
   output = SR[i][h-1] + W[i][j][0];

   bool predict=(output>=0);
   
   /*Updating speculative state SR , partial sum*/
   std::vector<SInt16> SR_1(h);
   for(UInt32 a=0;a <n;a++){
       for(UInt32 b=2;b <=h;b++){
	    UInt32 a_k=h-b;
	   if(predict){
		   SR_1[a_k+1]=SR[a][a_k]+ W[a][j][b-2];
	   }else{
		   SR_1[a_k+1]=SR[a][a_k]- W[a][j][b-2];
	   }  
       }
       SR[a]=SR_1;
   }
   for(UInt32 k=0;k<n;k++){
	   SR[k][0]=0;
   }

   return predict;
}

void PiecewiseLinearBranchPredictor::update(bool predicted, bool actual, IntPtr ip, IntPtr target)
{
   updateCounters(predicted, actual);

   /*Updating speculative state R , partial sum*/
   UInt32 c = ip %m;
   std::vector<SInt16> R_1(h);
   for(UInt32 a=0;a <n;a++){
       
       for(UInt32 b=2;b <=h;b++){
	       UInt32 a_k= h-b;
	   if(actual){
		   R_1[a_k+1]=R[a][a_k]+ W[a][c][b-2];
	   }else{
		   R_1[a_k+1]=R[a][a_k]- W[a][c][b-2];
	   }  
       }
       R[a]=R_1;
   }
   for(UInt32 k=0;k<n;k++){
	   R[k][0]=0;
   }

   /*Misprediction recovery*/
   if(predicted != actual){
	   SR=R;
   }
   
   /*Updating weight 3D vector*/
   UInt32 addr=ip%n;


   if(abs(output)<theta || predicted != actual){
	   if (actual){
		   W[addr][0][0] += 1;  
	   }else{
		   W[addr][0][0] -= 1;
	   }

	   for(UInt32 j=0 ; j<h; j++){
		if(ghr[j]==actual){
		  W[addr][GA[j]][j]  += 1;
	   	}else{
		  W[addr][GA[j]][j]  -= 1;
	   	}

	   }
   }



   /*Shifting values of address and global history*/
   for(UInt32 k=(ghr.size()-1);k>0;k--){
		   ghr[k]=ghr[k-1];
   }
   for(UInt32 l=(GA.size()-1);l>0;l--){
		   GA[l]=GA[l-1];
   }
   /*Updating address and global history with most recent values*/
   ghr[0]= actual;
   GA[0]= ip%m;


}
