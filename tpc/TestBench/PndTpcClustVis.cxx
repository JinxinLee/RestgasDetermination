//***************************************************
// Event Display implementation: 3D View
// For the GEM-TPC decoding/monitoring software
//
// based on the genfit Display written by Karl Bicker
//
// author: Johannes Rauch
//         E18, Technische Universitaet Muenchen
//
//***************************************************

#include "PndTpcClustVis.h"

#include "PndTpcAbsClusterFinder.h"
#include "PndTpcClusterFinder.h"
#include "PndTpcClusterFinderSimple.h"
#include "PndTpcSPHit.h"

#include "TGeoManager.h"
#include "TStopwatch.h"
#include "FairRootManager.h"

#include "GFAbsTrackRep.h"
#include "GFAbsRecoHit.h"
#include "GFDetPlane.h"
#include "GFException.h"
#include "GFKalman.h"
#include "GFTools.h"
#include "GFTrack.h"
#include "GFTrackCand.h"

#include "GeaneTrackRep.h"
#include "GeaneTrackRep2.h"
#include "RKTrackRep.h"

#define DEBUG 0


PndTpcClustVis* PndTpcClustVis::eventDisplay = NULL;


PndTpcClustVis::PndTpcClustVis():
  tree(NULL), digisBranch(NULL), clustersBranch(NULL), preFitBranch(NULL), postFitBranch(NULL),
  guiEvent(0), fEventId(0), ClHasChanged(true),
  doClustering(false), ClMode(2), ClTimeslice(3),ClTimecut(2),
  ClSingleDigiClAmpCut(15), ClClAmpCut(9),
  ClElPerADC(600.), ClErrorNorm(300.),
  ClSimpleCl(true), ClSimpleTimeslice(4), ClSimpleMaxClusterSlice(3000),
  instantRedraw(true), drawTpc(false), drawRawDigis(false), drawDigis(false),
  drawClusters(true), drawClusterErrors(false),
  drawRiemannTracks(true), drawFitMarkers(false),
  doPR(true), doMerge(true), doClean(false),
  _sorting(3), _interactionZ(0), _sortingMode(true),
  PRNHits(1000000),
  _minpoints(4), _planecut(0.04), _riproxcut(0.1), _szcut(0.2), _proxcut(1.9), _helixcut(0.4),
  _TTproxcut(2.5), _TTplanecut(0.025), _TTszcut(0.33), _TTdipcut(0.1), _TThelixcut(0.6),
  PRHasChanged(true),
  fRiemannScale(8.6),
  doFit(false), useGeane(false), numIts(0), smooth(false), Bz(0)
{
  if(!gApplication) {
    std::cout << "In PndTpcClustVis ctor: gApplication not found, creating..." << std::flush;
    TApplication* rootApp = new TApplication("ROOT_application", 0, 0);
    std::cout << "done!" << std::endl;
  }
  if(!gEve) {
    std::cout << "In PndTpcClustVis ctor: gEve not found, creating..." << std::flush;
    TEveManager::Create();
    std::cout << "done!" << std::endl;
  }

  //init colors
  //colors.push_back(kRed);
  colors.push_back(kGreen);
  colors.push_back(kBlue);
  colors.push_back(kCyan+1);
  colors.push_back(kMagenta);
  colors.push_back(kYellow+1);
  colors.push_back(kRed-7);
  colors.push_back(kSpring+5);
  colors.push_back(kCyan-3);
  colors.push_back(kOrange+1);

  // Build hit factory -----------------------------
  clusterArray = new TClonesArray("PndTpcCluster");
  _theRecoHitFactory = new GFRecoHitFactory();
  _theRecoHitFactory->addProducer(2,new GFRecoHitProducer<PndTpcCluster,PndTpcSPHit>(clusterArray));
}


void PndTpcClustVis::initDigimapper(double drifField, 
                                    double gain, double spread,
                                    double zGem,
                                    double samplingFreq,
                                    double wallclock,
                                    std::string gasfile,
                                    std::string padplanefile,
                                    std::string padshapefile){
  fgain=gain;

  // init Digimapper
  std::cout<<"init DigiMapper with \n"
     <<" Drift Field   : "<<drifField<<std::endl
     <<" Gain          : "<<gain<<std::endl
     <<" Spread        : "<<spread<<std::endl
     <<" zGem          : "<<zGem<<std::endl
     <<" Sampling Freq : "<<samplingFreq<<std::endl
     <<" t0            : "<<wallclock<<std::endl
     <<" Gas           : "<<gasfile<<std::endl
     <<" PadPlane      : "<<padplanefile<<std::endl
     <<" PadShapes     : "<<padshapefile<<std::endl;

  fgas = new PndTpcGas(gasfile.c_str(), drifField);
  fgem = new PndTpcGem(gain, spread);
  fpadShapes = new PndTpcPadShapePool(padshapefile.c_str(),
                                      *fgem,
                                      0.5, // lookup range
                                      0.02, // Lookup Step
                                      0.01); // LookupIntegrationStep
  fpadplane = new PndTpcPadPlane(padplanefile.c_str(), fpadShapes);
  fzGem = zGem;
  double sf = samplingFreq;
  double t0 = wallclock; // time offset in ns
  PndTpcDigiMapper::getInstance(false)->init(fpadplane,fgem,fgas,fpadShapes,fzGem,t0,sf);

  // build a clusterbuffer for each sector;
  nsectors=PndTpcDigiMapper::getInstance()->getPadPlane()->GetNSectors();
  std::cerr << "Found " << nsectors << " sectors in padplane" << std::endl;
  for(unsigned int  isect=0;isect<nsectors;++isect){
    buffermap[isect]=new std::vector<PndTpcCluster*>;
  }
}


PndTpcClustVis* PndTpcClustVis::getInstance() {
  if(eventDisplay == NULL) {
    eventDisplay = new PndTpcClustVis();
  }
  return eventDisplay;
}


PndTpcClustVis::~PndTpcClustVis() { reset(); }


