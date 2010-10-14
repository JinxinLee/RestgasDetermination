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
   fStore(false), fAmpCut(0.), fZStackLimit(0), fClLimit(500)
    
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

  fMonitorArray = new TClonesArray("TCanvas");
  //ioman->Register("PRView", "PatternReco", fMonitorArray, fStore);
  
  colors.push_back(kRed+1);
  colors.push_back(kGreen+2);
  colors.push_back(kBlue+4);
  colors.push_back(kOrange);
    
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

      
  std::cout<<"PndTpcSLPatternRecoTask::Exec"<<std::endl;
  // Reset output Arrays
  if(fTrackArray==0) Fatal("PndTpcSLPatternReco::Exec()","No TrackArray");
  fTrackArray->Delete();
  
  // copy into vector
  std::vector<PndTpcCluster*> cll;      //all clusters
  //their representation in the par. space:
  std::vector<Hypersurface4D*> hitreps; 
  
  TH3D* clHist;
  TH3D* clHist2;
  TH2D* repHistXY;
  TH2D* repHistXZ;
  TCanvas* canv;
  //TCanvas* canv = new((*fMonitorArray)[fMonitorArray->GetEntriesFast()]) TCanvas();
 
   
  if(fStore) {
    std::string clName = "cl_Ev";
    std::string repNameXY = "repXY_Ev";
    std::string repNameXZ = "repXZ_Ev";
    std::string canvName = "canv_Ev";
    std::stringstream ss;
    ss<<counter;
    clName.append(ss.str());
    repNameXY.append(ss.str());
    repNameXZ.append(ss.str());
    canvName.append(ss.str());
    clHist = new TH3D(clName.c_str(), clName.c_str(), 100,xMin,xMax,
		      100,yMin,yMax, 100, zMin, zMax);
    clHist->SetMarkerStyle(20);
    repHistXY = new TH2D(repNameXY.c_str(), repNameXY.c_str(), 
			 100, fMins[0],fMaxs[0],
			 100,fMins[1],fMaxs[1]);
    repHistXZ = new TH2D(repNameXZ.c_str(), repNameXZ.c_str(), 
			 100,fMins[2],fMaxs[2],
			 100,fMins[3],fMaxs[3]);
    clHist2 = (TH3D*)clHist->Clone();
    canv = new TCanvas(canvName.c_str());
  }
 
  //look at: 189, 237/273 (?)
  
  unsigned int totCl=fClusterArray->GetEntriesFast();
  TVector3 pos;
  for(unsigned int i=0;i<totCl;++i){    // initial loop over clusters
    PndTpcCluster* cl=(PndTpcCluster*)fClusterArray->At(i);
    
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
  } //end initial loop over clusters


  //Cut away cluster-abundances on single pads
  if(fZStackLimit>0) {
    std::map<unsigned int, std::vector<PndTpcCluster*> > clMap; //<padId, Cl*>
    std::set<PndTpcCluster*> acceptedClusters;
    for(unsigned int c=0; c<cll.size(); c++) {
      PndTpcCluster* clus = cll[c];
      unsigned int nDigi = clus->nDigi();
      std::set<unsigned int> padIds; //padIds of this cluster
      for(unsigned int d=0; d<nDigi; d++)
	padIds.insert(clus->getDigi(d).padId());
      std::set<unsigned int>::iterator it;
      for(it=padIds.begin(); it!=padIds.end(); it++)
	(clMap[*it]).push_back(clus);
    }
    cll.clear();
    std::map<unsigned int, std::vector<PndTpcCluster*> >::iterator mapit;
    for(mapit=clMap.begin(); mapit!=clMap.end(); mapit++) {
      unsigned int entries = mapit->second.size();
      if(entries>fZStackLimit)
	continue;
      for(unsigned int x=0; x<entries; x++)
	acceptedClusters.insert(mapit->second[x]);
    }
    //convert back to cll vector:
    std::set<PndTpcCluster*>::iterator itCl;
    for(itCl=acceptedClusters.begin(); itCl!=acceptedClusters.end(); itCl++)
      cll.push_back(*itCl);
  }
	
      
  if(cll.size()>fClLimit) {
    std::cout<<"Bad event: more than "<<fClLimit<<" clusters; Aborting"<<std::endl;
    return;
  }
	  
      
  // ------- CLUSTER SORTING ----------------------
  if(fDistSorting) {
    std::sort(cll.begin(),cll.end(),PndTpcClusterDist(false)); 
    std::cout<<"\n **** using DISTANCE presorting of PndTpcClusters ****"
	     <<std::endl;
  }
  if(fXSorting)
    std::sort(cll.begin(),cll.end(),clusterSortX); 
  

  for(unsigned int i=0;i<cll.size();++i){    //second loop: create Hough reps
    PndTpcCluster* cl = cll[i];
    pos=cl->pos();
    
    double x = pos.X();
    double y = pos.Y();
    double z = pos.Z();
    
    hitreps.push_back(new Hypersurface4D(x,y,*fRep,
					 x,z,*fRep,i));
    hitreps.back()->setParamSpace(fMins, fMaxs);
       
    repHistXY->GetListOfFunctions()->Add(hitreps.back()->getTF1_1()->Clone());
    repHistXZ->GetListOfFunctions()->Add(hitreps.back()->getTF1_2()->Clone());
  } //end loop over clusters
  
   
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
    return;    
  }
  
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
  

   
  //build actual trackCand objects
  std::map<unsigned int,GFTrackCand> candlist;
  std::vector<TPolyLine3D*> lines;
  std::vector<TPolyMarker3D*> markerlist;
  for(unsigned int i=0; i<solutions.size(); i++) {
    GFTrackCand cand=candlist[i];
    markerlist.push_back(new TPolyMarker3D(solutions[i]->size()));
    markerlist.back()->SetMarkerStyle(20);
    if(i<colors.size())
      markerlist.back()->SetMarkerColor(colors[i]);
    
    for(unsigned int c=0; c<(solutions[i])->size(); c++) {
      PndTpcCluster* cl = (solutions[i])->at(c);
      TVector3 clpos = cl->pos();
      markerlist[i]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
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
    
    double limit = 1.e-6;
    
    double m1;  //slope in the x-y plane
    if(std::abs(TMath::Tan(theta1))>limit)
      m1 = -1./(TMath::Tan(theta1));
    else 
      m1 = 1.e3; 
    
    double m2; //slope in the x-z plane
    if(std::abs(TMath::Tan(theta2))>limit)
      m2 = -1./(TMath::Tan(theta2));
    else 
      m2 = 1.e3;
    
    double t1 = r1/(TMath::Sin(theta1));
    double t2 = r2/(TMath::Sin(theta2));
    
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
      lines.push_back(new TPolyLine3D(2,x,y,z,"l"));
      lines.back()->SetLineWidth(2);
    }
    std::cout<<"Resulting line paramaters: "<<std::endl
	     <<"theta1: "<<theta1<<";  r1:"<<r1<<std::endl
	     <<"theta2: "<<theta2<<";  r2:"<<r2<<std::endl
	     <<"m1: "<<m1<<";  t1:"<<t1<<std::endl
	     <<"m2: "<<m2<<";  t2:"<<t2<<std::endl
	     <<"x0: "<<x[0]<<";  x1:"<<x[1]<<std::endl
      	     <<"y0: "<<y[0]<<";  y1:"<<y[1]<<std::endl
	     <<"z0: "<<z[0]<<";  z1:"<<z[1]<<std::endl;
    
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
    canv->Divide(4,1);
    TVirtualPad* thePad = canv->cd(1);
    thePad->GetListOfPrimitives()->Add(clHist);
    thePad = canv->cd(2);
    thePad->GetListOfPrimitives()->Add(clHist2);
    for(unsigned int k=0; k<markerlist.size(); k++)
      thePad->GetListOfPrimitives()->Add(markerlist[k]);
    for(unsigned int l=0; l<lines.size(); l++)
      thePad->GetListOfPrimitives()->Add(lines[l]);
    thePad = canv->cd(3);
    thePad->GetListOfPrimitives()->Add(repHistXY);
    for(unsigned int b=0; b<boxlistXY.size(); b++) 
      thePad->GetListOfPrimitives()->Add(boxlistXY[b]);
    thePad = canv->cd(4);
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
    
    delete clHist;
    delete repHistXY;
    delete repHistXZ;
    fHistoFile->Close();
    
  }

  //cleaning of markerlist not needed, VirtualPad took ownership
  
  
  return;
}

void
PndTpcSLPatternRecoTask::SetStoreHistograms(TString file) {
  fStore=true;
  fHistoFileName=file;
}
