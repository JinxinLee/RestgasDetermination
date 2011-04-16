//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTrackingTask
//      see PndTpcRiemannTrackingTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Johannes Rauch
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcRiemannTrackingTask.h"

// C/C++ Headers ----------------------


// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFRecoHitFactory.h"
#include "GFException.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannTrackFinder.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHough.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcRiemannHTCorrelator.h"
#include "PndTpcRiProxHTCorrelator.h"
#include "PndTpcSzHTCorrelator.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcProximityTTCorrelator.h"
#include "PndTpcSzTTCorrelator.h"
#include "PndTpcRiemannTTCorrelator.h"
#include "GFTrackCand.h"
#include "GFTrack.h"
#include "LSLTrackRep.h"
#include "RKTrackRep.h"
#include "TH1I.h"
#include "TH1D.h"
#include "McIdCollection.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "GFDetPlane.h"
#include "TDatabasePDG.h"
#include "FairField.h"
#include "PndConstField.h"
#include "PndFieldAdaptor.h"
#include "GFFieldManager.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include "TFile.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "TCanvas.h"
#include "TBox.h"
#include "TVirtualPad.h"
#include "TPolyLine3D.h"
#include "TMath.h"

#include <cmath>

using std::fabs;

// Class Member definitions -----------

ClassImp(PndTpcRiemannTrackingTask)

PndTpcRiemannTrackingTask::PndTpcRiemannTrackingTask()
  : FairTask("PndTpc Pattern Reco"), _persistence(kFALSE), fStore(false),
    _sortingMode(false), _sorting(3), _interactionZ(0.),
    _mergeTracks(false),
    _proxcut(1), _riproxcut(0.05),
    _planecut(0.1),
    _minpoints(10),
    _szcut(2.),
    _TTproxcut(2.),
    _TTszcut(2.),
    _TTplanecut(0.001),
    _riemannscale(24.6),
    _clusterBranchName("PndTpcCluster")
  {;}

PndTpcRiemannTrackingTask::~PndTpcRiemannTrackingTask(){
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
  if(fStore) {
    fHistoFile->Close();
    delete fHistoFile;
  }
}

void 
PndTpcRiemannTrackingTask::SetSortingParameters(
                 bool sortingMode,
                 int sorting,
                 double interactionZ){
  _sortingMode=sortingMode;
  _sorting=sorting;
  _interactionZ=interactionZ;
}

void
PndTpcRiemannTrackingTask::SetTrkFinderParameters(
                 double proxcut,
					       double riproxcut,
					       double planecut,
					       double szcut,
					       unsigned int minpointsforfit){
  _proxcut=proxcut;
  _riproxcut=riproxcut;
  _planecut=planecut;
  _szcut=szcut;
  _minpoints=minpointsforfit;
}

void
PndTpcRiemannTrackingTask::SetTrkMergerParameters(
					       double TTproxcut,
					       double TTszcut,
					       double TTplanecut){
  _TTproxcut=TTproxcut;
  _TTszcut=TTszcut;
  _TTplanecut=TTplanecut;
}

InitStatus
PndTpcRiemannTrackingTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","Cluster-array not found!");
      return kERROR;
    }
  // Get input collection
