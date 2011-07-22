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


#ifndef _PNDTPCCLUSTVIS_H
#define _PNDTPCCLUSTVIS_H

//#include "FairTask.h"

#include "PndTpcEvent.h"

#include<iostream>

#include <assert.h>
#include <cmath>
#include <exception>
#include <iostream>

#include <GFAbsRecoHit.h>
#include <GFAbsTrackRep.h>
#include <GFConstField.h>
#include <GFDetPlane.h>
#include <GFException.h>
#include <GFFieldManager.h>
#include <GFTrack.h>
#include <GFRecoHitFactory.h>

#include "FairGeanePro.h"

#include <TApplication.h>
#include <TEveBrowser.h>
//#include <TEveBox.h>
#include <TEveManager.h>
#include <TEveEventManager.h>
#include <TEveGeoNode.h>
#include <TEveGeoShape.h>
#include <TEveStraightLineSet.h>
#include <TDecompSVD.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGeoEltu.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoNode.h>
#include <TGeoSphere.h>
#include <TGeoTube.h>
#include <TGeoBBox.h>
#include <TTree.h>
#include <TMath.h>
#include <TMatrixT.h>
#include <TMatrixDEigen.h>
#include <TROOT.h>
#include <TVector2.h>
#include <TVector3.h>
#include <TVectorD.h>
#include <TSystem.h>
#include <string>
#include <vector>

#include "PndTpcGas.h"
#include "PndTpcDigi.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcDigiAmplitude.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcPadPlane.h"
#include "PndTpcAbsPadShape.h"
#include "PndTpcPad.h"
#include "PndTpcGem.h"

#include "PndTpcRiemannTrack.h"
#include "PndTpcRiemannHit.h"
#include "PndTpcRiemannTrackFinder.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcHelixHTCorrelator.h"
#include "PndTpcProximityTTCorrelator.h"
#include "PndTpcRiemannTTCorrelator.h"
#include "PndTpcDipTTCorrelator.h"

#include "PndTpcDigi.h"
#include "PndTpcCluster.h"
#include "McId.h"
#include "McIdCollection.h"



class PndTpcClustVis : public TObject {
private:
  PndTpcClustVis();

public:
  ~PndTpcClustVis();
  static PndTpcClustVis* getInstance();

  /** @brief Drop all events.*/
  void reset();

  void setTree(TTree* treeIn);

  void initDigimapper(double drifField,
		      double gain, double spread,
		      double zGem, 
		      double samplingFreq,
		      double wallclock,
		      std::string gasfile,
		      std::string padplanefile,
		      std::string padshapefile);

  void setFieldZ(double B){Bz=B;}

  void setRiemannscale(double scale){fRiemannScale=scale;}

  /** @brief Go to the next event or step a certain number of events ahead.*/
  void next(unsigned int stp = 1);

  /** @brief Go to the previous event or step a certain number of events back.*/
  void prev(unsigned int stp = 1);

  /** @brief Go to event with index id.*/
  void gotoEvent(int id);

  /** @brief Get the total number of events stored.*/
  int getNEvents();

  /** @brief Open the event display.*/
  void open();

  void guiGoto();
  void guiSetDrawParams();
  void guiSetClusterfinderParams();
  void guiSetTrackingParams();	
  void guiSetFittingParams();

  void writeDebugLogger();

private:
  static PndTpcClustVis* eventDisplay;
  int fEventId;
  std::string fOption;

  std::vector<Color_t> colors;

  TTree* tree;
  TClonesArray* digisBranch;
  TClonesArray* clustersBranch;
  TClonesArray* preFitBranch;
  TClonesArray* postFitBranch;

  PndTpcPadPlane* fpadplane;
  PndTpcPadShapePool* fpadShapes;
  const PndTpcGem* fgem;
  const PndTpcGas* fgas;
  double fzGem;
  double fgain;

  std::map<unsigned int, std::vector<PndTpcCluster*>*> fbuffermap;
  std::vector<PndTpcCluster*>* fcluster_buffer;
  TClonesArray* clusterArray;
  std::vector<PndTpcRiemannTrack*> friemannlist;
  unsigned int fnsectors;

  GFRecoHitFactory* _theRecoHitFactory;
  FairGeanePro* GeanePro;

