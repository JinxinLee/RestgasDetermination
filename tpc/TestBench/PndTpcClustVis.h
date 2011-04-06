//***************************************************
// Event Display implementation: 3D View
// For the GEM-TPC decoding/monitoring software
//
// author: Johannes Rauch
//         E18, Technische Universitaet Muenchen
//
//**************************************************


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
#include <TApplication.h>
#include <TEveBrowser.h>
#include <TEveBox.h>
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
#include "PndTpcRiemannHTCorrelator.h"
#include "PndTpcRiProxHTCorrelator.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcProximityTTCorrelator.h"
#include "PndTpcRiemannTTCorrelator.h"
#include "PndTpcSzHTCorrelator.h"
#include "PndTpcSzTTCorrelator.h"

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

  void initDigimapper(double drifField);

  /** @brief Go to the next event or step a certain number of events ahead.*/
  void next(unsigned int stp = 1);

  /** @brief Go to the previous event or step a certain number of events back.*/
  void prev(unsigned int stp = 1);

  /** @brief Go to event with index id.*/
  void gotoEvent(int id);

  /** @brief Get the total number of events stored.*/
  int getNEvents();

  /** @brief Set the display options.
   *
   * The option string lets you steer the way the events are displayed. The following
   * options are available:\n
   * \n
   * 'A': Autoscale errors. The representation of hits are scaled with the error found
   *      their covariance matrix. This can lead to hits not being displayed beause the
   *      errors are too small. Autoscaling ensures that the errors are scaled up
   *      sufficiently to ensure all hits are displayed. However, this can lead to unwanted
   *      results if there are only a few hits with very small errors, as all hits are scaled
   *      by the same factor to ensure consistency.\n\n
   * 'D': Draw detectors. This causes a simple representation for all detectors to be drawn. For
   *      planar detectors, this is a plane with the same position and orientation of the real
   *      detector plane, but with different size. For wires, this is a tube whose diameter
   *      is equal to the value measured by the wire. Spacepoint hits are not affected by this
   *      option.\n\n
   * 'H': Draw hits. This causes the hits to be visualized. Normally, the size of the hit
   *      representation is connected to the covariance matrix of the hit, scaled by the value
   *      set in setErrScale which is normally 1. See also option 'A' and 'S'. Normally used in
   *      connection with 'D'.\n\n
   * 'R': Draw Hits added via the function addHits. Then option 'H' shouldn't be used, otherwise
   *      the hits belonging to a track will be plotted twice. This feature is in a beta stage.\n\n
   * 'G': Draw geometry. Draw also the geometry in the gGeoManager. This feature is experimental
   *      and may lead to strang things being drawn.\n\n
   * 'M': Draw track markers. Draw the intersection points between the track and the virtual
   *      (and/or real) detector planes. Can only be used in connection with 'T'.\n\n
   * 'P': Draw detector planes. Draws the virtual (and/or real) detector planes.\n\n
   * 'S': Scale manually. This leads to the spacepoint hits (and only them up to now!) being drawn
   *      as spheres with radius 0.5 scaled with the error scale factor. Can be used if the scaling
   *      with errors leads to problems.\n\n
   * 'T': Draw Track. Draw the track as straight lines between the virtual (and/or real) detector
   *      planes.\n\n
   * 'X': Draw silent. Does not run the TApplication.
   *
   */
  void setOptions(std::string opts = "CD");

  /** @brief Set the scaling factor for the visualization of the errors.*/
  void setErrScale(double errScale = 1.);

  /** @brief Get the error scaling factor.*/
  double getErrScale();

  /** @brief Open the event display.*/
  void open();



  void guiGoto();
  void guiSetClusterfinderParams();
  void guiSetDrawParams();
  void guiSetTrackingParams();	

private:
  static PndTpcClustVis* eventDisplay;
  int fEventId;
  double fErrorScale;
  std::string fOption;

  std::vector<Color_t> colors;

  TTree* tree;
  TClonesArray* digisBranch;
  TClonesArray* clustersBranch;
  TClonesArray* preFitBranch;

  PndTpcPadPlane* fpadplane;
  PndTpcPadShapePool* fpadShapes;
  const PndTpcGem* fgem;
  const PndTpcGas* fgas;
  double fzGem;


  TGNumberEntry* guiEvent;
  TGCheckButton* guiDrawTpc;
  TGNumberEntry* guiMode;
  TGNumberEntry* giuTimeslice;
  TGNumberEntry* giuTimecut;
  TGNumberEntry* guiSingeDigiClAmpCut;
  TGCheckButton* guiSimpleCl;
  TGNumberEntry* giuSimpleTimeslice;

  TGCheckButton* guiDrawDigis;
  TGCheckButton* guiDrawClusters;
  TGCheckButton* guiDrawClustersErrors;
  TGCheckButton* guiDoPR;
  TGCheckButton* guiDoMerge;

  TGNumberEntry* guisorting;
  TGNumberEntry* guiinteractionZ;
  TGCheckButton* guisortingMode;
  TGNumberEntry* guiminpoints;
  TGNumberEntry* guiplanecut;
  TGNumberEntry* guiriproxcut;
  TGNumberEntry* guiszcut;
  TGNumberEntry* guiproxcut;
  TGNumberEntry* guiTTproxcut;
  TGNumberEntry* guiTTplanecut;
  TGNumberEntry* guiTTszcut;

  bool drawTpc;
  int ClMode;
  int ClTimeslice;
  int ClTimecut;
  int ClSingeDigiClAmpCut;
  bool ClSimpleCl;
  int ClSimpleTimeslice;

  bool drawDigis;
  bool drawClusters;
  bool drawClusterErrors;
  bool doPR;
  bool doMerge;

  int _sorting;
  double _interactionZ;
  bool _sortingMode;
  int _minpoints;
  double _planecut;
  double _riproxcut;
  double _szcut;
  double _proxcut;
  double _TTproxcut;
  double _TTplanecut;
  double _TTszcut;


  /** @brief Build the buttons for event navigation.*/
  void makeGui();

  /** @brief Draw an event.*/
  void drawEvent(unsigned int id, bool resetCam=true);

  /** @brief Create a box around o, orientet along u and v with widths ud, vd and depeth and
   *  return a pointer to the box object.
   */
  //TEveBox* boxCreator(TVector3 o, TVector3 u, TVector3 v, float ud, float vd, float depth);

  /** @brief Coordinate trinsformation to plane coordinates.*/
  TVector2 getCoordsInPlane(TVector3 p, TVector3 o, TVector3 u, TVector3 v);

  /** @brief NOT IPMLEMENTED!*/
  TVector2 getCoordsInPlane(TVector3 p, GFDetPlane plane);

public:
  ClassDef(PndTpcClustVis,1)
};

#endif
