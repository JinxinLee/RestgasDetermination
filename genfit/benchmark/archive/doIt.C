void doIt(){
  //  gROOT->ProcessLine("TFile::Open(\"1.00GeV_0.025cmRes_45.00deg_50000ev.root\");");
  gROOT->ProcessLine(".x ../../trunk2/tpc/TestChamber/macro/christian_style.C");
  gROOT->ProcessLine(" .L ../../trunk2/genfit/benchmark/archive/plotPulls.C+ ");
  gROOT->ProcessLine("plotPulls p(t)");
  gROOT->ProcessLine("p.Loop(\"\")");
}
