{
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gSystem->AddIncludePath(" -I$VMCWORKDIR/fts -I$VMCWORKDIR/fts/FtsTracking -I$VMCWORKDIR/emc -I$VMCWORKDIR/emc/EmcDigi -I$VMCWORKDIR/emc/EmcMC -I$VMCWORKDIR/emc/EmcTools -I$VMCWORKDIR/emc/EmcReco -I$VMCWORKDIR/emc/EmcData -I$VMCWORKDIR/tof -I$VMCWORKDIR/tpc  -I$VMCWORKDIR/pnddata/FtsData -I$VMCWORKDIR/rho -I$VMCWORKDIR/lhetrack -I$VMCWORKDIR/rho/RhoBase -I$VMCWORKDIR/drc -I$VMCWORKDIR/mvd/MvdData -I$VMCWORKDIR/gpid -I$VMCWORKDIR/mcstack -I$VMCWORKDIR/base -I$VMCWORKDIR/base/sim -I$VMCWORKDIR/base/event -I$VMCWORKDIR/base/steer -I$VMCWORKDIR/pnddata/PidData -I$VMCWORKDIR/field -I$VMCWORKDIR/base/field -I$VMCWORKDIR/trackbase -I$VMCWORKDIR/pnddata/TrackData -I$VMCWORKDIR/pnddata -I$VMCWORKDIR/tools -I$VMCWORKDIR/fairtools -I$VMCWORKDIR/parbase ");
	gROOT->Macro("plotTrackCands.C+(1)");
}


