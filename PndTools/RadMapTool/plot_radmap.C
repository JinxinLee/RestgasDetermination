#ifndef __CINT__

#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "THnSparse.h"
#include "TVector3.h"

#include "FairRadMapPoint.h"

#include <fstream>
#include <string>
#include <cmath>

//for the units
#include <G4PhysicalConstants.hh>
#include <G4SystemOfUnits.hh>

#include "Mesh.h"

#endif
using namespace std;

void plot_radmap(Long64_t nevreq=1000, 
                 TString inputfile = "RadMap_Out_Sim.root", 
                 TString outputfile="RadMap_Out.root");
int main(){
  plot_radmap();
}
// macro reads from the RadMap Branch
// and fills histograms
void plot_radmap(Long64_t nevreq, 
                 TString inputfile, 
                 TString outputfile)
{
  // open an output file
  TFile *fout = new TFile(outputfile,"RECREATE");

  // chain for the cbmsim tree in the simulation files
  TChain mych("cbmsim");

  ifstream ifile("filecollectionn.dat");
  string line;
  
  mych.Add("blablabla2.root");

  Long64_t nentries = (Long64_t)mych.GetEntries();
  cout << "we have " << nentries << " events" << endl;
  TClonesArray *fRadMapPoint = new TClonesArray("FairRadMapPoint");
  mych.SetBranchAddress("RadMap", &fRadMapPoint);
  
  // BoxMesh* FullEdepZX = new BoxMesh("FullEdepZX",
  //                                   250,0.,250.,
  //                                   1, -0.5, 0.5,
  //                                   1200,-200.,1000.);
  // FullEdepZX->SetQuantity(Edep);
  // FullEdepZX->SetOrientation(ZX);

  // BoxMesh* FullEdepZXa = new BoxMesh("FullEdepZXa",
  //                                    250,0.,250.,
  //                                    1, -0.5, 0.5,
  //                                    440,-200.,240.);
  // FullEdepZXa->SetQuantity(Edep);
  // FullEdepZXa->SetOrientation(ZX);

  // BoxMesh* FullEdepZXb = new BoxMesh("FullEdepZXb",
  //                                    250,0.,250.,
  //                                    1, -0.5, 0.5,
  //                                    760, 240.,1000.);
  // FullEdepZXb->SetQuantity(Edep);
  // FullEdepZXb->SetOrientation(ZX);

  // BoxMesh* FullEdepZXc = new BoxMesh("FullEdepZXc",
  //                                    200,0.,20.,
  //                                    1, -0.5, 0.5,
  //                                    1000,-100.,400.);
  // FullEdepZXc->SetQuantity(Edep);
  // FullEdepZXc->SetOrientation(ZX);

  // BoxMesh* FullDoseZX = new BoxMesh("FullDoseZX",
  //                                   250,0.,250.,
  //                                   1, -0.5, 0.5,
  //                                   1200,-200.,1000.);
  // FullDoseZX->SetQuantity(Dose);
  // FullDoseZX->SetOrientation(ZX);

  // BoxMesh* FullDoseZXa = new BoxMesh("FullDoseZXa",
  //                                    250,0.,250.,
  //                                    1, -0.5, 0.5,
  //                                    440,-200.,240.);
  // FullDoseZXa->SetQuantity(Dose);
  // FullDoseZXa->SetOrientation(ZX);

  // BoxMesh* FullDoseZXb = new BoxMesh("FullDoseZXb",
  //                                    250,0.,250.,
  //                                    1, -0.5, 0.5,
  //                                    760, 240.,1000.);
  // FullDoseZXb->SetQuantity(Dose);
  // FullDoseZXb->SetOrientation(ZX);


  // BoxMesh* FullDoseZXc = new BoxMesh("FullDoseZXc",
  //                                    200,0.,20.,
  //                                    1, -0.5, 0.5,
  //                                    1000,-100.,400.);
  // FullDoseZXc->SetQuantity(Dose);
  // FullDoseZXc->SetOrientation(ZX);

  // BoxMesh* FullDoseZY = new BoxMesh("FullDoseZY",
  //                                   1, -0.5, 0.5,
  //                                   250,0.,250.,
  //                                   1200,-200.,1000.);
  // FullDoseZY->SetQuantity(Dose);
  // FullDoseZY->SetOrientation(ZY);

  // BoxMesh* FullDoseZYa = new BoxMesh("FullDoseZYa",
  //                                    1, -0.5, 0.5,
  //                                    250,0.,250.,
  //                                    440,-200.,240.);
  // FullDoseZYa->SetQuantity(Dose);
  // FullDoseZYa->SetOrientation(ZY);

  // BoxMesh* FullDoseZYb = new BoxMesh("FullDoseZYb",
  //                                    1, -0.5, 0.5,
  //                                    250,0.,250.,
  //                                    760,240.,1000.);
  // FullDoseZYb->SetQuantity(Dose);
  // FullDoseZYb->SetOrientation(ZY);

  // BoxMesh* FullDoseZYc = new BoxMesh("FullDoseZYc",
  //                                    1, -0.5, 0.5,
  //                                    200,0.,20.,
  //                                    1000,-100.,400.);
  // FullDoseZYc->SetQuantity(Dose);
  // FullDoseZYc->SetOrientation(ZY);



  // BoxMesh* NeutronEdepZX = new BoxMesh("NeutronEdepZX",
  //                                      250,0.,250.,
  //                                      1, -0.5, 0.5,
  //                                      1200,-200.,1000.);
  // NeutronEdepZX->SetQuantity(Edep);
  // NeutronEdepZX->SetFilter("(Pid==2212)");
  // NeutronEdepZX->SetOrientation(ZX);

  // BoxMesh* NeutronEdepZXa = new BoxMesh("NeutronEdepZXa",
  //                                       250,0.,250.,
  //                                       1, -0.5, 0.5,
  //                                       440,-200.,240.);
  // NeutronEdepZXa->SetQuantity(Edep);
  // NeutronEdepZXa->SetFilter("(Pid==2212)");
  // NeutronEdepZXa->SetOrientation(ZX);

  // BoxMesh* NeutronEdepZXb = new BoxMesh("NeutronEdepZXb",
  //                                       250,0.,250.,
  //                                       1, -0.5, 0.5,
  //                                       760,240.,1000.);
  // NeutronEdepZXb->SetQuantity(Edep);
  // NeutronEdepZXb->SetFilter("(Pid==2212)");
  // NeutronEdepZXb->SetOrientation(ZX);

  // BoxMesh* NeutronEdepZXc = new BoxMesh("NeutronEdepZXc",
  //                                       200,0.,20.,
  //                                       1, -0.5, 0.5,
  //                                       100,-100.,400.);
  // NeutronEdepZXc->SetQuantity(Edep);
  // NeutronEdepZXc->SetFilter("(Pid==2212)");
  // NeutronEdepZXc->SetOrientation(ZX);


  // BoxMesh* NeutronDoseZX = new BoxMesh("NeutronDoseZX",
  //                                      250,0.,250.,
  //                                      1, -0.5, 0.5,
  //                                      1200,-200.,1000.);
  // NeutronDoseZX->SetQuantity(Dose);
  // NeutronDoseZX->SetFilter("(Pid==2212)");
  // NeutronDoseZX->SetOrientation(ZX);

  // BoxMesh* NeutronDoseZXa = new BoxMesh("NeutronDoseZXa",
  //                                       250,0.,250.,
  //                                       1, -0.5, 0.5,
  //                                       440,-200.,240.);
  // NeutronDoseZXa->SetQuantity(Dose);
  // NeutronDoseZXa->SetFilter("(Pid==2212)");
  // NeutronDoseZXa->SetOrientation(ZX);

  // BoxMesh* NeutronDoseZXb = new BoxMesh("NeutronDoseZXb",
  //                                       250,0.,250.,
  //                                       1, -0.5, 0.5,
  //                                       760,240.,1000.);
  // NeutronDoseZXb->SetQuantity(Dose);
  // NeutronDoseZXb->SetFilter("(Pid==2212)");
  // NeutronDoseZXb->SetOrientation(ZX);

  // BoxMesh* NeutronDoseZXc = new BoxMesh("NeutronDoseZXc",
  //                                       200,0.,20.,
  //                                       1, -0.5, 0.5,
  //                                       1000,-100.,400.);
  // NeutronDoseZXc->SetQuantity(Dose);
  // NeutronDoseZXc->SetFilter("(Pid==2212)");
  // NeutronDoseZXc->SetOrientation(ZX);


  // BoxMesh* FullEdepSciLi2XY0 = new BoxMesh("FullEdepSciLi2XY0",
  //                                          1100, -30.,80.,
  //                                          1100, -30, 80,
  //                                          1,-40.,110.);
  // FullEdepSciLi2XY0->SetQuantity(Edep);
  // FullEdepSciLi2XY0->SetOrientation(XY);

  // BoxMesh* FullEdepSciLi2XY1 = new BoxMesh("FullEdepSciLi2XY1",
  //                                          1100, -30.,80.,
  //                                          1100, -30, 80,
  //                                          1,-40.,110.);
  // FullEdepSciLi2XY1->SetQuantity(Edep);
  // FullEdepSciLi2XY1->SetOrientation(XY, 14.3, Zz);

  // BoxMesh* FullEdepSciLi2XY2 = new BoxMesh("FullEdepSciLi2XY2",
  //                                          1100,  -30.,80.,
  //                                          900, 40, 50,
  //                                          1,-40.,110.);
  // FullEdepSciLi2XY2->SetQuantity(Edep);
  // FullEdepSciLi2XY2->SetOrientation(XY, 14.3, Zz);


  // BoxMesh* FullEdepSciLi2XY3 = new BoxMesh("FullEdepSciLi2XY3",
  //                                          180,  -9.,9.,
  //                                          32, 46, 49.2,
  //                                          1,-40.,110.);
  // FullEdepSciLi2XY3->SetQuantity(Edep);
  // FullEdepSciLi2XY3->SetOrientation(XY, 14.3, Zz);


  // BoxMesh* FullEdepSciLi2XY4 = new BoxMesh("FullEdepSciLi2XY4",
  //                                          180,  -9.,9.,
  //                                          22, 46.5, 48.7,
  //                                          1,-40.,110.);
  // FullEdepSciLi2XY4->SetQuantity(Edep);
  // FullEdepSciLi2XY4->SetOrientation(XY, 14.3, Zz);


  // BoxMesh* FullEdepSciLi0 = new BoxMesh("FullEdepSciLi0",
  //                                       300,-50.,250.,
  //                                       1, 46, 49.2,
  //                                       400,-200.,200.);
  // FullEdepSciLi0->SetQuantity(Edep);
  // FullEdepSciLi0->SetOrientation(ZX, 14.3, Zz);


  // BoxMesh* FullDoseSciLi0 = new BoxMesh("FullDoseSciLi0",
  //                                       300,-50.,250.,
  //                                       1, 46, 49.2,
  //                                       400,-200.,200.);
  // FullDoseSciLi0->SetQuantity(Dose);
  // FullDoseSciLi0->SetOrientation(ZX, 14.3, Zz);


  // BoxMesh* FullEdepSciLi1 = new BoxMesh("FullEdepSciLi1",
  //                                       300,-50.,250.,
  //                                       1, 46.5, 48.7,
  //                                       400,-200.,200.);
  // FullEdepSciLi1->SetQuantity(Edep);
  // FullEdepSciLi1->SetOrientation(ZX, 14.3, Zz);

  // BoxMesh* DensitySciLi1 = new BoxMesh("DensitySciLi1",
  //                                      300,-50.,250.,
  //                                      1, 46.5, 48.7,
  //                                      400,-200.,200.);
  // DensitySciLi1->SetQuantity(Density);
  // DensitySciLi1->SetOrientation(ZX, 14.3, Zz);


  // BoxMesh* FullDoseSciLi1 = new BoxMesh("FullDoseSciLi1",
  //                                       300,-50.,250.,
  //                                       1, 46.5, 48.7,
  //                                       400,-200.,200.);
  // FullDoseSciLi1->SetQuantity(Dose);
  // FullDoseSciLi1->SetOrientation(ZX, 14.3, Zz);


  // BoxMesh* FullDoseXY0 = new BoxMesh("FullDoseXY0",
  //                                          1100, -30.,80.,
  //                                          1100, -30, 80,
  //                                          1,-40.,110.);
  // FullDoseXY0->SetQuantity(Dose);
  // FullDoseXY0->SetOrientation(XY);





  // BoxMesh* FullDoseFP0 = new BoxMesh("FullDoseFP0",
  //                                    900, 10.,100.,
  //                                    1, -1, 1,
  //                                    225,203.5,226.);
  // FullDoseFP0->SetQuantity(Dose);
  // FullDoseFP0->SetOrientation(ZX);

  // BoxMesh* FullDoseFP1 = new BoxMesh("FullDoseFP1",
  //                                    1, -1, 1,
  //                                    900, 10.,100.,
  //                                    225,203.5,226.);
  // FullDoseFP1->SetQuantity(Dose);
  // FullDoseFP1->SetOrientation(ZY);

  // BoxMesh* FullDoseFPXY = new BoxMesh("FullDoseFPXY",
  //                                     1000, 0.,100.,
  //                                     1000, 0, 100,
  //                                     1,208.5,209.5);
  //                                     // 1,203.5,226.);
  // FullDoseFPXY->SetQuantity(Dose);
  // FullDoseFPXY->SetOrientation(XY);

  // BoxMesh* FullDoseFPXY2 = new BoxMesh("FullDoseFPXY2",
  //                                      100, 0.,10.,
  //                                      140, 16, 30,
  //                                      1,208.5,209.5);
  //                                       // 1,203.5,226.);
  // FullDoseFPXY2->SetQuantity(Dose);
  // FullDoseFPXY2->SetOrientation(XY);

  // BoxMesh* FullDoseFPXY3 = new BoxMesh("FullDoseFPXY3",
  //                                      100, 0.,10.,
  //                                      140, 16, 30,
  //                                      1,208.,210.);
  //                                       // 1,203.5,226.);
  // FullDoseFPXY3->SetQuantity(Dose);
  // FullDoseFPXY3->SetOrientation(XY);

  // BoxMesh* FullDoseFPXY4 = new BoxMesh("FullDoseFPXY4",
  //                                      100, 0.,10.,
  //                                      140, 16, 30,
  //                                      // 1,208.,210.);
  //                                      1,203.5,226.);
  // FullDoseFPXY4->SetQuantity(Dose);
  // FullDoseFPXY4->SetOrientation(XY);
  
  // BoxMesh* MassFPXY = new BoxMesh("MassFPXY",
  //                                 1000, 0.,100.,
  //                                 1000, 0, 100,
  //                                 1,208.5,209.5);
  //                                     // 1,203.5,226.);
  // MassFPXY->SetQuantity(Mass);
  // MassFPXY->SetOrientation(XY);

  // BoxMesh* MassFPXY2 = new BoxMesh("MassFPXY2",
  //                                  100, 0.,10.,
  //                                  140, 16, 30,
  //                                  1,208.5,209.5);
  //                                     // 1,203.5,226.);
  // MassFPXY2->SetQuantity(Mass);
  // MassFPXY2->SetOrientation(XY);

  
  // BoxMesh* EnergyFPXY = new BoxMesh("EnergyFPXY",
  //                                   100, 0.,10.,
  //                                   140, 16, 30,
  //                                   1,208.5,209.5);
  // EnergyFPXY->SetQuantity(Edep);
  // EnergyFPXY->SetOrientation(XY);

  BoxMesh* fluence = new BoxMesh("fluence",
                                 10, 0.,10.,
                                 14, 16, 30,
                                 1,208.5,209.5);
  fluence->SetQuantity(Fluence);
  // fluence->SetFilter("(Pid==2212)");
  fluence->SetFilter("(Pid==1)");
  fluence->SetOrientation(XY);

  for (Long64_t i=0; i<nentries; i++) {

    cout << "event " << i << endl;
        
    mych.GetEntry(i);
    Int_t npoints = fRadMapPoint->GetEntries();

    cout << npoints << endl;
    // loop over points
    for (Int_t ii=0; ii < npoints; ii++) {
      
      FairRadMapPoint *p= (FairRadMapPoint *) fRadMapPoint->At(ii);
      // FullEdepZX->Fill(p);
      // FullEdepZXa->Fill(p);
      // FullEdepZXb->Fill(p);
      // FullEdepZXc->Fill(p);
      // FullDoseZX->Fill(p);
      // FullDoseZXa->Fill(p);
      // FullDoseZXb->Fill(p);
      // FullDoseZXb->Fill(p);
      // FullDoseZXc->Fill(p);
      // FullDoseZY->Fill(p);
      // FullDoseZYa->Fill(p);
      // FullDoseZYb->Fill(p);
      // FullDoseZYc->Fill(p);
      // NeutronEdepZX->Fill(p);
      // NeutronEdepZXa->Fill(p);
      // NeutronEdepZXb->Fill(p);
      // NeutronEdepZXc->Fill(p);
      // NeutronDoseZX->Fill(p);
      // NeutronDoseZXa->Fill(p);
      // NeutronDoseZXb->Fill(p);
      // NeutronDoseZXc->Fill(p);

      // FullEdepSciLi2XY0->Fill(p);
      // FullEdepSciLi2XY1->Fill(p);
      // FullEdepSciLi2XY2->Fill(p);
      // FullEdepSciLi2XY3->Fill(p);
      // FullEdepSciLi2XY4->Fill(p);
      // FullDoseXY0->Fill(p);
      
      // FullEdepSciLi0->Fill(p);
      // FullDoseSciLi0->Fill(p);
      // FullEdepSciLi1->Fill(p);
      // FullDoseSciLi1->Fill(p);

      // DensitySciLi1->Fill(p);
      
      // FullDoseFP0->Fill(p);
      // FullDoseFP1->Fill(p);
      // FullDoseFPXY->Fill(p);
      // FullDoseFPXY2->Fill(p);
      // FullDoseFPXY3->Fill(p);
      // FullDoseFPXY4->Fill(p);
      // MassFPXY->Fill(p);
      // MassFPXY2->Fill(p);
      // EnergyFPXY->Fill(p);

      fluence->Fill(p);
    }
  }


  // FullEdepSciLi2XY0->Scale(1./nentries);
  // FullEdepSciLi2XY1->Scale(1./nentries);
  // FullEdepSciLi2XY2->Scale(1./nentries);
  // FullEdepSciLi2XY3->Scale(1./nentries);
  // FullEdepSciLi2XY4->Scale(1./nentries);
  
  // FullEdepSciLi0->Scale(1./nentries);
  // FullDoseSciLi0->Scale(1./nentries);
  // FullEdepSciLi1->Scale(1./nentries);
  // FullDoseSciLi1->Scale(1./nentries);

  // FullEdepZX->Scale(1./nentries);
  // FullEdepZXa->Scale(1./nentries);
  // FullEdepZXb->Scale(1./nentries);
  // FullEdepZXc->Scale(1./nentries);
  // FullDoseZX->Scale(1./nentries);
  // FullDoseZXa->Scale(1./nentries);
  // FullDoseZXb->Scale(1./nentries);
  // FullDoseZXc->Scale(1./nentries);
  // FullDoseZY->Scale(1./nentries);
  // FullDoseZYa->Scale(1./nentries);
  // FullDoseZYb->Scale(1./nentries);
  // FullDoseZYc->Scale(1./nentries);
  // NeutronEdepZX->Scale(1./nentries);
  // NeutronEdepZXa->Scale(1./nentries);
  // NeutronEdepZXb->Scale(1./nentries);
  // NeutronEdepZXc->Scale(1./nentries);
  // NeutronDoseZX->Scale(1./nentries);
  // NeutronDoseZXa->Scale(1./nentries);
  // NeutronDoseZXb->Scale(1./nentries);
  // NeutronDoseZXc->Scale(1./nentries);
  // FullDoseXY0->Scale(1./nentries);

  // FullDoseFP0->Scale(1./nentries);
  // FullDoseFP1->Scale(1./nentries);
  // FullDoseFPXY->Scale(1./nentries);
  // FullDoseFPXY2->Scale(1./nentries);
  // FullDoseFPXY3->Scale(1./nentries);
  // FullDoseFPXY4->Scale(1./nentries);

  // MassFPXY->Scale(1./nentries);
  // MassFPXY2->Scale(1./nentries);
  // EnergyFPXY->Scale(1./nentries);

  // FullEdepSciLi2XY0->Save();
  // FullEdepSciLi2XY1->Save();
  // FullEdepSciLi2XY2->Save();
  // FullEdepSciLi2XY3->Save();
  // FullEdepSciLi2XY4->Save();
  
  // FullEdepSciLi0->Save();
  // FullDoseSciLi0->Save();
  // FullEdepSciLi1->Save();
  // FullDoseSciLi1->Save();

  // DensitySciLi1->Save();


  // FullEdepZX->Save();
  // FullEdepZXa->Save();
  // FullEdepZXb->Save();
  // FullEdepZXc->Save();
  // FullDoseZX->Save();
  // FullDoseZXa->Save();
  // FullDoseZXb->Save();
  // FullDoseZXc->Save();
  // FullDoseZY->Save();
  // FullDoseZYa->Save();
  // FullDoseZYb->Save();
  // FullDoseZYc->Save();
  // NeutronEdepZX->Save();
  // NeutronEdepZXa->Save();
  // NeutronEdepZXb->Save();
  // NeutronDoseZX->Save();
  // NeutronDoseZXa->Save();
  // NeutronDoseZXb->Save();
  // NeutronDoseZXc->Save();
  // FullDoseXY0->Save();

  // FullDoseFP0->Save();
  // FullDoseFP1->Save();
  // FullDoseFPXY->Save();
  // FullDoseFPXY2->Save();
  // FullDoseFPXY3->Save();
  // FullDoseFPXY4->Save();
  // MassFPXY->Save();
  // MassFPXY2->Save();
  // EnergyFPXY->Save();

  fluence->Save();

}


