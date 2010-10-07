//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcSLPatternRecoTask
//      see PndTpcSLPatternRecoTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcSLPatternRecoTask.h"

// C/C++ Headers ----------------------
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFException.h"
#include "PndTpcCluster.h"
#include "PndTpcClusterZ.h"
#include "PndTpcClusterDist.h"
#include "GFTrackCand.h"
#include "GFTrack.h"
#include "TF1.h"
#include "TVector3.h"
#include "FairRunAna.h"
#include "RKTrackRep.h"
#include "Hypersurface4D.h"
#include "Hough4DNode.h"
#include "FairField.h"
#include "PndFieldAdaptor.h"
#include "GFFieldManager.h"

#include "TFile.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TCanvas.h"
#include "TBox.h"
#include "TVirtualPad.h"
#include "TPolyLine3D.h"
#include "TMath.h"

// Class Member definitions -----------

ClassImp(PndTpcSLPatternRecoTask)

bool
clusterSortX(PndTpcCluster* cl1, PndTpcCluster* cl2) {
  return cl1->pos().X()<cl2->pos().X();
}


PndTpcSLPatternRecoTask::PndTpcSLPatternRecoTask()
:  FairTask("PndTpc SL Hough Pattern Reco"),
  fPersistence(kFALSE),fDistSorting(kFALSE),
  fDepth(6), fThresh(6), fMin(5), counter(0),
  fXZ(true), fZY(false), _cutbigpad(kFALSE), _cutsmallpad(kFALSE),
  fStore(false), fAmpCut(0.)
    
{
  fClusterBranchName = "PndTpcCluster";
  fRep = new TF1("rep","[0]*cos(x)+[1]*sin(x)",-6,6); //standard rep
}

PndTpcSLPatternRecoTask::~PndTpcSLPatternRecoTask(){
  delete fRep;
  if(fStore)
    delete fHistoFile;
}


//helper functor for node sorting
bool compareNodes (Hough4DNode* n1, Hough4DNode* n2) { 
  return (n1->getVote() > n2->getVote()); 
}


void 
PndTpcSLPatternRecoTask::SetParameterSpace(double* mins, double* maxs) {
  fMins[0] = mins[0];
  fMins[1] = mins[1];
  fMins[2] = mins[2];
  fMins[3] = mins[3];
  fMaxs[0] = maxs[0]; 
  fMaxs[1] = maxs[1];
  fMaxs[2] = maxs[2];
  fMaxs[3] = maxs[3];

}


