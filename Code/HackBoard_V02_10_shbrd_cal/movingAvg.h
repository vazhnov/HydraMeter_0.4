#include "util.h"


#ifndef MOVINGAVG_H
#define MOVINGAVG_H

#define MOVINGAVG_SIZE 64 //MUST be a power of 2

class movingAvg{
	int32_t treeBase[MOVINGAVG_SIZE];
	int32_t sums[MOVINGAVG_SIZE];
	//sums goes sums[0] = treeBase[0] + treeBase[1] ... sums[size/2-1] = treeBase[size-2] + treeBase[size-1]
	//THEN sums[size/2] = sums[0] + sums[1] ... sums[size/2 + size/4] = sums[size/2-2] + sums[size/2-1]
	//and so on, until sums[size-2] is the total sum of everything
	//algorithm for this is, sum[n>>1 | 1<<(log(size)-1)] =  sum[n & 0xFFFE] + sum[n | 1];
	//this is REALLY efficient to loop with some bit hacking, which is why I chose this format
	
	uint32_t elemi = 0; //last element index/index of latest element
	#define MOVINGAVG_UPPER_BIT ((uint32_t)MOVINGAVG_SIZE>>1)
	movingAvg(){
		for(int i=0;i<MOVINGAVG_SIZE;i++){
			treeBase[i] = 0;
			sums[i] = 0;
		}
		
	}
	int32_t getSum(){
		return sums[MOVINGAVG_SIZE-2];
	}
	//add an element to the list, remove the oldest element, return the new sum
	int32_t addElement(int32_t in){
		elemi = (elemi + 1) % MOVINGAVG_SIZE;
		treeBase[elemi] = in;
		uint32_t i = elemi & 0xFFFFFE;
		//semi-magic bitshifting algorithm to figure out what indecies in sums to add together
		uint32_t next;
		while(i != (MOVINGAVG_SIZE-2)){
			next = (i>>1) | MOVINGAVG_UPPER_BIT; 
			sums[next] = sums[i] + sums[i+1];
			i = next;
		}
		return sums[next];
	}
	#undef MOVINGAVG_UPPER_BIT
};

class movingAvgf{
	public:
	float treeBase[MOVINGAVG_SIZE];
	float sums[MOVINGAVG_SIZE];
	//sums goes sums[0] = treeBase[0] + treeBase[1] ... sums[size/2-1] = treeBase[size-2] + treeBase[size-1]
	//THEN sums[size/2] = sums[0] + sums[1] ... sums[size/2 + size/4] = sums[size/2-2] + sums[size/2-1]
	//and so on, until sums[size-2] is the total sum of everything
	//algorithm for this is, sum[n>>1 | 1<<(log(size)-1)] =  sum[n & 0xFFFE] + sum[n | 1];
	//this is REALLY efficient to loop with some bit hacking, which is why I chose this format
	
	uint32_t elemi = 0; //last element index/index of latest element
	#define MOVINGAVG_UPPER_BIT ((uint32_t)MOVINGAVG_SIZE>>1)
	
	void clear();
	float getSum();
	float getAvg();
	float addElement(float in);
	movingAvgf(){
		clear();
	}
};
	
	void movingAvgf::clear(){
		for(int i=0;i<MOVINGAVG_SIZE;i++){
			treeBase[i] = 0;
			sums[i] = 0;
		}
	}
	
	float movingAvgf::getSum(){
		return sums[MOVINGAVG_SIZE-2];
	}
	
	float movingAvgf::getAvg(){
		return sums[MOVINGAVG_SIZE-2] / MOVINGAVG_SIZE;
	}
	
	//add an element to the list, remove the oldest element, return the new SUM (not avg)
	float movingAvgf::addElement(float in){
		elemi = (elemi + 1) % MOVINGAVG_SIZE;
		treeBase[elemi] = in;
		//semi-magic bitshifting algorithm to figure out what indecies in sums to add together
		uint32_t i = (elemi & 0xFFFFFE)/2;
		sums[i] = treeBase[elemi & 0xFFFFFE] + treeBase[elemi | 1];
		i = i & 0xFFFFFE;
		uint32_t next=0;
		while(i != (MOVINGAVG_SIZE-2)){
			next = ((i>>1) | MOVINGAVG_UPPER_BIT); 
			
			if(next <= 0 || next>=MOVINGAVG_SIZE || i>=MOVINGAVG_SIZE){
				Serial.printf("ERROR:OOB %x, %x\n",next,i);
			}else{
				sums[next] = sums[i] + sums[i+1];
				i = next & 0xFFFFFE;
			}
		}
		return sums[next];
	}
	#undef MOVINGAVG_UPPER_BIT
	

#endif