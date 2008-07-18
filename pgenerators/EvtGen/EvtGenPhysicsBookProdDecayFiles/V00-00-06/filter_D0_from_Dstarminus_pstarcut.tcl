#====================== filter_D0_from_Dstarplus_pstarcut.tcl =============== 
# 
module talk GefSelectFilter 
# Select the filter GefPdtList which is a BooObject and copy into "Pdt" 
    BooNew Pdt = GefPdtList 
# Ask a D0 in each event 
    BooObjects Pdt or anti-D0 
# Set the CMS-momentum range: 
    BooObjects Pdt p  1.5  11.0 
# To save CPU time define the ancestor (i.e. the D*+ mother) 
    BooObjects Pdt ancestors  D*- 
# Tell the module to use this filter before the event is transformed to the lab: 
    beforeFilter set Pdt 
exit 
#================================================================== 