void PndTpcClustVis::reset() {}


void PndTpcClustVis::setTree(TTree* treeIn) {
  tree = treeIn;
  if(tree==NULL) {
    std::cerr<<"WARNING: Tree not found!"<<std::endl;
    exit(1);
  }
  //else tree->Print();

  tree->SetBranchAddress("PndTpcDigi", &digisBranch);
  if(digisBranch==NULL) std::cerr<<"WARNING: No Digi Branch found!"<<std::endl;
  else digisBranch->Print();

  tree->SetBranchAddress("PndTpcCluster", &clustersBranch);
  if(clustersBranch==NULL) std::cerr<<"WARNING: No Cluster Branch found!"<<std::endl;
  else clustersBranch->Print();

  if (digisBranch==NULL && clustersBranch==NULL) exit(1);

  //tree->SetBranchAddress("TrackPreFit", &preFitBranch);
}


void PndTpcClustVis::next(unsigned int stp) { gotoEvent(fEventId+stp);}

void PndTpcClustVis::prev(unsigned int stp) { gotoEvent(fEventId-stp);}


int PndTpcClustVis::getNEvents() { return tree->GetEntries(); }


void PndTpcClustVis::gotoEvent(int id) {
  if(id < 0) id = 0;
  else if(id >= tree->GetEntries()) id = tree->GetEntries() - 1;

  bool resetCam = kFALSE;
  if(id!=fEventId){
    resetCam = kTRUE;
    ClHasChanged=true;
    PRHasChanged=true;
  }

  fEventId = id;

  if(gEve->GetCurrentEvent()!=NULL) gEve->GetCurrentEvent()->DestroyElements();
  std::cout << "\nAt event " << fEventId << std::endl;
  drawEvent(fEventId, resetCam);
}


void PndTpcClustVis::open() {
  if(getNEvents() > 0) drawEvent(0);
  makeGui();
  gApplication->Run(kTRUE);
}


