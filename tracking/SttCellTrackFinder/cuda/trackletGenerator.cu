#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <set>
//to get Macro-Values
#include "../MacrosForGPUComputing.h"

#define cudaCheckError() { \
cudaError_t e=cudaGetLastError(); \
if(e!=cudaSuccess) { \
printf("Cuda failure %s:%d: '%s'\n",__FILE__,__LINE__,cudaGetErrorString(e)); \
exit(0); \
} \
}

using namespace std;

int GetNumThreads(int numSttHits){
  //heuristic minimum number of threads
  if(numSttHits<128)
    return 128;
  //maximum number of threads
  if(numSttHits>MAX_THREADS_PER_BLOCK)
    return 0;

  return ((numSttHits+WARP_SIZE-1)/WARP_SIZE)*WARP_SIZE;

}

__global__ void ExtractData(const int* dev_tubeNeighborings,
		const int* dev_sttHits, int numUnskewedHits, int numSkewedHits,
		int* dev_hitIndices, int* dev_hitNeighbors, int* dev_numHitNeighbors,
		int* dev_states) {

	int numSttHits = numUnskewedHits + numSkewedHits;
	if (threadIdx.x < numSttHits) {
		int tubeID = dev_sttHits[threadIdx.x];
		int tubeIndexNeighbors, tubeIndexHits;
		int curNeighbor, numHitNeighbors = 0;
		int maxNumNeighbors, shiftNeighborings, shiftHits;

		if (threadIdx.x >= numUnskewedHits) {
			//tube is skewed

			//relative index in tubeNeighborings + (offset)
			tubeIndexNeighbors = tubeID - START_TUBE_ID_SKEWED
					+ (NUM_UNSKEWED_STRAWS * MAX_UNSKEWED_NEIGHBORS);
			//realtive index in sttHits +(offset)
			tubeIndexHits = threadIdx.x - numUnskewedHits
					+ (numUnskewedHits * MAX_UNSKEWED_NEIGHBORS);

			maxNumNeighbors = MAX_SKEWED_NEIGHBORS;
			shiftNeighborings = NUM_SKEWED_STRAWS;
			shiftHits = numSkewedHits;

		} else {
			//tube is unskewed

			//relative index in tubeNeighborings + (offset)
			tubeIndexNeighbors = tubeID - 1;
			if (tubeID > END_TUBE_ID_SKEWED)
				tubeIndexNeighbors -= (END_TUBE_ID_SKEWED - START_TUBE_ID_SKEWED
						+ 1);
			//realtive index in sttHits +(offset)
			tubeIndexHits = threadIdx.x;

			maxNumNeighbors = MAX_UNSKEWED_NEIGHBORS;
			shiftNeighborings = NUM_UNSKEWED_STRAWS;
			shiftHits = numUnskewedHits;
		}

		for (int i = 0; i < maxNumNeighbors; ++i) {
			curNeighbor = dev_tubeNeighborings[i * shiftNeighborings
					+ tubeIndexNeighbors];
			if (dev_hitIndices[curNeighbor] != -1) {
				dev_hitNeighbors[numHitNeighbors * shiftHits + tubeIndexHits] =
						dev_hitIndices[curNeighbor];
				++numHitNeighbors;
			}
		}

		dev_numHitNeighbors[threadIdx.x] = numHitNeighbors;
//       printf("Extraxt data for thread %i, tubeID %i, hitNeighbors %i \n",threadIdx.x, dev_sttHits[threadIdx.x], numHitNeighbors);

		if (numHitNeighbors < 3) {
			dev_states[threadIdx.x] = tubeID;
		} else {
			dev_states[threadIdx.x] = NUM_STRAWS + 1;
		}
	}
}

__global__ void EvaluateStates(const int* dev_sttHits, int numUnskewedHits,
		int numSkewedHits, const int* dev_hitNeighbors,
		const int* dev_numHitNeighbors, int* dev_states) {

	__shared__ int sum[MAX_THREADS_PER_BLOCK];
	int numNewStatesFinal = 1;
	int numNewStatesLocal;
	int numLoops = 0;

	while (numNewStatesFinal != 0) {

		++numLoops;

		numNewStatesLocal = 0;

//     if(threadIdx.x==0) printf("start next loop\n");

		if (threadIdx.x < (numUnskewedHits + numSkewedHits)) {

			int numHitNeighbors = dev_numHitNeighbors[threadIdx.x];

			if (numHitNeighbors < 3) {

				int shiftHits, hitIndex;

				if (threadIdx.x < numUnskewedHits) {
					hitIndex = threadIdx.x;
					shiftHits = numUnskewedHits;
				} else {
					//realtive index in hitNeighbors + (offset)
					hitIndex = threadIdx.x - numUnskewedHits
							+ (numUnskewedHits * MAX_UNSKEWED_NEIGHBORS);
					shiftHits = numSkewedHits;
				}

				int min = dev_states[threadIdx.x];
				for (int i = 0; i < numHitNeighbors; ++i) {
					if (min
							> dev_states[dev_hitNeighbors[i * shiftHits
									+ hitIndex]]) {
						min = dev_states[dev_hitNeighbors[i * shiftHits
								+ hitIndex]];
						++numNewStatesLocal;
					}
				}

				dev_states[threadIdx.x] = min;
			}
		}

		sum[threadIdx.x] = numNewStatesLocal;
		__syncthreads();

		int offset = MAX_THREADS_PER_BLOCK / 2;
		while (offset != 0) {
			if (threadIdx.x < offset) {
				sum[threadIdx.x] += sum[threadIdx.x + offset];

			}
			__syncthreads();
			offset /= 2;
		}

		numNewStatesFinal = sum[0];

		//make sure each thread knows the new sum --> start next loop
		__syncthreads();

	} //end while

//	if (threadIdx.x == 0)
//		printf("EvaluateStates: #Loops %i\n", numLoops);

}

