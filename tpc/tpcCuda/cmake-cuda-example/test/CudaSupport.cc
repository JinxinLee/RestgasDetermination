//===========================================================================
/// \file CudaSupport.cc Test CUDA support

#include <cuda_runtime_api.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

extern "C" void incrementArray(float* array, size_t array_size, float inc_val);

//===========================================================================
#define CUDA_CALL(call) \
	do { \
		cudaError_t _err = (call); \
		if(_err != cudaSuccess) { \
			fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(_err)); \
		} \
	} while(0)

//===========================================================================
void printCudaProperties(const struct cudaDeviceProp& properties)
{
	printf(" - Name              : %s\n", properties.name);
	printf(" - Tot. Glob. Mem.   : %u bytes (%u MiB)\n", 
			properties.totalGlobalMem, properties.totalGlobalMem >> 20);
	printf(" - Shared Mem./Blk.  : %u bytes (%u KiB)\n", 
			properties.sharedMemPerBlock, properties.sharedMemPerBlock >> 10);
	printf(" - Registers/Blk.    : %i\n", properties.regsPerBlock);
	printf(" - Warp Size         : %i\n", properties.warpSize);
	printf(" - Memory Pitch      : %u bytes (%u KiB)\n", 
			properties.memPitch, properties.memPitch >> 10);
	printf(" - Max. Threads/Blk. : %i\n", properties.maxThreadsPerBlock);
	printf(" - Max. Threads Dim. : %ix%ix%i\n", 
			properties.maxThreadsDim[0],
			properties.maxThreadsDim[1],
			properties.maxThreadsDim[2]);
	printf(" - Max. Grid Size    : %ix%ix%i\n", 
			properties.maxGridSize[0], 
			properties.maxGridSize[1],
			properties.maxGridSize[2]);
	printf(" - Clock Rate        : %i MHz\n", 
			properties.clockRate / 1000); // Struct val. is in KHz
	printf(" - Tot. Const. Mem.  : %u bytes (%u KiB)\n", 
			properties.totalConstMem, properties.totalConstMem >> 10);
	printf(" - Compute Cap. Ver. : %i.%i\n",
			properties.major, properties.minor);
	printf(" - Texture Alignment : %u bytes\n",
			properties.textureAlignment);
	printf(" - Device Overlap    : %s\n",
			properties.deviceOverlap ? "true" : "false");
	printf(" - Num. Multiproc.   : %i\n", properties.multiProcessorCount);
	printf(" - Kernel Timeout    : %s\n",
			properties.kernelExecTimeoutEnabled ? "true" : "false");
}

//===========================================================================
int main(int argc, char** argv)
{
	printf("CUDA test program.\n");

	int device_count = 0;

	CUDA_CALL( cudaGetDeviceCount(&device_count) );
	if(device_count < 1) {
		fprintf(stderr, "Need at least one CUDA-capable device.\n");
		exit(1);
	}

	printf("Found %i CUDA-capable device(s).\n", device_count);
	for(int device_idx=0; device_idx<device_count; ++device_idx)
	{
		struct cudaDeviceProp properties;
		CUDA_CALL( cudaGetDeviceProperties(&properties, device_idx) );

		printf("Device index %i:\n", device_idx);
		printCudaProperties(properties);
	}

	const size_t ARRAY_SIZE = 16000;

	float* test_array = new float[ARRAY_SIZE];
	for(size_t offset=0; offset < ARRAY_SIZE; ++offset)
	{
		test_array[offset] = (float)(offset & 0xff);
	}

	const float inc_val = 2.5f;
	incrementArray(test_array, ARRAY_SIZE, inc_val);

	size_t incorrect_count = 0;
	for(size_t offset=0; offset < ARRAY_SIZE; ++offset)
	{
		if(test_array[offset] != (float)(offset & 0xff) + inc_val)
		{
			++incorrect_count;
		}
	}
	
	if(incorrect_count > 0) {
		fprintf(stderr, "ERROR!!! -> Incorrect count: %u\n", incorrect_count);
	} else {
		printf("Test passed.\n");
	}

	return 0;
}

// vim:sw=4:ts=4:cindent:noet