void PndTpcClustVis::drawEvent(unsigned int id, bool resetCam) {

  // Draw tpc
  if(drawTpc){
    // TODO: DRAW CORRECT FOR FOPI AND PANDA
    bool panda = false;
    if(fRiemannScale >10) panda = true;

    std::cerr<<"drawTpc..."<<std::endl;
    double tpcLength = 72.5;
    if (panda) tpcLength = 150;

    tpcLength*=0.5;
    TGeoMatrix* tpc_trans;
    if(panda)
      tpc_trans = new TGeoGenTrans(0,0,tpcLength-39.5, 1,1,1, 0);
    else
      tpc_trans = new TGeoGenTrans(0,0,tpcLength-62, 1,1,1, 0);

    TEveGeoShape* tpc_shape = new TEveGeoShape("tpc_shape");

    if(panda)
      tpc_shape->SetShape(new TGeoTube(15.,42., tpcLength));
    else
      tpc_shape->SetShape(new TGeoTube(5.,15., tpcLength));

    tpc_shape->SetTransMatrix(*tpc_trans);

    tpc_shape->SetMainColor(kBlue);
    tpc_shape->SetMainTransparency(80);
    gEve->AddElement(tpc_shape);
  }


  //std::cerr<<"tree->GetEntry("<<id<<")...";
  tree->GetEntry(id);
  //std::cerr<<"done"<<std::endl;

  //
  // Clustering
  //
  if(doClustering && digisBranch!=NULL && ClHasChanged){ // run ClusterFinder and fill fcluster_buffer (only if Clustering params have changed, else keep old clusters)
    std::cerr<<"Run Cluster finder..."<<std::endl;

    this->clearBufferMap();

    fcluster_buffer=buffermap[0];

    PndTpcAbsClusterFinder* ffinder = 0;

    if(!ClSimpleCl){
      ffinder=new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),
              fcluster_buffer, ClTimeslice, ClMode, -1,true,1.,ClTimecut,fgain/ClElPerADC,ClErrorNorm);
      ffinder->checkConsistency();
    }
    else{
      ffinder=new PndTpcClusterFinderSimple(PndTpcDigiMapper::getInstance()->getPadPlane(),
              fcluster_buffer, ClSimpleTimeslice,fgain/ClElPerADC,ClErrorNorm);
      ((PndTpcClusterFinderSimple*)(ffinder))->setNoXclust(false);
      ((PndTpcClusterFinderSimple*)(ffinder))->setMaxClusterSlice(ClSimpleMaxClusterSlice);
    }

    ffinder->setTrivialClustering(false);
    ffinder->saveRaw();
    ffinder->reset();
    
    //process digis
    int ndigis = digisBranch->GetEntriesFast();
    std::vector<PndTpcDigi*> digis(ndigis);

    for(int k=0; k<ndigis; ++k){
      PndTpcDigi* digi=(PndTpcDigi*)digisBranch->At(k);
      digis[k]=digi;
    }
    std::cout<<"number of digis: "<<ndigis<<std::endl;
    TStopwatch timer;
    timer.Start();
    try{
      ffinder->process(digis);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    } catch (...) {
      std::cout << "unknown exception..." << std::endl;
    }
    timer.Stop();
    Double_t ctime = timer.CpuTime();
    std::cerr<<"cpu time for clustering: "<<ctime<<std::endl;

    delete ffinder;

    // clean up clusters
    int i=0;
    while(i<fcluster_buffer->size()){
      if( ((*fcluster_buffer)[i])->amp() <= ClClAmpCut * ((*fcluster_buffer)[i])->size() ||
          (((*fcluster_buffer)[i])->size()==1 && ((*fcluster_buffer)[i])->amp()<=ClSingleDigiClAmpCut)){
        delete (*fcluster_buffer)[i];
        (*fcluster_buffer).erase( (*fcluster_buffer).begin()+i );
      }
      else ++i;
    }

  }
  else if(!doClustering && clustersBranch!=NULL){ // fill clusters in cluster_buffer (and use buffermap)
    this->clearBufferMap();
    std::cerr<<"Fetching clusters from cluster branch..."<<std::endl;
    unsigned int ncl=clustersBranch->GetEntriesFast();
    for(unsigned int isect=0;isect<nsectors;++isect)
      buffermap[isect]->reserve(ncl/nsectors+10);
    for(unsigned int i=0; i<ncl; ++i){
      PndTpcCluster *cluster = (PndTpcCluster*)clustersBranch->At(i);
      buffermap[cluster->sector()]->push_back(cluster);
    }
    std::cout << "number of clusters: " << ncl << std::endl;
  } //  end else (read clusters from file)
  else{
    std::cerr<<"Warning: no clusters were created"<<std::endl;
  }

  // copy clusters to TClonesArray needed for RecoHitFactory
  if(doFit){
    std::cerr<<"copy clusters to TClonesArray needed for RecoHitFactory..."<<std::endl;
    unsigned int icl = 0;
    for(unsigned int isect=0;isect<nsectors;++isect){
      for(unsigned int i=0; i<buffermap[isect]->size(); ++i){
        PndTpcCluster* cl = (*(buffermap[isect]))[i];
        cl->SetIndex(icl);
        new ((*clusterArray)[icl]) PndTpcCluster(*cl);
        ++icl;
      }

    }
  }

  //
  // DRAW
  //
  if(drawRawDigis && digisBranch!=NULL){ // draw raw digis
    std::cerr<<"draw raw digis..."<<std::endl;
    unsigned int ndigis = digisBranch->GetEntriesFast();
    std::cout<<"number of digis: "<<ndigis<<std::endl;
    for(unsigned int j=0; j<ndigis; ++j){ // loop over digis
      const PndTpcDigi* digi=(PndTpcDigi*)digisBranch->At(j);
      this->drawDigi(digi);
    } // end loop over digis
  }// end draw raw digis


  // draw clusters and digis
  if(drawDigis || drawClusters){
    std::cerr<<"draw digis and/or clusters..."<<std::endl;
    for(unsigned int isect=0;isect<nsectors;++isect){ // loop over sectors
      fcluster_buffer=buffermap[isect];
      unsigned int ncl=fcluster_buffer->size();
      std::cerr << "number of clusters: " << ncl << " in sector " << isect << std::endl;
      unsigned int tenpercent=(unsigned int)(ncl*0.1);

      for(unsigned int i=0; i<ncl; i++){ // loop over clusters
        //************ Progress messages **********************
        // if(i%10000==0){std::cout<<".";std::cout.flush();}
        // if(i%tenpercent==0){
        //   std::cout<<"["
        //            <<ceil((double)i*100/(double)ncl)<<"%"
        //            <<"]";
        //   std::cout.flush();
        // }
        // ****************************************************

        int colour = i%colors.size();

        PndTpcCluster *cluster = (*fcluster_buffer)[i];

        // get Digis from Cluster & draw
        if(drawDigis){
          int ndigis = cluster->nDigi();

          for(unsigned int j=0; j<ndigis; ++j){ // loop over digis
            const PndTpcDigi* digi = cluster->getDigi(j);
            this->drawDigi(digi, false, colors[colour]);
          } // end loop over digis
        } // end draw digis

        if(drawClusters && !doPR) this->drawCluster(cluster, colors[colour]);

      }// end loop over clusters
    }// end loop over sectors;
    std::cout << std::endl;
  }
  // end DRAW

  //
  // Pattern Reco
  //
  if(doPR && (PRHasChanged || ClHasChanged)){
    std::cerr << "Starting Pattern Reco..." << std::endl;

    PRHasChanged=false;

    // clean up riemannlist!
    for(int i=0; i<riemannlist.size(); ++i){
      if(riemannlist[i]!=NULL) delete riemannlist[i];
    }
    riemannlist.clear();

    // init TrackFinder
    PndTpcRiemannTrackFinder* _trackfinder= new PndTpcRiemannTrackFinder();
    _trackfinder->setSorting(_sorting);
    _trackfinder->setInteractionZ(_interactionZ);
    _trackfinder->setSortingMode(_sortingMode);
    _trackfinder->setMinHitsForFit(_minpoints);
    _trackfinder->setScale(fRiemannScale);
    _trackfinder->setMaxNumHitsForPR(PRNHits);

    // Hit-Track Correlators
    _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut));
    _trackfinder->addCorrelator(new PndTpcHelixHTCorrelator(_helixcut));

    // Track-Track Correlators
    _trackfinder->addTTCorrelator(new PndTpcProximityTTCorrelator(_TTproxcut));
    _trackfinder->addTTCorrelator(new PndTpcDipTTCorrelator(_TTdipcut, _TThelixcut));
    _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));

    /// PLAN: 
    /// 1) build several cluster buffer, sectorwise
    /// 2) run trackfinder over each clusterbuffer independently
    /// 3) put all found tracklets into one list
    /// 4) then do start merging

    std::vector<PndTpcRiemannTrack*> riemannTemp;

    // loop over sectors
    for(unsigned int isect=0;isect<nsectors;++isect){
      std::cerr << "... building tracks in sector " << isect << std::endl;
      fcluster_buffer=buffermap[isect];
      _trackfinder->buildTracks(*fcluster_buffer,riemannTemp);
      if(doClean) _trackfinder->cleanTracks(riemannlist, _szcut, _planecut);
      if(doMerge) _trackfinder->mergeTracks(riemannTemp);

      // copy tracklets of this sector to global list
      unsigned int ntrklts=riemannTemp.size();
      riemannlist.reserve(riemannlist.size()+ntrklts);
      for(unsigned int it=0;it<ntrklts;++it){
        riemannlist.push_back(riemannTemp[it]);
      }
      riemannTemp.clear();
    } // end loop over sectors

    if(doClean) _trackfinder->cleanTracks(riemannlist, _szcut, _planecut);

    if(doMerge && nsectors>1) {
      if(_sorting==3){
        _trackfinder->setSorting(2);
        _trackfinder->mergeTracks(riemannlist);
        _trackfinder ->setSorting(_sorting);
      }
      _trackfinder->mergeTracks(riemannlist);
    }

    if(doClean && nsectors>1) _trackfinder->cleanTracks(riemannlist, _szcut, _planecut);


    // print MCIDs
    /*for(unsigned int itrk=0; itrk<riemannlist.size(); ++itrk){
      std::cout<<"Riemann Track "<<itrk<<std::endl;
      for (unsigned int icl=0; icl<riemannlist[itrk]->getNumHits(); ++icl){
        std::cout<<" Cluster "<<icl<<std::endl;
        for (unsigned int imc=0; imc<riemannlist[itrk]->getHit(icl)->cluster()->nMcIds(); ++imc){
          std::cout<<"   McId "<<imc<<"  "<<riemannlist[itrk]->getHit(icl)->cluster()->mcId().ID(imc)<<std::endl;
        }
      }
    }*/

    std::cerr << "Pattern Reco finished" << std::endl;
  } // end PR

  ClHasChanged=false;


  // draw PR results
  if(doPR && (drawClusters || drawRiemannTracks)){
    std::cerr<<"draw PR results..."<<std::endl;
    for(unsigned int ir=0;ir<riemannlist.size();ir+=1){ // loop over trackcands
      PndTpcRiemannTrack* trkcand = riemannlist[ir];
      unsigned int nhits=trkcand->getNumHits();

      int colour = ir%colors.size();

      TVector3 old_track_pos, pos;
      TEveStraightLineSet* track_lines = NULL;

      for(unsigned int ih=0;ih<nhits;++ih){ // loop over clusters
        PndTpcCluster* cluster = trkcand->getHit(ih)->cluster();

        if(drawClusters) this->drawCluster(cluster, colors[colour]);

        // connect clusters
        if(drawRiemannTracks){
          pos = cluster->pos();
          if(track_lines==NULL) track_lines = new TEveStraightLineSet;
          if(ih > 0) {
            track_lines->AddLine(old_track_pos(0), old_track_pos(1), old_track_pos(2), pos(0), pos(1), pos(2));
            track_lines->SetMainColor(colors[colour]);
            track_lines->SetLineWidth(1);
          }
          old_track_pos = pos;
        }

      } // end loop over clusters

      if(track_lines != NULL) gEve->AddElement(track_lines);
    } // end loop over trackcands
  } // end draw PR results


  //
  // Build GFTracks, run Kalman and draw track
  //
  if(doPR && doFit){
    std::cerr<<"Fitting..."<<std::endl;
    GFFieldManager::getInstance()->init(new GFConstField(0.,0.,Bz));


    int minhits = 3; // minimum hits needed to build pndtrackcands and GFTrackCands
    if(minhits<_minpoints) minhits=_minpoints;
    double pbackup = 2.;  // momentum value that is set when other initialisations fail

    // loop over riemann tracks
    for(unsigned int itrk=0; itrk<riemannlist.size(); ++itrk){
      PndTpcRiemannTrack* trk=riemannlist[itrk];
      int nhits=trk->getNumHits();

      if (DEBUG) std::cout<<"Tracklet "<<itrk<<"   nhits = "<<nhits;

      // check if enough points
      if(nhits<_minpoints || nhits<minhits){
        std::cout<<" - skipping, not enough hits"<<std::endl;
        continue;
      }
      // check if track too steep
      double trackSinDip = trk->sinDip();
      if (TMath::Abs(trackSinDip)<0.01) {
        if (DEBUG) std::cout<<" - skipping, sin(dip) too small: "<<trackSinDip<<std::endl;
        continue;
      }
      // ceck if momentum high enough
      double p = trk->getMom(Bz);
      if (Bz==0) p=pbackup;
      if(p<1E-4) {
        if (DEBUG) std::cout<<" - skipping, momentum too small: "<<p*1E3<<" MeV"<<std::endl;
        continue;
      }
      if (DEBUG) std::cout<<std::endl;

      // create GFTrackCands
      GFTrackCand* cand=new GFTrackCand();

      // fill hits into GFTrackCands and pndcands from small to big Radius
      bool invertedTrack = false;
      double r1=trk->getHit(0)->cluster()->pos().Perp();
      double r2=trk->getHit(nhits-1)->cluster()->pos().Perp();
      if(r1<=r2 || true){
        for(unsigned int ih=0; ih<nhits; ++ih){
          cand->addHit(2,trk->getHit(ih)->cluster()->index());
        }
      }
      else {
        for(unsigned int ih=nhits; ih>0; --ih){
          cand->addHit(2,trk->getHit(ih-1)->cluster()->index());
        }
        invertedTrack = true;
      }// finished filling hits


      // get seed values
      int winding = trk->winding(); // we look in z direction!

      TVector3 pos1, direction;

      if(invertedTrack) {
        trk->getPosDirOnHelix(trk->getNumHits()-1, pos1, direction);
        direction *= -1.;
        winding*=-1.;
      }
      else trk->getPosDirOnHelix(0, pos1, direction);

      TVector3 poserr(0.3,0.3,0.3);

      TVector3 mom = p * direction;
      TVector3 momerr(fabs(mom.X()),fabs(mom.Y()),fabs(mom.Z()));
      momerr *= 1./TMath::Sqrt(nhits);

      // pdg
      int pdg = winding * 211; // Todo: pions hardcoded atm
      if(Bz<0) pdg *= -1;

      double trackR = trk->r();

      if (DEBUG) {
        double trackDip = trk->dip();
        std::cout<<" center of track "; trk->center().Print();
        std::cout<<" Radius of track [cm]: " << trackR << std::endl;
        std::cout<<" Dip of track [deg]:   " << trackDip/TMath::Pi()*180 << std::endl;
        std::cout<<" seed values: "<<std::endl;
        std::cout<<"  start position: "; pos1.Print();
        std::cout<<"  momentum [GeV]: "<<p<<std::endl;
        std::cout<<"  p_perp [GeV]:   " << trackR*0.0003*Bz <<std::endl;
        std::cout<<"  direction: "; direction.Print();
        std::cout<<"  winding: "<<winding<<std::endl;
        std::cout<<"  invertedTrack: "<<invertedTrack<<std::endl;
        std::cout<<"  pdg id: "<<pdg<<std::endl;
      }

      cand->setCurv(trackR); //  actually this is never used
      cand->setDip(trk->dip());

      GFAbsTrackRep* rep;

      if(useGeane){
        TGeant3* geant3  = new  TGeant3("C++ Interface to Geant3");
        GeanePro = new FairGeanePro();
        const GFDetPlane* initialPlane = new GFDetPlane(pos1, direction);
        const TVector3 cmom(mom);
        const TVector3 cposerr(poserr);
        const TVector3 cmomerr(momerr);

        //rep = new GeaneTrackRep(GeanePro, *initialPlane, cmom, cposerr, cmomerr, pdg/TMath::Abs(pdg)*3, pdg);
        rep = new GeaneTrackRep2(*initialPlane, cmom, cposerr, cmomerr, pdg);
      }
      else{
        rep = new RKTrackRep(pos1, mom, poserr, momerr,pdg);
      }


      GFTrack* track=new GFTrack(rep, smooth);
      track->setCandidate(*cand); // here the candidate is copied!
      delete cand;

      //
      // run Kalman
      //
      GFKalman fitter;
      fitter.setNumIterations(numIts);

      // Load RecoHits
      try {
        track->addHitVector(_theRecoHitFactory->createMany(track->getCand()));
      }
      catch(GFException& e) {
        std::cout << e.what() << std::endl;
        e.info();
        throw e;
      }

      if(numIts>0){
        try{
          std::cerr << "Calling processTrack ... ";
          fitter.processTrack(track);
          std::cerr << "finished\n" << std::endl;
        }
        catch (GFException& e){
          std::cout<<e.what()<<std::endl;
          delete track;
          continue;
        }
      }

      //
      // DRAW Fit
      //
      double charge = rep->getCharge();

      assert(winding/charge>0);

      if(rep->getStatusFlag()) {
        std::cout << "Warning: Trying to display a track with status flag != 0...";
        if(smooth) {
          std::cout << "trying without smoothing!";
          smooth = false;
        }
        std::cout << std::endl;
      }

      TVector3 track_pos;
      TVector3 old_track_pos;

      TEveStraightLineSet* track_lines = NULL;

      for(int j = 0; j < nhits; j++) { // loop over all hits in the track

        GFAbsRecoHit* hit = track->getHit(j);
        GFDetPlane plane;

        // get the hit infos ------------------------------------------------------------------
        if(smooth && numIts>0) {
          TMatrixT<double> state;
          TMatrixT<double> cov;
          TMatrixT<double> auxInfo;
          GFTools::getSmoothedData(track, 0, j, state, cov, plane, auxInfo);
          rep->setData(state, plane, &cov, &auxInfo);
        } else {
          try{
            plane = hit->getDetPlane(rep);
            rep->extrapolate(plane);
          }
          catch(GFException& e) {
            std::cerr << "Error: Exception caught (getDetPlane): Hit " << j << " in Track " << itrk << " skipped!" << std::endl;
            std::cerr << e.what();
            break;
            /*if (e.isFatal()) {
              std::cerr<<"Fatal exception, skipping track"<<std::endl;
              break;
            }
            else{
              std::cerr<<"Exception, skipping hit"<<std::endl;
              continue;
            }*/
          }
        }
        track_pos = rep->getPos(plane);
        // finished getting the hit infos -----------------------------------------------------

        // sort hit infos into variables ------------------------------------------------------
        TVector3 o = plane.getO();
        TVector3 u = plane.getU();
        TVector3 v = plane.getV();

        std::string hit_type = hit->getPolicyName();

        bool planar_hit = false;
        bool planar_pixel_hit = false;
        bool space_hit = false;
        bool wire_hit = false;
        double_t hit_u = 0;
        double_t hit_v = 0;
        double_t plane_size = 4;
        double_t hit_res_u = 0.5;
        double_t hit_res_v = 0.5;
        double_t hit_res_z = 0.5;

        TMatrixT<double> hit_coords(hit->getHitCoord(plane));
        TMatrixT<double> hit_cov(hit->getHitCov(plane));
        int hit_coords_dim = hit_coords.GetNrows();

        if(hit_type == "GFPlanarHitPolicy") {
          planar_hit = true;
          if(hit_coords_dim == 1) {
            hit_u = hit_coords(0,0);
            hit_res_u = hit_cov(0,0);
          } else if(hit_coords_dim == 2) {
            planar_pixel_hit = true;
            hit_u = hit_coords(0,0);
            hit_v = hit_coords(1,0);
            hit_res_u = hit_cov(0,0);
            hit_res_v = hit_cov(1,1);
          }
        } else if (hit_type == "GFSpacepointHitPolicy") {
          space_hit = true;
          plane_size = 4;
        } else if (hit_type == "GFWireHitPolicy") {
          wire_hit = true;
          hit_u = hit_coords(0,0);
          plane_size = 4;
        } else {
          std::cout << "Track " << itrk << ", Hit " << j << ": Unknown policy name: skipping hit!" << std::endl;
          break;
        }

        if(plane_size < 4) plane_size = 4;
        // finished setting variables ---------------------------------------------------------

        // draw track if corresponding option is set ------------------------------------------
        if(true) {
          if(track_lines == NULL) track_lines = new TEveStraightLineSet;
          if(j > 0) track_lines->AddLine(old_track_pos(0), old_track_pos(1), old_track_pos(2), track_pos(0), track_pos(1), track_pos(2));
          old_track_pos = track_pos;
          if(charge > 0) track_lines->SetLineColor(kRed);
          else track_lines->SetLineColor(kAzure);
          track_lines->SetLineWidth(2);
          if(drawFitMarkers) track_lines->AddMarker(track_pos(0), track_pos(1), track_pos(2));
        }
        // finished drawing track -------------------------------------------------------------

      } // end loop over all hits in the track

      if(track_lines != NULL) gEve->AddElement(track_lines);
      delete track;
    }

  }

  if(resetCam) gEve->Redraw3D(kTRUE);
  else gEve->Redraw3D(kFALSE);
}


