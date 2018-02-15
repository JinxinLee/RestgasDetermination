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

#include "PndRadMapBoxMesh.h"

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

  // ifstream ifile("filecollectionn.dat");
  // string line;
  
  // mych.Add("blablabla2.root");
  mych.Add(inputfile.Data());

  Long64_t nentries = (Long64_t)mych.GetEntries();
  cout << "we have " << nentries << " events" << endl;
  TClonesArray *fRadMapPoint = new TClonesArray("FairRadMapPoint");
  mych.SetBranchAddress("RadMap", &fRadMapPoint);

  std::vector<PndRadMapBoxMesh*> BM;

  BM.push_back(new PndRadMapBoxMesh("EmcEdepXY",
                                    250,0.,250.,
                                    250,0.,250.,
                                    1, -40, 110));
  BM.at(BM.size()-1)->SetQuantity(Edep);
  BM.at(BM.size()-1)->SetOrientation(XY);
  
  BM.push_back(new PndRadMapBoxMesh("EmcAllFluenceZX",
                                    1, 50, 50,
                                    250,0.,250.,
                                    150, -40, 110));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(ZY, 14.3, Zz);
  BM.at(BM.size()-1)->SetVerbosityLevel(0);

  BM.push_back(new PndRadMapBoxMesh("EmcCFluenceZX",
                                    1, 50, 50,
                                    250,0.,250.,
                                    150, -40, 110));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(ZY, 14.3, Zz);
  BM.at(BM.size()-1)->SetFilter("(Ch!=0)");//charged

  BM.push_back(new PndRadMapBoxMesh("EmcCnepFluenceZX",
                                    1, 50, 50,
                                    250,0.,250.,
                                    150, -40, 110));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(ZY, 14.3, Zz);
  BM.at(BM.size()-1)->SetFilter("(Ch!=0) && (Pid!=11) && (Pid!=-11)");//charged but e+-

  BM.push_back(new PndRadMapBoxMesh("EmcPFluenceZX",
                                    1, 50, 50,
                                    250,0.,250.,
                                    150, -40, 110));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(ZY, 14.3, Zz);
  BM.at(BM.size()-1)->SetFilter("(Pid==2212)");//proton

  BM.push_back(new PndRadMapBoxMesh("EmcNFluenceZX",
                                    1, 50, 50,
                                    250,0.,250.,
                                    150, -40, 110));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(ZY, 14.3, Zz);
  BM.at(BM.size()-1)->SetFilter("(Pid==2112)");//neutron


  BM.push_back(new PndRadMapBoxMesh("EmcAllFluencePawelXY",
                           210, -105.  , 105.,
                           210, -105.  , 105,
                           1  , 207., 207.));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(XY);

  BM.push_back(new PndRadMapBoxMesh("EmcCFluencePawelXY",
                           210, -105.  , 105.,
                           210, -105.  , 105,
                           1  , 207., 207.));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(XY);
  BM.at(BM.size()-1)->SetFilter("(Ch!=0)");//charged (including e+-)

  BM.push_back(new PndRadMapBoxMesh("EmcCnepFluencePawelXY",
                           210, -105.  , 105.,
                           210, -105.  , 105,
                           1  , 207., 207.));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(XY);
  BM.at(BM.size()-1)->SetFilter("(Ch!=0) && (Pid!=11) && (Pid!=-11) ");//charged particles but not e+-

  BM.push_back(new PndRadMapBoxMesh("EmcPFluencePawelXY",
                           210, -105.  , 105.,
                           210, -105.  , 105,
                           1  , 207., 207.));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(XY);
  BM.at(BM.size()-1)->SetFilter("(Pid==2212)");//proton

  BM.push_back(new PndRadMapBoxMesh("EmcNFluencePawelXY",
                           210, -105.  , 105.,
                           210, -105.  , 105,
                           1  , 207., 207.));
  BM.at(BM.size()-1)->SetQuantity(Fluence);
  BM.at(BM.size()-1)->SetOrientation(XY);
  BM.at(BM.size()-1)->SetFilter("(Pid==2112)");//proton


 ///////////////////////////////////////////////////////
  
  // BM.push_back(new PndRadMapBoxMesh("EmcEdepXY",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , 207., 223.));
  // BM.at(BM.size()-1)->SetQuantity(Edep);
  // BM.at(BM.size()-1)->SetOrientation(XY);
  
  // BM.push_back(new PndRadMapBoxMesh("EmcTwosXY",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , 222., 223.));
  // BM.at(BM.size()-1)->SetQuantity(Twos);
  // BM.at(BM.size()-1)->SetOrientation(XY);
  
  // BM.push_back(new PndRadMapBoxMesh("EmcDoseXY",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , 222., 223.));
  // BM.at(BM.size()-1)->SetQuantity(Dose);
  // BM.at(BM.size()-1)->SetOrientation(XY);
  
  // BM.push_back(new PndRadMapBoxMesh("EmcTwoXY",
  //                          1000,   2  ,  3,
  //                          1000,  23.5, 24.5,
  //                          1   , 222. , 223.));
  // BM.at(BM.size()-1)->SetQuantity(Twos);
  // BM.at(BM.size()-1)->SetOrientation(XY);
  
  // BM.push_back(new PndRadMapBoxMesh("EmcTwoXZ",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , 222., 223.));
  // BM.at(BM.size()-1)->SetQuantity(Twos);
  // BM.at(BM.size()-1)->SetOrientation(XY);

  // BM.push_back(new PndRadMapBoxMesh("EmcTwoYZ",
  //                          1   ,   2  ,  3,
  //                          1000,  23.5, 24.5,
  //                          1000, 222. , 223.));
  // BM.at(BM.size()-1)->SetQuantity(Twos);
  // BM.at(BM.size()-1)->SetOrientation(YZ);
  // BM.push_back(new PndRadMapBoxMesh("EmcTwoXY",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , 222., 223.));
  // BM.at(BM.size()-1)->SetQuantity(Twos);
  // BM.at(BM.size()-1)->SetOrientation(XY);
  
  // BM.push_back(new PndRadMapBoxMesh("EmcMassXY",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , -40., 110.));
  // BM.at(BM.size()-1)->SetQuantity(Mass);
  // BM.at(BM.size()-1)->SetOrientation(XY);
  // BM.at(BM.size()-1)->SetVerbosityLevel(0);

  // BM.push_back(new PndRadMapBoxMesh("EmcDensityXY",
  //                          210, -105.  , 105.,
  //                          210, -105.  , 105,
  //                          1  , 222., 223.));
  // BM.at(BM.size()-1)->SetQuantity(Density);
  // BM.at(BM.size()-1)->SetOrientation(XY);


  // BM.push_back(new PndRadMapBoxMesh("EdepZX_1",
  //       			    250,0,250,
  //       			    1,-0.5,0.5,
  //       			    1200,-200,1000));
  // BM.at(BM.size()-1)->SetQuantity(Edep);
  // BM.at(BM.size()-1)->SetOrientation(ZX);

  // BM.push_back(new PndRadMapBoxMesh("MassZX_1",
  //       			    250,0,250,
  //       			    1,-0.5,0.5,
  //       			    1200,-200,1000));
  // BM.at(BM.size()-1)->SetQuantity(Mass);
  // BM.at(BM.size()-1)->SetOrientation(ZX);

  // BM.push_back(new PndRadMapBoxMesh("DoseZX_1",
  //       			    250,0,250,
  //       			    1,-0.5,0.5,
  //       			    1200,-200,1000));
  // BM.at(BM.size()-1)->SetQuantity(Dose);
  // BM.at(BM.size()-1)->SetOrientation(ZX);
 
  // BM.push_back(new PndRadMapBoxMesh("EdepZY_1",
  //       			    1,-0.5,0.5,
  //       			    250,0,250,
  //       			    1200,-200,1000));
  // BM.at(BM.size()-1)->SetQuantity(Edep);
  // BM.at(BM.size()-1)->SetOrientation(ZY);

  // BM.push_back(new PndRadMapBoxMesh("DoseZY_1",
  //       			    1,-0.5,0.5,
  //       			    250,0,250,
  //       			    1200,-200,1000));
  // BM.at(BM.size()-1)->SetQuantity(Dose);
  // BM.at(BM.size()-1)->SetOrientation(ZY);
  
  // BM.push_back(new PndRadMapBoxMesh("MassZY_1",
  //       			    1,-0.5,0.5,
  //       			    250,0,250,
  //       			    1200,-200,1000));
  // BM.at(BM.size()-1)->SetQuantity(Mass);
  // BM.at(BM.size()-1)->SetOrientation(ZY);





  for (Long64_t i=0; i<nentries; i++) {
    cout << "event " << i << endl;        
    mych.GetEntry(i);
    Int_t npoints = fRadMapPoint->GetEntries();

    cout << npoints << endl;
    // loop over points
    for (Int_t ii=0; ii < npoints; ii++) {

      //if (i==99)cout<<ii<<" "<<npoints<<endl;
      
      FairRadMapPoint *p= (FairRadMapPoint *) fRadMapPoint->At(ii);
      for(unsigned int iii = 0; iii < BM.size(); iii++){
        BM.at(iii)->Fill(p);
      }
    }
  }
  cout << "Save " << BM.size() << endl;        
  for(unsigned int i = 0; i < BM.size(); i++){
    BM.at(i)->Scale(1./nentries);//normalizing it to 1 event!
    BM.at(i)->Save(fout);
  }
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
