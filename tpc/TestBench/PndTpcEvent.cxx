//=======================================================
//
//  PndTpcEvent class, PndTpcEvent.cxx
//  
//  Written by Xiaodong Zhang on October 02, 2009
//  Physics Department E18, TUM
// 
//=======================================================

#include "PndTpcEvent.h"

ClassImp(PndTpcEvent)

//___________________________________________________________________________
PndTpcEvent::PndTpcEvent()
:eventNb(-1), runNb(-1)
,nbofSamplesDrift(300), nbofSamplesBeforeDrift(10)
,nbofSamplesAfterDrift(10), firstSample(0)
,HV(100), angle(0), triggerType("rnd"), triggerRate(-1)
,temperature(-1), err_mask(0)   // 0 means no error, otherwise error. 
{;}

//___________________________________________________________________________
PndTpcEvent::~PndTpcEvent()
{;} 

//___________________________________________________________________________
std::ostream& operator<<(std::ostream& os, const PndTpcEvent& me)
{
  os << "PndTpcEvent:\n"
     << "   eventNb(nbInRun)       ="<<me.eventNb<<endl 
     << "   runNb                  ="<<me.runNb<<endl
     << "   firstSample            ="<<me.firstSample<<endl
     << "   nbofSamplesDrift       ="<<me.nbofSamplesDrift<<endl 
     << "   nbofSamplesBeforeDrift ="<<me.nbofSamplesBeforeDrift<<endl
     << "   nbofSamplesAfterDrift  ="<<me.nbofSamplesAfterDrift<<endl
     << "   HV(in % of GEM setup)  ="<<me.HV<<endl
     << "   angle( of TPC)         ="<<me.angle<<endl
     << "   triggerType            ="<<me.triggerType<<endl
     << "   triggerRate            ="<<me.triggerRate<<endl
     << "   temperature            ="<<me.temperature<<endl
     << "   err_mask               ="<<me.err_mask<<endl
     << "   time                   ="<<ctime(&(me._time))<<endl;
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
  string tmp;
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
}

//=======================================================
//
//  log information here
//   
//=======================================================