void PndTpcClustVis::clearBufferMap(){
  for(unsigned int isect=0;isect<nsectors;++isect){
    /*if (doClustering){
      for(unsigned icl=0; icl<buffermap[isect]->size(); ++icl){
        delete (*(buffermap[isect]))[icl];
      }
    }*/
    buffermap[isect]->clear();
  }
  clusterArray->Delete();
}


void PndTpcClustVis::drawDigi(const PndTpcDigi* digi, bool raw, Color_t color){
  // map digi
  TVector3 pos;
  if(digi->padId()<0) return;
  PndTpcDigiMapper::getInstance()->map(digi,pos);

  TGeoMatrix* det_trans = new TGeoGenTrans(pos.X(), pos.Y(), pos.Z(), 1,1,1, 0);
  TEveGeoShape* digi_shape = new TEveGeoShape("digi_shape");

  // calculate and norm amp
  double amp = digi->amp(); // should be ~ 6 .. 2000
  if(amp<1) return;
  amp = TMath::Log(amp); // ~ 0.8 .. 3.3
  amp *= 0.02;
  if(raw) amp -= 0.001;

  digi_shape->SetShape(new TGeoTube(0.,amp, 0.05 ) );
  digi_shape->SetTransMatrix(*det_trans);

  if(!raw) digi_shape->SetMainColor(color);
  else digi_shape->SetMainColor(kGray);
  if(!raw) digi_shape->SetMainTransparency(45);
  else digi_shape->SetMainTransparency(50);
  gEve->AddElement(digi_shape);
}


