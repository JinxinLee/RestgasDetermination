#include "histodivider.hh"

using namespace std;

// int main(const char* filename="Geant4/RadMap_Out.root"){
int main(){
  const char* filename="Geant4/RadMap_Out.root";
  gStyle->SetOptStat(0);

  TFile* file = new TFile(filename);

  TH2D* signal = NULL;
  TH2D* normal = NULL;
  std::vector<TH2D*> H;


  
  TFile* ofile = new TFile("figures.root", "RECREATE");
  // file->ls();
  Double_t norm = 1000;
/*

  TDirectory* dir = NULL;
  TH2D* htemps = NULL;
  TH2D* htempb = NULL;

  dir = (TDirectory*)file->Get("EmcAllFluenceZX_ZY_14.3");
  htemps = (TH2D*)dir->Get("EmcAllFluenceZX_Histo_ZY_14.3");
  htempb = (TH2D*)dir->Get("EmcAllFluenceZX_StatHisto_ZY_14.3");
  cout << "1 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;


  


  

  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  "Z [cm]", "Y [cm]",
  "Full particle fluence (at X=50cm) [1/cm^2]",
  false, false, true,
  -1, -1,
  "figures.pdf"));



  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcNFluenceZX_ZY_14.3");
  htemps = (TH2D*)dir->Get("EmcNFluenceZX_Histo_ZY_14.3");
  htempb = (TH2D*)dir->Get("EmcNFluenceZX_StatHisto_ZY_14.3");

  cout << "2 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  "Z [cm]", "Y [cm]",
  "Neutron fluence (at X=50cm) [1/cm^2]",
  false, false, true,
  -1, -1,
  "figures.pdf"));



  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcPFluenceZX_ZY_14.3");
  htemps = (TH2D*)dir->Get("EmcPFluenceZX_Histo_ZY_14.3");
  htempb = (TH2D*)dir->Get("EmcPFluenceZX_StatHisto_ZY_14.3");

  cout << "3 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  "Z [cm]", "Y [cm]",
  "Proton fluence (at X=50cm) [1/cm^2]",
  false, false, true,
  -1, -1,
  "figures.pdf"));



  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcAllFluencePawelXY_XY_0");
  htemps = (TH2D*)dir->Get("EmcAllFluencePawelXY_Histo_XY_0");
  htempb = (TH2D*)dir->Get("EmcAllFluencePawelXY_StatHisto_XY_0");

  cout << "4 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.02e7/norm,
  "Z [cm]", "Y [cm]",
  "All Particle fluence (at X=50cm) [1/cm^2/event]",
  false, false, true,
  -1, -1,
  "figures.pdf"));
  Quater(H);



  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcCFluencePawelXY_XY_0");
  htemps = (TH2D*)dir->Get("EmcCFluencePawelXY_Histo_XY_0");
  htempb = (TH2D*)dir->Get("EmcCFluencePawelXY_StatHisto_XY_0");

  cout << "5 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.02e7/norm,
  "Z [cm]", "Y [cm]",
  "Charged fluence (at X=50cm) [1/cm^2/event]",
  false, false, true,
  -1, -1,
  "figures.pdf"));
  Quater(H);



  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcCnepFluencePawelXY_XY_0");
  htemps = (TH2D*)dir->Get("EmcCnepFluencePawelXY_Histo_XY_0");
  htempb = (TH2D*)dir->Get("EmcCnepFluencePawelXY_StatHisto_XY_0");

  cout << "6 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.02e7/norm,
  "Z [cm]", "Y [cm]",
  "Charged fluence (without e^{\pm})(at X=50cm) [1/cm^2/events]",
  false, false, true,
  -1, -1,
  "figures.pdf"));
  Quater(H);



  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcPFluencePawelXY_XY_0");
  htemps = (TH2D*)dir->Get("EmcPFluencePawelXY_Histo_XY_0");
  htempb = (TH2D*)dir->Get("EmcPFluencePawelXY_StatHisto_XY_0");

  cout << "7 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.02e7/norm,
  "Z [cm]", "Y [cm]",
  "Proton fluence (at X=50cm) [1/cm^2/events]",
  false, false, true,
  -1, -1,
  "figures.pdf"));
  Quater(H);




  delete htemps;
  delete htempb;
  delete dir;

  dir = (TDirectory*)file->Get("EmcNFluencePawelXY_XY_0");
  htemps = (TH2D*)dir->Get("EmcNFluencePawelXY_Histo_XY_0");
  htempb = (TH2D*)dir->Get("EmcNFluencePawelXY_StatHisto_XY_0");

  cout << "8 - " << dir << ' ' << htemps << ' ' << htempb 
  << endl;
  
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.61e14/norm,
  // H.push_back(plot(htemps  ,htempb  , c1 , 0, 1./norm,
  H.push_back(plot(htemps  ,htempb  , c1 , 0, 1.02e7/norm,
  "Z [cm]", "Y [cm]",
  "Neutron fluence (at X=50cm) [1/cm^2/events]",
  false, false, true,
  -1, -1,
  "figures.pdf"));
  Quater(H);




//-------------------
//reaction rate ~1e7/s
//-------------------

ofile->Write();
*/
  std::vector<TObjArray*> vto = ReadNames("names.dat");
  std::cout << "in the file names.dat there eare " << vto.size() << " valid entries\n";
  std::cout << vto.at(0)->GetSize() << std::endl;
  // for(int i = 0; i < vto.at(0)->GetSize(); i++)
  //   std::cout << i << ' ' << vto.at(0)->At(i) << std::endl;
  
  TIter next(file->GetListOfKeys());
  TKey *key;
  std::vector<TDirectory*> Dirs;
  
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    // if (cl->InheritsFrom("TH2")){
    if (cl->InheritsFrom("TDirectory")){
      Dirs.push_back((TDirectory*)key->ReadObj());
    }
  }
  std::cout << "There are " << Dirs.size() << " directories in the file\n";


  // std::string E = exec("find Geant4.15GeV/ -type f -name \"*Sim*.root\"  | wc -l");
  // Int_t norm = atoi(E.c_str());

  printf("%i file are created\n", norm);  
  Int_t u = 0;


  for(unsigned int di = 0; di < Dirs.size(); di++){
    TDirectory* dir = Dirs.at(di);
    TString TS(dir->GetName());
    unsigned int si = 9999;
    for(unsigned int ii = 0; ii < vto.size(); ii++){
      if(vto.at(ii)->At(0)){
        // cout << di << '\t' << ii << '\t'
        //      << TS.Data() << '\t'
        //      << (dynamic_cast<TObjString*>(vto.at(ii)->At(0)))->String()// << " - " << (TS == ((TObjString)(vto.at(ii)->At(0))).String())
        //      << endl;
        if(TS == (dynamic_cast<TObjString*>(vto.at(ii)->At(0)))->String()) si = ii;
      }
    }
    std::cout << TS.Data() << ' ' << si << endl;    
    if(si < 9999){
      TObjArray* TOA = TS.Tokenize("_");
      TString Sbegin = (dynamic_cast<TObjString*>(TOA->At(0)))->String();
      TString Send = (dynamic_cast<TObjString*>(TOA->At(1)))->String();
      Send += "_";
      Send += (dynamic_cast<TObjString*>(TOA->At(2)))->String();
      // std::cout << Sbegin.Data() << ' ' << Send.Data() << endl;
      TString HName = Sbegin + "_Histo_" + Send;
      TString SHName = Sbegin + "_StatHisto_" + Send;
      // std::cout << HName.Data() << ' ' << SHName.Data() << endl;

      signal = (TH2D*)dir->Get(HName.Data());
      normal = (TH2D*)dir->Get(SHName.Data());


      if(signal && normal){
        // cout << atof((dynamic_cast<TObjString*>(vto.at(si)->At(4)))->String()) << ' '
        //      << (dynamic_cast<TObjString*>(vto.at(si)->At(2)))->String() << ' '
        //      << (dynamic_cast<TObjString*>(vto.at(si)->At(3)))->String() << ' '
        //      << (dynamic_cast<TObjString*>(vto.at(si)->At(1)))->String() << ' '
        //      << endl;

        TH2D* h2 = plot(signal  ,normal  , c1 , 0, 
                        atof((dynamic_cast<TObjString*>(vto.at(si)->At(4)))->String())/norm,
                        (dynamic_cast<TObjString*>(vto.at(si)->At(2)))->String(),
                        (dynamic_cast<TObjString*>(vto.at(si)->At(3)))->String(),
                        (dynamic_cast<TObjString*>(vto.at(si)->At(1)))->String(),
                        false, false, true,
                        -1, -1,
                        "figures.pdf");
          
        H.push_back(h2);
//       H.at(H.size()-1)->Write();
        delete signal;
        signal = NULL;
        delete normal;
        normal = NULL;
      }
    }
  }
  ofile->Write();
  return 0;
}