__device__ bool AddIndex(int* dev_multiStates, int numSttHits,
		int indexToInsert, int tid) {

	bool inserted = false;

	//space left?
	if (dev_multiStates[(MAX_MULTISTATE_NUM - 1) * numSttHits + tid] == 0) {
		for (int i = 0; i < MAX_MULTISTATE_NUM; ++i) {
			if (dev_multiStates[i * numSttHits + tid] == indexToInsert) {
				break;
			}
			if (dev_multiStates[i * numSttHits + tid] == 0) {
				dev_multiStates[i * numSttHits + tid] = indexToInsert;
				inserted = true;
				break;
			}
		}
	}

	return inserted;
}

__global__ void EvaluateMultistates(const int* dev_sttHits, int numUnskewedHits,
		int numSkewedHits, const int* dev_hitNeighbors,
		const int* dev_numHitNeighbors, int* dev_states, int* dev_multiStates) {

	__shared__ int sum[MAX_THREADS_PER_BLOCK];
	int numNewStatesFinal = 1;
	int numNewStatesLocal;
	int numLoops = 0;

	while (numNewStatesFinal != 0) {
		++numLoops;
		numNewStatesLocal = 0;

//     if(threadIdx.x==0) printf("start next loop\n");
		int numSttHits = (numUnskewedHits + numSkewedHits);
		if (threadIdx.x < numSttHits) {

			int numHitNeighbors = dev_numHitNeighbors[threadIdx.x];

			if (numHitNeighbors > 2) {

				int shiftHits, hitIndex;

				if (threadIdx.x < numUnskewedHits) {
					hitIndex = threadIdx.x;
					shiftHits = numUnskewedHits;
				} else {
					//realtive index in hitNeighbors + (offset)
					hitIndex = threadIdx.x - numUnskewedHits
							+ (numUnskewedHits * MAX_UNSKEWED_NEIGHBORS);
					shiftHits = numSkewedHits;
				}

				int curNeighborIndex;

				for (int i = 0; i < numHitNeighbors; ++i) {
					curNeighborIndex =
							dev_hitNeighbors[i * shiftHits + hitIndex];

					if (dev_states[curNeighborIndex]
							<= dev_sttHits[curNeighborIndex]) {
						//neighbor is unambiguous
						//copy index of neighbor
						if (AddIndex(dev_multiStates, numSttHits,
								curNeighborIndex, threadIdx.x)) {
							++numNewStatesLocal;
						}

					} else {
						//neighbor is ambiguous
						//copy multistate of neighbor
						for (int j = 0; j < MAX_MULTISTATE_NUM; ++j) {
							if (dev_multiStates[j * numSttHits
									+ curNeighborIndex] != 0) {
								if (AddIndex(dev_multiStates, numSttHits,
										dev_multiStates[j * numSttHits
												+ curNeighborIndex],
										threadIdx.x)) {
									++numNewStatesLocal;
								}
							} else {
								break;
							}
						}

					}
				}

			}
		}

		sum[threadIdx.x] = numNewStatesLocal;
		__syncthreads();

		int offset = MAX_THREADS_PER_BLOCK / 2;
		while (offset != 0) {
			if (threadIdx.x < offset) {
				sum[threadIdx.x] += sum[threadIdx.x + offset];

			}
			__syncthreads();
			offset /= 2;
		}

		numNewStatesFinal = sum[0];

//      if(threadIdx.x==0) printf("loop: num new states %i\n", numNewStatesFinal);

		//make sure each thread knows the new sum --> start next loop
		__syncthreads();

	}	    //end while

//	if (threadIdx.x == 0)
//		printf("EvaluateStates: #Loops %i\n", numLoops);

}

