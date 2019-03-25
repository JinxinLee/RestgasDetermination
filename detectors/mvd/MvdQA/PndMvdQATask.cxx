// -------------------------------------------------------------------------
// -----                PndMvdQATask source file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TF1.h"

// framework includes
#include "FairRootManager.h"
#include "PndMvdQATask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "../pnddata/PndMCTrack.h"
#include "FairHit.h"
// PndSds includes
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"


// -----   Default constructor   -------------------------------------------
PndMvdQATask::PndMvdQATask() : PndBlackBoxTask("PndMvdQATask"), fProperDifference(false), fGeoH(nullptr)
{
   SetPixelBoundaries();
   SetStripBoundaries();


}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdQATask::~PndMvdQATask()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdQATask::Init()
{

    std::cout << "PndMvdQATask::Init()" << std::endl;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    std::cout << "-E- PndMvdQATask::Init: " << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get MCPoints array
  fMcPoints = (TClonesArray*) ioman->GetObject("MVDPoint");
  if ( ! fMcPoints ) {
    std::cout << "-W- PndMvdQATask::Init: "<< "No PndSdsMCPoint array!" << std::endl;
    return kERROR;
  }

  // Get MCPoints array
  fPixelHits = (TClonesArray*) ioman->GetObject("MVDHitsPixel");
  if ( ! fPixelHits ) {
    std::cout << "-W- PndMvdQATask::Init: "<< "No MVDHitsPixel array!" << std::endl;
    return kERROR;
  }

  // Get MCPoints array
  fStripHits = (TClonesArray*) ioman->GetObject("MVDHitsStrip");
  if ( ! fStripHits ) {
    std::cout << "-W- PndMvdQATask::Init: "<< "No MVDHitsStrip array!" << std::endl;
    return kERROR;
  }

  fHStripSingle = new TH1D("StripSingle","Strip Single Digi Cluster",100,-0.008,0.008);
  fHStripDouble = new TH1D("StripDouble","Strip Multi Digi Cluster",100,-0.008,0.008);
  fHStripAll =    new TH1D("StripAll","Strip All Hits",100,-0.008,0.008);
  fHStripClusterSize = new TH1D("StripClusterSize", "Strip Cluster Size", 11, -0.5, 10.5);
  fHStripEnergy = new TH1D("StripEnergy", "Strip Energy Difference", 100, -0.00004, 0.00004);
  fHStripTime   = new TH1D("StripTime", "Strip Time Difference", 100, -20, 20);


  fHPixelSingle = new TH1D("PixelSingle","Pixel Single Digi Cluster",100,-0.008,0.008);
  fHPixelDouble = new TH1D("PixelDouble","Pixel Multi Digi Cluster",100,-0.008,0.008);
  fHPixelAll =    new TH1D("PixelAll","Pixel All Hits",100,-0.008,0.008);
  fHPixelClusterSize = new TH1D("PixelClusterSize", "Pixel Cluster Size", 11, -0.5, 10.5);
  fHPixelEnergy = new TH1D("PixelEnergy", "Pixel Energy Difference", 100, -0.00004, 0.00004);
  fHPixelTime   = new TH1D("PixelTime", "Pixel Time Difference", 100, -20, 20);



  if (fVerbose > 1) std::cout << "-I- gGeoManager = "<<gGeoManager << std::endl;


  std::cout << "PndMvdQATask::Init() finished" << std::endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndMvdQATask::SetParContainers()
{
    if ( fGeoH == NULL )
        fGeoH = PndGeoHandling::Instance();

    fGeoH->SetParContainers();
    fGeoH->FillSensorMap();
//
//    fGeoH->SetParContainers();
//    fGeoH->PrintSensorNames();

    std::cout << "PndMvdQATask::SetParContainers() finished" << std::endl;
}


// -----   Public method Exec   --------------------------------------------
void PndMvdQATask::Exec(Option_t*)
{
    for(int pixIndex = 0; pixIndex < fPixelHits->GetEntriesFast(); pixIndex++){
        PndSdsHit* myHit = (PndSdsHit*)fPixelHits->At(pixIndex);
        CalculateDifference(myHit);
        if (fProperDifference == kTRUE){
            fHPixelAll->Fill(fSpatialDifference.X());
            fHPixelEnergy->Fill(fEnergyDifference);
            fHPixelTime->Fill(fTimeDifference);
            if (myHit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDPixelDigis")).GetNLinks() == 1){
                fHPixelSingle->Fill(fSpatialDifference.X());
            } else {
                fHPixelDouble->Fill(fSpatialDifference.X());
            }
            fHPixelClusterSize->Fill(myHit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDPixelDigis")).GetNLinks());
        }
    }

    for(int strIndex = 0; strIndex < fStripHits->GetEntriesFast(); strIndex++){
        PndSdsHit* myHit = (PndSdsHit*)fStripHits->At(strIndex);
        CalculateDifference(myHit);
        if (fProperDifference == kTRUE){
            fHStripAll->Fill(fSpatialDifference.X());
            fHStripEnergy->Fill(fEnergyDifference);
            fHStripTime->Fill(fTimeDifference);
            if (myHit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDStripDigis")).GetNLinks() == 2){
                fHStripSingle->Fill(fSpatialDifference.X());
            } else {
                fHStripDouble->Fill(fSpatialDifference.X());
            }
            fHStripClusterSize->Fill(myHit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDStripDigis")).GetNLinks());
        }
    }
}
// -------------------------------------------------------------------------

void PndMvdQATask::SetPixelBoundaries()
{
   fPixelBoundaries.push_back(std::make_pair(-1e-03,1e-03));   //mean value of small double gaussian fit
   fPixelBoundaries.push_back(std::make_pair(2e-04, 1e-03));   //sigma value of small double gaussian fit
   fPixelBoundaries.push_back(std::make_pair(-1e-03, 1e-03));  //mean value of wide double gaussian fit
   fPixelBoundaries.push_back(std::make_pair(2e-04, 1e-02));   //sigma value of small double gaussian fit
   fPixelBoundaries.push_back(std::make_pair(-1e-03, 1e-03));  //mean value of single hit distribution
   fPixelBoundaries.push_back(std::make_pair(2e-04, 1e-02));   //rms of single hit distribution
   fPixelBoundaries.push_back(std::make_pair(2, 3));           //mean value of cluster size
   fPixelBoundaries.push_back(std::make_pair(0.8, 1.2));       //sigma value of cluster size
   fPixelBoundaries.push_back(std::make_pair(-1e-05, 1e-05));  //mean value of energy resolution
   fPixelBoundaries.push_back(std::make_pair(2e-06, 5e-06));   //sigma value of energy resolution
   fPixelBoundaries.push_back(std::make_pair(-2, 2));          //mean value of time resolution
   fPixelBoundaries.push_back(std::make_pair(2, 10));          //sigma value of time resolution

}

void PndMvdQATask::SetStripBoundaries()
{
   fStripBoundaries.push_back(std::make_pair(-1e-03,1e-03));   //mean value of small double gaussian fit
   fStripBoundaries.push_back(std::make_pair(2e-04, 1e-03));   //sigma value of small double gaussian fit
   fStripBoundaries.push_back(std::make_pair(-1e-03, 1e-03));  //mean value of wide double gaussian fit
   fStripBoundaries.push_back(std::make_pair(2e-04, 1e-02));   //sigma value of small double gaussian fit
   fStripBoundaries.push_back(std::make_pair(-1e-03, 1e-03));  //mean value of single hit distribution
   fStripBoundaries.push_back(std::make_pair(2e-04, 1e-02));   //rms of single hit distribution
   fStripBoundaries.push_back(std::make_pair(3, 4));           //mean value of cluster size
   fStripBoundaries.push_back(std::make_pair(0.8, 1.2));       //sigma value of cluster size
   fStripBoundaries.push_back(std::make_pair(-1e-05, 1e-05));  //mean value of energy resolution
   fStripBoundaries.push_back(std::make_pair(2e-06, 10e-06));   //sigma value of energy resolution
   fStripBoundaries.push_back(std::make_pair(-2, 2));          //mean value of time resolution
   fStripBoundaries.push_back(std::make_pair(2, 10));          //sigma value of time resolution

}

void PndMvdQATask::CalculateDifference(PndSdsHit* hit){
    fSpatialDifference.SetXYZ(0.0, 0.0, 0.0);
    fTimeDifference = 0.0;
    fEnergyDifference = 0.0;
    fProperDifference = kFALSE;

    if(fVerbose > 1) std::cout << *(FairMultiLinkedData_Interface*)hit << std::endl;
    FairMultiLinkedData mcLinks = hit->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDPoint"));
    if (fVerbose > 1) std::cout << mcLinks << std::endl;
    if (mcLinks.GetNLinks() == 1){
        PndSdsMCPoint* point = (PndSdsMCPoint*)FairRootManager::Instance()->GetCloneOfLinkData(mcLinks.GetLink(0));
        if (point != nullptr){
            TVector3 mcMaster = 0.5 * (point->GetPosition() + point->GetPositionOut());
            TVector3 mcLocal = fGeoH->MasterToLocalShortId(mcMaster, point->GetSensorID());
            TVector3 hitLocal = fGeoH->MasterToLocalShortId(hit->GetPosition(), hit->GetSensorID());
            fSpatialDifference = hitLocal - mcLocal;
            fEnergyDifference = hit->GetEloss() - point->GetEnergyLoss();
            fTimeDifference = hit->GetTimeStamp() - point->GetTime();
            if (fVerbose > 1){
                std::cout << "Hit: " << *hit << std::endl;
                std::cout << "McPoint: " << *point << std::endl;
                std::cout << "MCMaster: ";
                mcMaster.Print();
                std::cout << " MCLocal ";
                mcLocal.Print();
                std::cout << std::endl;
                std::cout << "hitLocal: ";
                hitLocal.Print();
                std::cout << " Difference: ";
                fSpatialDifference.Print();
                std::cout << std::endl;
            }
            fProperDifference = kTRUE;
            delete(point);
        }

    }
}


std::vector<Double_t> PndMvdQATask::DoubleGaussFit(TH1* hisDiff)
{
    Double_t par[6] = { 0., 0., 0.0005, 0., 0., 0.002 };
    //prefit peak
    TF1* g2 = new TF1("g2", "gaus", -0.002, 0.002);
    hisDiff->Fit(g2, "RQ");
    g2->GetParameters(&par[0]);
    //fit total
    TF1* total = new TF1("total", "gaus(0)+gaus(3)", -0.008, 0.008);
    total->SetParameters(par);
    total->SetLineColor(kRed);
    total->SetLineWidth(2);
    total->SetLineStyle(7);
    hisDiff->Fit(total, "RQ");
    total->GetParameters(&par[0]);
    std::vector<Double_t> result;
    for (int i = 0; i < 6; i++){
        result.push_back(par[i]);
    }
    total->DrawCopy();
    return result;
}

void PndMvdQATask::DrawPixelHistos(){
    TCanvas* can1 = new TCanvas("MvdResPlotPixel", "MVD point resolution pixel");
    can1->Divide(2,3);
    can1->cd(1);
    fHPixelAll->DrawCopy("pe");
    can1->cd(2);
    fHPixelSingle->DrawCopy("pe");
    can1->cd(3);
    fHPixelDouble->DrawCopy("pe");
    can1->cd(4);
    fHPixelClusterSize->DrawCopy();
    can1->cd(5);
    fHPixelEnergy->DrawCopy();
    can1->cd(6);
    fHPixelTime->DrawCopy();
    can1->SaveAs("PixelHistos.png");
}

void PndMvdQATask::DrawStripHistos(){
    TCanvas* can2 = new TCanvas("MvdResPlotStrip", "MVD point resolution strip");
    can2->Divide(2,3);
    can2->cd(1);
    fHStripAll->DrawCopy("pe");
    can2->cd(2);
    fHStripSingle->DrawCopy("pe");
    can2->cd(3);
    fHStripDouble->DrawCopy("pe");
    can2->cd(4);
    fHStripClusterSize->DrawCopy();
    can2->cd(5);
    fHStripEnergy->DrawCopy();
    can2->cd(6);
    fHStripTime->DrawCopy();
    can2->SaveAs("StripHistos.png");
}

bool PndMvdQATask::TestPixel(){
    bool result = false;
    if (fPixelSpatialRes[1] > fPixelBoundaries[0].first && fPixelSpatialRes[1] < fPixelBoundaries[0].second) result=true;
    else { result=false; std::cout << "Pixel small mean value error: " << fPixelBoundaries[0].first << " < " << fPixelSpatialRes[1] << " < " << fPixelBoundaries[0].second << std::endl;}
    if (fPixelSpatialRes[2] > fPixelBoundaries[1].first && fPixelSpatialRes[2] < fPixelBoundaries[1].second) result=true;
    else { result=false; std::cout << "Pixel small sigma value error: " << fPixelBoundaries[1].first << " < " << fPixelSpatialRes[2] << " < " << fPixelBoundaries[1].second << std::endl;}
    if (fPixelSpatialRes[4] > fPixelBoundaries[2].first && fPixelSpatialRes[4] < fPixelBoundaries[2].second) result=true;
    else { result=false; std::cout << "Pixel broad mean value error: " << fPixelBoundaries[2].first << " < " << fPixelSpatialRes[4] << " < " << fPixelBoundaries[2].second << std::endl;}
    if (fPixelSpatialRes[5] > fPixelBoundaries[3].first && fPixelSpatialRes[5] < fPixelBoundaries[3].second) result=true;
    else { result=false; std::cout << "Pixel broad mean value error: " << fPixelBoundaries[3].first << " < " << fPixelSpatialRes[5] << " < " << fPixelBoundaries[3].second << std::endl;}
    if (fHPixelSingle->GetMean() > fPixelBoundaries[4].first && fHPixelSingle->GetMean() < fPixelBoundaries[4].second) result=true;
    else { result=false; std::cout << "Single pixel mean value error: " << fPixelBoundaries[4].first << " < " << fHPixelSingle->GetMean() << " < " << fPixelBoundaries[4].second << std::endl;}
    if (fHPixelSingle->GetRMS() > fPixelBoundaries[5].first && fHPixelSingle->GetRMS() < fPixelBoundaries[5].second) result=true;
    else { result=false; std::cout << "Single pixel RMS value error: " << fPixelBoundaries[5].first << " < " << fHPixelSingle->GetMean() << " < " << fPixelBoundaries[5].second << std::endl;}
    if (fHPixelClusterSize->GetMean() > fPixelBoundaries[6].first && fHPixelClusterSize->GetMean() < fPixelBoundaries[6].second) result=true;
    else { result=false; std::cout << "Pixel cluster size mean error: " << fPixelBoundaries[6].first << " < " << fHPixelClusterSize->GetMean() << " < " << fPixelBoundaries[6].second << std::endl;}
    if (fHPixelClusterSize->GetRMS() > fPixelBoundaries[7].first && fHPixelClusterSize->GetRMS() < fPixelBoundaries[7].second) result=true;
    else { result=false; std::cout << "Pixel cluster size sigma error: " << fPixelBoundaries[7].first << " < " << fHPixelClusterSize->GetRMS() << " < " << fPixelBoundaries[7].second << std::endl;}
    if (fHPixelEnergy->GetMean() > fPixelBoundaries[8].first && fHPixelEnergy->GetMean() < fPixelBoundaries[8].second) result=true;
    else { result=false; std::cout << "Pixel energy mean error: " << fPixelBoundaries[8].first << " < " << fHPixelEnergy->GetMean() << " < " << fPixelBoundaries[8].second << std::endl;}
    if (fHPixelEnergy->GetRMS() > fPixelBoundaries[9].first && fHPixelEnergy->GetRMS() < fPixelBoundaries[9].second) result=true;
    else { result=false; std::cout << "Pixel energy cluster error: " << fPixelBoundaries[9].first << " < " << fHPixelEnergy->GetRMS() << " < " << fPixelBoundaries[9].second << std::endl;}
    if (fHPixelTime->GetMean() > fPixelBoundaries[10].first && fHPixelTime->GetMean() < fPixelBoundaries[10].second) result=true;
    else { result=false; std::cout << "Pixel energy mean error: " << fPixelBoundaries[10].first << " < " << fHPixelTime->GetMean() << " < " << fPixelBoundaries[10].second << std::endl;}
    if (fHPixelTime->GetRMS() > fPixelBoundaries[11].first && fHPixelTime->GetRMS() < fPixelBoundaries[11].second) result=true;
    else { result=false; std::cout << "Pixel energy cluster error: " << fPixelBoundaries[11].first << " < " << fHPixelTime->GetRMS() << " < " << fPixelBoundaries[11].second << std::endl;}

    return result;
}

bool PndMvdQATask::TestStrip(){
    bool result = false;
    if (fStripSpatialRes[1] > fStripBoundaries[0].first && fStripSpatialRes[1] < fStripBoundaries[0].second) result=true;
    else { result=false; std::cout << "Strip small mean value error: " << fStripBoundaries[0].first << " < " << fStripSpatialRes[1] << " < " << fStripBoundaries[0].second << std::endl;}
    if (fStripSpatialRes[2] > fStripBoundaries[1].first && fStripSpatialRes[2] < fStripBoundaries[1].second) result=true;
    else { result=false; std::cout << "Strip small sigma value error: " << fStripBoundaries[1].first << " < " << fStripSpatialRes[2] << " < " << fStripBoundaries[1].second << std::endl;}
    if (fStripSpatialRes[4] > fStripBoundaries[2].first && fStripSpatialRes[4] < fStripBoundaries[2].second) result=true;
    else { result=false; std::cout << "Strip broad mean value error: " << fStripBoundaries[2].first << " < " << fStripSpatialRes[4] << " < " << fStripBoundaries[2].second << std::endl;}
    if (fStripSpatialRes[5] > fStripBoundaries[3].first && fStripSpatialRes[5] < fStripBoundaries[3].second) result=true;
    else { result=false; std::cout << "Strip broad mean value error: " << fStripBoundaries[3].first << " < " << fStripSpatialRes[5] << " < " << fStripBoundaries[3].second << std::endl;}
    if (fHStripSingle->GetMean() > fStripBoundaries[4].first && fHStripSingle->GetMean() < fStripBoundaries[4].second) result=true;
    else { result=false; std::cout << "Single Strip mean value error: " << fStripBoundaries[4].first << " < " << fHStripSingle->GetMean() << " < " << fStripBoundaries[4].second << std::endl;}
    if (fHStripSingle->GetRMS() > fStripBoundaries[5].first && fHStripSingle->GetRMS() < fStripBoundaries[5].second) result=true;
    else { result=false; std::cout << "Single Strip RMS value error: " << fStripBoundaries[5].first << " < " << fHStripSingle->GetMean() << " < " << fStripBoundaries[5].second << std::endl;}
    if (fHStripClusterSize->GetMean() > fStripBoundaries[6].first && fHStripClusterSize->GetMean() < fStripBoundaries[6].second) result=true;
    else { result=false; std::cout << "Strip cluster size mean error: " << fStripBoundaries[6].first << " < " << fHStripClusterSize->GetMean() << " < " << fStripBoundaries[6].second << std::endl;}
    if (fHStripClusterSize->GetRMS() > fStripBoundaries[7].first && fHStripClusterSize->GetRMS() < fStripBoundaries[7].second) result=true;
    else { result=false; std::cout << "Strip cluster size sigma error: " << fStripBoundaries[7].first << " < " << fHStripClusterSize->GetRMS() << " < " << fStripBoundaries[7].second << std::endl;}
    if (fHStripEnergy->GetMean() > fStripBoundaries[8].first && fHStripEnergy->GetMean() < fStripBoundaries[8].second) result=true;
    else { result=false; std::cout << "Strip energy mean error: " << fStripBoundaries[8].first << " < " << fHStripEnergy->GetMean() << " < " << fStripBoundaries[8].second << std::endl;}
    if (fHStripEnergy->GetRMS() > fStripBoundaries[9].first && fHStripEnergy->GetRMS() < fStripBoundaries[9].second) result=true;
    else { result=false; std::cout << "Strip energy cluster error: " << fStripBoundaries[9].first << " < " << fHStripEnergy->GetRMS() << " < " << fStripBoundaries[9].second << std::endl;}
    if (fHStripTime->GetMean() > fStripBoundaries[10].first && fHStripTime->GetMean() < fStripBoundaries[10].second) result=true;
    else { result=false; std::cout << "Strip energy mean error: " << fStripBoundaries[10].first << " < " << fHStripTime->GetMean() << " < " << fStripBoundaries[10].second << std::endl;}
    if (fHStripTime->GetRMS() > fStripBoundaries[11].first && fHStripTime->GetRMS() < fStripBoundaries[11].second) result=true;
    else { result=false; std::cout << "Strip energy cluster error: " << fStripBoundaries[11].first << " < " << fHStripTime->GetRMS() << " < " << fStripBoundaries[11].second << std::endl;}

    return result;
}

void PndMvdQATask::PrintDartMeasurementPixel()
{
    cout << "<DartMeasurement name=\"pixelMean1\" type=\"numeric/double\">" << fPixelSpatialRes[1] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelSigma1\" type=\"numeric/double\">" << fPixelSpatialRes[2] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelMean2\" type=\"numeric/double\">" << fPixelSpatialRes[4] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelSigma2\" type=\"numeric/double\">" << fPixelSpatialRes[5] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelSingleMean\" type=\"numeric/double\">" << fHPixelSingle->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelSingleRMS\" type=\"numeric/double\">" << fHPixelSingle->GetRMS() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelClusterMean\" type=\"numeric/double\">" << fHPixelClusterSize->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelClusterRMS\" type=\"numeric/double\">" << fHPixelClusterSize->GetRMS() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelEnergyMean\" type=\"numeric/double\">" << fHPixelEnergy->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelEnergyRMS\" type=\"numeric/double\">" << fHPixelEnergy->GetRMS() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelTimeMean\" type=\"numeric/double\">" << fHPixelTime->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"pixelTimeRMS\" type=\"numeric/double\">" << fHPixelTime->GetRMS() << "</DartMeasurement>" << endl;

    TString path=fWorkdir;
    path += "/PixelHistos.png";
    cout << "<DartMeasurementFile name=\"PixelHistos.png\" type=\"image/png\">" << path.Data()<< "</DartMeasurementFile>" << endl;

}

void PndMvdQATask::PrintDartMeasurementStrip()
{
    cout << "<DartMeasurement name=\"stripMean1\" type=\"numeric/double\">" << fStripSpatialRes[1] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripSigma1\" type=\"numeric/double\">" << fStripSpatialRes[2] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripMean2\" type=\"numeric/double\">" << fStripSpatialRes[4] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripSigma2\" type=\"numeric/double\">" << fStripSpatialRes[5] << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripSingleMean\" type=\"numeric/double\">" << fHStripSingle->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripSingleRMS\" type=\"numeric/double\">" << fHStripSingle->GetRMS() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripClusterMean\" type=\"numeric/double\">" << fHStripClusterSize->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripClusterRMS\" type=\"numeric/double\">" << fHStripClusterSize->GetRMS() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripEnergyMean\" type=\"numeric/double\">" << fHStripEnergy->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripEnergyRMS\" type=\"numeric/double\">" << fHStripEnergy->GetRMS() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripTimeMean\" type=\"numeric/double\">" << fHStripTime->GetMean() << "</DartMeasurement>" << endl;
    cout << "<DartMeasurement name=\"stripTimeRMS\" type=\"numeric/double\">" << fHStripTime->GetRMS() << "</DartMeasurement>" << endl;

    TString path=fWorkdir;
    path += "/StripHistos.png";
    cout << "<DartMeasurementFile name=\"StripHistos.png\" type=\"image/png\">" << path.Data() << "</DartMeasurementFile>" << endl;


}

void PndMvdQATask::FinishTask(){

    fPixelSpatialRes = DoubleGaussFit(fHPixelDouble);
    fStripSpatialRes = DoubleGaussFit(fHStripDouble);

    DrawPixelHistos();
    bool testPixel = TestPixel();
    PrintDartMeasurementPixel();

    DrawStripHistos();
    bool testStrip = TestStrip();
    PrintDartMeasurementStrip();

    if (testPixel && testStrip) {
        std::cout << " Test passed" << std::endl;
        std::cout << " All ok " << std::endl;
    } else {
        std::cout<<"Something is wrong:"<<endl;
        std::cout<<"Test of Pixel: "<< ((testPixel) ? "ok" : "bad") <<std::endl;
        std::cout<<"Test of Strip: "<< ((testStrip) ? "ok" : "bad") <<std::endl;
    }

}

ClassImp(PndMvdQATask);
