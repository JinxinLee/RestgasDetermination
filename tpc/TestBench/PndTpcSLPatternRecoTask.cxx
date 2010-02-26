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
#include "Hypersurface2D.h"
#include "Hough2DNode.h"
#include "FairField.h"
#include "PndFieldAdaptor.h"
#include "GFFieldManager.h"


// Class Member definitions -----------

ClassImp(PndTpcSLPatternRecoTask)

PndTpcSLPatternRecoTask::PndTpcSLPatternRecoTask()
 :  FairTask("PndTpc SL Hough Pattern Reco"),
    fPersistence(kFALSE),fDistSorting(kTRUE),
  fDepth(6), fThresh(6), fMin(5), counter(0)
    
{
  fClusterBranchName = "PndTpcCluster";
  fRep = new TF1("rep","[0]*cos(x)+[1]*sin(x)",-6,6); //standard rep
}

PndTpcSLPatternRecoTask::~PndTpcSLPatternRecoTask(){

}



//helper functor for node sorting
bool compareNodes (Hough2DNode* n1, Hough2DNode* n2) { 
  return (n1->getVote() > n2->getVote()); 
}


void 
PndTpcSLPatternRecoTask::SetParameterSpace(double* mins, double* maxs) {
  fMins[0] = mins[0];
  fMins[1] = mins[1];
  fMaxs[0] = maxs[0];
  fMaxs[1] = maxs[1];
  
}

void 
PndTpcSLPatternRecoTask::SetParameterSpace(double min1, double min2, 
					   double max1, double max2){
  fMins[0] = min1;
  fMins[1] = min2;
  fMaxs[0] = max1;
  fMaxs[1] = max2;
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
  
  return kSUCCESS;
}

