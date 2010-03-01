//=======================================================
//
//  PndTpcEvent class, PndTpcEvent.h
//  
//  Written by Xiaodong Zhang on October 07, 2009
//  Physics Department E18, TUM
//
//=======================================================

//PndTpcEvent.h 
#ifndef PNDTPCEVENT_H
#define PNDTPCEVENT_H

#include "TObject.h" 
#include "PndTpcSample.h"
#include <list>
#include <string>
#include <ctime>

using namespace std;

//______________________________________________________
class PndTpcEvent : public TObject { 

private:
  std::vector<PndTpcSample> eventVector;
  long int                  eventNb; // nbInRun of event header
  long int                  runNb;
  time_t                    _time;
  unsigned int              firstSample;
  unsigned int              nbofSamplesDrift;
  unsigned int              nbofSamplesBeforeDrift;
  unsigned int              nbofSamplesAfterDrift;
  int 		            HV;      // in % of nominal voltage of GEM 
  int      	            angle;
  string                    triggerType;
  unsigned int              triggerRate;
  int                       temperature;
  int                       err_mask;

public:
  //********** Constructor and destructor   ****************//
  PndTpcEvent();
  virtual ~PndTpcEvent();
  
  //************* reload the ostream operator **************//  
  friend std::ostream& operator<<(std::ostream& os, const PndTpcEvent& me);

  //************  other functions *************************//
  std::vector<PndTpcSample> getEventVector() const { return eventVector; }
  long int                      getEventNb() const { return eventNb; } 
  long int                        getRunNb() const { return runNb; }
  time_t                           getTime() const { return _time; }
  unsigned int              getFirstSample() const { return firstSample; }
  unsigned int         getNbofSamplesDrift() const { return nbofSamplesDrift; }
  unsigned int   getNbofSamplesBeforeDrift() const { return nbofSamplesBeforeDrift; }
  unsigned int    getNbofSamplesAfterDrift() const { return nbofSamplesAfterDrift; }
  int                                getHV() const { return HV; }
  int                             getAngle() const { return angle; }
  string                    getTriggerType() const { return triggerType; }
  unsigned int              getTriggerRate() const { return triggerRate; }
  int                       getTemperature() const { return temperature; }
  int                                 good() const { return !err_mask; }

  void        eventVectorPushBack(PndTpcSample pts) { eventVector.push_back(pts); }
  void                      setEventNb(long int en) { eventNb = en; }
  void                        setRunNb(long int rn) { runNb = rn; }
  void                            setTime(time_t t) { _time = t; }
  void              setFirstSample(unsigned int fs) { firstSample = fs; }
  void        setNbofSamplesDrift(unsigned int nsd) { nbofSamplesDrift = nsd; }
  void setNbofSamplesBeforeDrift(unsigned int nsbd) { nbofSamplesBeforeDrift = nsbd; }
  void  setNbofSamplesAfterDrift(unsigned int nsad) { nbofSamplesAfterDrift = nsad; }
  void                                setHV(int hv) { HV = hv; }
  void                             setAngle(int ag) { angle = ag; }
  void                    setTriggerType(string ty) { triggerType = ty; } 
  void              setTriggerRate(unsigned int tr) { triggerRate = tr; } 
  void                       setTemperature(int tp) { temperature = tp;}

  const char* getSystemDate();
  void                reset();

  ClassDef(PndTpcEvent, 1)  //Event structure 
};

#endif //  PNDTPCEVENT_H 

//=======================================================
//  log information here
//   
//=======================================================