void PndTpcClustVis::drawCluster(const PndTpcCluster* cluster, Color_t color){
  TVector3 pos;
  TVector3 err;

  pos = cluster->pos();
  TGeoMatrix* det_trans = new TGeoGenTrans(pos.X(), pos.Y(), pos.Z(), 1., 1., 1., 0);
  TEveGeoShape* cluster_shape = new TEveGeoShape("cluster_shape");

  if(drawClusterErrors){
    err = cluster->sig();
    cluster_shape->SetShape(new TGeoBBox(err.X(), err.Y(), err.Z()) );
  }
  else cluster_shape->SetShape(new TGeoSphere(0., 0.25) );

  cluster_shape->SetTransMatrix(*det_trans);

  cluster_shape->SetMainColor(color);
  cluster_shape->SetMainTransparency(40);
  gEve->AddElement(cluster_shape);
}



void PndTpcClustVis::makeGui() {
  TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);

  TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1200, 800);
  frmMain->SetWindowName("Pandoras Playground");
  frmMain->SetCleanup(kDeepCleanup);

  TGTextButton* tb = 0;
  TGLabel* lbl = 0;
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();

  TGHorizontalFrame* hf = new TGHorizontalFrame(frmMain); {
    // evt number entry
    lbl = new TGLabel(hf, "Go to event: ");
    hf->AddFrame(lbl);
    guiEvent = new TGNumberEntry(hf, 0, 9,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99999);
    hf->AddFrame(guiEvent);
    guiEvent->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiGoto()");

    // redraw button
    tb = new TGTextButton(hf, "Redraw Event");
    hf->AddFrame(tb);
    tb->Connect("Clicked()", "PndTpcClustVis", fh, "guiGoto()");
  }
  frmMain->AddFrame(hf);


  // draw options
  hf = new TGHorizontalFrame(frmMain); {
    lbl = new TGLabel(hf, "\n Draw Options");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiInstantRedraw =  new TGCheckButton(hf, "Instant redraw after changing parameters");
    if(instantRedraw) guiInstantRedraw->Toggle();
    hf->AddFrame(guiInstantRedraw);
    guiInstantRedraw->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawTpc =  new TGCheckButton(hf, "Draw TPC");
    if(drawTpc) guiDrawTpc->Toggle();
    hf->AddFrame(guiDrawTpc);
    guiDrawTpc->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawRawDigis =  new TGCheckButton(hf, "Draw Digis from DigiBranch");
    if(drawRawDigis) guiDrawRawDigis->Toggle();
    hf->AddFrame(guiDrawRawDigis);
    guiDrawRawDigis->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawDigis =  new TGCheckButton(hf, "Draw Digis from Clusters");
    if(drawDigis) guiDrawDigis->Toggle();
    hf->AddFrame(guiDrawDigis);
    guiDrawDigis->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawClusters =  new TGCheckButton(hf, "Draw Clusters");
    if(drawClusters) guiDrawClusters->Toggle();
    hf->AddFrame(guiDrawClusters);
    guiDrawClusters->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawClustersErrors =  new TGCheckButton(hf, "Draw Cluster Errors");
    if(drawClusterErrors) guiDrawClustersErrors->Toggle();
    hf->AddFrame(guiDrawClustersErrors);
    guiDrawClustersErrors->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
    guiDrawRiemannTracks =  new TGCheckButton(hf, "Draw Riemann Tracks");
    if(drawRiemannTracks) guiDrawRiemannTracks->Toggle();
    hf->AddFrame(guiDrawRiemannTracks);
    guiDrawRiemannTracks->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawFitMarkers =  new TGCheckButton(hf, "Draw Fit-Markers");
    if(drawFitMarkers) guiDrawFitMarkers->Toggle();
    hf->AddFrame(guiDrawFitMarkers);
    guiDrawFitMarkers->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);


  // Clusterfinder Params
  hf = new TGHorizontalFrame(frmMain); {
    lbl = new TGLabel(hf, "\n Clustering");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDoClustering =  new TGCheckButton(hf, "Run Clustering");
    if(doClustering) guiDoClustering->Toggle();
    hf->AddFrame(guiDoClustering);
    guiDoClustering->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiMode = new TGNumberEntry(hf, ClMode, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2);
    hf->AddFrame(guiMode);
    guiMode->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Mode");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    giuTimeslice = new TGNumberEntry(hf, ClTimeslice, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 100);
    hf->AddFrame(giuTimeslice);
    giuTimeslice->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Timeslice");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    giuTimecut = new TGNumberEntry(hf, ClTimecut, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 100);
    hf->AddFrame(giuTimecut);
    giuTimecut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Timecut");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);


  hf = new TGHorizontalFrame(frmMain); {
    guiSingleDigiClAmpCut = new TGNumberEntry(hf, ClSingleDigiClAmpCut, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2000);
    hf->AddFrame(guiSingleDigiClAmpCut);
    guiSingleDigiClAmpCut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Single Digi-Cluster Amp cut");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiClAmpCut = new TGNumberEntry(hf, ClClAmpCut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2000);
    hf->AddFrame(guiClAmpCut);
    guiClAmpCut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Cluster Amp cut");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
    guiElPerADC = new TGNumberEntry(hf, ClElPerADC, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2000);
    hf->AddFrame(guiElPerADC);
    guiElPerADC->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Electrons per ADC count");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
    guiErrorNorm = new TGNumberEntry(hf, ClErrorNorm, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2000);
    hf->AddFrame(guiErrorNorm);
    guiErrorNorm->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Error Normalization Constant");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);



  hf = new TGHorizontalFrame(frmMain); {
    guiSimpleCl =  new TGCheckButton(hf, "Use Simple Clustering");
    if(ClSimpleCl) guiSimpleCl->Toggle();
    hf->AddFrame(guiSimpleCl);
    guiSimpleCl->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    giuSimpleTimeslice = new TGNumberEntry(hf, ClSimpleTimeslice, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 100);
    hf->AddFrame(giuSimpleTimeslice);
    giuSimpleTimeslice->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Timeslice for Simple Clustering");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    giuSimpleMaxClusterSlice = new TGNumberEntry(hf, ClSimpleMaxClusterSlice, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEAPositive,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 1000000);
    hf->AddFrame(giuSimpleMaxClusterSlice);
    giuSimpleMaxClusterSlice->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Clusterslice for Simple Clustering z sectoring");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  frmMain->MapSubwindows();
  frmMain->Resize();
  frmMain->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("Options & Clustering", 0);


  browser->StartEmbedding(TRootBrowser::kLeft);

  TGMainFrame* frmMain2 = new TGMainFrame(gClient->GetRoot(), 1200, 800);
  frmMain2->SetWindowName("Pandoras Playground");
  frmMain2->SetCleanup(kDeepCleanup);



  hf = new TGHorizontalFrame(frmMain2); {
    lbl = new TGLabel(hf, "\n Pattern Recognition");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiDoPR =  new TGCheckButton(hf, "Run Pattern Recognition");
    if(doPR) guiDoPR->Toggle();
    hf->AddFrame(guiDoPR);
    guiDoPR->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain2->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain2); {
    // evt number entry
    lbl = new TGLabel(hf, "Do PR up to Hit: ");
    hf->AddFrame(lbl);
    guiPRNHits = new TGNumberEntry(hf, PRNHits, 9,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99999);
    hf->AddFrame(guiPRNHits);
    guiPRNHits->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");


    // redraw button
    tb = new TGTextButton(hf, "Redraw Event");
    hf->AddFrame(tb);
    tb->Connect("Clicked()", "PndTpcClustVis", fh, "guiGoto()");
  }
  frmMain2->AddFrame(hf);

  // sorting parameters
  hf = new TGHorizontalFrame(frmMain2); {
    guisorting = new TGNumberEntry(hf, _sorting, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 4);
    hf->AddFrame(guisorting);
    guisorting->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Sorting Mode");
    hf->AddFrame(lbl);
  }
  //frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiinteractionZ = new TGNumberEntry(hf, _interactionZ, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          -80, 100);
    hf->AddFrame(guiinteractionZ);
    guiinteractionZ->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Z-position of interaction point (for sorting Mode 4)");
    hf->AddFrame(lbl);
  }
  //frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guisortingMode =  new TGCheckButton(hf, "Use sorting of riemann tracker");
    if(_sortingMode) guisortingMode->Toggle();
    hf->AddFrame(guisortingMode);
    guisortingMode->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain2->AddFrame(hf);

  // Trackfinder Parameters
  hf = new TGHorizontalFrame(frmMain2); {
    guiminpoints = new TGNumberEntry(hf, _minpoints, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 200);
    hf->AddFrame(guiminpoints);
    guiminpoints->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "min points for helix-fit");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiplanecut = new TGNumberEntry(hf, _planecut, 6,999, TGNumberFormat::kNESRealFour,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2);
    hf->AddFrame(guiplanecut);
    guiplanecut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Planecut");
    hf->AddFrame(lbl);
  }
  //frmMain2->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain2); {
    guiriproxcut = new TGNumberEntry(hf, _riproxcut, 6, 999, TGNumberFormat::kNESRealFour,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2);
    hf->AddFrame(guiriproxcut);
    guiriproxcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Riemann proximity cut");
    hf->AddFrame(lbl);
  }
  //frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiszcut = new TGNumberEntry(hf, _szcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiszcut);
    guiszcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "sz cut");
    hf->AddFrame(lbl);
  }
  //frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiproxcut = new TGNumberEntry(hf, _proxcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiproxcut);
    guiproxcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Proximity cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guihelixcut = new TGNumberEntry(hf, _helixcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guihelixcut);
    guihelixcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Helix cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);

  // Trackmerger Parameters
  hf = new TGHorizontalFrame(frmMain2); {
    guiDoMerge =  new TGCheckButton(hf, "Do TrackMerging");
    if(doMerge) guiDoMerge->Toggle();
    hf->AddFrame(guiDoMerge);
    guiDoMerge->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiTTproxcut = new TGNumberEntry(hf, _TTproxcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTTproxcut);
    guiTTproxcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT Proximity cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiTTplanecut = new TGNumberEntry(hf, _TTplanecut, 6,999, TGNumberFormat::kNESRealFour,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 0.99);
    hf->AddFrame(guiTTplanecut);
    guiTTplanecut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT plane cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain2); {
    guiTTdipcut = new TGNumberEntry(hf, _TTdipcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTTdipcut);
    guiTTdipcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT dip cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiTThelixcut = new TGNumberEntry(hf, _TThelixcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTThelixcut);
    guiTThelixcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT helix cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain2); {
    guiDoClean =  new TGCheckButton(hf, "Clean Tracks before and after merging");
    if(doClean) guiDoClean->Toggle();
    hf->AddFrame(guiDoClean);
    guiDoClean->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain2->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain2); {
    guiTTscale = new TGNumberEntry(hf, fRiemannScale, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTTscale);
    guiTTscale->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "RiemannScale");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);

  // Fitting Parameters
  hf = new TGHorizontalFrame(frmMain2); {
    lbl = new TGLabel(hf, "\n Fitting");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiDoFit =  new TGCheckButton(hf, "Run Kalman");
    if(doFit) guiDoFit->Toggle();
    hf->AddFrame(guiDoFit);
    guiDoFit->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiUseGeane =  new TGCheckButton(hf, "Use Geane");
    if(useGeane) guiUseGeane->Toggle();
    hf->AddFrame(guiUseGeane);
    guiUseGeane->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
  }
  //frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiNumIts = new TGNumberEntry(hf, numIts, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 20);
    hf->AddFrame(guiNumIts);
    guiNumIts->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
    lbl = new TGLabel(hf, "Number of iterations for the Kalman Filter");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiSmooth =  new TGCheckButton(hf, "Use smoothing");
    if(smooth) guiDoFit->Toggle();
    hf->AddFrame(guiSmooth);
    guiSmooth->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
  }
  frmMain2->AddFrame(hf);

  frmMain2->MapSubwindows();
  frmMain2->Resize();
  frmMain2->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("PR & Fitting", 0);
}