/*
  _mvdArray=(TClonesArray*) ioman->GetObject("MVDPoint");

  if(_mvdArray==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","mvd-array not found!");
    }

*/

  // create and register output array
  _trackArray = new TClonesArray("GFTrack");
  ioman->Register("TrackPreFit","GenFit",_trackArray,true);

  _riemannTrackArray = new TClonesArray("PndTpcRiemannTrack");
  ioman->Register("RiemannTrack","Tpc",_riemannTrackArray,_persistence);

  _riemannHitArray = new TClonesArray("PndTpcRiemannHit");
  ioman->Register("RiemannHit","Tpc",_riemannHitArray,_persistence);
    
  
  _trackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("PndTrackCandTpc","Tpc",_trackCandArray,_persistence);

  _pndTrackArray = new TClonesArray("PndTrack");
  ioman->Register("PndTrackTpc","Tpc",_pndTrackArray,_persistence);


  //if(_field==NULL){
  //  Error("DemoRiemannTrackingTask::Init","BField not found!");
  //  return kERROR;
  // 
  //_fieldIfc=new FairFieldAdaptor(_field);

  _trackfinder= new PndTpcRiemannTrackFinder();
  _trackfinder->setSorting(_sorting);
  _trackfinder->setInteractionZ(_interactionZ);
  _trackfinder->setSortingMode(_sortingMode);
  _trackfinder->setMinHitsForFit(_minpoints);
  _trackfinder->setScale(_riemannscale);

  // Hit-Track Correlators
  _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut));
  _trackfinder->addCorrelator(new PndTpcRiProxHTCorrelator(_riproxcut));
  _trackfinder->addCorrelator(new PndTpcSzHTCorrelator(_szcut));
  _trackfinder->addCorrelator(new PndTpcRiemannHTCorrelator(_planecut));

  // Track-Track Correlators
  _trackfinder->addTTCorrelator(new PndTpcProximityTTCorrelator(_TTproxcut));
  _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));
  _trackfinder->addTTCorrelator(new PndTpcSzTTCorrelator(_TTszcut));
 
  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",100,0,100);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1.01);
  _trackMcIdsH=new TH1D("trkmcids","# mcids in track",25,0,25);
  
  // GeanePro will get Geometry and BField from the Run
  //_geanePro = new FairGeanePro();
  

  colors.push_back(kRed);
  colors.push_back(kGreen);
  colors.push_back(kBlue);
  colors.push_back(kCyan+1);
  colors.push_back(kMagenta);
  colors.push_back(kYellow+1);
  colors.push_back(kGray);
  colors.push_back(kRed-7);
  colors.push_back(kSpring+5);
  colors.push_back(kCyan-3);
  colors.push_back(kOrange+1);

  TDirectory* tmp=gDirectory;
  tmp->cd();
  if(fStore) {
    fHistoFile = new TFile(fHistoFileName, "update");
  }

  return kSUCCESS;
}

