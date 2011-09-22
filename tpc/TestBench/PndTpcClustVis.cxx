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
#include "PndMultiField.h"
#include "PndMCTrack.h"

#include "TGeoManager.h"
#include "TStopwatch.h"
#include "FairRootManager.h"
#include "TDatabasePDG.h"

#include "GFAbsTrackRep.h"
#include "GFAbsRecoHit.h"
#include "GFDetPlane.h"
#include "GFException.h"
#include "GFKalman.h"
#include "GFDaf.h"
#include "GFTools.h"
#include "GFTrack.h"
#include "GFTrackCand.h"

#include "FairGeane.h"
#include "FairGeanePro.h"
#include "GeaneTrackRep.h"
#include "GeaneTrackRep2.h"
#include "RKTrackRep.h"

#include "TCanvas.h"
#include "TH2D.h"

#include "DebugLogger.h"

#include "PndDetectorList.h"

#include <iostream>
#include <typeinfo>

#define DEBUG 0
#define MINMOM 0.2 //minimum momentum for efficiency calculation


PndTpcClustVis* PndTpcClustVis::eventDisplay = NULL;


PndTpcClustVis::PndTpcClustVis():
  tree(NULL), digisBranch(NULL), clustersBranch(NULL), preFitBranch(NULL), postFitBranch(NULL), MCTrackBranch(NULL),
  NsectorsToProcess(16), fpurityCut(1.), fpurityLoCut(0.),
  guiEvent(0), fEventId(0), ClHasChanged(true),
  doClustering(false), ClMode(2), ClTimeslice(4),ClTimecut(2),
  ClSingleDigiClAmpCut(15), ClClAmpCut(9),
  ClElPerADC(600.), ClErrorNorm(300.),
  ClSimpleCl(true), ClSimpleTimeslice(7), ClSimpleMaxClusterSlice(3000),
  instantRedraw(true), drawTpc(true), TpcTransp(80), drawRawDigis(false), drawDigis(false),
  drawClusters(false), drawClusterErrors(false),
  drawRiemannTracks(true), drawPOCA(false), drawFitMarkers(false),
  plotHistos(false),
  doPR(true), clearUnfitted(true), doMerge(false),
  doClean(false),
 
  _sortingMode(true),
  _sorting(3), 
  _interactionZ(0.),

 /* _minpoints(3), // panda settings
  _proxcut(1.9),
  _proxZstretch(1.6),
  _helixcut(0.2),
  _maxRMS(0.15),

  doGlobMerge(true),
  _TTproxcut(15.0),
  _TTdipcut(0.2),
  _TThelixcut(0.5),
  _TTplanecut(0.3),*/

  // FOPI settings
  _minpoints(3),
  _proxcut(2.0),
  _proxZstretch(1.6),
  _helixcut(0.4),
  _maxRMS(0.3),

  doGlobMerge(true),
  _TTproxcut(15.0),
  _TTdipcut(0.2),
  _TThelixcut(0.5),
  _TTplanecut(0.3),
 
  doMergeCurlers(true),


  _TTszcut(0.33),
  PRNHits(999999999), PRStage(11),
  _planecut(0.04), _riproxcut(0.1), _szcut(0.2),
  
  PRHasChanged(true),
  fRiemannScale(8.6), initDip(4.),
  doFit(false), useDAF(false), invertCharge(false), useGeane(false), numIts(0), smooth(false), Bz(0)
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
  _theRecoHitFactory->addProducer(kTpcCluster,new GFRecoHitProducer<PndTpcCluster,PndTpcSPHit>(clusterArray));
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
  fnsectors=PndTpcDigiMapper::getInstance()->getPadPlane()->GetNSectors();
  std::cerr << "Found " << fnsectors << " sectors in padplane" << std::endl;
  for(unsigned int  isect=0;isect<fnsectors;++isect){
    fbuffermap[isect]=new std::vector<PndTpcCluster*>;
  }

  // init geane stuff
  //fRun = new FairRunAna();
  //GeanePro = new FairGeanePro(); // todo: crashes
  //fRun->SetBeamMom(15);
  //PndMultiField *fField= new PndMultiField("FULL");
  //fRun->SetField(fField);



  // init histos
  PREffAll = new TH1D("PREffAll", "found tracks/total MC tracks", 101, 0,1.01) ;
  trkPurityAll = new TH1D("trkPurityAll", "Track Purity", 101,0,1.01) ;
  trkCompletenessAll = new TH1D("trkCompletenessAll", "Track Completeness", 101,0,1.01) ;

  PREffPrim = new TH1D("PREffPrim", "found tracks/total MC tracks - Primaries", 101, 0,1.01) ;
  trkPurityPrim = new TH1D("trkPurityPrim", "Track Purity - Primaries", 101,0,1.01) ;
  trkCompletenessPrim = new TH1D("trkCompletenessPrim", "Track Completeness - Primaries", 101,0,1.01) ;

  PREffSec = new TH1D("PREffSec", "found tracks/total MC tracks - Secondaries", 101, 0,1.01) ;
  trkPuritySec = new TH1D("trkPuritySec", "Track Purity - Secondaries", 101,0,1.01) ;
  trkCompletenessSec = new TH1D("trkCompletenessSec", "Track Completeness - Secondaries", 101,0,1.01) ;


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

  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("PndTpcDigi.*",1);
  tree->SetBranchStatus("PndTpcCluster.*",1);

  tree->SetBranchAddress("PndTpcDigi", &digisBranch);
  if(digisBranch==NULL) std::cerr<<"WARNING: No Digi Branch found!"<<std::endl;
  else digisBranch->Print();

  tree->SetBranchAddress("PndTpcCluster", &clustersBranch);
  if(clustersBranch==NULL) std::cerr<<"WARNING: No Cluster Branch found!"<<std::endl;
  else clustersBranch->Print();

  if (digisBranch==NULL && clustersBranch==NULL) exit(1);

  //tree->SetBranchAddress("TrackPreFit", &preFitBranch);
}

