#include "simulator.h"
#include "2bc_gskew_branch_predictor.h"
#include <math.h> 

bcGskewBranchPredictor::bcGskewBranchPredictor(String name, core_id_t core_id, UInt32 size)
   : BranchPredictor(name, core_id)
   , bim(size)
   , g0(size)
   , g1(size)
   , meta(size)
{
	sizeT=size;
	nbits= log2(sizeT);
	m_mask = 0;
      for (unsigned int i = 0 ; i < nbits ; i++)
      {
         m_mask |= (1L<<i);
      }

}

bcGskewBranchPredictor::~bcGskewBranchPredictor()
{
}

bool bcGskewBranchPredictor::predict(IntPtr ip, IntPtr target)
{
   /*Implementaing the hashing functions*/
   UInt32 nthbit = (ip&m_mask) >> (nbits-1);
   UInt32 zthbit = (ip&m_mask) & 1;
   UInt32 nxor0 = nthbit ^ zthbit;
   UInt32 h_v1 =((ip&m_mask) >>1)|(nxor0<< (nbits-1));

   UInt32 nthbit1 = (ghr&m_mask) >> (nbits-1);
   UInt32 shifted1 = (ghr&m_mask) << 1;
   UInt32 n1thbit1 = shifted1 >> (nbits-1);
   UInt32 hinv_v2 = shifted1 | (nthbit1 ^ n1thbit1); 

   UInt32 indexb = ip % sizeT;
   UInt32 indexg0 = (h_v1 & m_mask) ^ (hinv_v2 & m_mask) ^ (ghr & m_mask);
   UInt32 indexg1 = (h_v1 & m_mask) ^ (hinv_v2 & m_mask) ^ (ip & m_mask);
   UInt32 indexm = ((ip & m_mask) + (ghr & m_mask))&m_mask;

   bool predict1 = bim.predict(indexb,target);
   bool predict2 = g0.predict(indexg0,target);
   bool predict3 = g1.predict(indexg1,target);
   bool predict4 = meta.predict(indexm,target);

   bool predict5 = (predict1&predict2)|(predict2&predict3)|(predict1&predict3);
   if(predict4){
	   return predict5;
   }else{
	   return predict1;
   }
}

void bcGskewBranchPredictor::update(bool predicted, bool actual, IntPtr ip, IntPtr target)
{
   updateCounters(predicted, actual);
   UInt32 nthbit = (ip&m_mask) >> (nbits-1);
   UInt32 zthbit = (ip&m_mask) & 1;
   UInt32 nxor0 = nthbit ^ zthbit;
   UInt32 h_v1 =((ip&m_mask) >>1)|(nxor0 << (nbits-1));

   UInt32 nthbit1 = (ghr&m_mask) >> (nbits-1);
   UInt32 shifted1 = (ghr&m_mask) << 1;
   UInt32 n1thbit1 = shifted1 >> (nbits-1);
   UInt32 hinv_v2 = shifted1 | (nthbit1 ^ n1thbit1); 

   UInt32 indexb = ip % sizeT;
   UInt32 indexg0 = (h_v1 & m_mask) ^ (hinv_v2 & m_mask) ^ (ghr & m_mask);
   UInt32 indexg1 = (h_v1 & m_mask) ^ (hinv_v2 & m_mask) ^ (ip & m_mask);
   UInt32 indexm = ((ip & m_mask) + (ghr & m_mask))&m_mask;

   bool predict1 = bim.predict(indexb,target);
   bool predict2 = g0.predict(indexg0,target);
   bool predict3 = g1.predict(indexg1,target);
   bool predict4 = meta.predict(indexm,target);

   bool predict5 = (predict1&predict2)|(predict2&predict3)|(predict1&predict3);
  if(predict5 != predict4){
	  meta.update(predicted,actual,indexm,target);
  } 
   if(predicted == actual){
	 if(predict4){
	     if(predict1==actual){
		     bim.update(predicted,actual,indexm,target);
	     }
	     if(predict2==actual){
		     g0.update(predicted,actual,indexg0,target);
	     }
	     if(predict3==actual){
		     g1.update(predicted,actual,indexg1,target);
	     }
	     
  	 }else{
	     bim.update(predicted,actual,indexm,target);
	 }


   }else{
    bim.update(predicted,actual,indexb,target);
    g0.update(predicted,actual,indexg0,target);
    g1.update(predicted,actual,indexg1,target);
    }

   ghr =((ghr<<1) | (UInt32)actual);


}


