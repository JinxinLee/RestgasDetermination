###################
##  Generator level filter 
##
##  Select BBbar events with a Psi with at least 1.3 in Pstar
##   
##
##  D. Lange August 15, 2000
## 
##
##################

mod talk GefSelectFilter 
      BooObjects GefPdtList or J/psi
      BooObjects GefPdtList p 1.3 2.2
      beforeFilter set GefPdtList
exit