void PndTpcClustVis::guiGoto(){
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  Long_t n = guiEvent->GetNumberEntry()->GetIntNumber();
  fh->gotoEvent(n);
}


void PndTpcClustVis::guiSetDrawParams(){
  if (!guiInstantRedraw->IsOn()) instantRedraw=false;

  drawTpc=(guiDrawTpc->IsOn());
  drawRawDigis=(guiDrawRawDigis->IsOn());
  drawDigis=(guiDrawDigis->IsOn());
  drawClusters=(guiDrawClusters->IsOn());
  drawClusterErrors=(guiDrawClustersErrors->IsOn());
  doPR=(guiDoPR->IsOn());

  drawRiemannTracks=(guiDrawRiemannTracks->IsOn());
  drawFitMarkers=(guiDrawFitMarkers->IsOn());

  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);

  if (guiInstantRedraw->IsOn()) instantRedraw=true;
}


void PndTpcClustVis::guiSetClusterfinderParams(){
  ClHasChanged=true;

  doClustering=(guiDoClustering->IsOn());
  ClMode = guiMode->GetNumberEntry()->GetIntNumber();
  ClTimeslice = giuTimeslice->GetNumberEntry()->GetIntNumber();
  ClTimecut = giuTimecut->GetNumberEntry()->GetIntNumber();
  ClSingleDigiClAmpCut = guiSingleDigiClAmpCut->GetNumberEntry()->GetIntNumber();
  ClClAmpCut = guiClAmpCut->GetNumberEntry()->GetNumber();
  ClElPerADC = guiElPerADC->GetNumberEntry()->GetNumber();
  ClErrorNorm = guiErrorNorm->GetNumberEntry()->GetNumber();
  ClSimpleCl=(guiSimpleCl->IsOn());
  ClSimpleTimeslice = giuSimpleTimeslice->GetNumberEntry()->GetIntNumber();
  ClSimpleMaxClusterSlice = giuSimpleMaxClusterSlice->GetNumberEntry()->GetIntNumber();
  
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}