void PrintResults(const int* sttHits, int numSttHits, const  int *states){

  set<int> trackletStates;
  for(int i=0; i<numSttHits; ++i){
    if(states[i]!= NUM_STRAWS+1){
      trackletStates.insert(states[i]);
    }
  }
  cout<<endl;
  int stateSum=0;

  cout<<"StartTracklets:"<<endl;
  for(set<int>::iterator it=trackletStates.begin(); it!=trackletStates.end(); ++it){
    cout<<*it<<": ";
    for(int i=0; i<numSttHits; ++i){
      if(states[i]==*it){
	stateSum+=*it;
	cout<<sttHits[i]<<" ";

      }
    }
    cout<<endl;
  }

  int multiStateOffset=numSttHits;
  cout<<"MultiStates:"<<endl;
  for(int i=0; i<numSttHits; ++i){

    if(states[i] == NUM_STRAWS+1){
      cout<<sttHits[i]<<": ";
      for(int j=0; j<MAX_MULTISTATE_NUM; ++j){
	if(states[multiStateOffset+j*numSttHits+i]!=0){
	 cout<< sttHits[states[multiStateOffset+j*numSttHits+i]]<<" ";
	 stateSum+=sttHits[states[multiStateOffset+j*numSttHits+i]];
	}else{
	  break;
	}
      }
      cout<<endl;

    }

  }

  cout<<"final state sum: "<<stateSum<<endl;
}

extern "C" int* EvaluateAllStates(int * dev_tubeNeighborings, int* sttHits,
		int numUnskewedHits, int numSkewedHits, int* hitIndices) {
	//printf("Calculating states for PndSttCellTrackletGenerator on GPU\n");

	//device pointer
	int *dev_hitNeighbors, *dev_sttHits, *dev_hitIndices, *dev_numHitNeighbors,
			*dev_states, *dev_multiStates;

	int numSttHits = numSkewedHits + numUnskewedHits;

	//allocate device memory
	cudaMalloc((void**) &dev_hitNeighbors,sizeof(int)* (numUnskewedHits * MAX_UNSKEWED_NEIGHBORS+ numSkewedHits * MAX_SKEWED_NEIGHBORS));
	cudaCheckError();
	cudaMalloc((void**) &dev_sttHits, sizeof(int) * numSttHits);
	cudaCheckError();
	cudaMalloc((void**) &dev_numHitNeighbors, sizeof(int) * numSttHits);
	cudaCheckError();
	cudaMalloc((void**) &dev_hitIndices, sizeof(int) * (NUM_STRAWS + 1));
	cudaCheckError();
	cudaMalloc((void**) &dev_states, sizeof(int) * numSttHits);
	cudaCheckError();
	cudaMalloc((void**) &dev_multiStates,sizeof(int) * numSttHits * MAX_MULTISTATE_NUM);
	cudaCheckError();
	//set multistates to 0 (means no multistate)
	cudaMemset(dev_multiStates, 0, sizeof(int) * numSttHits * MAX_MULTISTATE_NUM);
	cudaCheckError();

	//copy sttHits to device
	cudaMemcpy(dev_sttHits, sttHits, sizeof(int) * numSttHits,cudaMemcpyHostToDevice);
	cudaCheckError();
	cudaMemcpy(dev_hitIndices, hitIndices, sizeof(int) * (NUM_STRAWS + 1),cudaMemcpyHostToDevice);
	cudaCheckError();

	int num_threads=GetNumThreads(numSttHits);

	//calculate hit neighbors
	ExtractData<<<1,num_threads>>>(dev_tubeNeighborings, dev_sttHits, numUnskewedHits, numSkewedHits, dev_hitIndices, dev_hitNeighbors, dev_numHitNeighbors, dev_states);

	//Evakuate states
	EvaluateStates<<<1,num_threads>>>(dev_sttHits, numUnskewedHits, numSkewedHits, dev_hitNeighbors, dev_numHitNeighbors, dev_states);
	EvaluateMultistates<<<1,num_threads>>>(dev_sttHits, numUnskewedHits, numSkewedHits, dev_hitNeighbors, dev_numHitNeighbors, dev_states, dev_multiStates);

	int* states= (int*) malloc(sizeof(int)*(numSttHits+numSttHits*MAX_MULTISTATE_NUM));
	cudaMemcpy(states, dev_states, sizeof(int)*numSttHits, cudaMemcpyDeviceToHost);
	cudaMemcpy(states+numSttHits, dev_multiStates, sizeof(int)*numSttHits*MAX_MULTISTATE_NUM, cudaMemcpyDeviceToHost);
	//PrintResults(sttHits, numSttHits, states);

	cudaFree(dev_hitNeighbors);
	cudaCheckError();
	cudaFree(dev_hitIndices);
	cudaCheckError();
	cudaFree(dev_sttHits);
	cudaCheckError();
	cudaFree(dev_numHitNeighbors);
	cudaCheckError();
	cudaFree(dev_states);
	cudaCheckError();
	cudaFree(dev_multiStates);
	cudaCheckError();

	return states;
}

extern "C" int* AllocateStaticData(int* cpu_tubeNeighborings, int numElements) {
	printf("Allocate static data\n");

	int* dev_tubeNeighborings;
	cudaMalloc((void**) &dev_tubeNeighborings, sizeof(int) * numElements);
	cudaCheckError();
	cudaMemcpy(dev_tubeNeighborings, cpu_tubeNeighborings,sizeof(int) * numElements, cudaMemcpyHostToDevice);
	cudaCheckError();

	return dev_tubeNeighborings;
}

extern "C" void FreeStaticData(int* dev_tubeNeighborings){
	cudaFree(dev_tubeNeighborings);
	cudaCheckError();
}