void
PndTpcSLPatternRecoTask::Exec(Option_t* opt)
{
  counter++;
  
  double MIN0 = fMins[0];
  double MIN1 = fMins[1];
  double MAX0 = fMaxs[0];
  double MAX1 = fMaxs[1];
  
  
  
  double x_OFF = 5.f; //TODO: read from par
  
  //get the magnetic field for curvature seeding
  FairField* field=FairRunAna::Instance()->GetField();
  GFFieldManager::getInstance()->init(new PndFieldAdaptor(field));
  
  std::cout<<"PndTpcSLPatternRecoTask::Exec"<<std::endl;
  // Reset output Arrays
  if(fTrackArray==0) Fatal("PndTpcSLPatternReco::Exec()","No TrackArray");
  fTrackArray->Delete();
  
  // copy into vector
  std::vector<PndTpcCluster*> cll;      //all clusters
  std::vector<Hypersurface2D*> hitreps; //their representation in the par. space
  
  unsigned int totCl=fClusterArray->GetEntriesFast();
  TVector3 pos;
  for(unsigned int i=0;i<totCl;++i){    // loop over clusters
    PndTpcCluster* cl=(PndTpcCluster*)fClusterArray->At(i);
    cl->SetIndex(i);                    //INDEXNG
    cll.push_back(cl);
    
    pos = cl->pos();
    
    //TODO: parameter management and specifiable projection plane
    double x = pos.X()+x_OFF;
    double z = pos.Z();

    //std::cout<<"x: "<<x<<"  z: "<<z<<std::endl;
    
    hitreps.push_back(new Hypersurface2D(x,z,*fRep,i));
    hitreps.back()->setParamSpace(fMins, fMaxs);
    
  } //end loop over clusters
  
  
// Begin FHT search -----------------------------------------------------
  
  //initialize root node:
  double center[2] = {0.f, 0.f};
  Hough2DNode* root = new Hough2DNode(center,0,totCl);
  //for(int i=0; i<hitreps.size(); i++)
  //  (hitreps[i])->testIntersect(root);
  
  //test if every hit was inside that node
  //int rootvotes = root->getVote();
  //if(rootvotes!=totCl) {
  //  std::cerr<<"Clusters: "<<totCl<<";  Votes: "<<rootvotes
  //	     <<";  Hitreps: "<<hitreps.size()
  //     <<";  EVENT: "<<counter<<std::endl;
    //Fatal("PndTpcSLPatternReco::Exec()",
    //  "root votes wrong! Parameter space was not properly initialized");
  //}
  
  //start actual FHT search

  std::vector<Hough2DNode*> survivors;
  std::vector<Hough2DNode*> sons;
  survivors.push_back(root);

    
  for(int t=1; t<fDepth; t++) { //iteration depth
    
    for(int n=0; n<survivors.size(); n++) { //loop over survivors
      //create sons
      Hough2DNode* the_node = survivors[n];
      double* son_arr = the_node->getSonArray();
      for (int s=0; s<4; s++) {
	sons.push_back(new Hough2DNode(son_arr+2*s,the_node->getLevel()+1,totCl));
      }
      delete survivors.at(n); //clean up last generation
      
    } //end loop over survivors
    
    survivors.clear();

    for(int s=0; s<sons.size(); s++) {
      Hough2DNode* node = sons.at(s);
      
      //simple, no check for hitlists etc. :
      for(int i=0; i<hitreps.size(); i++)
	(hitreps[i])->testIntersect(node);
      if(node->getVote()>=fThresh)
	survivors.push_back(node);
      else
	delete sons.at(s);
    }
    
    sons.clear();
  } //finished tree search


//End FHT search; Begin candidate extraction -----------------------------

  
  std::vector<std::vector<PndTpcCluster*>*> solutions; //track candidates
  std::vector<Hough2DNode*> cand_nodes;
  
  //extract tracks until solutions have less clusters than minCL
  while(true) {
    //sort nodes by final votes
    sort(survivors.begin(), survivors.end(), compareNodes);
    
    //extract clusters from best node
    bool* bestHitList = survivors.front()->getHitList();
    
    std::vector<PndTpcCluster*>* sol = new std::vector<PndTpcCluster*>();
    for(int c=0; c<totCl; c++) {
      if(bestHitList[c])
	sol->push_back(cll[c]);
    }
     //discard track candidates with less than fMin hits:
    if(sol->size()<fMin) 
      break;
        
    cand_nodes.push_back(survivors.front());  //remember cand. nodes
    
    //remove hits for first node from all others
    for(int p=0; p<totCl; p++) {
      if(!bestHitList[p])
	continue;
      for(int n=0; n<survivors.size(); n++)
	if(survivors[n]->checkHit(p))
	  survivors[n]->removeHit(p);
    }
        
    solutions.push_back(sol);    
  }
  
// End candidate extraction ----------------------------------------------


  // sort clusters 
  if(fDistSorting) {
    std::sort(cll.begin(),cll.end(),PndTpcClusterDist(false)); 
    std::cout<<"\n **** using DISTANCE presorting of PndTpcClusters ****"<<std::endl;
  }
  else
    std::sort(cll.begin(),cll.end(),PndTpcClusterZ(false)); 
  
  std::cout<<"  done."<<std::endl;
  
  //build actual trackCand objects
  std::map<unsigned int,GFTrackCand*> candlist;
  for(unsigned int i=0; i<solutions.size(); i++) {
    
    //HACK: find error on Monday
    if(i>0)
      continue;
    GFTrackCand* cand=candlist[i];
    for(unsigned int c=0; c<(solutions[i])->size(); c++) {
      if(cand==NULL){
	cand=new GFTrackCand();
	candlist[i]=cand;
      }
      PndTpcCluster* cl = (solutions[i])->at(c);
      cand->addHit(2,cl->index());
    }
    
    //extract candidate seed information
    Hough2DNode* cand_node = cand_nodes[i];
    if(cand_node==NULL)
      continue;
    
    double* cent = cand_node->getCenter();
    std::cout<<"Debug: got cent"<<std::endl;
    //TODO: HOW CAN THERE BE A SEGFAULT??
    if(cent==NULL)
      continue;
    
    //double theta = (cent[0] + 0.5f)*(fMaxs[0]-fMins[0])+fMins[0];
    //double r_shift = (cent[1] + 0.5f)*(fMaxs[1]-fMins[1])+fMins[1];

    double theta = (cent[0] + 0.5f)*(MAX0-MIN0)+MIN0;
    double r_shift = (cent[1] + 0.5f)*(MAX1-MIN1)+MIN1;
    
    double r = r_shift - x_OFF*cos(theta);
    
    double m;
    if(tan(theta)>1.e-4)
      m = -1./(tan(theta));
    else 
      m = 1.e3; 
    //double t = r/(sin(theta));
    
    //TODO: flexible geometry 
    TVector3 mom;
    mom.SetXYZ(0.1,0.,m*0.1);
    mom=mom.Unit();
       
    //requires nicely sorted candidates.
    TVector3 clpos=(solutions[i])->at(0)->pos();
    
    TVector3 poserr(1.,1.,1.);
    //large mom error in the unknown projection:
    TVector3 momerr(mom.X()*0.1,0.5,mom.Z()*0.1); 
   
    //init the trackrep
    int pdg = 11;
    RKTrackRep* rep = new RKTrackRep(clpos,mom,poserr,momerr,pdg);
   
    //build GFTrack object
    
    GFTrack* trk=new((*fTrackArray)[fTrackArray->GetEntriesFast()]) GFTrack(rep);
    trk->setCandidate(*cand); // here the candidate is copied! 
   
  }
  
    
  std::cout<<"PdnTpcSLPatternRecoTask::Exec() "
	   <<fTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;
  return;
}

