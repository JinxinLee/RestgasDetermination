
SET(CMAKE_CXX_FLAGS -g)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-DTESTCHAMBER)
PROJECT(ParticleGun)

ENABLE_TESTING()

INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include
					./../src)


#add ROOT lib
SET(ROOT_LIB $ENV{ROOTSYS}/lib)
SET(ROOT_LIBS -L${ROOT_LIB} -lMinuit -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -pthread -lm -ldl -rdynamic)



ADD_EXECUTABLE(particleGun particleGun.cxx ../src/ConfigFile.cpp)
TARGET_LINK_LIBRARIES(particleGun ${ROOT_LIBS})