  TGCheckButton* guiInstantRedraw;
  bool instantRedraw;
  TGCheckButton* guiDrawTpc;
  bool drawTpc;
  TGNumberEntry* guiTpcTransp;
  int TpcTransp;
  TGCheckButton* guiDrawRawDigis;
  bool drawRawDigis;
  TGCheckButton* guiDrawDigis;
  bool drawDigis;
  TGCheckButton* guiDrawClusters;
  bool drawClusters;
  TGCheckButton* guiDrawClustersErrors;
  bool drawClusterErrors;

  TGNumberEntry* guiNsectorsToProcess;
  unsigned int NsectorsToProcess;
  TGCheckButton* guiDrawRiemannTracks;
  bool drawRiemannTracks;
  TGNumberEntry* guifpurityLoCut;
  double fpurityLoCut;
  TGNumberEntry* guifpurityCut;
  double fpurityCut;
  TGCheckButton* guiDrawPOCA;
  bool drawPOCA;
  TGCheckButton* guiDrawFitMarkers;
  bool drawFitMarkers;

  TGCheckButton* guiDoClustering;
  bool doClustering;
  TGNumberEntry* guiEvent;
  TGNumberEntry* guiMode;
  int ClMode;
  TGNumberEntry* giuTimeslice;
  int ClTimeslice;
  TGNumberEntry* giuTimecut;
  int ClTimecut;
  TGNumberEntry* guiSingleDigiClAmpCut;
  int ClSingleDigiClAmpCut;
  TGNumberEntry* guiClAmpCut;
  double ClClAmpCut;
  TGNumberEntry* guiElPerADC;
  double ClElPerADC;
  TGNumberEntry* guiErrorNorm;
  double ClErrorNorm;
  TGCheckButton* guiSimpleCl;
  bool ClSimpleCl;
  TGNumberEntry* giuSimpleTimeslice;
  int ClSimpleTimeslice;
  TGNumberEntry* giuSimpleMaxClusterSlice;
  int ClSimpleMaxClusterSlice;

  bool ClHasChanged;

  TGCheckButton* guiDoPR;
  bool doPR;
  TGNumberEntry* guiPRNHits;
  int PRNHits;
  TGNumberEntry* guiPRStage;
  unsigned int PRStage;
  TGCheckButton* guiClearUnfitted;
  bool clearUnfitted;
  TGCheckButton* guiDoMerge;
  bool doMerge;
  TGCheckButton* guiDoGlobMerge;
  bool doGlobMerge;

  TGCheckButton* guiDoClean;
  bool doClean;

  TGNumberEntry* guisorting;
  int _sorting;
  TGNumberEntry* guiinteractionZ;
  double _interactionZ;
  TGCheckButton* guisortingMode;
  bool _sortingMode;
  TGNumberEntry* guiminpoints;
  int _minpoints;
  TGNumberEntry* guiplanecut;
  double _planecut;
  TGNumberEntry* guiriproxcut;
  double _riproxcut;
  TGNumberEntry* guiszcut;
  double _szcut;
  TGNumberEntry* guiproxcut;
  double _proxcut;
  TGNumberEntry* guiproxZstretch;
  double _proxZstretch;
  TGNumberEntry* guihelixcut;
  double _helixcut;
  TGNumberEntry* guiTTproxcut;
  double _TTproxcut;
  TGNumberEntry* guiTTplanecut;
  double _TTplanecut;
  TGNumberEntry* guiTTszcut;
  double _TTszcut;
  TGNumberEntry* guiTTdipcut;
  double _TTdipcut;
  TGNumberEntry* guiTThelixcut;
  double _TThelixcut;
  TGNumberEntry* guiTTscale;
  double fRiemannScale;
  TGNumberEntry* guiInitDip;
  double initDip;

  bool PRHasChanged;

  TGCheckButton* guiDoFit;
  bool doFit;
  TGCheckButton* guiInvertCharge;
  bool invertCharge;
  TGCheckButton* guiUseGeane;
  bool useGeane;
  TGNumberEntry* guiNumIts;
  int numIts;
  TGCheckButton* guiSmooth;
  bool smooth;

  double Bz;


  /** @brief Build the GUI.*/
  void makeGui();

  /** @brief Draw an event.*/
  void drawEvent(unsigned int id, bool resetCam=true);

  void clearBufferMap();
  void drawDigi(const PndTpcDigi* digi, bool raw=true, Color_t color=kGray);
  void drawCluster(const PndTpcCluster* cluster, Color_t color=kGray);

public:
  ClassDef(PndTpcClustVis,1)
};

#endif