void PndTpcClustVis::setMCTree(TTree* treeIn) {
  mctree = treeIn;
  if(mctree==NULL) {
    std::cerr<<"WARNING: MC Tree not found!"<<std::endl;
    exit(1);
  }
  //else tree->Print();
  mctree->SetBranchStatus("*",0);
  mctree->SetBranchStatus("MCTrack.*",1);

  mctree->SetBranchAddress("MCTrack", &MCTrackBranch);
  if(MCTrackBranch==NULL) std::cerr<<"WARNING: No MCTrack Branch found!"<<std::endl;
  else MCTrackBranch->Print();

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


  // init histos
  /*PREffAll = new TH1D("PREffAll", "found tracks/total MC tracks (>50 MeV)", 101, 0,1.01) ;
  trkPurityAll = new TH1D("trkPurityAll", "Track Purity (>50 MeV)", 101,0,1.01) ;
  trkCompletenessAll = new TH1D("trkCompletenessAll", "Track Completeness (>50 MeV)", 101,0,1.01) ;

  PREffPrim = new TH1D("PREffPrim", "found tracks/total MC tracks - Primaries (>50 MeV)", 101, 0,1.01) ;
  trkPurityPrim = new TH1D("trkPurityPrim", "Track Purity - Primaries (>50 MeV)", 101,0,1.01) ;
  trkCompletenessPrim = new TH1D("trkCompletenessPrim", "Track Completeness - Primaries (>50 MeV)", 101,0,1.01) ;

  PREffSec = new TH1D("PREffSec", "found tracks/total MC tracks - Secondaries (>50 MeV)", 101, 0,1.01) ;
  trkPuritySec = new TH1D("trkPuritySec", "Track Purity - Secondaries (>50 MeV)", 101,0,1.01) ;
  trkCompletenessSec = new TH1D("trkCompletenessSec", "Track Completeness - Secondaries (>50 MeV)", 101,0,1.01) ;
*/


  if (NsectorsToProcess>fnsectors) NsectorsToProcess=fnsectors;

  // Draw tpc
  double tpcLength(72.5);
  double tpcOffset(-62);
  bool panda = false;
  if(fRiemannScale >10) panda = true;
  if (panda) {
    tpcLength = 150;
    tpcOffset = -39.5;
  }

  if(drawTpc){

    std::cerr<<"drawTpc..."<<std::endl;

    TGeoMatrix* tpc_trans;
    tpc_trans = new TGeoGenTrans(0,0,0.5*tpcLength+tpcOffset, 1,1,1, 0);

    TEveGeoShape* tpc_shape = new TEveGeoShape("tpc_shape");

    if(panda)
      tpc_shape->SetShape(new TGeoTube(15.,42., 0.5*tpcLength));
    else
      tpc_shape->SetShape(new TGeoTube(5.,15., 0.5*tpcLength));

    tpc_shape->SetTransMatrix(*tpc_trans);

    tpc_shape->SetMainColor(kBlue);
    tpc_shape->SetMainTransparency(char(TpcTransp));
    gEve->AddElement(tpc_shape);
  }


  tree->GetEntry(id);

  //
  // Clustering
  //
  if(doClustering && digisBranch!=NULL && ClHasChanged){ // run ClusterFinder and fill fcluster_buffer (only if Clustering params have changed, else keep old clusters)
    std::cerr<<"Run Cluster finder..."<<std::endl;

    clearBufferMap();

    fcluster_buffer=fbuffermap[0];

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

    // distribute to sectors
    i=0;
    while(i<fcluster_buffer->size()){
      if( ((*fcluster_buffer)[i])->sector() != 0){
        fbuffermap[((*fcluster_buffer)[i])->sector()]->push_back(((*fcluster_buffer)[i]));
        (*fcluster_buffer).erase( (*fcluster_buffer).begin()+i );
      }
      else ++i;
    }

  }
  else if(!doClustering && clustersBranch!=NULL && ClHasChanged){ // fill clusters in cluster_buffer (and use fbuffermap)
    this->clearBufferMap();
    std::cerr<<"Fetching clusters from cluster branch..."<<std::endl;
    unsigned int ncl=clustersBranch->GetEntriesFast();
    for(unsigned int isect=0; isect<fnsectors; ++isect)
      fbuffermap[isect]->reserve(ncl/fnsectors + 2000);
    unsigned int createdClusters(0);
    for(unsigned int i=0; i<ncl; ++i){
      PndTpcCluster *cluster = (PndTpcCluster*)clustersBranch->At(i);

      // omit clusters outside chamber
      /*if (true && (//cluster->pos().X()<0 || // todo: take out again!!
                   cluster->pos().Z()<tpcOffset ||
                   cluster->pos().Z()>tpcLength+tpcOffset)) continue; // TODO: make configurable!!!!
*/
      fbuffermap[cluster->sector()]->push_back(cluster);
      ++createdClusters;
    }
    std::cout << "number of clusters: " << createdClusters << std::endl;
  } //  end else (read clusters from file)
  else{
    std::cerr<<"Warning: no clusters were created"<<std::endl;
  }

  TH2D* ClusterPurity;
  if(plotHistos){
    ClusterPurity = new TH2D("ClusterPurityVsR", "ClusterPurityVsR", 50, 0,50, 100,0,1);
  }
  
  // build fClustersPerId
  if (plotHistos && ClHasChanged) {
    std::cout << "build fClustersPerId" << std::endl;
    fClustersPerId.clear();

    nTotTrks=0;
    nTotTrksPrim=0;
    nTotTrksSec=0;

    for(unsigned int isect=0; isect<fnsectors; ++isect){
      for(unsigned int i=0; i<fbuffermap[isect]->size(); ++i){
        PndTpcCluster* cl = (*(fbuffermap[isect]))[i];
        McId ID = cl->mcId().DominantID();
        ClusterPurity->Fill(cl->pos().Perp(), cl->mcId().MaxRelWeight());
        
        if (fClustersPerId.count(ID) == 0) {
          fClustersPerId[ID] = 1;
        }
        else fClustersPerId[ID] += 1;
        // count tracks only with min number of hits
        if (fClustersPerId[ID] == 1) {
          // check momentum
          if (MCTrackBranch!=NULL){
            if(ID.mceventID()!=0){
              mctree->GetEntry(ID.mceventID()-1);
              double mom = ((PndMCTrack*)(MCTrackBranch->At(ID.mctrackID())))->GetMomentum().Mag();
              if (mom < MINMOM) continue;
            }
            else continue; // physics track
          }

          ++nTotTrks;
          if (ID.mcsecID()==0) ++nTotTrksPrim;
          else ++nTotTrksSec;
        }
      }
    }
  }// end build fClustersPerId

  // copy clusters to TClonesArray needed for RecoHitFactory
  if(doFit){
    std::cerr<<"copy clusters to TClonesArray needed for RecoHitFactory..."<<std::endl;
    unsigned int icl = 0;
    for(unsigned int isect=0;isect<fnsectors;++isect){
      for(unsigned int i=0; i<fbuffermap[isect]->size(); ++i){
        PndTpcCluster* cl = (*(fbuffermap[isect]))[i];
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
    for(unsigned int isect=0;isect<NsectorsToProcess;++isect){ // loop over sectors
      fcluster_buffer=fbuffermap[isect];
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
  TH2D* QualityVsPurity;
  TH1D* nHitsFoundTracks;
  TH1D* nHitsUnFoundTracks;
  TH1D* FoundMom;
  TH1D* UnFoundMom;
  TH1D* FoundTheta;
  TH1D* UnFoundTheta;
  TH1D* UnFoundTotCl;
  TH2D* RealVsMcMom;

  if(plotHistos){
    QualityVsPurity = new TH2D("QualityVsPurity", "QualityVsPurity", 100,0,1, 100,0,1);
    nHitsFoundTracks = new TH1D("nHitsFoundTracks","nHitsFoundTracks", 100, 0, 100);
    nHitsUnFoundTracks = new TH1D("nHitsUnFoundTracks","nHitsUnFoundTracks", 100, 0, 100);
    FoundMom = new TH1D("FoundMom","FoundMom", 1000, 0, 5);
    UnFoundMom = new TH1D("UnFoundMom","UnFoundMom > 10 hits", 1000, 0, 5);
    FoundTheta = new TH1D("FoundTheta","FoundTheta", 90, 0, 180);
    UnFoundTheta = new TH1D("UnFoundTheta","UnFoundTheta > 10 hits", 90, 0, 180);
    UnFoundTotCl = new TH1D("UnFoundTotCl","UnFound Total Clusters", 200, 0, 200);
    RealVsMcMom = new TH2D("RealVsMcMom", "RealVsMcMom", 500, 0, 2,  500, 0, 2);
  }


  if(doPR && (PRHasChanged || ClHasChanged)){
    std::cerr << "Starting Pattern Reco..." << std::endl;

    PRHasChanged=false;

    // clean up friemannlist!
    for(int i=0; i<friemannlist.size(); ++i){
      if(friemannlist[i]!=NULL) delete friemannlist[i];
    }
    friemannlist.clear();

    // init TrackFinder
    PndTpcRiemannTrackFinder* _trackfinder= new PndTpcRiemannTrackFinder();
    _trackfinder->setSorting(_sorting);
    _trackfinder->setInteractionZ(_interactionZ);
    _trackfinder->setSortingMode(_sortingMode);
    _trackfinder->setMinHitsForFit(_minpoints);
    _trackfinder->setScale(fRiemannScale);
    _trackfinder->setMaxNumHitsForPR(PRNHits);

    _trackfinder->setProxcut(_proxcut);
    _trackfinder->setHelixcut(_helixcut);
    _trackfinder->setTTProxcut(_TTproxcut);

    // Hit-Track Correlators
    _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut, _proxZstretch, _helixcut));
    _trackfinder->addCorrelator(new PndTpcHelixHTCorrelator(_helixcut));

    // Track-Track Correlators
    _trackfinder->addTTCorrelator(new PndTpcDipTTCorrelator(_TTproxcut, _TTdipcut, _TThelixcut));
    _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));


    //----------------------------------------------------------------------------------------------------


    unsigned int _minHitsZ(10);
    unsigned int _minHitsR(10);
    unsigned int _minHitsPhi(10);


    // copy the buffermap, because PR manipulates it!
    std::map<unsigned int, std::vector<PndTpcCluster*>*> fbuffermapCopy;
    for(unsigned int isect=0;isect<fnsectors;++isect){
      fbuffermapCopy[isect] = new std::vector<PndTpcCluster*>(*(fbuffermap[isect]));
    }

    unsigned int nTotCl(0); // number of total clusters
    for(unsigned int isect=0;isect<NsectorsToProcess;++isect){
      nTotCl += fbuffermapCopy[isect]->size();
    }

    std::vector<PndTpcRiemannTrack*> riemannTempSec; // temporary storage, reused for every sector

    TStopwatch timer;
    timer.Start();

    unsigned int currentStage(0);

    // first loop over sectors
    for(unsigned int isect=0;isect<NsectorsToProcess;++isect){
      if (fbuffermapCopy[isect]->size() == 0) continue;
      std::cerr << "\n... building tracks in sector " << isect << " from " << fbuffermapCopy[isect]->size() << " clusters" << std::endl;

      fcluster_buffer=fbuffermapCopy[isect];

      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 2, _minHitsZ, 0.7*_maxRMS);
      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 3, _minHitsR, _maxRMS);

      riemannTempSec.clear();
    } // end loop over sectors


    if(PRStage > currentStage++ && doGlobMerge && fnsectors>1) {
      std::cerr << "\n merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
      _trackfinder->mergeTracks(friemannlist);
      std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
    }


    // resectorize: 10 sectors, left and right half of chamber, each 5 sectors in z
    int factor, zSlice, nSlices(5);
    if (fnsectors > 2*nSlices){
      std::vector<PndTpcCluster*> remainingClusters;
      remainingClusters.reserve(200000);

      for(unsigned int isect=0; isect<fnsectors; ++isect){
        for(unsigned int iCl=0; iCl<fbuffermapCopy[isect]->size(); ++iCl){
          remainingClusters.push_back((*fbuffermapCopy[isect])[iCl]);
        }
      }
      clearBufferMap();
      PndTpcCluster* Cl;
      for(unsigned int iCl=0; iCl<remainingClusters.size(); ++iCl){
        Cl = remainingClusters[iCl];
        if (Cl->pos().X()<0) factor=0;
        else factor=1;

        for (zSlice=0; zSlice<nSlices; ++zSlice){
          if (Cl->pos().Z() < tpcOffset + (zSlice+1)*tpcLength/nSlices) break;
        }

        fbuffermapCopy[factor + 2*zSlice]->push_back(Cl);
      }
    }

    // second loop over sectors
    double lowLim(tpcOffset), upLim;
    for(unsigned int isect=0;isect<NsectorsToProcess;++isect){
      if (fbuffermapCopy[isect]->size() == 0) continue;
      std::cerr << "\n... building tracks in sector " << isect << " from " << fbuffermapCopy[isect]->size() << " clusters" << std::endl;

      fcluster_buffer=fbuffermapCopy[isect];

      currentStage=5;

      // fill riemannTempSec with tracks lying in the sector
      upLim = lowLim + tpcLength/nSlices;
      for (unsigned int i=0; i<friemannlist.size(); ++i){
        double z(friemannlist[i]->getFirstHit()->z());
        if (z>lowLim-2 && z<upLim+2){
          riemannTempSec.push_back(friemannlist[i]);
          continue;
        }

        z = friemannlist[i]->getLastHit()->z();
        if (z>lowLim-2 && z<upLim+2){
          riemannTempSec.push_back(friemannlist[i]);
          continue;
        }
      }
      lowLim=upLim;

      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 5, _minHitsPhi, _maxRMS);
      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, -5, _minHitsPhi, _maxRMS);

      riemannTempSec.clear();

    } // end loop over sectors


    if(doGlobMerge && fnsectors>1) {
      std::cerr << "\n merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
      _trackfinder->mergeTracks(friemannlist);
      std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
    }



    // third loop over sectors
    lowLim = tpcOffset;
    for(unsigned int isect=0;isect<NsectorsToProcess;++isect){
      if (fbuffermapCopy[isect]->size() == 0) continue;
      std::cerr << "\n... building tracks in sector " << isect << " from " << fbuffermapCopy[isect]->size() << " clusters" << std::endl;

      fcluster_buffer=fbuffermapCopy[isect];


      // fill riemannTempSec with tracks lying in the sector
      upLim = lowLim + tpcLength/nSlices;
      for (unsigned int i=0; i<friemannlist.size(); ++i){
        double z(friemannlist[i]->getFirstHit()->cluster()->pos().Z());
        if (z>lowLim-1 && z<upLim+1){
          riemannTempSec.push_back(friemannlist[i]);
          continue;
        }

        z = friemannlist[i]->getLastHit()->cluster()->pos().Z();
        if (z>lowLim-1 && z<upLim+1){
          riemannTempSec.push_back(friemannlist[i]);
          continue;
        }
      }
      lowLim=upLim;

      unsigned int nTrks = riemannTempSec.size();


      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 2, _minpoints+1, _maxRMS*1.5);
      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 3, _minpoints+3, _maxRMS);
      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 5, _minpoints+1, _maxRMS*1.5);
      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, -5, _minpoints+1, _maxRMS*1.5);

      riemannTempSec.clear();
    } // end loop over sectors


    if(doGlobMerge && fnsectors>1) {
      std::cerr << "\nfinal merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
      _trackfinder->mergeTracks(friemannlist);
      std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
    }




    if(doMergeCurlers){
      std::vector<PndTpcRiemannTrack*> riemannTempCurl;
      for (unsigned int i=0; i<friemannlist.size(); ++i){
        if (friemannlist[i]->isFitted() &&
            friemannlist[i]->getNumHits() > 5 &&
            friemannlist[i]->r() < 30. &&
            fabs(friemannlist[i]->m()*1.57) < 120){ // Pi/2
          riemannTempCurl.push_back(friemannlist[i]);
          friemannlist.erase(friemannlist.begin() + i);
          --i;
        }
      }
      PndTpcRiemannTrackFinder* trackfinder= new PndTpcRiemannTrackFinder();
      trackfinder->setSorting(_sorting);
      trackfinder->setInteractionZ(_interactionZ);
      trackfinder->setSortingMode(_sortingMode);
      trackfinder->setMinHitsForFit(_minpoints);
      trackfinder->setScale(fRiemannScale);
      trackfinder->setMaxNumHitsForPR(PRNHits);

      trackfinder->setProxcut(_proxcut);
      trackfinder->setTTProxcut(30.);


      // Track-Track Correlators
      double blowUp = 5.;
      trackfinder->addTTCorrelator(new PndTpcDipTTCorrelator(30., blowUp*_TTdipcut, blowUp*_TThelixcut));
      trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(1.5*_TTplanecut, 20));

      std::cerr << "\nmerge curlers: merge " << riemannTempCurl.size() << " tracks ... ";
      trackfinder->mergeTracks(riemannTempCurl);
      std::cerr << " done - created " << riemannTempCurl.size() << " merged tracks" <<std::endl;

      delete trackfinder;

      for (unsigned int i=0; i<riemannTempCurl.size(); ++i){
        friemannlist.push_back(riemannTempCurl[i]);
      }
    }// end merge curlers
    
    // clear unfitted and small tracklets with < 6 hits
    if(clearUnfitted){
      for (unsigned int i=0; i<friemannlist.size(); ++i){
        if (friemannlist[i]->getNumHits() < 6 ||
            !(friemannlist[i]->isFitted())){
          friemannlist.erase(friemannlist.begin() + i);
          --i;
        }
      }
    }

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();



    unsigned int nUsedCl(0);
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      nUsedCl += friemannlist[i]->getNumHits();
    }

    int nTracksFromPR = friemannlist.size();

    std::cerr << "Pattern Reco finished, found tracks: " << nTracksFromPR << "\n";
    std::cerr << "used " << nUsedCl << " of " << nTotCl << " Clusters \n";
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
    printf("RealTime=%f minutes, CpuTime=%f minutes\n",rtime/60.,ctime/60.);

    // fill histograms
    if(plotHistos){
      unsigned int foundTrks(0), foundTrksPrim(0), foundTrksSec(0);
      for (unsigned int i=0; i<friemannlist.size(); ++i){
        PndTpcRiemannTrack* trk = friemannlist[i];
        McId DomID = trk->mcid().DominantID();

        //check momentum
        if (MCTrackBranch!=NULL && DomID.mceventID()!=0){
          if (DomID.mceventID()!=0){
            mctree->GetEntry(DomID.mceventID()-1);
            double mom = ((PndMCTrack*)(MCTrackBranch->At(DomID.mctrackID())))->GetMomentum().Mag();
            RealVsMcMom->Fill(mom, trk->getMom(20));
            if (mom < MINMOM) {
              // todo: for testing purposes clear low mom tracks
              friemannlist.erase(friemannlist.begin() + i);
              --i;
              continue;
            }
          }
          else {
            // todo: for testing purposes clear physics tracks
            friemannlist.erase(friemannlist.begin() + i);
            --i;
            continue;
          }
        }

        int nCorrectCl(0);
        int nHits(trk->getNumHits());
        for (unsigned int iHit=0; iHit<nHits; ++iHit){
          if (trk->getHit(iHit)->cluster()->mcId().DominantID() == DomID) ++ nCorrectCl;
        }
        double completeness = double(nCorrectCl) / double(fClustersPerId[DomID]);

        //double purity = trk->mcid().MaxRelWeight();
        double purity = double(nCorrectCl)/double(nHits);

        if (completeness > 0.5 && purity > 0.5) {
          ++foundTrks;
          nHitsFoundTracks->Fill(nHits);
          FoundMom->Fill(trk->getMom(20));
          FoundTheta->Fill(trk->dip()*180/TMath::PiOver2());
          if (DomID.mcsecID()==0) ++foundTrksPrim;
          else ++foundTrksSec;
        }
        else {
          nHitsUnFoundTracks->Fill(nHits);
          UnFoundTotCl->Fill(fClustersPerId[DomID]);
          if (nHits > 10){
            UnFoundMom->Fill(trk->getMom(20));
            UnFoundTheta->Fill(trk->dip()*180/TMath::PiOver2());
          }
        }

        QualityVsPurity->Fill(purity, trk->distRMS());

        trkPurityAll->Fill(purity);
        trkCompletenessAll->Fill(completeness);

        if (DomID.mcsecID()==0) {
          trkPurityPrim->Fill(purity);
          trkCompletenessPrim->Fill(completeness);
        }
        else {
          trkPuritySec->Fill(purity);
          trkCompletenessSec->Fill(completeness);
        }

        // todo: for testing purposes clear found tracks
        /*if (completeness > 0.5 && purity > 0.5) {
          friemannlist.erase(friemannlist.begin() + i);
          --i;
        }*/
      }

      PREffAll->Fill(foundTrks/double(nTotTrks));
      PREffPrim->Fill(foundTrksPrim/double(nTotTrksPrim));
      if (nTotTrksSec > 0) PREffSec->Fill(foundTrksSec/double(nTotTrksSec));


      std::cerr << "\nTotal tracks in tpc: " << nTotTrks << " \tPrimaries: " << nTotTrksPrim << " \tSecondaries: " << nTotTrksSec << "\n";
      std::cerr << "Good \"found\" tracks: " << foundTrks << " \tOther tracks: " << nTracksFromPR - foundTrks << "\n";


      // draw histos
      std::cout << "Drawing histograms";

      TCanvas* c1 = new TCanvas();
      c1->Divide(3,3);

      c1->cd(1);
      PREffAll->Draw();
      c1->cd(2);
      trkPurityAll->Draw();
      c1->cd(3);
      trkCompletenessAll->Draw();

      c1->cd(4);
      PREffPrim->Draw();
      c1->cd(5);
      trkPurityPrim->Draw();
      c1->cd(6);
      trkCompletenessPrim->Draw();

      c1->cd(7);
      PREffSec->Draw();
      c1->cd(8);
      trkPuritySec->Draw();
      c1->cd(9);
      trkCompletenessSec->Draw();


      TCanvas* c2 = new TCanvas();
      c2->Divide(3,3);

      c2->cd(1);
      nHitsFoundTracks->Draw();
      c2->cd(2);
      nHitsUnFoundTracks->Draw();
      c2->cd(3);
      QualityVsPurity->Draw("colz");
      c2->cd(4);
      FoundMom->Draw();
      c2->cd(5);
      UnFoundMom->Draw();
      c2->cd(6);
      ClusterPurity->Draw("colz");
      c2->cd(7);
      FoundTheta->Draw();
      c2->cd(8);
      UnFoundTheta->Draw();
      c2->cd(9);
      RealVsMcMom->Draw("colz");
      //UnFoundTotCl->Draw();

      std::cout << " ... done\n";

    }// end plot histos

    // clear and delete buffermapCopy stuff
    for(unsigned int isect=0;isect<fnsectors;++isect){
      fbuffermapCopy[isect]->clear();
      delete fbuffermapCopy[isect];
    }

    //----------------------------------------------------------------------------------------------------
  } // end PR



  ClHasChanged=false;

  // draw PR results
  unsigned int counter=0;
  if(doPR && (drawClusters || drawRiemannTracks)){
    std::cerr<<"draw PR results..."<<std::endl;
    for(unsigned int ir=0; ir<friemannlist.size(); ir+=1){ // loop over trackcands

      PndTpcRiemannTrack* trkcand = friemannlist[ir];

      // cut on purity -> draw only bad tracks
      if((trkcand->mcid().MaxRelWeight()<fpurityLoCut || trkcand->mcid().MaxRelWeight()>fpurityCut) &&
         (fpurityLoCut>0. || fpurityCut<1.)){
	      //std::cerr << "Skippping good track" << std::endl;
	      continue;
      }


      unsigned int nhits=trkcand->getNumHits();

      int colour = ir%colors.size();

      TVector3 old_track_pos, pos;
      TEveStraightLineSet* track_lines = NULL;

      if(drawPOCA) old_track_pos = friemannlist[ir]->pocaToIP();


      for(unsigned int ih=0;ih<nhits;++ih){ // loop over clusters
        PndTpcCluster* cluster = trkcand->getHit(ih)->cluster();

        if(drawClusters) this->drawCluster(cluster, colors[colour]);

        // connect clusters
        if(drawRiemannTracks){
          pos = cluster->pos();
          if(track_lines==NULL) track_lines = new TEveStraightLineSet;
          if(ih > 0 || (ih==0 && drawPOCA && friemannlist[ir]->isFitted()) ) {
            track_lines->AddLine(old_track_pos(0), old_track_pos(1), old_track_pos(2), pos(0), pos(1), pos(2));
            track_lines->SetMainColor(colors[colour]);
            track_lines->SetLineWidth(1);
          }
          old_track_pos = pos;
        }

      } // end loop over clusters

      if(track_lines != NULL) gEve->AddElement(track_lines);
    } // end loop over trackcands
    std::cerr<<" done"<<std::endl;
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

    // init the fitters
    GFKalman* fitterK = new GFKalman();
    fitterK->setNumIterations(numIts);

    GFDaf* fitterDAF = new GFDaf();
    //fitterDAF->setBetas(81.,20.,8.,4.,1.,1.,1.);
    //fitterDAF->setProbCut(0.001);



    // loop over riemann tracks
    for(unsigned int itrk=0; itrk<friemannlist.size(); ++itrk){
      PndTpcRiemannTrack* trk=friemannlist[itrk];
      int nhits=trk->getNumHits();

      if (DEBUG) std::cout<<"Tracklet "<<itrk<<"   nhits = "<<nhits;

      // check if enough points
      if(nhits<minhits){
        std::cout<<" - skipping, not enough hits"<<std::endl;
        continue;
      }
      // check if fitted
      if(!trk->isFitted() && !trk->isInitialized() || trk->isInitialized()){
        std::cout<<" - skipping, riemann track not fitted"<<std::endl;
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
      if(p<0.02) { // 20 MeV
        if (DEBUG) std::cout<<" - skipping, momentum too small: "<<p*1E3<<" MeV"<<std::endl;
        continue;
      }
      if (DEBUG) std::cout<<std::endl;

      unsigned int trackId(trk->mcid().DominantID().mctrackID());

       // check pdg
       int pdg(211);
       //if(1) pdg=((PndMCTrack*)(_mcTrackArray->At(trackId)))->GetPdgCode();

       double pdgCharge(TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.);

       int winding(trk->winding()); // we look in z direction!


       if (pdgCharge < 0) {
         pdg *= -1;
         pdgCharge *= -1.;
       }
       if (winding > 0) {
         pdg *= -1;
         pdgCharge *= -1.;
       }
       if (Bz < 0) {
         pdg *= -1;
         pdgCharge *= -1.;
       }


       // check sorting
       bool invertTrack(true);

       TVector3 ps1=trk->getFirstHit()->cluster()->pos();
       TVector3 ps2=trk->getLastHit()->cluster()->pos();

       if(ps1.Z() < ps2.Z()-7) invertTrack = false;
       else if(ps1.Z() > ps2.Z()+7) invertTrack = true;
       else if (ps1.Perp()>ps2.Perp()+5) invertTrack = true;
       else if (ps1.Perp()<ps2.Perp()) invertTrack = false;
       else invertTrack = true;

       if (invertTrack){
         winding*=-1;
         pdg *= -1;
         pdgCharge *= -1;
       }


       TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(pdg);
       if(part == 0){
         if (DEBUG) std::cout << " - skipping, unknown PDG id: " << pdg;
         continue;
       }

       if (DEBUG) std::cout<<std::endl;




       // create GFTrackCands
       GFTrackCand* cand=new GFTrackCand();

       // fill hits into GFTrackCands and pndcands from small to big Radius and get seed values
       TVector3 pos1, direction;
       unsigned int planeId(0);

       if(!invertTrack){
         for(unsigned int ih=0; ih<nhits; ++ih){
           cand->addHit(21,trk->getHit(ih)->cluster()->index(), 0, planeId++);
         }
         trk->getPosDirOnHelix(0, pos1, direction);
       }
       else { // invert track
         for(unsigned int ih=nhits; ih>0; --ih){
           cand->addHit(21,trk->getHit(ih-1)->cluster()->index(), 0, planeId++);
         }
         trk->getPosDirOnHelix(trk->getNumHits()-1, pos1, direction);
         direction *= -1.;
       }// finished filling hits



       TVector3 poserr(1,1,1);
       //poserr*=trk->resolution();
       poserr *= 0.2;

       TVector3 mom(p * direction);
       //TVector3 momerr(fabs(mom.X()),fabs(mom.Y()),fabs(mom.Z()));
       //momerr *= trk->resolution();
       TVector3 momerr(0.2,0.2,0.2);

       double trackR = trk->r();

       if (DEBUG) {
         double trackDip = trk->dip();
         std::cout<<" center of track "; trk->center().Print();
         std::cout<<" Radius of track [cm]: " << trackR;
         std::cout<<"\n Dip of track [deg]:   " << trackDip/TMath::Pi()*180;
         std::cout<<"\n seed values: ";
         std::cout<<"\n  start position: "; pos1.Print();
         std::cout<<"  momentum [GeV]: "<<p;
         std::cout<<"\n  p_perp [GeV]:   " << trackR*0.0003*TMath::Abs(Bz);
         std::cout<<"\n  direction: "; direction.Print();
         std::cout<<"  winding: "<<winding;
         std::cout<<"\n  invertTrack: "<<invertTrack;
         std::cout<<"\n  pdg id: "<<pdg<<std::endl;
       }

       // set seed values to cands
       cand->setCurv(1./trackR);
       cand->setDip(trk->dip());
       cand->setComplTrackSeed(pos1, mom, pdg, poserr, momerr*(1./p));
       cand->setMcTrackId(trackId);




       //RK TRACKREP
       RKTrackRep* rep = new RKTrackRep(pos1, mom, poserr, momerr, pdg);

       // store GFTracks in output array
       GFTrack* track=new GFTrack(rep);
       track->setCandidate(*cand);
      delete cand;

      //SMOOTHING
      if(smooth) track->setSmoothing(true);

      //
      // run Kalman
      //

      // Load RecoHits
      try {
        track->addHitVector(_theRecoHitFactory->createMany(track->getCand()));
      }
      catch(GFException& e) {
        std::cout << e.what() << std::endl;
        e.info();
        throw e;
      }

      if(numIts>0 || useDAF){
        try{
          std::cerr << "Calling processTrack ... ";
          if(useDAF) fitterDAF->processTrack(track);
          else fitterK->processTrack(track);
          std::cerr << "finished\n";
        }
        catch (GFException& e){
          std::cout<<e.what()<<std::endl;
          delete track;
          continue;
        }
        catch (std::exception& e){
          std::cout << "exception caught: "<<e.what()<<std::endl;
          delete track;
          continue;
        }
        catch (...){
          std::cout<<"unknown exception"<<std::endl;
          delete track;
          continue;
        }
      }

      if(useDAF){
        std::cout<<"Weights: ";
        std::vector<std::vector<std::vector<double> > > weights = fitterDAF->getWeights();

        unsigned int cnter(0);
        for (unsigned int irep=0; irep<weights.size(); ++irep){
          for (unsigned int i=0; i<weights[irep].size(); ++i){
            for (unsigned int j=0; j<weights[irep][i].size(); ++j){
              //if (++cnter > nhits) goto breakFor;
              std::cout<<weights[irep][i][j]<<" ";
            }
          }
        }
        breakFor:
        std::cout<<"\n\n";


      }

      //
      // DRAW Fit
      //
      double charge = rep->getCharge();

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
          TMatrixT<double> state;
        if(smooth && numIts>0) {
          TMatrixT<double> cov;
          TMatrixT<double> auxInfo;
          GFTools::getSmoothedData(track, 0, j, state, cov, plane, auxInfo);
          rep->setData(state, plane, &cov, &auxInfo);
        } else {
          try{
            plane = hit->getDetPlane(rep);
            rep->extrapolate(plane, state);
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

      // test stepalong
      /*double stepsize(0);
      for (unsigned int j=0; j<50; ++j) {

        stepsize += 3.;

        std::cout<<"stepalong "<<stepsize<<std::endl;

        TVector3 dir;

        double step;

        try{
          step =  rep->stepalong(stepsize, track_pos, dir);
        }
        catch(GFException& e) {
            std::cerr << "Error: Exception caught!" << std::endl;
            std::cerr << e.what();
            break;
        }

        std::cout<<"stepsize-step"<<stepsize-step<<std::endl;

        track_lines->AddLine(old_track_pos(0), old_track_pos(1), old_track_pos(2), track_pos(0), track_pos(1), track_pos(2));

        old_track_pos = track_pos;
      }*/


      if(track_lines != NULL) gEve->AddElement(track_lines);
      delete track;
    } // end loop over riemann tracks

    delete fitterK;
    delete fitterDAF;

    std::cerr<<" done"<<std::endl;

  } // end Build GFTracks, run Kalman and draw track

  if(resetCam) gEve->Redraw3D(kTRUE);
  else gEve->Redraw3D(kFALSE);
}


void PndTpcClustVis::buildTracks(PndTpcRiemannTrackFinder* trackfinder,
                   std::vector<PndTpcCluster*>* clusterBuffer,
                   std::vector<PndTpcRiemannTrack*>* TrackletList,
                   int sorting,
                   unsigned int minHits,
                   double maxRMS,
                   bool skipCrossingAreas,
                   bool skipAndDelete){

  trackfinder->setSorting(sorting);
  trackfinder->setMinHits(minHits);
  trackfinder->SkipCrossingAreas(skipCrossingAreas);
  trackfinder->SetSkipAndDelete(skipAndDelete);

  std::vector<PndTpcRiemannTrack*> TrackletListCopy = *TrackletList;

  int nTracksIn(TrackletList->size());
  int nClIn(clusterBuffer->size());


  PndTpcRiemannTrack* LastTrackIn;
  if(nTracksIn > 0) LastTrackIn = TrackletList->back();

  trackfinder->buildTracks(*clusterBuffer, *TrackletList);

  unsigned int nGoodTrks(0), nErasedCl(0), nHits;
  PndTpcRiemannTrack* trk;

  for(unsigned int i=0; i<TrackletList->size(); ++i){
    trk = (*TrackletList)[i];

    nHits = trk->getNumHits();

    if (trk->distRMS() < maxRMS &&
        (nHits >= minHits || trk->isGood())){
      trk->setFinished(false);
      trk->setGood();
      // clear clusters from good tracklets
      for(unsigned int iCl=0; iCl < nHits; ++iCl){
        clusterBuffer->erase( remove(clusterBuffer->begin(), clusterBuffer->end(),
                                     trk->getHit(iCl)->cluster()),
                              clusterBuffer->end() );
      }
      ++nGoodTrks;

      //push back unique track to riemannlist
      if (std::find(TrackletListCopy.begin(), TrackletListCopy.end(), trk) == TrackletListCopy.end()){
        friemannlist.push_back(trk);
      }
    }
    else{ // delete bad tracklets
      if (trk->isGood()){ // track has been found before (->clusters were taken out) but does not pass quality criteria anymore -> fill clusters back into buffer
        for(unsigned int iCl=0; iCl < nHits; ++iCl){
          clusterBuffer->push_back(trk->getHit(iCl)->cluster());
        }
      }
      // delete hits and track
      trk->deleteHits();
      delete trk;
      TrackletList->erase(TrackletList->begin()+i);

      // also has to be removed from friemannlist
      friemannlist.erase( remove(friemannlist.begin(), friemannlist.end(), trk),
                          friemannlist.end() );

      --i;
    }
  }

  std::cerr << "   found good tracks: " <<  nGoodTrks-nTracksIn << ", reduced nCl by " << nClIn-clusterBuffer->size() << std::endl;
}


void PndTpcClustVis::clearBufferMap(){
  for(unsigned int isect=0;isect<fnsectors;++isect){
    /*if (doClustering){
      for(unsigned icl=0; icl<fbuffermap[isect]->size(); ++icl){
        delete (*(fbuffermap[isect]))[icl];
      }
    }*/
    fbuffermap[isect]->clear();
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
  //else cluster_shape->SetShape(new TGeoSphere(0., 0.25) );
  else cluster_shape->SetShape(new TGeoBBox(0.1,0.1,0.1) );

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
    guiTpcTransp = new TGNumberEntry(hf, TpcTransp, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 100);
    hf->AddFrame(guiTpcTransp);
    guiTpcTransp->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
    lbl = new TGLabel(hf, "TPC Transparency");
    hf->AddFrame(lbl);
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
    guiDrawPOCA =  new TGCheckButton(hf, "Draw POCAs to z-axis");
    if(drawPOCA) guiDrawPOCA->Toggle();
    hf->AddFrame(guiDrawPOCA);
    guiDrawPOCA->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDrawFitMarkers =  new TGCheckButton(hf, "Draw Fit-Markers");
    if(drawFitMarkers) guiDrawFitMarkers->Toggle();
    hf->AddFrame(guiDrawFitMarkers);
    guiDrawFitMarkers->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiPlotHistos =  new TGCheckButton(hf, "Plot Histograms");
    if(plotHistos) guiPlotHistos->Toggle();
    hf->AddFrame(guiPlotHistos);
    guiPlotHistos->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);


  // event mix options
  hf = new TGHorizontalFrame(frmMain); {
    lbl = new TGLabel(hf, "\n Event Mix Options");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiNsectorsToProcess = new TGNumberEntry(hf, NsectorsToProcess, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 80);
    hf->AddFrame(guiNsectorsToProcess);
    guiNsectorsToProcess->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
    lbl = new TGLabel(hf, "Number of sectors to process");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guifpurityLoCut = new TGNumberEntry(hf, fpurityLoCut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 1);
    hf->AddFrame(guifpurityLoCut);
    guifpurityLoCut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
    lbl = new TGLabel(hf, "Draw only tracks with purity more than x");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guifpurityCut = new TGNumberEntry(hf, fpurityCut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 1);
    hf->AddFrame(guifpurityCut);
    guifpurityCut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
    lbl = new TGLabel(hf, "Draw only tracks with purity less than x");
    hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    tb = new TGTextButton(hf, "Write DebugLogger files");
    hf->AddFrame(tb);
    tb->Connect("Clicked()", "PndTpcClustVis", fh, "writeDebugLogger()");
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

  hf = new TGHorizontalFrame(frmMain2); {
    // evt number entry
    lbl = new TGLabel(hf, "Do PR stage (11=all): ");
    hf->AddFrame(lbl);
    guiPRStage = new TGNumberEntry(hf, PRStage, 9,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 11);
    hf->AddFrame(guiPRStage);
    guiPRStage->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
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
  frmMain2->AddFrame(hf);
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
  frmMain2->AddFrame(hf);
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
    guiproxZstretch = new TGNumberEntry(hf, _proxZstretch, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEAPositive,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiproxZstretch);
    guiproxZstretch->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Proximity cut z stretch");
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
  hf = new TGHorizontalFrame(frmMain2); {
    guiMaxRMS = new TGNumberEntry(hf, _maxRMS, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiMaxRMS);
    guiMaxRMS->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "RMS cut");
    hf->AddFrame(lbl);
  }
  frmMain2->AddFrame(hf);

  // Trackmerger Parameters
  hf = new TGHorizontalFrame(frmMain2); {
    guiClearUnfitted =  new TGCheckButton(hf, "Clear small and unfitted tracklets");
    if(clearUnfitted) guiClearUnfitted->Toggle();
    hf->AddFrame(guiClearUnfitted);
    guiClearUnfitted->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiDoMerge =  new TGCheckButton(hf, "Do TrackMerging in sectors");
    if(doMerge) guiDoMerge->Toggle();
    hf->AddFrame(guiDoMerge);
    guiDoMerge->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiDoGlobMerge =  new TGCheckButton(hf, "Do global TrackMerging");
    if(doGlobMerge) guiDoGlobMerge->Toggle();
    hf->AddFrame(guiDoGlobMerge);
    guiDoGlobMerge->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiDoMergeCurlers =  new TGCheckButton(hf, "Do curler merging");
    if(doMergeCurlers) guiDoMergeCurlers->Toggle();
    hf->AddFrame(guiDoMergeCurlers);
    guiDoMergeCurlers->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
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
    guiDoClean =  new TGCheckButton(hf, "Find only target tracks");
    if(doClean) guiDoClean->Toggle();
    hf->AddFrame(guiDoClean);
    guiDoClean->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  //frmMain2->AddFrame(hf);

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
  hf = new TGHorizontalFrame(frmMain2); {
    guiInitDip = new TGNumberEntry(hf, initDip, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEAAnyNumber,
                          TGNumberFormat::kNELLimitMinMax,
                          -1000, 1000);
    hf->AddFrame(guiInitDip);
    guiInitDip->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Init Dip");
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
    guiUseDAF =  new TGCheckButton(hf, "Use DAF");
    if(useDAF) guiUseDAF->Toggle();
    hf->AddFrame(guiUseDAF);
    guiUseDAF->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiInvertCharge =  new TGCheckButton(hf, "Invert Charge");
    if(invertCharge) guiInvertCharge->Toggle();
    hf->AddFrame(guiInvertCharge);
    guiInvertCharge->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
  }
  frmMain2->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain2); {
    guiUseGeane =  new TGCheckButton(hf, "Use Geane");
    if(useGeane) guiUseGeane->Toggle();
    hf->AddFrame(guiUseGeane);
    guiUseGeane->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetFittingParams()");
  }
  frmMain2->AddFrame(hf);
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
  instantRedraw=(guiInstantRedraw->IsOn());

  drawTpc=(guiDrawTpc->IsOn());
  TpcTransp=guiTpcTransp->GetNumberEntry()->GetIntNumber();
  drawRawDigis=(guiDrawRawDigis->IsOn());
  drawDigis=(guiDrawDigis->IsOn());
  drawClusters=(guiDrawClusters->IsOn());
  drawClusterErrors=(guiDrawClustersErrors->IsOn());
  doPR=(guiDoPR->IsOn());

  drawRiemannTracks=(guiDrawRiemannTracks->IsOn());
  NsectorsToProcess = guiNsectorsToProcess->GetNumberEntry()->GetNumber();
  fpurityLoCut = guifpurityLoCut->GetNumberEntry()->GetNumber();
  fpurityCut = guifpurityCut->GetNumberEntry()->GetNumber();
  drawPOCA=(guiDrawPOCA->IsOn());
  drawFitMarkers=(guiDrawFitMarkers->IsOn());

  plotHistos=(guiPlotHistos->IsOn());

  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
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
  PRStage = guiPRStage->GetNumberEntry()->GetIntNumber();
  _sorting = guisorting->GetNumberEntry()->GetIntNumber();
  _interactionZ = guiinteractionZ->GetNumberEntry()->GetNumber();
  _sortingMode=(guisortingMode->IsOn());
  _minpoints = guiminpoints->GetNumberEntry()->GetIntNumber();
  _planecut = guiplanecut->GetNumberEntry()->GetNumber();
  _riproxcut = guiriproxcut->GetNumberEntry()->GetNumber();
  _szcut = guiszcut->GetNumberEntry()->GetNumber();
  _proxcut = guiproxcut->GetNumberEntry()->GetNumber();
  _proxZstretch = guiproxZstretch->GetNumberEntry()->GetNumber();
  _helixcut = guihelixcut->GetNumberEntry()->GetNumber();
  _maxRMS = guiMaxRMS->GetNumberEntry()->GetNumber();

  _TTproxcut = guiTTproxcut->GetNumberEntry()->GetNumber();
  _TTplanecut = guiTTplanecut->GetNumberEntry()->GetNumber();
  //_TTszcut = guiTTszcut->GetNumberEntry()->GetNumber();
  _TTdipcut = guiTTdipcut->GetNumberEntry()->GetNumber();
  _TThelixcut = guiTThelixcut->GetNumberEntry()->GetNumber();
  fRiemannScale=guiTTscale->GetNumberEntry()->GetNumber();
  initDip=guiInitDip->GetNumberEntry()->GetNumber();
  clearUnfitted=(guiClearUnfitted->IsOn());
  doMerge=(guiDoMerge->IsOn());
  doGlobMerge=(guiDoGlobMerge->IsOn());
  doMergeCurlers=(guiDoMergeCurlers->IsOn());
  doClean=(guiDoClean->IsOn());

  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}


void PndTpcClustVis::guiSetFittingParams(){
  doFit=(guiDoFit->IsOn());
  useDAF=(guiUseDAF->IsOn());
  invertCharge=(guiInvertCharge->IsOn());
  useGeane=(guiUseGeane->IsOn());
  numIts=guiNumIts->GetNumberEntry()->GetNumber();
  smooth=(guiSmooth->IsOn());

  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}


void PndTpcClustVis::writeDebugLogger(){
  DebugLogger::Instance()->WriteFiles();
}



ClassImp(PndTpcClustVis)