void PndTpcClustVis::guiSetTrackingParams(){
  PRHasChanged=true;

  PRNHits = guiPRNHits->GetNumberEntry()->GetIntNumber();
  _sorting = guisorting->GetNumberEntry()->GetIntNumber();
  _interactionZ = guiinteractionZ->GetNumberEntry()->GetNumber();
  _sortingMode=(guisortingMode->IsOn());
  _minpoints = guiminpoints->GetNumberEntry()->GetIntNumber();
  _planecut = guiplanecut->GetNumberEntry()->GetNumber();
  _riproxcut = guiriproxcut->GetNumberEntry()->GetNumber();
  _szcut = guiszcut->GetNumberEntry()->GetNumber();
  _proxcut = guiproxcut->GetNumberEntry()->GetNumber();
  _helixcut = guihelixcut->GetNumberEntry()->GetNumber();
  _TTproxcut = guiTTproxcut->GetNumberEntry()->GetNumber();
  _TTplanecut = guiTTplanecut->GetNumberEntry()->GetNumber();
  //_TTszcut = guiTTszcut->GetNumberEntry()->GetNumber();
  _TTdipcut = guiTTdipcut->GetNumberEntry()->GetNumber();
  _TThelixcut = guiTThelixcut->GetNumberEntry()->GetNumber();
  fRiemannScale=guiTTscale->GetNumberEntry()->GetNumber();
  doMerge=(guiDoMerge->IsOn());
  doClean=(guiDoClean->IsOn());
  
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}


void PndTpcClustVis::guiSetFittingParams(){
  doFit=(guiDoFit->IsOn());
  useGeane=(guiUseGeane->IsOn());
  numIts=guiNumIts->GetNumberEntry()->GetNumber();
  smooth=(guiSmooth->IsOn());

  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}


ClassImp(PndTpcClustVis)