void
PndTpcRiemannTrackingTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcRiemannTrackingTask::Exec; Event Number: "<<counter<<std::endl;
  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No TrackArray");
   _trackArray->Delete();
  
  if(_pndTrackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No PndTrackArray");
     _pndTrackArray->Delete();
  if(_trackCandArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No TrackCandArray");
     _trackCandArray->Delete();

  if(_riemannTrackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No RiemannTrackArray");
     _riemannTrackArray->Delete();
  if(_riemannHitArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No RiemannHitArray");
     _riemannHitArray->Delete();

     
  // build histograms & visualisation of clusters
  TCanvas* canv;
  TPolyMarker3D clusters;
  if(fStore) {
    fHistoFile->cd();

    //chamber geometry:
    double xMin = -15.;
    double xMax = 15.;
    double yMin = -15.;
    double yMax = 15.;
    double zMin = 0.;
    double zMax = 70.;

    std::string canvName = "canv_Ev";
    std::string clName;
    std::stringstream ss;
    ss<<counter;
    canvName.append(ss.str());

    clName = "Clusters Ev_";
    clName.append(ss.str());
    fHistCont["clHist"] = new TH3D(clName.c_str(), "Clusters", 10,xMin,xMax,
             10,yMin,yMax, 10, zMin, zMax);
    fHistCont["clHist"]->SetMarkerStyle(20);
    fHistCont["clHist"]->SetMarkerSize(0.5);

    fHistCont["clHist2"] = (TH3D*)fHistCont["clHist"]->Clone();
    clName = "Riemann trackcands Ev_";
    clName.append(ss.str());
    fHistCont["clHist2"]->SetNameTitle(clName.c_str(),"Riemann trackcands");

    fHistCont["clHist4"] = (TH3D*)fHistCont["clHist"]->Clone();
    clName = "merged Riemann trackcands Ev_";
    clName.append(ss.str());
    fHistCont["clHist4"]->SetNameTitle(clName.c_str(),"merged Riemann trackcands");


    fHistCont["clHist6"] = (TH3D*)fHistCont["clHist"]->Clone();
    clName = "GF trackcands Ev_";
    clName.append(ss.str());
    fHistCont["clHist6"]->SetNameTitle(clName.c_str(), "GF trackcands");

    clName = "Riemann trackcands (on sphere) Ev_";
    clName.append(ss.str());
    fHistCont["clHist3"] = new TH3D(clName.c_str(), "Riemann trackcands", 10,-0.6,0.6,
             10,-0.6,0.6, 10, -0.1, 1.1);
    fHistCont["clHist3"]->SetMarkerStyle(20);
    fHistCont["clHist3"]->SetMarkerSize(0.5);

    fHistCont["clHist5"] = (TH3D*)fHistCont["clHist3"]->Clone();
    clName = "merged Riemann trackcands (on sphere) Ev_";
    clName.append(ss.str());
    fHistCont["clHist5"]->SetNameTitle(clName.c_str(), "merged Riemann trackcands");

    canv = new TCanvas(canvName.c_str());
    counter++;

    unsigned int totCl=_clusterArray->GetEntriesFast();
    TVector3 pos;

    clusters.SetMarkerStyle(20);
    clusters.SetMarkerSize(0.1);

    for(unsigned int i=0;i<totCl;++i){
      PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(i);
      pos = cl->pos();

      // visualisation of clusters
      clusters.SetNextPoint(pos.X(), pos.Y(), pos.Z());
    }
  }


  //get the magnetic field for curvature seeding
   double Bz=0;
   FairField* field=FairRunAna::Instance()->GetField();
   bool CField = dynamic_cast<PndConstField*>(field);
   GFFieldManager::getInstance()->init(new PndFieldAdaptor(field));
   if(!CField) {
     std::cerr<<"PndTpcRiemannTrackingTask: "
              <<"No const field! Curvature seeding not valid... Setting Bz=2T"
              <<std::endl;
     Bz=2.;
   }
   //this is crap, but better than hardcoding for the moment ...
   else
     Bz=field->GetBz(0.,0.,0.);


  unsigned int ncl=_clusterArray->GetEntriesFast();
  std::vector<PndTpcCluster*> clusterlist(ncl);
  
  for(unsigned int icl=0; icl<ncl; ++icl){
    clusterlist[icl] = (PndTpcCluster*)_clusterArray->At(icl);
  }
  std::cout<<"RiemannTrackingTask number of clusters: "<<ncl<<std::endl;

  std::vector<PndTpcRiemannTrack*> riemannlist;

  _trackfinder->buildTracks(clusterlist,riemannlist);

  // visualisation of TrackCands
  std::vector<TPolyMarker3D*> markerlist;
  std::vector<TPolyLine3D*>   markerlistLines;
  std::vector<TPolyMarker3D*> RiemannMarkerlist;
  std::vector<TPolyLine3D*>   RiemannMarkerlistLines;
  std::vector<TPolyLine3D*>   RiemannLines;
  if(fStore) {
    // loop over candidates
    for(unsigned int ir=0;ir<riemannlist.size();++ir){
      PndTpcRiemannTrack* trkcand = riemannlist[ir];
      unsigned int nhits=trkcand->getNumHits();
      markerlist.push_back(new TPolyMarker3D(nhits));
      markerlist.back()->SetMarkerStyle(20);
      markerlist.back()->SetMarkerSize(0.5);
      markerlistLines.push_back(new TPolyLine3D(nhits));
      RiemannMarkerlist.push_back(new TPolyMarker3D(nhits));
      RiemannMarkerlist.back()->SetMarkerStyle(20);
      RiemannMarkerlist.back()->SetMarkerSize(0.5);
      RiemannMarkerlistLines.push_back(new TPolyLine3D(nhits));
      RiemannLines.push_back(new TPolyLine3D());

      int colour = ir%colors.size();
      markerlist.back()->SetMarkerColor(colors[colour]);
      markerlistLines.back()->SetLineColor(colors[colour]);
      RiemannMarkerlist.back()->SetMarkerColor(colors[colour]);
      RiemannMarkerlistLines.back()->SetLineColor(colors[colour]);
      RiemannLines.back()->SetLineColor(colors[colour]);


      // loop over clusters
      for(unsigned int ih=0;ih<nhits;++ih){
        PndTpcCluster* cl = trkcand->getHit(ih)->cluster();
        TVector3 clpos = cl->pos();
        markerlist[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
        markerlistLines[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());

        PndTpcRiemannHit* ch = trkcand->getHit(ih);
        clpos = ch->x();
        RiemannMarkerlist[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
        RiemannMarkerlistLines[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
      }

      // visualisation of the plane
      if(trkcand->isFittedPlane()){
        TVector3 n = trkcand->n();
        TVector3 av = trkcand->av();
        TVector3 nz(0.,0.,1.);

        //vector perp to n in x y plane
        TVector3 perpXY = n.Cross(nz);
        TVector3 perp   = n.Cross(perpXY);
        perpXY.SetMag(0.15);
        perp.SetMag(0.15);

        double c = -1*trkcand->c();
        RiemannLines[ir]->SetNextPoint(av[0]+perpXY[0], av[1]+perpXY[1], av[2]+perpXY[2]); //
        RiemannLines[ir]->SetNextPoint(av[0]-perpXY[0], av[1]-perpXY[1], av[2]-perpXY[2]); //
        RiemannLines[ir]->SetNextPoint(av[0],           av[1],           av[2]); //
        RiemannLines[ir]->SetNextPoint(av[0]+perp[0],   av[1]+perp[1],   av[2]+perp[2]); //
        RiemannLines[ir]->SetNextPoint(av[0]-perp[0],   av[1]-perp[1],   av[2]-perp[2]); //
      }
    }
  } // end visualisation of TrackCands

  if(_mergeTracks)
    _trackfinder->mergeTracks(riemannlist);

  // visualisation of merged TrackCands
  std::vector<TPolyMarker3D*> markerlist2;
  std::vector<TPolyLine3D*>   markerlistLines2;
  std::vector<TPolyMarker3D*> RiemannMarkerlist2;
  std::vector<TPolyLine3D*>   RiemannMarkerlistLines2;
  std::vector<TPolyLine3D*>   RiemannLines2;
  std::vector<TPolyLine3D*>   seeds;

  if(fStore) {
    // loop over candidates
    for(unsigned int ir=0;ir<riemannlist.size();++ir){
      PndTpcRiemannTrack* trkcand = riemannlist[ir];
      unsigned int nhits=trkcand->getNumHits();
      markerlist2.push_back(new TPolyMarker3D(nhits));
      markerlist2.back()->SetMarkerStyle(20);
      markerlist2.back()->SetMarkerSize(0.5);
      markerlistLines2.push_back(new TPolyLine3D(nhits));
      RiemannMarkerlist2.push_back(new TPolyMarker3D(nhits));
      RiemannMarkerlist2.back()->SetMarkerStyle(20);
      RiemannMarkerlist2.back()->SetMarkerSize(0.5);
      RiemannMarkerlistLines2.push_back(new TPolyLine3D(nhits));
      RiemannLines2.push_back(new TPolyLine3D());

      int colour = ir%colors.size();
      markerlist2.back()->SetMarkerColor(colors[colour]);
      markerlistLines2.back()->SetLineColor(colors[colour]);
      RiemannMarkerlist2.back()->SetMarkerColor(colors[colour]);
      RiemannMarkerlistLines2.back()->SetLineColor(colors[colour]);
      RiemannLines2.back()->SetLineColor(colors[colour]);

      // loop over clusters
      for(unsigned int ih=0;ih<nhits;++ih){
        PndTpcCluster* cl = trkcand->getHit(ih)->cluster();
        TVector3 clpos = cl->pos();
        markerlist2[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
        markerlistLines2[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());

        PndTpcRiemannHit* ch = trkcand->getHit(ih);
        clpos = ch->x();
        RiemannMarkerlist2[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
        RiemannMarkerlistLines2[ir]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
      }

      if(trkcand->isFittedPlane()){
        TVector3 n = trkcand->n();
        TVector3 av = trkcand->av();
        TVector3 nz(0.,0.,1.);

        //vector perp to n in x y plane
        TVector3 perpXY = n.Cross(nz);
        TVector3 perp   = n.Cross(perpXY);
        perpXY.SetMag(0.15);
        perp.SetMag(0.15);

        double c = -1*trkcand->c();
        RiemannLines2[ir]->SetNextPoint(av[0]+perpXY[0], av[1]+perpXY[1], av[2]+perpXY[2]); //
        RiemannLines2[ir]->SetNextPoint(av[0]-perpXY[0], av[1]-perpXY[1], av[2]-perpXY[2]); //
        RiemannLines2[ir]->SetNextPoint(av[0],           av[1],           av[2]); //
        RiemannLines2[ir]->SetNextPoint(av[0]+perp[0],   av[1]+perp[1],   av[2]+perp[2]); //
        RiemannLines2[ir]->SetNextPoint(av[0]-perp[0],   av[1]-perp[1],   av[2]-perp[2]); //
      }
    }
  } // end visualisation of TrackCands


  // build GFTrackCands
  std::vector<GFTrackCand*> candlist;
  std::vector<TPolyLine3D*> lines;

  unsigned int nr=riemannlist.size();
  for(unsigned int ir=0; ir<nr; ++ir){ // loop over Riemann tracks

    int minhits = 4; // minimum hits needed to build pndtrackcands and GFTrackCands
    if(minhits<_minpoints) minhits=_minpoints;
    double pbackup = 2.;  // momentum value that is set when other initialisations fail

    PndTpcRiemannTrack* trk=riemannlist[ir];
    int nhits=trk->getNumHits();
    
    // store PndTpcRiemannTracks in output array
    new((*_riemannTrackArray)[_riemannTrackArray->GetEntriesFast()]) PndTpcRiemannTrack(*trk);
    for(unsigned int ih=0;ih<nhits;++ih){
      PndTpcRiemannHit* hit=trk->getHit(ih);
      new ((*_riemannHitArray)[_riemannHitArray->GetEntriesFast()]) PndTpcRiemannHit(*hit);
    }

    std::cout<<"Tracklet "<<ir<<"   nhits="<<nhits;

    // check if enough points
    if(nhits<_minpoints || nhits<minhits){
      std::cout<<" - skipping, not enough hits: "<<nhits<<std::endl;
      continue;
    }
    
    // check if momentum not high enough
    // calculate momentum
    // p = 0.3*BR/dip (R in meters, B in T; we have R in cm, B in kG)
    double p;
    double trackR = trk->r();
    double trackDip = trk->dip();
    if (TMath::Abs(sin(trackDip))<0.1) p=pbackup;
    else p=trackR/sin(trackDip)*0.0003*Bz; 
    if (Bz==0) p=pbackup;
    if(p<4E-3) {
      std::cout<<" - skipping, momentum too small: "<<p*1E3<<" MeV"<<std::endl;
      //continue;
    }
    std::cout<<std::endl;

    // store pndtracks and pndcands in output array
    PndTrackCand* pndcand=new((*_trackCandArray)[_trackCandArray->GetEntriesFast()]) PndTrackCand();
    PndTrack* pndtrack=new((*_pndTrackArray)[_pndTrackArray->GetEntriesFast()]) PndTrack();
    pndtrack->SetTrackCand(*pndcand);

    // create GFTrackCands
    GFTrackCand* cand=new GFTrackCand();

    // fill hits into GFTrackCands and pndcands from small to big Radius
    bool invertedTrack = false;
    double r1=trk->getHit(0)->cluster()->pos().Perp();
    double r2=trk->getHit(nhits-1)->cluster()->pos().Perp();
    if(r1<=r2){
      for(unsigned int ih=0;ih<nhits;++ih){
        cand->addHit(2,trk->getHit(ih)->cluster()->index());
        pndcand->AddHit(2,trk->getHit(ih)->cluster()->index(),trk->getHit(ih)->cluster()->pos().Mag());
      }
    }
    else {
      for(unsigned int ih=nhits-1;ih>0;--ih){
        cand->addHit(2,trk->getHit(ih)->cluster()->index());
        pndcand->AddHit(2,trk->getHit(ih)->cluster()->index(),trk->getHit(ih)->cluster()->pos().Mag());
      }
      cand->addHit(2,trk->getHit(0)->cluster()->index());  
      pndcand->AddHit(2,trk->getHit(0)->cluster()->index(),trk->getHit(0)->cluster()->pos().Mag()); 
      invertedTrack = true;
    }// finished filling hits


    //
    // calculate seed values
    //

    // build approximate momentum vector
    std::vector<TVector3> slidingAvrg;
    if(!invertedTrack)
      for(int i=0; i<minhits; ++i) 
        slidingAvrg.push_back( trk->getHit(i)->cluster()->pos() );
    else
      for(int i=nhits-1; i>nhits-1-minhits; --i) 
        slidingAvrg.push_back( trk->getHit(i)->cluster()->pos() );
    
    while(slidingAvrg.size()>2){      
      for(int i=0; i<slidingAvrg.size()-1; ++i) 
        slidingAvrg[i] = 0.5*(slidingAvrg[i]) + 0.5*(slidingAvrg[i+1]);
      slidingAvrg.pop_back();
    }

    TVector3 direction=(slidingAvrg[1]-slidingAvrg[0]);
    direction.SetMag(1.);

    TVector3 mom = p * direction;
    TVector3 momerr(0.1*fabs(mom.X()),0.1*fabs(mom.Y()),0.1*fabs(mom.Z()));

    // start position
    TVector3 pos1;
    if(!invertedTrack) pos1 = trk->getHit(0)->cluster()->pos();
    else pos1 = trk->getLastHit()->cluster()->pos();
    TVector3 poserr(0.3,0.3,0.3);

    // pdg
    int pdg = trk->winding()>0 ? 211 : -211; // Todo: pions hardcoded atm
    if(Bz<0) pdg *= -1.;

    std::cout<<" Radius of track [cm]: " << trackR << std::endl;
    std::cout<<" Dip of track [deg]:   " << trackDip/TMath::Pi()*180 << std::endl;
    std::cout<<" seed values: "<<std::endl;
    std::cout<<"  start position: ";
    pos1.Print();
    std::cout<<"  momentum [GeV]: "<<p<<std::endl;
    std::cout<<"  p_perp [GeV]:   " << trackR*0.0003*Bz <<std::endl;
    std::cout<<"  direction: ";    
    direction.Print();
    std::cout<<"  winding: "<<trk->winding()<<std::endl;
    std::cout<<"  invertedTrack: "<<invertedTrack<<std::endl;
    std::cout<<"  pdg id: "<<pdg<<std::endl;


    // set seed values to cands
    pndcand->setTrackSeed(pos1,direction,1./p);

    cand->setCurv(trackR); //  actually this is never used
    cand->setDip(trk->dip());

    RKTrackRep* rep = new RKTrackRep(pos1, mom, poserr, momerr,pdg);

    candlist.push_back(cand);


    // check Monte Carlo Truth
    McIdCollection mcid;
    mcid.ClearData();
    for(unsigned int i=0;i<cand->getNHits();++i){
      unsigned int detId;
      unsigned int hitId;
      cand->getHit(i,detId,hitId);
      mcid.AddIDCollection(clusterlist[hitId]->mcId());
    }
    _trackPurityH->Fill(mcid.MaxRelWeight());
    _trackMcIdsH->Fill(mcid.nIDs());
    _trackSizeH->Fill(cand->getNHits());


    // store GFTracks in output array
    GFTrack* gftrk=new((*_trackArray)[_trackArray->GetEntriesFast()]) GFTrack(rep);
    gftrk->setCandidate(*cand); // here the candidate is copied!
    //Is this what we want?
    

    // visualisation of seed values
    if(fStore){
      seeds.push_back(new TPolyLine3D(2));
      seeds.back()->SetLineStyle(2);
      seeds.back()->SetLineColor(kGray);
      seeds.back()->SetNextPoint(pos1.X(), pos1.Y(), pos1.Z());
      mom *= 100;
      seeds.back()->SetNextPoint(pos1.X()+mom.X(), pos1.Y()+mom.Y(), pos1.Z()+mom.Z());
    }

  }// end loop over tracks
  

  std::cout<<"PndTpcRiemannTrackingTask::Exec:: "
	   <<candlist.size()<<" track candidates found."<<std::endl;
  _multiplicityHisto->Fill(candlist.size());


  if(fStore) {
    // visualisation of final GFTrackCands
    std::vector<TPolyMarker3D*> markerlist3;
    std::vector<TPolyLine3D*>   markerlistLines3;

    // loop over candidates
    for(unsigned int ic=0;ic<candlist.size();++ic){
      GFTrackCand* trkcand = candlist[ic];
      unsigned int numhits=trkcand->getNHits();
      markerlist3.push_back(new TPolyMarker3D(numhits));
      markerlist3.back()->SetMarkerStyle(20);
      markerlist3.back()->SetMarkerSize(0.5);
      markerlistLines3.push_back(new TPolyLine3D(numhits));

      int colour = ic%colors.size();
      markerlist3.back()->SetMarkerColor(colors[colour]);
      markerlistLines3.back()->SetLineColor(colors[colour]);

      // loop over clusters
      for(unsigned int ih=0;ih<numhits;++ih){
        unsigned int detId;
        unsigned int hitId;
        trkcand->getHit(ih,detId,hitId);
        PndTpcCluster* cl=(PndTpcCluster*)_clusterArray->At(hitId);
        TVector3 clpos = cl->pos();
        markerlist3[ic]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
        markerlistLines3[ic]->SetNextPoint(clpos.X(), clpos.Y(), clpos.Z());
      }
    }

    canv->Divide(3,2);
    TVirtualPad* thePad = canv->cd(1);
    thePad->GetListOfPrimitives()->Add(fHistCont["clHist"]);
    thePad->GetListOfPrimitives()->Add(&clusters);

    thePad = canv->cd(2);
    thePad->GetListOfPrimitives()->Add(fHistCont["clHist2"]);
    for(unsigned int k=0; k<markerlist.size(); k++){
      thePad->GetListOfPrimitives()->Add(markerlist[k]);
      thePad->GetListOfPrimitives()->Add(markerlistLines[k]);
    }
    for(unsigned int l=0; l<lines.size(); l++)
      thePad->GetListOfPrimitives()->Add(lines[l]);

    thePad = canv->cd(3);
    thePad->GetListOfPrimitives()->Add(fHistCont["clHist3"]);
    for(unsigned int k=0; k<RiemannLines.size(); k++){
      thePad->GetListOfPrimitives()->Add(RiemannMarkerlist[k]);
      thePad->GetListOfPrimitives()->Add(RiemannMarkerlistLines[k]);
      thePad->GetListOfPrimitives()->Add(RiemannLines[k]);
    }

    thePad = canv->cd(4);
    thePad->GetListOfPrimitives()->Add(fHistCont["clHist4"]);
    for(unsigned int k=0; k<markerlist2.size(); k++){
      thePad->GetListOfPrimitives()->Add(markerlist2[k]);
      thePad->GetListOfPrimitives()->Add(markerlistLines2[k]);
    }

    thePad = canv->cd(5);
    thePad->GetListOfPrimitives()->Add(fHistCont["clHist5"]);
    for(unsigned int k=0; k<RiemannLines2.size(); k++){
      thePad->GetListOfPrimitives()->Add(RiemannMarkerlist2[k]);
      thePad->GetListOfPrimitives()->Add(RiemannMarkerlistLines2[k]);
      thePad->GetListOfPrimitives()->Add(RiemannLines2[k]);
    }

    thePad = canv->cd(6);
    thePad->GetListOfPrimitives()->Add(fHistCont["clHist6"]);
    for(unsigned int k=0; k<markerlist3.size(); k++){
      thePad->GetListOfPrimitives()->Add(markerlist3[k]);
      thePad->GetListOfPrimitives()->Add(markerlistLines3[k]);
    }
    for(unsigned int k=0; k<seeds.size(); k++)
      thePad->GetListOfPrimitives()->Add(seeds[k]);

    //save the canvas
    fHistoFile->cd();
    canv->Write();

    for(unsigned int i=0; i<lines.size(); ++i) delete lines[i];
    for(unsigned int i=0; i<markerlist.size(); ++i) delete markerlist[i];
    for(unsigned int i=0; i<markerlist2.size(); ++i) delete markerlist2[i];
    for(unsigned int i=0; i<markerlist3.size(); ++i) delete markerlist3[i];
    for(unsigned int i=0; i<markerlistLines.size(); ++i) delete markerlistLines[i];
    for(unsigned int i=0; i<markerlistLines2.size(); ++i) delete markerlistLines2[i];
    for(unsigned int i=0; i<markerlistLines3.size(); ++i) delete markerlistLines3[i];
    for(unsigned int i=0; i<RiemannMarkerlist.size(); ++i) delete RiemannMarkerlist[i];
    for(unsigned int i=0; i<RiemannMarkerlist2.size(); ++i) delete RiemannMarkerlist2[i];
    for(unsigned int i=0; i<RiemannMarkerlistLines.size(); ++i) delete RiemannMarkerlistLines[i];
    for(unsigned int i=0; i<RiemannMarkerlistLines2.size(); ++i) delete RiemannMarkerlistLines2[i];
    for(unsigned int i=0; i<RiemannLines.size(); ++i) delete RiemannLines[i];
    for(unsigned int i=0; i<RiemannLines2.size(); ++i) delete RiemannLines2[i];
    for(unsigned int i=0; i<seeds.size(); ++i) delete seeds[i];
    for(std::map<std::string,TH1*>::iterator it =fHistCont.begin();it!=fHistCont.end();++it) delete (*it).second;

    fHistCont.clear();
    delete canv;
    //fHistoFile->Close();
  }

  return;
}

void
  PndTpcRiemannTrackingTask::SetStoreHistograms(TString file) {
  fStore=true;
  fHistoFileName=file;
}

void
  PndTpcRiemannTrackingTask::WriteHistograms(const TString& filename) {
  std::cerr<< "PndTpcRiemannTrackingTask::WriteHistograms"<<std::endl;

  TFile* file=FairRootManager::Instance()->GetOutFile();
  file->mkdir("RiemannTracking");
  file->cd("RiemannTracking");

  _multiplicityHisto->Write();
  delete _multiplicityHisto;
  _multiplicityHisto=NULL;

  _trackSizeH->Write();
  delete _trackSizeH;
  _trackSizeH=NULL;

  _trackPurityH->Write();
  delete _trackPurityH;
  _trackPurityH=NULL;

  _trackMcIdsH->Write();
  delete _trackMcIdsH;
  _trackMcIdsH=NULL;
}
