void doIt(){
  gROOT->ProcessLine("TFile::Open(\"0.50GeV_0.025cmRes_65.00deg_10000ev.root\");");
  gROOT->ProcessLine(".x ../trunk/tpc/TestChamber/macro/christian_style.C");
  gROOT->ProcessLine(" .L ../trunk/genfit/benchmark/plotPulls.C+ ");
  gROOT->ProcessLine("plotPulls p(t)");
  gROOT->ProcessLine("p.Loop(\"0.5 GeV/c muons under #theta = 65^{#circ}\")");
}