InitStatus
PndTpcSLPatternRecoTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcSLPatternRecoTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  fClusterArray=(TClonesArray*) ioman->GetObject(fClusterBranchName);

  if(fClusterArray==0)
    {
      Error("PndTpcSLPatternRecoTask::Init","Cluster-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fTrackArray = new TClonesArray("GFTrack");
  ioman->Register("TrackPreFit","GenFit",fTrackArray,fPersistence);

  //get the magnetic field for curvature seeding
  fField=(FairField*) FairRunAna::Instance()->GetField();
  GFFieldManager::getInstance()->init(new PndFieldAdaptor(fField));
    
  return kSUCCESS;
}

void
PndTpcSLPatternRecoTask::Exec(Option_t* opt)
{
  if(fStore) {
    fHistoFile = new TFile(fHistoFileName, "update");
    fHistoFile->cd();
  }
  counter++;

  //chamber geometry:
  double xMin = -5.;
  double xMax = 5.;
  double yMin = -5.;
  double yMax = 5.;
  double zMin = 0.;
  double zMax = 8.;
  
  double MIN0 = fMins[0];
  double MIN1 = fMins[1];
  double MIN2 = fMins[2];
  double MIN3 = fMins[3];
  double MAX0 = fMaxs[0];
  double MAX1 = fMaxs[1];
  double MAX2 = fMaxs[2];
  double MAX3 = fMaxs[3];

  // // // std::cout<<"MIN0: "<<MIN0<<std::endl;
  // // // std::cout<<"MIN1: "<<MIN1<<std::endl;
  // // // std::cout<<"MIN2: "<<MIN2<<std::endl;
  // std::cout<<"MIN3: "<<MIN3<<std::endl;
  // std::cout<<"MAX0: "<<MAX0<<std::endl;
  // std::cout<<"MAX1: "<<MAX1<<std::endl;
  // std::cout<<"MAX2: "<<MAX2<<std::endl;
  // std::cout<<"MAX3: "<<MAX3<<std::endl;

    
  std::cout<<"PndTpcSLPatternRecoTask::Exec"<<std::endl;
  // Reset output Arrays
  if(fTrackArray==0) Fatal("PndTpcSLPatternReco::Exec()","No TrackArray");
  fTrackArray->Delete();
  
  // copy into vector
  std::vector<PndTpcCluster*> cll;      //all clusters
  //their representation in the par. space:
  std::vector<Hypersurface4D*> hitreps; 
  
  TH3D* clHist;
  TH2D* repHistXY;
  TH2D* repHistXZ;
  TCanvas* canv = new TCanvas();
 
   
  if(fStore) {
    std::string clName = "cl_Ev";
    std::string repNameXY = "repXY_Ev";
    std::string repNameXZ = "repXZ_Ev";
    std::stringstream ss;
    ss<<counter;
    clName.append(ss.str());
    repNameXY.append(ss.str());
    repNameXZ.append(ss.str());
    clHist = new TH3D(clName.c_str(), clName.c_str(), 100,xMin,xMax,
		      100,yMin,yMax, 100, zMin, zMax);
    clHist->SetMarkerStyle(20);
    repHistXY = new TH2D(repNameXY.c_str(), repNameXY.c_str(), 
			 100, fMins[0],fMaxs[0],
			 100,fMins[1],fMaxs[1]);
    repHistXZ = new TH2D(repNameXZ.c_str(), repNameXZ.c_str(), 
			 100,fMins[2],fMaxs[2],
			 100,fMins[3],fMaxs[3]);
  }
  
  unsigned int totCl=fClusterArray->GetEntriesFast();
  TVector3 pos;
  int ii =0;
  for(unsigned int i=0;i<totCl;++i){    // loop over clusters
    PndTpcCluster* cl=(PndTpcCluster*)fClusterArray->At(i);
    //cl->SetIndex(i);                    //INDEXNG
    if(cl->amp()<fAmpCut)
      continue;
    pos = cl->pos();
    if(_cutsmallpad && pos.y()>0.6)
      continue;
    else if(_cutbigpad && pos.y()<0.6) 
      continue;
    
    cll.push_back(cl);
    if(fStore)
      clHist->Fill(pos.X(), pos.Y(), pos.Z());
    
    //TODO: parameter management and specifiable projection plane
    double x = pos.X();
    //double x = pos.X();
    double y = pos.Y();
    double z = pos.Z();
    
    hitreps.push_back(new Hypersurface4D(x,y,*fRep,
					 x,z,*fRep,ii));
    
    hitreps.back()->setParamSpace(fMins, fMaxs);
   
    
    repHistXY->GetListOfFunctions()->Add(hitreps.back()->getTF1_1()->Clone());
    repHistXZ->GetListOfFunctions()->Add(hitreps.back()->getTF1_2()->Clone());
    ii++;
  } //end loop over clusters
  
  // sort clusters 
  if(fDistSorting) {
    std::sort(cll.begin(),cll.end(),PndTpcClusterDist(false)); 
    std::cout<<"\n **** using DISTANCE presorting of PndTpcClusters ****"
	     <<std::endl;
  }
  if(fXSorting)
    std::sort(cll.begin(),cll.end(),clusterSortX); 
  
  //std::sort(cll.begin(),cll.end(),PndTpcClusterZ(false)); 
  
  
  // Begin FHT search -----------------------------------------------------
  
  //initialize root node:
  double rootCenter[4] = {0.f, 0.f, 0.f, 0.f};
  Hough4DNode* root = new Hough4DNode(rootCenter,0,cll.size());
  
  for(int i=0; i<hitreps.size(); i++)
    (hitreps[i])->testIntersect(root);
  
  //test if every hit was inside that node
  int rootvotes = root->getVote();
  std::cerr<<"DEBUG - Clusters (after cut): "<<cll.size()<<";  Votes: "<<rootvotes
	   <<";  Hitreps: "<<hitreps.size()
	   <<";  EVENT: "<<counter<<std::endl;
  
  //start actual FHT search
  
  std::vector<Hough4DNode*> survivors;
  std::vector<Hough4DNode*> sons;
  survivors.push_back(root);
  
  
  for(unsigned int t=1; t<fDepth; t++) { //iteration depth
    
    for(unsigned int n=0; n<survivors.size(); n++) { //loop over survivors
      //create sons
      Hough4DNode* the_node = survivors[n];
      const double* son_arr = the_node->getSonArray();
      for (int s=0; s<16; s++) {
	sons.push_back(new Hough4DNode(son_arr+4*s,
				       the_node->getLevel()+1,
				       cll.size()));
      }
      delete survivors[n]; //clean up last generation
      
    } //end loop over survivors
    
    survivors.clear();
    
    for(int s=0; s<sons.size(); s++) {
      Hough4DNode* node = sons[s];
      
      for(int i=0; i<hitreps.size(); i++)
	(hitreps[i])->testIntersect(node);
      if(node->getVote()>=fThresh)
	survivors.push_back(node);
      else
	delete sons[s];
    }
    
    sons.clear();
  } //finished tree search
  
  for(unsigned int h=0; h<hitreps.size(); h++)
    delete hitreps[h];
  hitreps.clear();
  
  
  //End FHT search; Begin candidate extraction -----------------------------
  
  unsigned int surs = survivors.size();
  //exit: PR failed
  if(surs==0) {
    std::cout<<"PndTpcSLPatternRecoTask::Exec(): Fail "
	     <<"- no suitable candidates found"<<std::endl;
    clHist->Write();
    repHistXY->Write();
    repHistXZ->Write();
    delete clHist;
    delete repHistXY;
    delete repHistXZ;
    return;
  }
  
  std::cout<<"DEBUG: Begin candidate extraction"<<std::endl;
  
  std::vector<std::vector<PndTpcCluster*>*> solutions; //track candidates
  std::vector<Hough4DNode*> cand_nodes;
  
  
  //extract tracks until solutions have less clusters than minCL
  while(true) {
    //sort nodes by final votes
    sort(survivors.begin(), survivors.end(), compareNodes);
    
    //extract clusters from best node
    const bool* bestHitList = survivors.front()->getHitList();
    
    std::vector<PndTpcCluster*>* sol = new std::vector<PndTpcCluster*>();
    for(int c=0; c<cll.size(); c++) {
      if(bestHitList[c])
	sol->push_back(cll[c]);
    }
    //discard track candidates with less than fMin hits:
    if(sol->size()<fMin) 
      break;
    
    cand_nodes.push_back(survivors.front());  //remember cand. nodes
    
    //remove hits for first node from all others
    for(int p=0; p<cll.size(); p++) {
      if(!bestHitList[p])
	continue;
      for(int n=0; n<survivors.size(); n++)
	if(survivors[n]->checkHit(p))
	  survivors[n]->removeHit(p);
    }
    solutions.push_back(sol);    
  }
  std::cout<<"PndTpcSLPatternRecoTask::Exec(): Found "<<solutions.size()
	   <<" track candidates"<<std::endl;
  
  // End candidate extraction ----------------------------------------------
  
  std::cout<<"  done."<<std::endl;
  
  //build actual trackCand objects
  std::map<unsigned int,GFTrackCand> candlist;
  TPolyLine3D* line;
  for(unsigned int i=0; i<solutions.size(); i++) {
    GFTrackCand cand=candlist[i];
    for(unsigned int c=0; c<(solutions[i])->size(); c++) {
      PndTpcCluster* cl = (solutions[i])->at(c);
      std::cout<<cl->pos().X()<<"   ";
      cand.addHit(2,cl->index());
    }
    std::cout<<std::endl;
    
    //extract candidate seed information
    Hough4DNode* cand_node = cand_nodes[i];
    if(cand_node==NULL)
      continue;
    
    const double* cent = cand_node->getCenter();
    //TODO: HOW CAN THERE BE A SEGFAULT??
    if(cent==NULL)
      continue;
   
    //------- calculate the seed values of the candidate ----------------
    
    double theta1 = (cent[0] + 0.5f)*(fMaxs[0]-fMins[0])+fMins[0];
    double r_shift1 = (cent[1] + 0.5f)*(fMaxs[1]-fMins[1])+fMins[1];
    
    //double r1 = r_shift1 - x_OFF*cos(theta1);
    double r1 = r_shift1;
    
    double theta2 = (cent[2] + 0.5f)*(fMaxs[2]-fMins[2])+fMins[2];
    double r_shift2 = (cent[3] + 0.5f)*(fMaxs[3]-fMins[3])+fMins[3];
    
    //double r2 = r_shift2 - x_OFF*cos(theta2);
    double r2 = r_shift2;
    
    double m1;  //slope in the x-y plane
    if(tan(theta1)>1.e-4)
      m1 = -1./(tan(theta1));
    else 
      m1 = 1.e3; 

    double m2; //slope in the x-z plane
    if(tan(theta2)>1.e-4)
      m2 = -1./(tan(theta2));
    else 
      m2 = 1.e3; 

    double t1 = r1/(sin(theta1));
    double t2 = r2/(sin(theta2));
    
    //resulting points for line
    double x[2];
    double y[2];
    double z[2];
    
    x[0] = xMin; x[1] = xMax;
    y[0] = m1*x[0]+t1;
    z[0] = m2*x[0]+t2;
    y[1] = m1*x[1]+t1;
    z[1] = m2*x[1]+t2;
    if(fStore) {   
      line = new TPolyLine3D(2,x,y,z,"l");
    }
    
    TVector3 mom;
    mom.SetXYZ(x[1]-x[0], y[1]-y[0], z[1]-z[0]);
    mom=mom.Unit();
       
    //requires nicely sorted candidates.
    TVector3 clpos=(solutions[i])->at(0)->pos();
    
    TVector3 poserr(1.,1.,1.);
    //large mom error in the unknown projection:
    TVector3 momerr(mom.X()*0.1,mom.Y()*0.1,mom.Z()*0.1); 
    
    //init the trackrep
    int pdg = 13; //muons - doesn't matter without mag field anyway
    RKTrackRep* rep = new RKTrackRep(clpos,mom,poserr,momerr,pdg);
    //build GFTrack object
    
    GFTrack* trk=new((*fTrackArray)[fTrackArray->GetEntriesFast()]) GFTrack(rep);
    trk->setCandidate(cand); // here the candidate is copied! 
  }
  
  std::vector<TBox*> boxlistXY;
  std::vector<TBox*> boxlistXZ;
  if(fStore) {
    for(int l=0; l < survivors.size(); l++) {
      const double* center = survivors[l]->getCenter();
      double length = survivors[l]->getSideLength();
      double x1 = (center[0]-0.5*length+0.5)*(fMaxs[0]-fMins[0])+fMins[0];
      double x2 = (center[0]+0.5*length+0.5)*(fMaxs[0]-fMins[0])+fMins[0];
      double y1 = (center[1]-0.5*length+0.5)*(fMaxs[1]-fMins[1])+fMins[1];
      double y2 = (center[1]+0.5*length+0.5)*(fMaxs[1]-fMins[1])+fMins[1];
      boxlistXY.push_back(new TBox(x1,y1,x2,y2));
      boxlistXY.back()->SetLineColor(kPink+10);
      boxlistXY.back()->SetFillStyle(0);
      boxlistXY.back()->SetDrawOption("l");

      double x3 = (center[2]-0.5*length+0.5)*(fMaxs[2]-fMins[2])+fMins[2];
      double x4 = (center[2]+0.5*length+0.5)*(fMaxs[2]-fMins[2])+fMins[2];
      double y3 = (center[3]-0.5*length+0.5)*(fMaxs[3]-fMins[3])+fMins[3];
      double y4 = (center[3]+0.5*length+0.5)*(fMaxs[3]-fMins[3])+fMins[3];
      boxlistXZ.push_back(new TBox(x3,y3,x4,y4));
      boxlistXZ.back()->SetLineColor(kPink+10);
      boxlistXZ.back()->SetFillStyle(0);
      boxlistXZ.back()->SetDrawOption("l");
    }
  }
  
  
  std::cout<<"PndTpcSLPatternRecoTask::Exec() "
	   <<fTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;
    
  if(fStore) {
    canv->Divide(3,1);
    TVirtualPad* thePad = canv->cd(1);
    thePad->GetListOfPrimitives()->Add(clHist);
    thePad->GetListOfPrimitives()->Add(line);
    thePad = canv->cd(2);
    thePad->GetListOfPrimitives()->Add(repHistXY);
    for(unsigned int b=0; b<boxlistXY.size(); b++) 
      thePad->GetListOfPrimitives()->Add(boxlistXY[b]);
    thePad = canv->cd(3);
    thePad->GetListOfPrimitives()->Add(repHistXZ);
    for(unsigned int b=0; b<boxlistXZ.size(); b++) 
      thePad->GetListOfPrimitives()->Add(boxlistXZ[b]);
  
    //save the canvas
    fHistoFile->cd();
    canv->Write();
    delete canv;
    for(unsigned int b=0; b<boxlistXY.size(); b++) 
      delete boxlistXY[b];
    boxlistXY.clear();
    for(unsigned int b=0; b<boxlistXZ.size(); b++) 
      delete boxlistXZ[b];
    boxlistXZ.clear();
    
    //clHist->Write();
    //repHist->Write();
    delete clHist;
    delete repHistXY;
    delete repHistXZ;
    fHistoFile->Close();
    
  }
  
  return;
}

void
PndTpcSLPatternRecoTask::SetStoreHistograms(TString file) {
  fStore=true;
  fHistoFileName=file;
}
