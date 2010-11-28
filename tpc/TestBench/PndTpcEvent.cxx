//=======================================================
//
//  PndTpcEvent class, PndTpcEvent.cxx
//  
//  Written by Xiaodong Zhang on October 02, 2009
//  Physics Department E18, TUM
// 
//=======================================================

#include "PndTpcEvent.h"
#include <iostream>

ClassImp(PndTpcEvent)


//___________________________________________________________________________
PndTpcEvent::PndTpcEvent()
:eventNb(-1), runNb(-1)
,nbofSamplesDrift(300), nbofSamplesBeforeDrift(10)
,nbofSamplesAfterDrift(10), firstSample(0)
,HV(100), angle(0), triggerType("rnd"), triggerRate(-1)
,temperature(-1), err_mask(0)   // 0 means no error, otherwise error. 
{
  eventVector.reserve(200000);
}

//___________________________________________________________________________
PndTpcEvent::~PndTpcEvent()
{;}

//___________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const PndTpcEvent& me)
{
  os << "PndTpcEvent:\n"
     << "   eventNb(nbInRun)       ="<<me.eventNb<<std::endl 
     << "   runNb                  ="<<me.runNb<<std::endl
     << "   firstSample            ="<<me.firstSample<<std::endl
     << "   nbofSamplesDrift       ="<<me.nbofSamplesDrift<<std::endl 
     << "   nbofSamplesBeforeDrift ="<<me.nbofSamplesBeforeDrift<<std::endl
     << "   nbofSamplesAfterDrift  ="<<me.nbofSamplesAfterDrift<<std::endl
     << "   HV(in % of GEM setup)  ="<<me.HV<<std::endl
     << "   angle( of TPC)         ="<<me.angle<<std::endl
     << "   triggerType            ="<<me.triggerType<<std::endl
     << "   triggerRate            ="<<me.triggerRate<<std::endl
     << "   temperature            ="<<me.temperature<<std::endl
     << "   err_mask               ="<<me.err_mask<<std::endl
     << "   time                   ="<<ctime(&(me._time))<<std::endl;
  return os;
}

//_______________________________________________________
const char* PndTpcEvent::getSystemDate()
{
  const char *date;
  time_t timer;
  timer = time(NULL);   //get the present time
  date = ctime(&timer); //convert the time_t to the string

  // **** erase the \n charactor from the "date" ******//
  std::string tmp;
  tmp = date;
  tmp.replace(tmp.length()-1, 2, ": ");
  date = tmp.data();
  
  return date;
}//End of the getSystemDate method.

//_________________________________________________________
void PndTpcEvent::reset()
{
  eventNb = -1; runNb = -1; 
  err_mask = 0;
  eventVector.clear();
  eventVector.reserve(200000);
}

//=======================================================
//
//  log information here
//   
//=======================================================