void Fill(TH2D& h2, Double_t d, Double_t alpha){
}

#ifdef __CINT__
void plot(TH2D* h2, double scale, double rmin, double rmax){

  h2->Scale(scale);

  TPaletteAxis *palette = new TPaletteAxis(h2->GetXaxis()->GetBinCenter(h2->GetNbinsX()), 0,
                                           h2->GetXaxis()->GetBinCenter(h2->GetNbinsX())+10,
                                           h2->GetYaxis()->GetBinCenter(h2->GetNbinsY()),h2);
  palette->SetLabelColor(1);
  palette->SetLabelFont(42);
  palette->SetLabelOffset(0.005);
  palette->SetLabelSize(0.035);
  palette->SetTitleOffset(1);
  palette->SetTitleSize(0.035);
  palette->SetFillColor(100);
  palette->SetFillStyle(1001);
  h2->GetListOfFunctions()->Add(palette,"br");


  cout << h2->GetMinimum(0) << ' ' << h2->GetMaximum() << endl;

  if((rmin > 0) || (rmax > 0))
    h2->GetZaxis()->SetRangeUser(rmin, rmax);
  else
    h2->GetZaxis()->SetRangeUser(h2->GetMinimum(0), h2->GetMaximum());
  h2->SetContour(99);
  h2->Draw("colz");
}
#endif
