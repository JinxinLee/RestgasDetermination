// Test error propagation and calculations.
// The output should be:
//Value: 1
//Error: 0.5
#include "EvtGenBase/EvtPatches.hh"

#include <stdio.h>
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtValError.hh"

int main(int argc, char* argv[]) 
{
  EvtValError x1(1.,0.4);
  EvtValError x2(1.,0.3);
  EvtValError prod = x1 * x2;

  printf("Value: %f\n",prod.value());
  printf("Error: %f\n",prod.error());
  
  return 0;
}

