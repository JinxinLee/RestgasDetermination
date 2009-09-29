//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcElectronicsTask
//      see PndTpcElectronicsTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcSignalSplitterTask.h"


// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <vector>


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairEventHeader.h"

#include "TClonesArray.h"
#include "PndTpcSignal.h"
#include "PndTpcDigiMapper.h"
#include "TFile.h"
#include "TFolder.h"
#include "TString.h"
#include "PndTpcPadPlane.h"
#include "TTree.h"
#include "TBranch.h"
#include "TROOT.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>


// Class Member definitions -----------


PndTpcSignalSplitterTask::PndTpcSignalSplitterTask()
  : FairTask("TPC SignalSplitter response"), fsingle(false)
{
  fsignalBranchName = "PndTpcSignal";
  fbase = "PndTpcSigRegion";
 }


PndTpcSignalSplitterTask::~PndTpcSignalSplitterTask()
{
  
}

InitStatus
PndTpcSignalSplitterTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcSignalSplitterTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fsignalArray=(TClonesArray*) ioman->GetObject(fsignalBranchName);
  
  if(fsignalArray==0)
    {
      Error("PndTpcSignalSplitterTask::Init","Signal-array not found!");
      return kERROR;
    }

  ioman->ActivateBranch("EventHeader.");
  fevtHeader=(FairEventHeader*) ioman->GetObject("EventHeader.");
  if(fevtHeader==0)
    {
      Error("PndTpcSignalSplitterTask::Init","EventHeader not found!");
      return kERROR;
    }

  fmapper=PndTpcDigiMapper::getInstance();

  // prepare files for splitting
  TFolder* out=(TFolder*) gROOT->GetRootFolder()->FindObjectAny("cbmout");
  out->SetName("cbmout_old");

  unsigned int nsec=fmapper->getPadPlane()->GetNSectors();
  //prepare empty sector counters
  femptycounter.resize(nsec,0);

  if(fsingle)nsec=1;
  

  for(int isec=0; isec<nsec; isec++){
    TString outfilename=fbase;
    outfilename+=isec;
    outfilename+=".root";

    std::cout<<"Creating Splitting file "<<outfilename<<std::endl;
    

    TFile* outfile=new TFile(outfilename,"recreate");
    if(outfile==0)continue;
    
    ffiles.push_back(outfilename);
    
    outfile->cd();
    TFolder* cbmout=(TFolder*)gROOT->FindObjectAny("cbmout");
    if(cbmout==0){
      cbmout = gROOT->GetRootFolder()->AddFolder("cbmout", "Output Folder");
      gROOT->GetListOfBrowsables()->Add(cbmout);
    }
    
    TFolder* header=cbmout->AddFolder("EvtHeader","EvtHeader");
    //header->SetName("EventHeader.");
    header->Add(fevtHeader);

    TClonesArray* sigarray=new TClonesArray("PndTpcSignal");
    TFolder* folder= cbmout->AddFolder("PndTpc","PndTpc");
    sigarray->SetName("PndTpcSignal");
    folder->Add(sigarray);

    TTree* outtree=new TTree("cbmsim","/cbmout",99);
    TranicateBranchNames(outtree, "cbmout");

    outfile->cd();
    cbmout->Write();
    outtree->Write();
    outfile->Close();
    
    cbmout->Remove(header);
    cbmout->Remove(folder);
    
    if(sigarray!=NULL)delete sigarray;    
    delete outfile;
  }

  out->SetName("cbmout");
  return kSUCCESS;
}


