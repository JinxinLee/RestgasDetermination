######################################
# Minimal example of how to compile CUDA inside of PandaRoot using CMake (should work with all other ROOT-involving CMake project, though.)
#
#
# By Andreas Herten (Forschungszentrum Jülich, 24.7.2013)
######################################


# Make sure CUDA is found at all.
# Additional options like: find_package(CUDA 2.3 EXACT QUIET REQUIERED)
find_package(CUDA)


# Set include paths (passed to nvcc in form of nvcc -Ipath1 -Ipath2 ...)
# Usually current source dir - with more complex project structures also more
CUDA_INCLUDE_DIRECTORIES(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  )

# Provide a list of actual .cu files
set(CUDAEXAMPLE_SRCS
  cudaExample.cu
  )

# Possibility to stear compiler flags - eg different compute capabilites
list(APPEND CUDA_NVCC_FLAGS --gpu-architecture sm_20)

# The name of the shared library
set(PNDCUDAEXAMPLESONAME "PndCudaExample")
set(PNDCUDAEXAMPLESONAME ${PNDCUDAEXAMPLESONAME} PARENT_SCOPE)

# message(${PNDCUDAEXAMPLESONAME}) #DEBUG test if variable is set

# Generate the actual library (object)
CUDA_ADD_LIBRARY(${PNDCUDAEXAMPLESONAME} 
  ${CUDAEXAMPLE_SRCS}
   SHARED # This seems to be, for our case, equivalent with set(BUILD_SHARED_LIBS ON)
  )


######################################
# Things to add into the parent CMakeList.txt:
# add_subdirectory(cudaExample) # With "cudaExample" being the directory of the actual cuda stuff (=this current directory)
# add_dependencies(yourProject ${PNDCUDAEXAMPLESONAME}) # With "yourProject" being the library currently being created in the parent CMakeFile.txt
# target_link_libraries(yourProject ${CMAKE_BINARY_DIR}/lib/lib${PNDCUDAEXAMPLESONAME}.so [.....]) # Here, add just the second argument (where our object is added to the target_link_libraries()).
#
# Then you should be done.
######################################
