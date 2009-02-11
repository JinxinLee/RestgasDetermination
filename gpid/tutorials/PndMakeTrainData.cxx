#include "PndMakeTrainData.h"

PndMakeTrainData::PndMakeTrainData()
{
 fNCLASS = 0;
}

PndMakeTrainData::~PndMakeTrainData()
{
}

void PndMakeTrainData::AddInFile(string className,string simFileName,string recoFileName)
{
 pair<string,string> stringPair;
 stringPair.first = simFileName;
 stringPair.second = recoFileName;
 vector <string>::iterator it;
// cout<<"  -I-  Adding Input files for the class"<<className<<endl;
 if (fInFileNameArray.find(className) == fInFileNameArray.end()) 
  {
    vector<pair<string,string> > stringVec;
    fInFileNameArray.insert(pair<string,vector<pair<string,string> > >(className,stringVec));
    (fInFileNameArray.find(className)->second).push_back(stringPair); 
    fNCLASS = fNCLASS +1;
    fClassNameArray.push_back(className);
   // cout<<"No of Classes added = "<<fNCLASS<<endl;   
  }
  else
  {
   (fInFileNameArray.find(className)->second).push_back(stringPair); 
  }

 cout << "  -I-  classes added for training are :";
  for (it=fClassNameArray.begin(); it<fClassNameArray.end(); it++)
    cout << " " << *it;
  cout<<endl;
}



void PndMakeTrainData::GenerateTree()
{
 cout<<"  -I-  Generating the Input Tree"<<endl;
 system("sleep 2");

// TFile *outfile = gROOT->FindObject(outFileName.c_str()); if (outfile) outfile->Close();
 TFile *outfile = new TFile(outFileName.c_str(),"recreate");
 TDirectory *dir = outfile->mkdir("out");
  
 TNtuple *ntuple = new TNtuple("dummy","dummy","PMag:dEdxTPC:dEdxMVD:speed:msquare");
 cout<<outfile<<endl;
 system("sleep 2");
 for(int i=0; i <fNCLASS; i++)
 {
  string className;
  vector < pair<string,string> > fileNameVec;
  className = fClassNameArray[i];
  fileNameVec = fInFileNameArray.find(className)->second;
 cout<<"                                       "<<fileNameVec.size()<<endl;
  for(int j = 0; j < fileNameVec.size();j++ )
   {
    string simFile,recoFile;
    pair<string,string> filePair;
    filePair = fileNameVec.at(j);
    simFile = filePair.first;
    recoFile = filePair.second;
    cout<<filePair.first<<"  "<<filePair.second<<"  "<<j<<endl;  
    ntuple->SetNameTitle(className.c_str(),className.c_str());
    FillNTuple(simFile,recoFile,*ntuple);
 //    for (int k=0; k<1000; k++)
   //        ntuple->Fill(0,0,0,0);


   }
    cout<<"No of entries in the class "<<className<<" is "<<ntuple->GetEntriesFast()<<endl;
    dir->cd(); 
   ntuple->Write();
    outfile->Write();
    ntuple->Reset();
 }
 outfile->Close();
}


void PndMakeTrainData::FillNTuple(string const inf1,string const inf2, TNtuple &ntuple)
{
  TFile *fsim1 = new TFile(inf1.c_str());
  TTree *tsim1 = ( TTree* )fsim1->Get("cbmsim");

  TFile *freco1 = new TFile(inf2.c_str());
  TTree *treco1 = ( TTree* )freco1->Get("cbmsim");

  TClonesArray *ArrMCTrack1 = new TClonesArray("PndMCTrack");
  tsim1->SetBranchAddress("MCTrack",&ArrMCTrack1);

//  TClonesArray *ArrTof1 = new TClonesArray("PndTofPoint");
//  tsim1->SetBranchAddress("TofPoint",&ArrTof1);

  TClonesArray *ArrTpc1 = new TClonesArray("PndTpcPoint");
  tsim1->SetBranchAddress("PndTpcPoint",&ArrTpc1);

  TClonesArray *ArrMvd1 = new TClonesArray("PndMvdMCPoint");
  tsim1->SetBranchAddress("MVDPoint",&ArrMvd1);

//  TClonesArray *ArrTofHit1 = new TClonesArray("PndTofHit");
//  treco1->SetBranchAddress("TofHit",&ArrTofHit1);

//  TClonesArray *ArrTrackPar1 = new TClonesArray("FairTrackParH");
//  treco1->SetBranchAddress("GeaneTrackPar",&ArrTrackPar1);

  TClonesArray *ArrPndPidCand1 = new TClonesArray("PndPidCand");
  treco1->SetBranchAddress("PndPidCand",&ArrPndPidCand1);
cout<<tsim1->GetEntries()<<" no of Events from "<<inf1<<"  "<<inf2<<endl;

  for (Int_t i =0; i < tsim1->GetEntries();i++)     
  {
     tsim1->GetEntry(i);
     treco1->GetEntry(i);
 //    PndTofPoint *tof = (PndTofPoint *) ArrTof1->At(0);
     PndPidCand *track = (PndPidCand *) ArrPndPidCand1->At(0);
//     FairTrackParH *trackpar = (FairTrackParH *) ArrTrackPar1->At(0);
     
     Double_t de_tpc=0;
     Double_t dx_tpc=0;
     for (Int_t j = 0; j < ArrTpc1->GetEntriesFast(); j++ )
     {
       PndTpcPoint *tpc = (PndTpcPoint *) ArrTpc1->At(j);
       if(tpc == 0)continue;
       Int_t trkID = tpc->GetTrackID();
       PndMCTrack *mctrack_g = (PndMCTrack *) ArrMCTrack1->At(trkID);
       Int_t pdg = mctrack_g->GetPdgCode();
       de_tpc += tpc->GetEnergyLoss();
       dx_tpc += tpc->GetLength();
     }

     Double_t de_mvd=0;
     Double_t dx_mvd=0;
     for (Int_t j = 0; j < ArrMvd1->GetEntriesFast(); j++ )
     {
       PndMvdMCPoint *mvd = (PndMvdMCPoint *) ArrMvd1->At(j);
       if(mvd == 0)continue;
       Int_t trkID = mvd->GetTrackID();
       PndMCTrack *mctrack_g = (PndMCTrack *) ArrMCTrack1->At(trkID);
       Int_t pdg = mctrack_g->GetPdgCode();
        de_mvd += mvd->GetEnergyLoss();
        dx_mvd += mvd->GetLength();
     }
if (track == NULL) continue;
//if (tofHit == NULL) continue;
//if (tof == NULL) continue;
//if (trackpar == NULL) continue;

Double_t s = track->Get("speed");
Double_t p = track->Get("PMag");
Double_t gama2 = 1.0/(1-s*s);
Double_t m2 = p*p/(s*s*gama2);
Double_t tpc = de_tpc/dx_tpc*1000000;
Double_t mvd = de_mvd/dx_mvd*1000000;
//cout<<p<<" "<<tpc<<"  "<<mvd<<" "<<s<<"  "<<m2<<endl;
ntuple.Fill(p,tpc,mvd,s,m2);
}
delete fsim1;
delete freco1;
delete ArrMCTrack1;
delete ArrTpc1;
delete ArrMvd1;
delete ArrPndPidCand1;

}
