/////////////////////////////////////////////////////////////
//  PndHoughApollonius
//  Calculate Apollonius Circles on GPU
/////////////////////////////////////////////////////////////////

/** PndHoughApollonius
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 02.02.2021
 *@version 3.0
 **
 ** PANDA class for calculating Apollonius circles
 ** Task level RECO
 **/

#include<iostream>
using namespace std;
#include <stdio.h>
#include <numeric>
#include <chrono>
#include <math.h>
#include <math_constants.h>

/**
 * @brief      CUDA Kernel to calculate the apollonius circles on the GPU
 *
 * @param[in]  k1    hit 1 as 1d array containing [x0, y0, r0, er0, x1, y2, r1, er1, .....]
 * @param[in]  k2    hit 2 as 1d array containing [x0, y0, r0, er0, x1, y2, r1, er1, .....]
 * @param[in]  k3    hit 3 as 1d array containing [x0, y0, r0, er0, x1, y2, r1, er1, .....]
 * @param[out] res   Result array to store the apollonius circles
 * @param[in]  N     Number of all multiplets
 */
__global__
void Apollonius(double* k1, double* k2, double* k3, double* res, int N){
  
  int s[8*3] = {-1,-1,-1,  -1,-1,1,  -1,1,-1,  -1,1,1,  1,-1,-1,  1,-1,1,  1,1,-1,  1,1,1};

  int index = blockIdx.x * blockDim.x + threadIdx.x;
  int stride = blockDim.x * gridDim.x;

  for (int n = index; n < N; n += stride){
    double a = 2 * (k1[n*4+0] - k2[n*4+0]);
    double b = 2 * (k1[n*4+1] - k2[n*4+1]);
    double d = ( k1[n*4+0] * k1[n*4+0] + k1[n*4+1] * k1[n*4+1] - k1[n*4+2] * k1[n*4+2]) - (k2[n*4+0] * k2[n*4+0] + k2[n*4+1] * k2[n*4+1] - k2[n*4+2] * k2[n*4+2]);
    double a_dash = 2 * (k1[n*4+0] - k3[n*4+0]);
    double b_dash = 2 * (k1[n*4+1] - k3[n*4+1]);
    double d_dash = (k1[n*4+0] * k1[n*4+0] + k1[n*4+1] * k1[n*4+1] - k1[n*4+2] * k1[n*4+2]) - (k3[n*4+0] * k3[n*4+0] + k3[n*4+1] * k3[n*4+1] - k3[n*4+2] * k3[n*4+2]);

    double N  = a * b_dash - a_dash * b;
    double Ax = b_dash * d - b * d_dash;
    double Cy = a * d_dash - a_dash * d;

    for(int i = 0; i < 8; i++){
      double c = 2 * (s[i*3+0] * k1[n*4+2] + s[i*3+1] * k2[n*4+2]);
      double c_dash = 2 * (s[i*3+0] * k1[n*4+2] + s[i*3+2] * k3[n*4+2]);
      double Bx = -b_dash * c + b * c_dash;
      double Dy = a_dash * c - a * c_dash;


      double A = (Bx * Bx) / (N * N) + (Dy * Dy) / (N * N) - 1;
      double B = 2 * Bx / N * (Ax / N - k1[n*4+0]) + 2 * Dy / N * (Cy / N - k1[n*4+1]) - 2 * s[i*3+0] * k1[n*4+2];
      double C = ( (Ax / N - k1[n*4+0]) * (Ax / N - k1[n*4+0]) ) + ( (Cy / N - k1[n*4+1]) * (Cy / N - k1[n*4+1]) ) - ( (s[i*3+0] * k1[n*4+2]) * (s[i*3+0] * k1[n*4+2]) );
      double D = (B * B) - 4 * A * C;

      double rk = (- B + sqrt(D)) / (2.0 * A);
      double xk = (Ax + Bx * rk) / N;
      double yk = (Cy + Dy * rk) / N;


      res[n*8*6+i*6+0] = xk;
      res[n*8*6+i*6+1] = yk;
      res[n*8*6+i*6+2] = rk;

      res[n*8*6+i*6+3] = 0;
      res[n*8*6+i*6+4] = 0;
      res[n*8*6+i*6+5] = 0;

    }
  }
}


/**
 * @brief      This functions allocates the memory on the GPU and calls the kernel.
 *
 * @param[in]  N     Number of multiplets.
 * @param[in]  hit0  hit 1 as 1d array containing [x0, y0, r0, er0, x1, y2, r1, er1, .....]
 * @param[in]  hit1  hit 2 as 1d array containing [x0, y0, r0, er0, x1, y2, r1, er1, .....]
 * @param[in]  hit2  hit 2 as 1d array containing [x0, y0, r0, er0, x1, y2, r1, er1, .....]
 * @param[out] res   Result array to store the apollonius circles
 */
extern "C" void ApolloniusCudaCalc(int N, double hit0[], double hit1[], double hit2[] ,double res[]){

    // allocate memory in device RAM
    double *d_k1, *d_k2, *d_k3, *d_res;
    cudaMalloc((void **) &d_k1, sizeof(double)*N*4);
    cudaMalloc((void **) &d_k2, sizeof(double)*N*4);
    cudaMalloc((void **) &d_k3, sizeof(double)*N*4);
    cudaMalloc((void **) &d_res, sizeof(double)*N*8*6+6*8+6);

    // copy matrices from host to device memory
    cudaMemcpy(d_k1, hit0, sizeof(double)*N*4, cudaMemcpyHostToDevice);
    cudaMemcpy(d_k2, hit1, sizeof(double)*N*4, cudaMemcpyHostToDevice);
    cudaMemcpy(d_k3, hit2, sizeof(double)*N*4, cudaMemcpyHostToDevice);
    cudaMemcpy(d_res, res, sizeof(double)*N*8*6+6*8+6, cudaMemcpyHostToDevice);

    int blockSize = 256;
    int numBlocks = (4*N + blockSize - 1) / blockSize;
    // Launch kernel 
    Apollonius<<<numBlocks, blockSize>>>(d_k1, d_k2, d_k3, d_res, N);    

    // Transefr results from device to host 
    cudaMemcpy(res, d_res, sizeof(double)*1, cudaMemcpyDeviceToHost); //*N*8*6+6*8+6, cudaMemcpyDeviceToHost);
    
    // Free memory
    cudaFree(d_k1);
    cudaFree(d_k2);
    cudaFree(d_k3);
    cudaFree(d_res);
}