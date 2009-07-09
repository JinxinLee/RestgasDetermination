//===========================================================================
/// \file CudaSupport_kernels.cu Kernels to test cuda support

#include <stdio.h>
#include <cmath>

//===========================================================================
const int BLOCK_WIDTH = 32;
const int BLOCK_HEIGHT = 16;
const int BLOCK_SIZE = BLOCK_WIDTH * BLOCK_HEIGHT;

//===========================================================================
#define CUDA_CALL(call) \
	do { \
		cudaError_t _err = (call); \
		if(_err != cudaSuccess) { \
			fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(_err)); \
		} \
	} while(0)

//===========================================================================
inline size_t ceil_div(const size_t a, const size_t b)
{
	return a/b + (a%b != 0);
}

//===========================================================================
__global__ void incrementKernel(float* source, float inc_val)
{
	int offset = threadIdx.x + (blockDim.x * threadIdx.y);
	offset += (blockDim.x * blockDim.y) * blockIdx.x;

	source[offset] += inc_val;
}

//===========================================================================
extern "C" __host__ void incrementArray(float* array, size_t array_count, 
		float inc_val) {
	size_t num_blocks = ceil_div(array_count, BLOCK_SIZE);

	size_t device_array_size = BLOCK_SIZE * num_blocks * sizeof(float);

	float* device_memory = NULL;

	CUDA_CALL( cudaMalloc( reinterpret_cast<void**>(&device_memory), device_array_size ) );

	CUDA_CALL( cudaMemcpy( device_memory, array, array_count*sizeof(float), cudaMemcpyHostToDevice ) );
	incrementKernel<<<num_blocks, dim3(BLOCK_WIDTH, BLOCK_HEIGHT)>>>(device_memory, inc_val);
	CUDA_CALL( cudaMemcpy( array, device_memory, array_count*sizeof(float), cudaMemcpyDeviceToHost ) );

	CUDA_CALL( cudaFree( device_memory ) );
	device_memory = NULL;
}

// vim:sw=4:ts=4:cindent:noet
