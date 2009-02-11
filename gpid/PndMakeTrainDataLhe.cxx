#include "PndMakeTrainDataLhe.h"
#include "PndEmcCluster.h"
#include "TGeoManager.h"
#include "PndEmcClusterMoments.h"
#include "PndEmcXClMoments.h"
#include "TChain.h"

PndMakeTrainDataLhe::PndMakeTrainDataLhe()
{
  fNCLASS = 0;
  
  TFile *f = new TFile("/daq/dtbina/vanni/test/points_tpcmvdemc_kminus.root");
  TGeoManager *gGeoManager = (TGeoManager*)f->FindObject("FAIRGeom"); 
  
  cout<<"Inside constructor"<<endl;
  
  // TGeoManager  *gGeoManager =  (TGeoManager*) gROOT->FindObject("FAIRGeom");
}

PndMakeTrainDataLhe::~PndMakeTrainDataLhe()
{
  fInFileNameArray.clear();
  fClassNameArray.clear();
}

void PndMakeTrainDataLhe::AddInFile(string className,string recoFileName)
{
  string FileString;
  FileString = recoFileName;
  vector <string>::iterator it;
  // cout<<"  -I-  Adding Input files for the class"<<className<<endl;
  if (fInFileNameArray.find(className) == fInFileNameArray.end()){
    vector<string> stringVec;
    fInFileNameArray.insert(pair<string,vector<string> >(className,stringVec));
    (fInFileNameArray.find(className)->second).push_back(FileString); 
    fNCLASS = fNCLASS +1;
    fClassNameArray.push_back(className);
    // cout<<"No of Classes added = "<<fNCLASS<<endl;   
  }
  else{
    (fInFileNameArray.find(className)->second).push_back(FileString); 
  }
  
  cout << "  -I-  classes added for training are :";
  for (it=fClassNameArray.begin(); it<fClassNameArray.end(); it++)
    cout << " " << *it;
  cout<<endl;
}

void PndMakeTrainDataLhe::GenerateTree()
{
  cout<<"  -I-  Generating the Input Tree"<<endl;
  system("sleep 2");
  
  // TFile *outfile = gROOT->FindObject(outFileName.c_str()); if
  // (outfile) outfile->Close();
  TFile *outfile = new TFile(outFileName.c_str(),"recreate");
  TDirectory *dir = outfile->mkdir("out");
  
  //TNtuple *ntuple = new TNtuple("dummy","dummy","PMag:tpc:mvd:beta:msquare");
  TNtuple *ntuple = new TNtuple("dummy","dummy","p:stt:mvd:beta:tof:thetaC:emc");
  cout<<outfile<<endl;
  system("sleep 2");
  for(int i=0; i <fNCLASS; i++){
    string className;
    vector <string>  fileNameVec;
    className = fClassNameArray[i];
    ntuple->SetNameTitle(className.c_str(),className.c_str());
    fileNameVec = fInFileNameArray.find(className)->second;
    TChain *recoChain = new TChain("cbmsim");
    //  cout<<"                                       "<<fileNameVec.size()<<endl;
    for(int j = 0; j < fileNameVec.size();j++ ){
      string fileName;
      fileName = fileNameVec.at(j);
      cout<<recoChain->AddFile((fileName).c_str())<<endl;
      cout<<fileName<<"  "<<j<<endl;  
    }
    FillNTuple(*recoChain,*ntuple);
    //   cout<<"No of entries in the class "<<className<<" is
    //   "<<ntuple->GetEntriesFast()<<endl;
    dir->cd(); 
    ntuple->Write();
    outfile->Write();
    ntuple->Reset();
    delete recoChain;
  }
  outfile->Close();
  delete outfile;
  delete dir;
  delete ntuple;
}

void PndMakeTrainDataLhe::FillNTuple(TChain  &recoChain, TNtuple &ntuple)
{
  TClonesArray *ArrLhe = new TClonesArray("PndLhePidTrack");
  recoChain.SetBranchAddress("LhePidTrack",&ArrLhe);
  
  TClonesArray *ArrEmc = new TClonesArray("PndEmcCluster");
  recoChain.SetBranchAddress("EmcCluster",&ArrEmc);
  //  gGeoManager =  (TGeoManager*) gROOT->FindObject("FAIRGeom");
  //cout<<gGeoManager<<endl;
  for (Int_t i =0; i < recoChain.GetEntries();i++)     
    {
      cout<<i<<"----------------------------------------------------------------------------------------------------------"<<endl;
      cout<<  recoChain.GetEntry(i)<<endl;
      PndLhePidTrack *track = (PndLhePidTrack *) ArrLhe->At(0);
      //if (track == NULL) continue;
      if (track != NULL){
	Double_t mvd = track->GetMvdELoss(); 
	Double_t stt = track-> GetSttDEDX();
	Double_t s = track->GetBeta();
	TVector3 pvec = track->GetMomentum();
	Double_t p = pvec.Mag();
	Double_t t= track->GetTof();
	Double_t tC= track->GetDrcThetaC();
	Double_t energy = track->GetEmcELoss();
	Int_t EmcIndex = track->GetEmcIndex();
	
	//if (EmcIndex == -1)continue;
	//if (EmcIndex != -1){
	  // PndEmcCluster *Cluster = (PndEmcCluster *) ArrEmc->At(EmcIndex);
	  // PndEmcClusterMoments mom = Cluster->Moments();
	  // mom->Center1();
	  /*
	    PndEmcXClMoments momX = Cluster->Xmoments();
	    Double_t mom00 =  momX.AbsZernikeMoment(0, 0);
	    Double_t mom10 =  momX.AbsZernikeMoment(1, 0);
	    Double_t mom11 =  momX.AbsZernikeMoment(1, 1);
	    Double_t mom20 =  momX.AbsZernikeMoment(2, 0);
	    Double_t mom21 =  momX.AbsZernikeMoment(2, 1);
	    Double_t mom22 =  momX.AbsZernikeMoment(2, 2);
	  */
      // Double_t mom1 = (mom.Centre1()).Mag();
      // Double_t mom2 = (mom.Centre2()).Mag();
	  
	  
      //cout<<EmcIndex<<" "<<mom00<<" "<<mom10<<" "<<mom11<<"
      //"<<mom20<<" "<<mom21<<" "<<mom22<<endl;
      /*
	Double_t gama2;
	if(gama2 !=1)
	gama2 = 1.0/(1.0-s*s);
	Double_t m2 = p*p/(s*s*gama2);
      */
	  
	  cout<<i<<"----------------------------------------------------------------------------------------------------------"<<endl;
	  ntuple.Fill(p,stt,mvd,s,t,tC,energy);
	  //}
      }
    }
}