// H.push_back(plot(signal  ,normal  , c1 , 0, 1.61e14/1000.,
// H.push_back(plot(signal  ,normal  , c1 , 0, 
//                  atoi(((TObjString)vto.at(si)->At(4)).String().Data()),
//                  ((TObjString)vto.at(si)->At(2)).String().Data(),
//                  ((TObjString)vto.at(si)->At(3)).String().Data(),
//                  ((TObjString)vto.at(si)->At(1)).String().Data(),
//                  false, false, true,
//                  -1, -1,
//                  "figures.pdf"));
//       if(TString(h->GetName()).Contains("endcap")){
//         Quater(H);
//       }
/*  

  while ((key = (TKey*)next())) {
    cout << "pitty\n";
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (cl->InheritsFrom("TDirectory")){
      TDirectory* dir = (TDirectory*)key->ReadObj();
      TString TS(dir->GetName());
      unsigned int si = 0;
      for(int ii = 0; ii < vto.size(); ii++){
        if(vto.at(si)->At(0)){
          if(TS = ((TObjString)(vto.at(si)->At(0))).String()) si = ii;            
        }
      }
      // while(TS != ((TObjString)(vto.at(si)->At(0))).String() && si < vto.size()) si++;
      std::cout << TS.Data() << ' ' << si << endl;
      // dir->cd();
      // // dir->ls();

      // TObjArray* TOA = TS.Tokenize("_");
      // TString Sbegin = ((TObjString*)TOA->At(0))->GetString();
      // TString Send = ((TObjString*)TOA->At(1))->GetString();
      // Send += "_";
      // Send += ((TObjString*)TOA->At(2))->GetString();

      // std::cout << Sbegin.Data() << ' ' << Send.Data() << endl;
      // TString HName = Sbegin + "_Histo_" + Send;
      // TString SHName = Sbegin + "_StatHisto_" + Send;
      // std::cout << HName.Data() << ' ' << SHName.Data() << endl;

      // signal = (TH2D*)dir->Get(HName.Data());
      // normal = (TH2D*)dir->Get(SHName.Data());
/////////////////////////

      
//     if(TString(h->GetName()).Contains("StatHist"))
  //       normal = (TH2D*)key->ReadObj();
  //     else
  //       signal = (TH2D*)key->ReadObj();
  //     // if(TString(signal->GetName()).Contains("Mass") ||
  //     //    TString(signal->GetName()).Contains("Density") ||
  //     //    TString(signal->GetName()).Contains("Two"))
  //     //   norm = 1;
  //     cout << "**** " << signal << ' ' << normal << " **** " << norm << endl;
      if(signal && normal){
        // H.push_back(plot(signal  ,normal  , c1 , 0, 1.61e14/1000.,
        H.push_back(plot(signal  ,normal  , c1 , 0, 1.61e14/norm,
                         "Z [cm]", "X [cm]",
                         "Full neutron fluence (1cm wide XY slice (Z[222cm, 223cm])) []",
                         false, false, true,
                         -1, -1,
                         "figures.pdf"));
  //       if(TString(h->GetName()).Contains("endcap")){
  //         Quater(H);
  //       }
  //       H.at(H.size()-1)->Write();
        delete signal;
        signal = NULL;
        delete normal;
        normal = NULL;
      }
    }
*/