void
PndTpcSignalSplitterTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcSignalSplitterTask::Exec"<<std::endl;
  

  //partition data according to regions
  std::cout<<"Building up sectormap ...";
  std::cout.flush();
  std::map<unsigned int,std::vector<PndTpcSignal*>* > secmap;
  Int_t ns=fsignalArray->GetEntriesFast();
  for(Int_t is=0;is<ns;++is){
    PndTpcSignal* sig=(PndTpcSignal*)fsignalArray->At(is);
    unsigned int id=fmapper->getPad(sig->padId())->sectorId();
    if(secmap[id]==NULL)secmap[id]=new std::vector<PndTpcSignal*>;
    secmap[id]->push_back(sig);
    if(is%10000==0){
      std::cout<<".";std::cout.flush();
    }
  }
  std::cout<<"finished. "<<secmap.size()<<" sectors hit"<<std::endl;
  // cleanup fsignal_array;
  //fsignalArray->Clear();

  // increase emptycounter for each sector!
  unsigned int nsec=ffiles.size();
  for(unsigned int isec=0; isec<nsec; ++isec){
    femptycounter[isec]+=1;
  }
  
  
  std::map<unsigned int,std::vector<PndTpcSignal*>* >::iterator secIt=secmap.begin();
  while(secIt!=secmap.end()){

    TString outfilename;
    if(fsingle){
      if(secIt->first!=fsecid){
	continue;
      }
      outfilename=ffiles[0];
    }
    else { 
      outfilename=ffiles[secIt->first];
    }

    // this sector was hit => decrease the femptycounter again!
    femptycounter[secIt->first]-=1;
    
    TClonesArray* sigarray=new TClonesArray("PndTpcSignal");
        
    TFile* outfile=new TFile(outfilename,"update");
    outfile->cd();
    TTree* tree=(TTree*)outfile->Get("cbmsim");
    tree->SetBranchAddress("PndTpcSignal",&sigarray);
    tree->SetBranchAddress("EventHeader.",&fevtHeader);
    
    // fill empty events according to counter
    unsigned int ne=femptycounter[secIt->first];
    if(ne!=0)std::cout<<"Sector "<<secIt->first<<": filling "<<ne<<" empty events"<<std::endl;
    for(unsigned int ie=0;ie<ne;++ie)tree->Fill();
    // CAREFULL: All empty events will have the same event header!!
    femptycounter[secIt->first]=0;

    std::vector<PndTpcSignal*>* sigv=secIt->second;
    if(sigv!=0){ // only put signals if there are some!
      unsigned int nsig=sigv->size();
      for(unsigned int i=0;i<nsig;++i){
	new((*sigarray)[i]) PndTpcSignal(*(sigv->at(i)));
      }
    // clear sector vector
    delete sigv;
    sigv=NULL;
    }
    
    
    tree->Fill();
    tree->Write();
    outfile->Close();
    if(sigarray!=NULL)delete sigarray;   
    delete outfile;
    
    ++secIt;
  }
  
  return;
}


void 
PndTpcSignalSplitterTask::TranicateBranchNames(TTree *fTree, const char *folderName)
{
  TFolder *cbm=(TFolder *)gROOT->FindObjectAny(folderName);
  if(cbm){
     TCollection* lf=cbm->GetListOfFolders();
     //TObject *evth = cbm->FindObject("EventHeader");
     TIterator* iter= lf->MakeIterator();
     TObjArray* Br= fTree->GetListOfBranches();
     TIterator* BrIter= Br->MakeIterator();
     TObject *obj;
     TObject *BrObj;

    // troncate in sub folder
    while((obj=iter->Next())) {
      TString ffn=cbm->GetName();
      ffn=ffn+".";
      ffn=ffn+obj->GetName();
      ffn=ffn+".";
      BrIter->Reset();
      while((BrObj=BrIter->Next())) {
        TBranch *b=(TBranch *)BrObj;
        TranicateBranchNames(b, ffn);
      }
    }
    delete  iter;
    delete  BrIter;
 }
}
//f____________________________________________________________________________
void 
PndTpcSignalSplitterTask::TranicateBranchNames(TBranch *b, TString ffn)
{
  TObject *BrObj;
  TString nn= b->GetName();
  nn.ReplaceAll(ffn.Data(),"");
  b->SetName(nn.Data());
  TObjArray* Br= b->GetListOfBranches();
  TIterator* BrIter= Br->MakeIterator();
  BrIter->Reset();
  while((BrObj=BrIter->Next())) {
    TBranch *b=(TBranch *)BrObj;
    TranicateBranchNames(b, ffn);
  }
  delete  BrIter;
}


void
PndTpcSignalSplitterTask::FillEmptyEvents(){
  unsigned int nsec=ffiles.size();
  for(unsigned int isec=0; isec<nsec; ++isec){
    TString outfilename=ffiles[isec];

    TClonesArray* sigarray=new TClonesArray("PndTpcSignal");
    
    TFile* outfile=new TFile(outfilename,"update");
    outfile->cd();
    TTree* tree=(TTree*)outfile->Get("cbmsim");
    tree->SetBranchAddress("PndTpcSignal",&sigarray);
    tree->SetBranchAddress("EventHeader.",&fevtHeader);
    
    // fill empty events according to counter
    unsigned int ne=femptycounter[isec];
    if(ne!=0)std::cout<<"Sector "<<isec<<": filling "<<ne<<" empty events"<<std::endl;
    for(unsigned int ie=0;ie<ne;++ie)tree->Fill();
    // CAREFULL: All empty events will have the same event header!!
    femptycounter[isec]=0;
    tree->Fill();
    tree->Write();
    outfile->Close();
    if(sigarray!=NULL)delete sigarray;   
    delete outfile;
  }
}













ClassImp(PndTpcSignalSplitterTask)
