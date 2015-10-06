 ################################################################################
 #    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    #
 #                                                                              #
 #              This software is distributed under the terms of the             #
 #         GNU Lesser General Public Licence version 3 (LGPL) version 3,        #
 #                  copied verbatim in the file "LICENSE"                       #
 ################################################################################

configure_file(${CMAKE_SOURCE_DIR}/mvd/MvdOfflineTBAnalysis_Topix4/MQ/ex1-sampler-sink.json ${CMAKE_BINARY_DIR}/bin/config/ex1-sampler-sink.json)
configure_file(${CMAKE_SOURCE_DIR}/mvd/MvdOfflineTBAnalysis_Topix4/MQ/ex2-sampler-processor-sink.json ${CMAKE_BINARY_DIR}/bin/config/ex2-sampler-processor-sink.json)

Set(INCLUDE_DIRECTORIES
  ${BASE_INCLUDE_DIRECTORIES}
  ${CMAKE_SOURCE_DIR}/mvd/MvdOfflineTBAnalysis_Topix4
  ${CMAKE_SOURCE_DIR}/mvd/MvdOfflineTBAnalysis_Topix4/MQ
  ${CMAKE_CURRENT_BINARY_DIR}
)

Message(STATUS ${BASE_INCLUDE_DIRECTORIES} " base include dir")
Message(STATUS ${CMAKE_CURRENT_BINARY_DIR} " binary dir")

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${Boost_INCLUDE_DIR}
  ${SYSTEM_INCLUDE_DIRECTORIES}

)


  Set(INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/GemData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/passive
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools

  ${CMAKE_SOURCE_DIR}/mvd/MvdOfflineTBAnalysis_Topix4

    ${CMAKE_SOURCE_DIR}/fairmq/zeromq
  )
  
  Set(SYSTEM_INCLUDE_DIRECTORIES
    ${SYSTEM_INCLUDE_DIRECTORIES}
    ${ZMQ_INCLUDE_DIR}
  )

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

Set(LINK_DIRECTORIES
  ${ROOT_LIBRARY_DIR}
  ${FAIRROOT_LIBRARY_DIR}
  ${Boost_LIBRARY_DIRS}
)

Link_Directories(${LINK_DIRECTORIES})

set(SRCS

  "PndMQExample1Sampler.cxx"
  "PndMQExample1Sink.cxx"
  "PndMQTopix4Sampler.cxx"
  "PndMQTopix4ProcessorTask.cxx"
  "PndMQTopix4Processor.cxx"
  "PndMQTopix4Sink.cxx"
)


# to copy src that are header-only files (e.g. c++ template) for FairRoot external installation
# manual install (globbing add not recommended)
#Set(FAIRMQHEADERS
#  tools/FairMQTools.h
#)
#install(FILES ${FAIRMQHEADERS} DESTINATION include)

set(DEPENDENCIES
  ${DEPENDENCIES}
  Base
  BaseMQ
  Core
  boost_thread
  fairmq_logger
  boost_log
  boost_log_setup
  boost_timer
  boost_system
  boost_filesystem
  boost_program_options
  boost_random
  boost_chrono
  boost_exception
  Mvd
)

set(LIBRARY_NAME testMQ)

GENERATE_LIBRARY()

set(Exe_Names
  ex1-sampler_pnd
  ex1-sink_pnd
  Topix4-sampler
  Topix4-processor
  Topix4-sink
)




set(Exe_Source
  runExample1Sampler.cxx
  runExample1Sink.cxx
  runPndTopixSampler.cxx
  runPndTopixProcessor.cxx
  runPndTopixSink.cxx
)

list(LENGTH Exe_Names _length)
math(EXPR _length ${_length}-1)

ForEach(_file RANGE 0 ${_length})
  list(GET Exe_Names ${_file} _name)
  list(GET Exe_Source ${_file} _src)
  set(EXE_NAME ${_name})
  set(SRCS ${_src})
  set(DEPENDENCIES FairMQ testMQ ${FAIRMQ_DEPENDENCIES})
  GENERATE_EXECUTABLE()
EndForEach(_file RANGE 0 ${_length})
