#include <iomanip>

int draw_qa_track_finding(TString inFileName)
{
  //    TPstyle();

  gStyle->SetPalette(1);

  Int_t fShowStation1 = 0;
  Int_t fShowStation2 = 4;
  Int_t nofStations = 6;

  TFile* inFile = new TFile(inFileName.Data());
  TH1F* fhMCAllVsP    = (TH1F*)inFile->Get("GemTrackFinderQA/hMCAllVsP");
  TH1F* fhMCAccVsP    = (TH1F*)inFile->Get("GemTrackFinderQA/hMCAccVsP");
  TH1F* fhMCPrimVsP   = (TH1F*)inFile->Get("GemTrackFinderQA/hMCPrimVsP");
  TH1F* fhMCSecVsP    = (TH1F*)inFile->Get("GemTrackFinderQA/hMCSecVsP");
  TH1F* fhMCRefVsP    = (TH1F*)inFile->Get("GemTrackFinderQA/hMCRefVsP");
  TH1F* fhRecoAccVsP  = (TH1F*)inFile->Get("GemTrackFinderQA/hRecoAccVsP");
  TH1F* fhRecoPrimVsP = (TH1F*)inFile->Get("GemTrackFinderQA/hRecoPrimVsP");
  TH1F* fhRecoSecVsP  = (TH1F*)inFile->Get("GemTrackFinderQA/hRecoSecVsP");
  TH1F* fhRecoRefVsP  = (TH1F*)inFile->Get("GemTrackFinderQA/hRecoRefVsP");

  TH1F* fhEffAccVsP  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffAccVsP");
  TH1F* fhEffPrimVsP = (TH1F*)inFile->Get("GemTrackFinderQA/hEffPrimVsP");
  TH1F* fhEffSecVsP  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffSecVsP");
  TH1F* fhEffRefVsP  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffRefVsP");
  TH1F* fhEffAccVsT  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffAccVsT");
  TH1F* fhEffPrimVsT = (TH1F*)inFile->Get("GemTrackFinderQA/hEffPrimVsT");
  TH1F* fhEffSecVsT  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffSecVsT");
  TH1F* fhEffRefVsT  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffRefVsT");
  TH1F* fhEffAccVsN  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffAccVsN");
  TH1F* fhEffPrimVsN = (TH1F*)inFile->Get("GemTrackFinderQA/hEffPrimVsN");
  TH1F* fhEffSecVsN  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffSecVsN");
  TH1F* fhEffRefVsN  = (TH1F*)inFile->Get("GemTrackFinderQA/hEffRefVsN");

  TH2F* fhMomResAccVsP  = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResAccVsP");
  TH2F* fhMomResPrimVsP = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResPrimVsP");
  TH2F* fhMomResSecVsP  = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResSecVsP");
  TH2F* fhMomResRefVsP  = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResRefVsP");
  TH2F* fhMomResAccVsT  = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResAccVsT");
  TH2F* fhMomResPrimVsT = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResPrimVsT");
  TH2F* fhMomResSecVsT  = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResSecVsT");
  TH2F* fhMomResRefVsT  = (TH2F*)inFile->Get("GemTrackFinderQA/hMomResRefVsT");

  TH1F* fhNofMCTracksPerEvent = (TH1F*)inFile->Get("GemTrackFinderQA/hNofMCTracksPerEvent");

  TH1F* fhNofHitsPerRecoTrack     = (TH1F*)inFile->Get("GemTrackFinderQA/hNofHitsPerRecoTrack");
  TH1F* fhNofCorrHitsPerRecoTrack = (TH1F*)inFile->Get("GemTrackFinderQA/hNofCorrHitsPerRecoTrack");
  TH1F* fhNofNoTrHitsPerRecoTrack = (TH1F*)inFile->Get("GemTrackFinderQA/hNofNoTrHitsPerRecoTrack");
  TH1F* fhNofOthTHitsPerRecoTrack = (TH1F*)inFile->Get("GemTrackFinderQA/hNofOthTHitsPerRecoTrack");
  TH1F* fhNofHitsPerGhost         = (TH1F*)inFile->Get("GemTrackFinderQA/hNofHitsPerGhost");
  TH1F* fhNofHitsPerClone         = (TH1F*)inFile->Get("GemTrackFinderQA/hNofHitsPerClone");

  TCanvas* recoCanvas = new TCanvas("GemTrackFinderQACanvas","Gem Track Finder QA",10,10,1200,800);
  TPad* recoPad[10];

  recoPad[0] = new TPad("titlePad",   "Title pad"                     ,0.00,0.90,1.00,1.00);
  recoPad[1] = new TPad("efficiencyPPad","Efficiency P pad"           ,0.00,0.45,0.33,0.90);
  recoPad[2] = new TPad("efficiencyTPad","Efficiency T pad"           ,0.33,0.45,0.67,0.90);
  recoPad[3] = new TPad("efficiencyNPad","Efficiency N pad"           ,0.67,0.45,1.00,0.90);

  recoPad[4] = new TPad("resolutionPad","Momentum resolution pad"     ,0.00,0.00,0.33,0.45);

  recoPad[5] = new TPad("hitsOfTrackPad","Hits of track pad"          ,0.33,0.00,0.67,0.45);
  recoPad[6] = new TPad("summaryPad",    "Summary pad"                ,0.67,0.00,1.00,0.45);

  for ( Int_t ipad = 0 ; ipad < 7 ; ipad++ ) {
    recoPad[ipad]->SetFillColor(0);
    recoPad[ipad]->SetBorderMode(0);
    recoPad[ipad]->Draw();
  }
  
  recoPad[0]->cd();
  TLegend* brp = new TLegend(0.1,0.1,0.9,0.9,"Gem Track Finder QA");
  brp->SetTextAlign(22);
  brp->SetTextSize(0.6);
  brp->SetTextColor(1);
  brp->SetBorderSize(0);
  brp->SetFillColor(0);
  brp->Draw();
  recoPad[0]->Update();

  recoPad[1]->cd();
  gPad->SetGridx(); 
  gPad->SetGridy();
  fhEffAccVsP->SetAxisRange(0.,110.,"Y");
  fhEffAccVsP->SetLineWidth(2);
  fhEffAccVsP->SetLineColor(1);
  fhEffAccVsP->SetTitle("Efficiency vs momentum");
  fhEffAccVsP->GetYaxis()->SetTitle("efficiency [%]");
  fhEffAccVsP->Draw("pe");
  fhEffPrimVsP->SetLineWidth(2);
  fhEffPrimVsP->SetLineColor(2);
  fhEffPrimVsP->Draw("pesame");
  fhEffRefVsP->SetLineWidth(2);
  fhEffRefVsP->SetLineColor(6);
  fhEffRefVsP->Draw("pesame");
  fhEffSecVsP->SetLineWidth(2);
  fhEffSecVsP->SetLineColor(4);
  fhEffSecVsP->Draw("pesame");
  TLegend* effLeg = new TLegend(0.2,0.15,0.5,0.3);
  effLeg->SetBorderSize(0);
  effLeg->SetFillColor(0);
  effLeg->AddEntry(fhEffRefVsP, "reference","pl");
  effLeg->AddEntry(fhEffPrimVsP,"primaries","pl");
  effLeg->AddEntry(fhEffAccVsP, "all" ,"pl");
  effLeg->AddEntry(fhEffSecVsP, "secondaries" ,"pl");
  effLeg->Draw();
  TLine* oneLine = new TLine(0.0,100.0,10.0,100.0);
  oneLine->SetLineStyle(2);
  oneLine->Draw();
  recoPad[1]->Update();

  recoPad[2]->cd();
  gPad->SetGridx();
  gPad->SetGridy();
  fhEffAccVsT->SetAxisRange(0.,110.,"Y");
  fhEffAccVsT->SetLineWidth(2);
  fhEffAccVsT->SetLineColor(1);
  fhEffAccVsT->SetTitle("Efficiency vs theta");
  fhEffAccVsT->GetYaxis()->SetTitle("efficiency [%]");
  fhEffAccVsT->Draw("pe");
  fhEffPrimVsT->SetAxisRange(0.,110.,"Y");
  fhEffPrimVsT->SetLineWidth(2);
  fhEffPrimVsT->SetLineColor(2);
  fhEffPrimVsT->Draw("pesame");
  fhEffRefVsT->SetLineWidth(2);
  fhEffRefVsT->SetLineColor(6);
  fhEffRefVsT->Draw("pesame");
  fhEffSecVsT->SetLineWidth(2);
  fhEffSecVsT->SetLineColor(4);
  fhEffSecVsT->Draw("pesame");
  TLegend* effLeg = new TLegend(0.6,0.4,0.9,0.6);
  effLeg->SetBorderSize(0);
  effLeg->SetFillColor(0);
  effLeg->AddEntry(fhEffRefVsT, "reference","pl");
  effLeg->AddEntry(fhEffPrimVsT,"primaries","pl");
  effLeg->AddEntry(fhEffAccVsT, "all" ,"pl");
  effLeg->AddEntry(fhEffSecVsT, "secondaries" ,"pl");
  effLeg->Draw();
  TLine* oneLine = new TLine(0.0,100.0,40.0,100.0);
  oneLine->SetLineStyle(2);
  oneLine->Draw();
  recoPad[2]->Update();

  recoPad[3]->cd();
  gPad->SetGridx();
  gPad->SetGridy();
  Int_t lastNotEmptyBin = fhEffAccVsN->GetNbinsX();
  while ( fhEffAccVsN->GetBinContent(lastNotEmptyBin) == 0 )
    lastNotEmptyBin--;
  fhEffAccVsN->SetAxisRange(-0.5,1.2*lastNotEmptyBin,"X");
  fhEffAccVsN->SetAxisRange(0.,110.,"Y");
  fhEffAccVsN->SetLineWidth(2);
  fhEffAccVsN->SetLineColor(1);
  fhEffAccVsN->SetTitle("Efficiency vs nof points");
  fhEffAccVsN->GetYaxis()->SetTitle("efficiency [%]");
  fhEffAccVsN->Draw("pe");
  fhEffPrimVsN->SetAxisRange(0.,110.,"Y");
  fhEffPrimVsN->SetLineWidth(2);
  fhEffPrimVsN->SetLineColor(2);
  fhEffPrimVsN->Draw("pesame");
  fhEffRefVsN->SetLineWidth(2);
  fhEffRefVsN->SetLineColor(6);
  fhEffRefVsN->Draw("pesame");
  fhEffSecVsN->SetLineWidth(2);
  fhEffSecVsN->SetLineColor(4);
  fhEffSecVsN->Draw("pesame");
  TLegend* effLeg = new TLegend(0.1,0.7,0.4,0.9);
  effLeg->SetBorderSize(0);
  effLeg->SetFillColor(0);
  effLeg->AddEntry(fhEffRefVsN, "reference","pl");
  effLeg->AddEntry(fhEffPrimVsN,"primaries","pl");
  effLeg->AddEntry(fhEffAccVsN, "all" ,"pl");
  effLeg->AddEntry(fhEffSecVsN, "secondaries" ,"pl");
  effLeg->Draw();
  TLine* oneLine = new TLine(0.0,100.0,40.0,100.0);
  oneLine->SetLineStyle(2);
  oneLine->Draw();
  recoPad[1]->Update();

  recoPad[4]->cd();
  fhMomResPrimVsP->SetAxisRange(-10.,10.,"Y");
  fhMomResPrimVsP->GetYaxis()->SetTitle("#delta{p}/p [%]");
  fhMomResPrimVsP->Draw("cont0");
  TH1F* fhLowBand = new TH1F("fhLowBand","Low Band",
			     fhMomResPrimVsP->GetXaxis()->GetNbins(),
			     fhMomResPrimVsP->GetXaxis()->GetXmin(),
			     fhMomResPrimVsP->GetXaxis()->GetXmax());
  TH1F* fhHigBand = new TH1F("fhHigBand","Hig Band",
			     fhMomResPrimVsP->GetXaxis()->GetNbins(),
			     fhMomResPrimVsP->GetXaxis()->GetXmin(),
			     fhMomResPrimVsP->GetXaxis()->GetXmax());
  TH1F* fhMomResB = new TH1F("fhMomResB","Momentum resolution",
			     fhMomResPrimVsP->GetXaxis()->GetNbins(),
			     fhMomResPrimVsP->GetXaxis()->GetXmin(),
			     fhMomResPrimVsP->GetXaxis()->GetXmax());
  for ( Int_t ibin = fhMomResPrimVsP->GetXaxis()->GetNbins() ; ibin > 1 ; ibin-- ) {
    TF1* gausFit = new TF1("gausFit","gaus");
    TH1F* tempProjY = (TH1F*)fhMomResPrimVsP->ProjectionY("tempProjY",ibin,ibin);
    tempProjY->Fit("gausFit","QN","",-5.,5.);
    fhLowBand->SetBinContent(ibin,gausFit->GetParameter(1)-gausFit->GetParameter(2));
    fhLowBand->SetBinError(ibin,0.01);
    fhHigBand->SetBinContent(ibin,gausFit->GetParameter(1)+gausFit->GetParameter(2));
    fhHigBand->SetBinError(ibin,0.01);//gausFit->GetPara(1)+gausFit->GetParError(2));
    fhMomResB->SetBinContent(ibin,gausFit->GetParameter(2));
    fhMomResB->SetBinError(ibin,0.01);//gausFit->GetPara(1)+gausFit->GetParError(2));
  }
  fhLowBand->SetLineWidth(2);
  fhHigBand->SetLineWidth(2);
  fhLowBand->Draw("Psame");
  fhHigBand->Draw("Psame");
  fhMomResB->SetLineWidth(2);
  fhMomResB->SetLineColor(2);
  fhMomResB->Draw("Csame");
  recoPad[4]->Update();
  /*
  recoPad[4]->cd();
  fhMomResPrimVsT->SetAxisRange(-10.,10.,"Y");
  fhMomResPrimVsT->GetYaxis()->SetTitle("#delta{p}/p [%]");
  fhMomResPrimVsT->Draw("cont0");
  TH1F* fhLowBand = new TH1F("fhLowBand","Low Band",
			     fhMomResPrimVsT->GetXaxis()->GetNbins(),
			     fhMomResPrimVsT->GetXaxis()->GetXmin(),
			     fhMomResPrimVsT->GetXaxis()->GetXmax());
  TH1F* fhHigBand = new TH1F("fhHigBand","Hig Band",
			     fhMomResPrimVsT->GetXaxis()->GetNbins(),
			     fhMomResPrimVsT->GetXaxis()->GetXmin(),
			     fhMomResPrimVsT->GetXaxis()->GetXmax());
  for ( Int_t ibin = fhMomResPrimVsT->GetXaxis()->GetNbins() ; ibin > 1 ; ibin-- ) {
    TF1* gausFit = new TF1("gausFit","gaus");
    TH1F* tempProjY = (TH1F*)fhMomResPrimVsT->ProjectionY("tempProjY",ibin,ibin);
    tempProjY->Fit("gausFit","QN","",-5.,5.);
    fhLowBand->SetBinContent(ibin,gausFit->GetParameter(1)-gausFit->GetParameter(2));
    fhLowBand->SetBinError(ibin,0.01);
    fhHigBand->SetBinContent(ibin,gausFit->GetParameter(1)+gausFit->GetParameter(2));
    fhHigBand->SetBinError(ibin,0.01);//gausFit->GetPara(1)+gausFit->GetParError(2));
  }
  fhLowBand->SetLineWidth(2);
  fhHigBand->SetLineWidth(2);
  fhLowBand->Draw("Psame");
  fhHigBand->Draw("Psame");
  recoPad[4]->Update();
  */
  recoPad[5]->cd();
  Int_t lastNotEmptyBin = fhNofHitsPerRecoTrack->GetNbinsX();
  while ( fhNofHitsPerRecoTrack->GetBinContent(lastNotEmptyBin) == 0 )
    lastNotEmptyBin--;

  fhNofHitsPerRecoTrack->SetAxisRange(-0.5,1.2*lastNotEmptyBin,"X");
  fhNofHitsPerRecoTrack->Draw();
  fhNofCorrHitsPerRecoTrack->SetLineColor(3);
  fhNofCorrHitsPerRecoTrack->Draw("same");
  fhNofNoTrHitsPerRecoTrack->SetLineColor(4);
  fhNofNoTrHitsPerRecoTrack->Draw("same");
  fhNofOthTHitsPerRecoTrack->SetLineColor(2);
  fhNofOthTHitsPerRecoTrack->Draw("same");
  TLegend* nhptLeg = new TLegend(0.25,0.65,0.6,0.85);
  nhptLeg->SetBorderSize(0);
  nhptLeg->SetFillColor(0);
  nhptLeg->AddEntry(fhNofHitsPerRecoTrack,"all hits","l");
  nhptLeg->AddEntry(fhNofCorrHitsPerRecoTrack,"mc(hit)=mc(track)","l");
  nhptLeg->AddEntry(fhNofNoTrHitsPerRecoTrack,"mc(hit)=-1","l");
  nhptLeg->AddEntry(fhNofOthTHitsPerRecoTrack,"mc(hit)=mc(!track)","l");
  nhptLeg->Draw();
  recoPad[5]->Update();

  Int_t fNofEvents = fhNofMCTracksPerEvent->GetEntries();
  Int_t fNofMCAll    = fhMCAllVsP   ->Integral();
  Int_t fNofMCAcc    = fhMCAccVsP   ->Integral();
  Int_t fNofMCPrim   = fhMCPrimVsP  ->Integral();
  Int_t fNofMCRef    = fhMCRefVsP  ->Integral(fhMCPrimVsP->FindBin(0.5),fhMCPrimVsP->GetNbinsX());
  Int_t fNofMCSec    = fhMCSecVsP   ->Integral();
  Int_t fNofRecoAcc  = fhRecoAccVsP ->Integral();
  Int_t fNofRecoPrim = fhRecoPrimVsP->Integral();
  Int_t fNofRecoRef  = fhRecoRefVsP->Integral(fhRecoPrimVsP->FindBin(0.5),fhRecoPrimVsP->GetNbinsX());
  Int_t fNofRecoSec  = fhRecoSecVsP ->Integral();
  Int_t fNofRecoGhosts = fhNofHitsPerGhost->Integral();
  Int_t fNofRecoClones = fhNofHitsPerClone->Integral();
  Double_t effAcc  = 100.*(Double_t)fNofRecoAcc /((Double_t)fNofMCAcc);
  Double_t effPrim = 100.*(Double_t)fNofRecoPrim/((Double_t)fNofMCPrim);
  Double_t effSec  = 100.*(Double_t)fNofRecoSec /((Double_t)fNofMCSec);
  Double_t effRef  = 100.*(Double_t)fNofRecoRef /((Double_t)fNofMCRef);
  Double_t ghPerEv = (Double_t)fNofRecoGhosts/((Double_t)fNofEvents);
  Double_t ghPerTr = (Double_t)fNofRecoGhosts/((Double_t)fNofMCAll);
  Double_t clPerEv = (Double_t)fNofRecoClones/((Double_t)fNofEvents);
  Double_t clPerTr = (Double_t)fNofRecoClones/((Double_t)fNofMCAll);
  recoPad[6]->cd();
  TPaveText* printoutPave = new TPaveText(0.0,0.0,1.0,1.0);
  printoutPave->SetTextAlign(23);
  printoutPave->SetBorderSize(0);
  printoutPave->SetFillColor(0);
  printoutPave->AddText(Form("%i events",fNofEvents));
  printoutPave->AddText(Form("%i MC tracks",fNofMCAll));
  printoutPave->AddText("Tracking efficiencies:");
  printoutPave->AddText(Form("all = %2.2f%% ( %d / %d )" ,effAcc ,fNofRecoAcc ,fNofMCAcc));
  printoutPave->AddText(Form("prim = %2.2f%% ( %d / %d )",effPrim,fNofRecoPrim,fNofMCPrim));
  printoutPave->AddText(Form("ref = %2.2f%% ( %d / %d )" ,effRef ,fNofRecoRef ,fNofMCRef));
  printoutPave->AddText(Form("sec = %2.2f%% ( %d / %d )" ,effSec ,fNofRecoSec ,fNofMCSec));
  printoutPave->AddText(Form("%i ghosts, %2.5f /event, %2.5f /MC tr.",fNofRecoGhosts,ghPerEv,ghPerTr));
  printoutPave->AddText(Form("%i clones, %2.5f /event, %2.5f /MC tr.",fNofRecoClones,clPerEv,clPerTr));
  printoutPave->Draw();

  recoPad[6]->Update();
  cout << "-------------------- PndGemTrackFinderQA : Summary ------------------" << endl;
  cout << " Events:        " << setw(10) << fNofEvents << endl;
  cout << " MC Tracks:     " << setw(10) << fNofMCAll << endl;
  cout << " reconstruable: " << setw(10) << fNofMCAcc  << " reconstructed: " << setw(10) << fNofRecoAcc  << " >>>> " << effAcc  << "%" << endl;
  cout << " primaries    : " << setw(10) << fNofMCPrim << " reconstructed: " << setw(10) << fNofRecoPrim << " >>>> " << effPrim << "%" << endl;
  cout << " reference    : " << setw(10) << fNofMCRef  << " reconstructed: " << setw(10) << fNofRecoRef  << " >>>> " << effRef  << "%" << endl;
  cout << " secondaries  : " << setw(10) << fNofMCSec  << " reconstructed: " << setw(10) << fNofRecoSec  << " >>>> " << effSec  << "%" << endl;
  cout << " ghosts       : " << setw(10) << fNofRecoGhosts << "   >>> " << setw(10) << ghPerEv << " per event   >>> " << setw(10) << ghPerTr << " per MC Track" << endl;
  cout << " clones       : " << setw(10) << fNofRecoClones << "   >>> " << setw(10) << clPerEv << " per event   >>> " << setw(10) << clPerTr << " per MC Track" << endl;
  cout << "---------------------------------------------------------------------" << endl; 

  return 0;
}
