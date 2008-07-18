mod talk GefSelectFilter
  BooNew q5Filter = GefPdtList
  BooObjects q5Filter or Xi*0 anti-Xi*0
  beforeFilter set q5Filter
exit

module talk RandomControl
    # Change the # of seeds allowed per event. Default is 3M:
    maxRandomsPerEvent  set  300000000
 
    # Change the # of events allowed per run. Default is 2K:
#    maxEventsPerRun  set  1000
 exit
 
 
mod talk GfiEvtGen
  maxNumFilterTrials set 100000000
exit
 

