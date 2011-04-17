//***************************************************
// Event Display implementation: 3D View
// For the GEM-TPC decoding/monitoring software
//
// author: Johannes Rauch
//         E18, Technische Universitaet Muenchen
//
//**************************************************

#include "PndTpcClustVis.h"

#include "PndTpcAbsClusterFinder.h"
#include "PndTpcClusterFinder.h"
#include "PndTpcClusterFinderSimple.h"


PndTpcClustVis* PndTpcClustVis::eventDisplay = NULL;

PndTpcClustVis::PndTpcClustVis():
  digisBranch(0),clustersBranch(0), guiEvent(0),
  doClustering(false), ClMode(2), ClTimeslice(3),ClTimecut(2), ClSingeDigiClAmpCut(20), ClSimpleCl(true), ClSimpleTimeslice(7),
  instantRedraw(false), drawTpc(false), drawDigis(false), drawClusters(false), drawClusterErrors(false),
  doPR(true), doMerge(true), _sorting(3), _interactionZ(0), _sortingMode(true), PRNHits(1000000),
  _minpoints(5), _planecut(0.05), _riproxcut(0.05), _szcut(0.25), _proxcut(2),
  _TTproxcut(2), _TTplanecut(2E-3), _TTszcut(2), fRiemannScale(24.6)
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

  fEventId = 0;
  setOptions();
  setErrScale();
}

void PndTpcClustVis::initDigimapper(double drifField, 
				    double gain, double spread, 
				    double zGem,
				    double samplingFreq,
				    double wallclock,
				    std::string gasfile,
				    std::string padplanefile,
				    std::string padshapefile){
  // init Digimapper // TODO: get from file!!
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

  fgas = new PndTpcGas(gasfile.c_str(),
                       drifField);  // Drift Field
  fgem = new PndTpcGem(gain,  // Gain
                       spread); // Spread
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
}


void PndTpcClustVis::setOptions(std::string opts) { fOption = opts; }


void PndTpcClustVis::setErrScale(double errScale) { fErrorScale = errScale; }


double PndTpcClustVis::getErrScale() { return fErrorScale; }


PndTpcClustVis* PndTpcClustVis::getInstance() {
  if(eventDisplay == NULL) {
    eventDisplay = new PndTpcClustVis();
  }
  return eventDisplay;
}


PndTpcClustVis::~PndTpcClustVis() { reset(); }


void PndTpcClustVis::reset() {

}


void PndTpcClustVis::setTree(TTree* treeIn) {
  tree = treeIn;
  tree->SetBranchAddress("PndTpcRawDigi", &digisBranch);
  if(digisBranch==NULL) tree->SetBranchAddress("PndTpcDigi", &digisBranch);
  tree->SetBranchAddress("PndTpcCluster", &clustersBranch);
  //tree->SetBranchAddress("TrackPreFit", &preFitBranch);
}


void PndTpcClustVis::next(unsigned int stp) {
  fEventId += stp;
  gotoEvent(fEventId);
}


void PndTpcClustVis::prev(unsigned int stp) {
  fEventId -= stp;
  gotoEvent(fEventId);
}


int PndTpcClustVis::getNEvents() { return tree->GetEntries(); }


void PndTpcClustVis::gotoEvent(int id) {
  if(id < 0) id = 0;
  else if(id >= tree->GetEntries()) id = tree->GetEntries() - 1;

  bool resetCam = kTRUE;
  if(id==fEventId) resetCam=kFALSE;

  std::cout<<"reset cam "<<resetCam<<std::endl;

  fEventId = id;

  std::cout << "At event " << id << std::endl;
  if(gEve->GetCurrentEvent()!=NULL)
    gEve->GetCurrentEvent()->DestroyElements();
  double old_error_scale = fErrorScale;
  drawEvent(fEventId, resetCam);
  //if(old_error_scale != fErrorScale) drawEvent(fEventId, resetCam); // if autoscaling changed the error, draw again.
  fErrorScale = old_error_scale;
}


void PndTpcClustVis::open() {
  bool drawSilent = false;
  bool drawGeometry = false;

// parse the global options
  for(size_t i = 0; i < fOption.length(); i++) {
    if(fOption.at(i) == 'X') drawSilent = true;
    if(fOption.at(i) == 'G') drawGeometry = true;
  }

// draw the geometry, does not really work yet. If it's fixed, the docu in the header file should be changed.
  if(drawGeometry) {
    TGeoNode* top_node = gGeoManager->GetTopNode();
    assert(top_node != NULL);
    TEveGeoTopNode* eve_top_node = new TEveGeoTopNode(gGeoManager, top_node);
    gEve->AddGlobalElement(eve_top_node);
  }

  if(getNEvents() > 0) {
    double old_error_scale = fErrorScale;
    drawEvent(0);
    //if(old_error_scale != fErrorScale) gotoEvent(0); // if autoscaling changed the error, draw again.
    fErrorScale = old_error_scale;
  }

  if(!drawSilent) {
    makeGui();
    gApplication->Run(kTRUE);
  }
}


void PndTpcClustVis::drawEvent(unsigned int id, bool resetCam) {
  // parse the option string ------------------------------------------------------------------------
  bool drawAutoScale = false;
  bool drawDetectors = false;
  bool drawHits = false;
  bool drawScaleMan = false;
  bool drawTrackMarkers = false;
  bool drawPlanes = false;
  bool drawTrack = false;

  if(fOption != "") {
    for(size_t i = 0; i < fOption.length(); i++) {
      if(fOption.at(i) == 'A') drawAutoScale = true;
      if(fOption.at(i) == 'D') drawDetectors = true;
      if(fOption.at(i) == 'H') drawHits = true;
      if(fOption.at(i) == 'M') drawTrackMarkers = true;
      if(fOption.at(i) == 'P') drawPlanes = true;
      if(fOption.at(i) == 'S') drawScaleMan = true;
      if(fOption.at(i) == 'T') drawTrack = true;
    }
  }
  // finished parsing the option string -------------------------------------------------------------

       
  // Draw tpc -------------------------------------------------------
  if(drawTpc){
    double tpcLength = 72.5;
    tpcLength*=0.5;
    TGeoMatrix* tpc_trans = new TGeoGenTrans(0,0,tpcLength,
                                             1,1,1, 0);

    TEveGeoShape* tpc_shape = new TEveGeoShape("tpc_shape");

    tpc_shape->SetShape(new TGeoTube(5.,15., tpcLength));
    tpc_shape->SetTransMatrix(*tpc_trans);
    // finished rotating and translating ------------------------------------------

    tpc_shape->SetMainColor(kBlue);
    tpc_shape->SetMainTransparency(80);
    gEve->AddElement(tpc_shape);
  }


  tree->GetEntry(id);


  // build a clusterbuffer for each sector;
  std::map<unsigned int, std::vector<PndTpcCluster*>*> buffermap;
  unsigned int nsectors=PndTpcDigiMapper::getInstance()->getPadPlane()->GetNSectors();
  std::cerr << "Found " << nsectors << " sectors in padplane" << std::endl;
  for(unsigned int  isect=0;isect<nsectors;++isect){
    buffermap[isect]=new std::vector<PndTpcCluster*>;
  }
  std::vector<PndTpcCluster*>* fcluster_buffer=buffermap[0];
    
  if(doClustering){ // run ClusterFinder and fill fcluster_buffer
    PndTpcAbsClusterFinder* ffinder = 0;

    // TODO: get from file!!
    if(!ClSimpleCl){
      ffinder=new PndTpcClusterFinder(PndTpcDigiMapper::getInstance()->getPadPlane(),
              fcluster_buffer,
              ClTimeslice, ClMode, -1,true,1.,ClTimecut,4000./600.,300.);
      ffinder->checkConsistency();
    }
    else{
      ffinder=new PndTpcClusterFinderSimple(PndTpcDigiMapper::getInstance()->getPadPlane(),
              fcluster_buffer,
              ClSimpleTimeslice,4000./600.,300.);
      ((PndTpcClusterFinderSimple*)(ffinder))->setNoXclust(false);
    }

    ffinder->setTrivialClustering(false);
    ffinder->saveRaw();
    ffinder->reset();
    
    if(digisBranch==NULL) std::cerr<<"PndTpcClustVis::drawEvent - Error: No Digi Array Found!"<<std::endl;
    //for sorting
    std::vector<PndTpcDigi*> digis;

    for(int k=0; k<digisBranch->GetEntries(); ++k){
      PndTpcDigi* digi=(PndTpcDigi*)digisBranch->At(k);
      digis.push_back(digi);
    }
    std::cout<<"number of digis: "<<digis.size()<<std::endl;
    try{
      ffinder->process(digis);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
    } catch (...) {
      std::cout << "unknown exception..." << std::endl;
    }

    //delete ffinder;

     int i=0;
     
     while(i<fcluster_buffer->size()){
       if( ((*fcluster_buffer)[i])->amp()<1 ||
           (((*fcluster_buffer)[i])->size()==1 && ((*fcluster_buffer)[i])->amp()<ClSingeDigiClAmpCut)){
         delete (*fcluster_buffer)[i];
         (*fcluster_buffer).erase( (*fcluster_buffer).begin()+i );
       }
       else ++i;
     }
     digis.clear();
  }
  else{ // fill clusters in cluster_buffer (and use buffermap)
    if(clustersBranch==NULL) std::cerr<<"PndTpcClustVis::drawEvent - Error: No Cluster Array Found!"<<std::endl;
    unsigned int ncl=clustersBranch->GetEntries();
    for(unsigned int isect=0;isect<nsectors;++isect)
      buffermap[isect]->reserve(ncl/nsectors+10);
    for(unsigned int i=0; i<ncl; ++i){
      PndTpcCluster *cluster = (PndTpcCluster*)clustersBranch->At(i);
      buffermap[cluster->sector()]->push_back(cluster);
    }
    std::cout << "number of clusters: " << ncl << std::endl;
  } //  end else (read clusters from file)
 
  // loop over sectors
  for(unsigned int isect=0;isect<nsectors;++isect){
    fcluster_buffer=buffermap[isect];
     unsigned int ncl=fcluster_buffer->size();
     std::cerr << "number of clusters: " << ncl << " in sector " << isect << std::endl;
  // loop over clusters
     unsigned int tenpercent=(unsigned int)(ncl*0.1);
  for(unsigned int i=0; i<ncl; i+=2){
    //************ Progress messages ************************
    // if(i%10000==0){std::cout<<".";std::cout.flush();}
    // if(i%tenpercent==0){
    //   std::cout<<"["
    //            <<ceil((double)i*100/(double)ncl)<<"%"
    //            <<"]";
    //   std::cout.flush();
    // }
    // ******************************************************

    int colour = i%colors.size();

    PndTpcCluster *cluster = (*fcluster_buffer)[i];

    // get Digis from Cluster & draw
    if(drawDigis && !doPR){
      int ndigis = cluster->nDigi();
      // loop over digis
      for(unsigned int j=0; j<ndigis; ++j){
        const PndTpcDigi* digi = cluster->getDigi(j);

        // map digi
        TVector3 pos;
        if(digi->padId()<0) continue;
        PndTpcDigiMapper::getInstance()->map(digi,pos);

        // rotate and translate -------------------------------------------------------
        TGeoMatrix* det_trans = new TGeoGenTrans(pos.X(), pos.Y(), pos.Z(),
                                                 1,1,1, 0);


        TEveGeoShape* digi_shape = new TEveGeoShape("digi_shape");

        // calculate and norm amp
        double amp = digi->amp(); // should be ~ 6 .. 2000
        if(amp<1) continue; 
        amp = TMath::Log(amp); // ~ 0.8 .. 3.3
        amp *= 0.02;
	
        digi_shape->SetShape(new TGeoTube(0.,amp, 0.05 ) );
        digi_shape->SetTransMatrix(*det_trans);
        // finished rotating and translating ------------------------------------------

        digi_shape->SetMainColor(colors[colour]);
        digi_shape->SetMainTransparency(50);
        gEve->AddElement(digi_shape);
      } // end loop over digis
    } // end draw digis

    if(drawClusters && !doPR){
      TVector3 pos;
      TVector3 err;

      pos = cluster->pos();
      // rotate and translate -------------------------------------------------------
      TGeoMatrix* det_trans = new TGeoGenTrans(pos.X(), pos.Y(), pos.Z(),
                                               1., 1., 1., 0);

      TEveGeoShape* cluster_shape = new TEveGeoShape("cluster_shape");

      if(drawClusterErrors){
        err = cluster->sig();
        cluster_shape->SetShape(new TGeoBBox(err.X(), err.Y(), err.Z()) );
      }
      else
        cluster_shape->SetShape(new TGeoSphere(0., 0.25) );

      cluster_shape->SetTransMatrix(*det_trans);
      // finished rotating and translating ------------------------------------------

      cluster_shape->SetMainColor(colors[colour]);
      cluster_shape->SetMainTransparency(40);
      gEve->AddElement(cluster_shape);
    }

  }// end loop over clusters
  }// end loop over sectors;

  std::cout << std::endl;
  // Pattern Reco
  if(doPR){
    std::cerr << "Starting Pattern Reco..." << std::endl;
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
    _trackfinder->addCorrelator(new PndTpcRiProxHTCorrelator(_riproxcut));
    _trackfinder->addCorrelator(new PndTpcSzHTCorrelator(_szcut));
    _trackfinder->addCorrelator(new PndTpcRiemannHTCorrelator(_planecut));

    // Track-Track Correlators
    _trackfinder->addTTCorrelator(new PndTpcProximityTTCorrelator(_TTproxcut));
    _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));
    _trackfinder->addTTCorrelator(new PndTpcSzTTCorrelator(_TTszcut));

    _trackfinder->setCoolingCuts(_planecut, _szcut);
    
    /// PLAN: 
    /// 1) build several cluster buffer, sectorwise
    /// 2) run trackfinder over each clusterbuffer independently
    /// 3) put all found tracklets into one list
    /// 4) then do start merging

    std::vector<PndTpcRiemannTrack*> riemannTemp;
    std::vector<PndTpcRiemannTrack*> riemannlist;
    
    // loop over sectors
    for(unsigned int isect=0;isect<nsectors;++isect){
      std::cerr << "... building tracks in sector " << isect << std::endl;
      fcluster_buffer=buffermap[isect];
      _trackfinder->buildTracks(*fcluster_buffer,riemannTemp);
      if(doMerge) _trackfinder->mergeTracks(riemannTemp);
      // copy tracklets of this sector to global list
      unsigned int ntrklts=riemannTemp.size();
      riemannlist.reserve(riemannlist.size()+ntrklts);
      for(unsigned int it=0;it<ntrklts;++it){
	      riemannlist.push_back(riemannTemp[it]);
      }
      riemannTemp.clear();
    } // end loop over sectors
    if(doMerge){
      std::cerr << "... merging tracks ..." << std::endl;
      _trackfinder->mergeTracks(riemannlist);
    }
    // draw	
    for(unsigned int ir=0;ir<riemannlist.size();ir+=1){ // loop over trackcands
      PndTpcRiemannTrack* trkcand = riemannlist[ir];
      unsigned int nhits=trkcand->getNumHits();

      int colour = ir%colors.size();

      TVector3 old_track_pos;
      TEveStraightLineSet* track_lines = NULL;

      for(unsigned int ih=0;ih<nhits;++ih){ // loop over clusters
        PndTpcCluster* cluster = trkcand->getHit(ih)->cluster();
        TVector3 pos;
        TVector3 err;

        pos = cluster->pos();


        if(drawClusters){
          // rotate and translate -------------------------------------------------------
          TGeoMatrix* det_trans = new TGeoGenTrans(pos.X(), pos.Y(), pos.Z(),
                                                 1., 1., 1., 0);

          TEveGeoShape* cluster_shape = new TEveGeoShape("cluster_shape");

          if(drawClusterErrors){
            err = cluster->sig();
            cluster_shape->SetShape(new TGeoBBox(err.X(), err.Y(), err.Z()) );
          }
          else
            cluster_shape->SetShape(new TGeoSphere(0., 0.25) );

          cluster_shape->SetTransMatrix(*det_trans);
          // finished rotating and translating ------------------------------------------

          cluster_shape->SetMainColor(colors[colour]);
          cluster_shape->SetMainTransparency(40);
          gEve->AddElement(cluster_shape);
        }


        // connect clusters
        if(true) {
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
      delete trkcand;trkcand=NULL;
    } // end loop over trackcands
    std::cerr << "Pattern Reco finished" << std::endl;
    // clean up riemannlist!
    riemannlist.clear();
  }

  // clean up buffermap
  for(unsigned int isect=0;isect<nsectors;++isect){
    buffermap[isect]->clear();
    delete buffermap[isect];
  }
  buffermap.clear();
  //fcluster_buffer->clear();

/*
  for(int i = 0; i < fEvents.at(id)->size(); i++) { // loop over all tracks in an event

    GFTrack *track=(GFTrack*)preFitBranch->At(j);

    GFAbsTrackRep* rep;
    rep = track->getTrackRep(0);
    unsigned int numhits = track->getNumHits();
    double charge = rep->getCharge();

    TVector3 track_pos;
    TVector3 old_track_pos;

    TEveStraightLineSet* track_lines = NULL;

    for(int j = 0; j < numhits; j++) { // loop over all hits in the track

      GFAbsRecoHit* hit = track->getHit(j);
      GFDetPlane plane;

      // get the hit infos ------------------------------------------------------------------
      try {
        plane = hit->getDetPlane(rep);
        track_pos = rep->getPos(plane);
      }
      catch(GFException& e) {
        std::cerr << "Exception cought (getDetPlane): Hit " << j << " in Track " << i << " skipped!" << std::endl;
        std::cerr << e.what();
        continue;
      }
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
      TMatrixT<double> hit_coords = hit->getHitCoord(plane);
      Int_t hit_coords_dim = hit_coords.GetNrows();
      TMatrixT<double> hit_coov = hit->getHitCov(plane);

      if(hit_type == "GFPlanarHitPolicy") {
        planar_hit = true;
        if(hit_coords_dim == 1) {
          hit_u = hit_coords(0,0);
          hit_res_u = hit_coov(0,0);
        } else if(hit_coords_dim == 2) {
          planar_pixel_hit = true;
          hit_u = hit_coords(0,0);
          hit_v = hit_coords(1,0);
          hit_res_u = hit_coov(0,0);
          hit_res_v = hit_coov(1,1);
        }
      } else if (hit_type == "GFSpacepointHitPolicy") {
        space_hit = true;
        plane_size = 4;
      } else if (hit_type == "GFWireHitPolicy") {
        wire_hit = true;
        hit_u = hit_coords(0,0);
        plane_size = 4;
      } else {
        std::cout << "Track " << i << ", Hit " << j << ": Unknown policy name: skipping hit!" << std::endl;
        break;
      }

      if(plane_size < 4) plane_size = 4;
      // finished setting variables ---------------------------------------------------------

      // draw track if corresponding option is set ------------------------------------------
      if(drawTrack) {
        if(track_lines == NULL) track_lines = new TEveStraightLineSet;
        if(j > 0) track_lines->AddLine(old_track_pos(0), old_track_pos(1), old_track_pos(2), track_pos(0), track_pos(1), track_pos(2));
        old_track_pos = track_pos;
        if(charge > 0) {
          track_lines->SetLineColor(kRed);
        } else {
          track_lines->SetLineColor(kBlue);
        }
        track_lines->SetLineWidth(2);
        if(drawTrackMarkers) {
          //track_lines->AddMarker(track_pos(0), track_pos(1), track_pos(2));
        }
      }
      // finished drawing track -------------------------------------------------------------

      // draw detectors if option is set, only important for wire hits ----------------------
      if(drawDetectors) {

        if(wire_hit) {
          TEveGeoShape* det_shape = new TEveGeoShape("det_shape");
          double pseudo_res_0 = fErrorScale*std::sqrt(hit_coov(0,0));
          if(!drawHits) { // if the hits are also drawn, make the tube smaller to avoid intersecting volumes
            det_shape->SetShape(new TGeoTube(0, hit_u, plane_size));
          } else {
            det_shape->SetShape(new TGeoTube(0, hit_u - pseudo_res_0, plane_size));
          }
          TVector3 norm = u.Cross(v);
          TGeoRotation* det_rot = new TGeoRotation("det_rot", (u.Theta()*180)/TMath::Pi(), (u.Phi()*180)/TMath::Pi(),
              (norm.Theta()*180)/TMath::Pi(), (norm.Phi()*180)/TMath::Pi(),
              (v.Theta()*180)/TMath::Pi(), (v.Phi()*180)/TMath::Pi()); // move the tube to the right place and rotate it correctly
          TGeoMatrix* det_trans = new TGeoCombiTrans(o(0),o(1),o(2),det_rot);
          det_shape->SetTransMatrix(*det_trans);
          det_shape->SetMainColor(kCyan);
          det_shape->SetMainTransparency(0);
          if((drawHits && (hit_u - pseudo_res_0 > 0)) || !drawHits) {
            gEve->AddElement(det_shape);
          }
        }

      }
      // finished drawing detectors ---------------------------------------------------------

      if(drawHits) {


        // draw spacepoint hits -----------------------------------------------------------
        if(space_hit) {

          // get eigenvalues of covariance to know how to draw the ellipsoid ------------
          TMatrixDEigen eigen_values(hit->getRawHitCov());
          TEveGeoShape* det_shape = new TEveGeoShape("det_shape");
          det_shape->SetShape(new TGeoSphere(0.,1.));
          TMatrixT<double> ev = eigen_values.GetEigenValues();
          TMatrixT<double> eVec = eigen_values.GetEigenVectors();
          TVector3 eVec1(eVec(0,0),eVec(1,0),eVec(2,0));
          TVector3 eVec2(eVec(0,1),eVec(1,1),eVec(2,1));
          TVector3 eVec3(eVec(0,2),eVec(1,2),eVec(2,2));
          TVector3 norm = u.Cross(v);
          // got everything we need -----------------------------------------------------


          TGeoRotation* det_rot = new TGeoRotation("det_rot", (eVec1.Theta()*180)/TMath::Pi(), (eVec1.Phi()*180)/TMath::Pi(),
              (eVec2.Theta()*180)/TMath::Pi(), (eVec2.Phi()*180)/TMath::Pi(),
              (eVec3.Theta()*180)/TMath::Pi(), (eVec3.Phi()*180)/TMath::Pi()); // the rotation is already clear

          // set the scaled eigenvalues -------------------------------------------------
          double pseudo_res_0 = fErrorScale*std::sqrt(ev(0,0));
          double pseudo_res_1 = fErrorScale*std::sqrt(ev(1,1));
          double pseudo_res_2 = fErrorScale*std::sqrt(ev(2,2));
          if(drawScaleMan) { // override again if necessary
            pseudo_res_0 = fErrorScale*0.5;
            pseudo_res_1 = fErrorScale*0.5;
            pseudo_res_2 = fErrorScale*0.5;
          }
          // finished scaling -----------------------------------------------------------

          // autoscale if necessary -----------------------------------------------------
          if(drawAutoScale) {
            double min_cov = std::min(pseudo_res_0,std::min(pseudo_res_1,pseudo_res_2));
            if(min_cov < 1e-5) {
              std::cout << "Track " << i << ", Hit " << j << ": Invalid covariance matrix (Eigenvalue < 1e-5), autoscaling not possible!" << std::endl;
            } else {
              if(min_cov <= 0.149) {
                double cor = 0.15 / min_cov;
                std::cout << "Track " << i << ", Hit " << j << ": Space hit covariance too small, rescaling by " << cor;
                fErrorScale *= cor;
                pseudo_res_0 *= cor;
                pseudo_res_1 *= cor;
                pseudo_res_2 *= cor;
                std::cout << " to " << fErrorScale << std::endl;
              }
            }
          }
          // finished autoscaling -------------------------------------------------------

          // rotate and translate -------------------------------------------------------
          TGeoMatrix* det_trans = new TGeoGenTrans(o(0),o(1),o(2),1/(pseudo_res_0),1/(pseudo_res_1),1/(pseudo_res_2),det_rot);
          det_shape->SetTransMatrix(*det_trans);
          // finished rotating and translating ------------------------------------------

          det_shape->SetMainColor(kYellow);
          det_shape->SetMainTransparency(0);
          gEve->AddElement(det_shape);
        }
        // finished drawing spacepoint hits -----------------------------------------------

        // draw wire hits -----------------------------------------------------------------
        if(wire_hit) {
          TEveGeoShape* det_shape = new TEveGeoShape("det_shape");
          double pseudo_res_0 = fErrorScale*std::sqrt(hit_coov(0,0));

          // autoscale if necessary -----------------------------------------------------
          if(drawAutoScale) {
            if(pseudo_res_0 < 1e-5) {
              std::cout << "Track " << i << ", Hit " << j << ": Invalid wire resolution (< 1e-5), autoscaling not possible!" << std::endl;
            } else {
              if(pseudo_res_0 < 0.0049) {
                double cor = 0.005 / pseudo_res_0;
                std::cout << "Track " << i << ", Hit " << j << ": Wire covariance too small, rescaling by " << cor;
                fErrorScale *= cor;
                pseudo_res_0 *= cor;
                std::cout << " to " << fErrorScale << std::endl;
              }
            }
          }
          // finished autoscaling -------------------------------------------------------

          det_shape->SetShape(new TGeoTube(std::min(0., (double)(hit_u - pseudo_res_0)), hit_u + pseudo_res_0, plane_size));
          TVector3 norm = u.Cross(v);

          // rotate and translate -------------------------------------------------------
          TGeoRotation* det_rot = new TGeoRotation("det_rot", (u.Theta()*180)/TMath::Pi(), (u.Phi()*180)/TMath::Pi(),
              (norm.Theta()*180)/TMath::Pi(), (norm.Phi()*180)/TMath::Pi(),
              (v.Theta()*180)/TMath::Pi(), (v.Phi()*180)/TMath::Pi());
          TGeoMatrix* det_trans = new TGeoCombiTrans(o(0),o(1),o(2),det_rot);
          det_shape->SetTransMatrix(*det_trans);
          // finished rotating and translating ------------------------------------------

          det_shape->SetMainColor(kYellow);
          det_shape->SetMainTransparency(50);
          gEve->AddElement(det_shape);
        }
        // finished drawing wire hits -----------------------------------------------------

      }

    }

    if(track_lines != NULL) gEve->AddElement(track_lines);

  }
*/

  if(resetCam){
    gEve->Redraw3D(kTRUE);
    std::cout<<"Reset Cam!"<<std::endl;
  }
  else{
    gEve->Redraw3D(kFALSE);
    std::cout<<"DON'T Reset Cam!"<<std::endl;
  }

}



/*
TEveBox* PndTpcClustVis::boxCreator(TVector3 o, TVector3 u, TVector3 v, float ud, float vd, float depth) {

  TEveBox* box = new TEveBox;
  float vertices[24];

  TVector3 norm = u.Cross(v);
  u *= (0.5*ud);
  v *= (0.5*vd);
  norm *= (0.5*depth);

  vertices[0] = o(0) - u(0) - v(0) - norm(0);
  vertices[1] = o(1) - u(1) - v(1) - norm(1);
  vertices[2] = o(2) - u(2) - v(2) - norm(2);
  vertices[3] = o(0) + u(0) - v(0) - norm(0);
  vertices[4] = o(1) + u(1) - v(1) - norm(1);
  vertices[5] = o(2) + u(2) - v(2) - norm(2);
  vertices[6] = o(0) + u(0) - v(0) + norm(0);
  vertices[7] = o(1) + u(1) - v(1) + norm(1);
  vertices[8] = o(2) + u(2) - v(2) + norm(2);
  vertices[9] = o(0) - u(0) - v(0) + norm(0);
  vertices[10] = o(1) - u(1) - v(1) + norm(1);
  vertices[11] = o(2) - u(2) - v(2) + norm(2);
  vertices[12] = o(0) - u(0) + v(0) - norm(0);
  vertices[13] = o(1) - u(1) + v(1) - norm(1);
  vertices[14] = o(2) - u(2) + v(2) - norm(2);
  vertices[15] = o(0) + u(0) + v(0) - norm(0);
  vertices[16] = o(1) + u(1) + v(1) - norm(1);
  vertices[17] = o(2) + u(2) + v(2) - norm(2);
  vertices[18] = o(0) + u(0) + v(0) + norm(0);
  vertices[19] = o(1) + u(1) + v(1) + norm(1);
  vertices[20] = o(2) + u(2) + v(2) + norm(2);
  vertices[21] = o(0) - u(0) + v(0) + norm(0);
  vertices[22] = o(1) - u(1) + v(1) + norm(1);
  vertices[23] = o(2) - u(2) + v(2) + norm(2);


  for(int k = 0; k < 24; k += 3) box->SetVertex((k/3), vertices[k], vertices[k+1], vertices[k+2]);

  return box;

}*/


TVector2 PndTpcClustVis::getCoordsInPlane(TVector3 p, TVector3 o, TVector3 u, TVector3 v) {
  TMatrixT<double> A(3, 2);
  TVectorD b(3);

  (b(0) = p(0) - o(0));
  (b(1) = p(1) - o(1));
  (b(2) = p(2) - o(2));

  (A(0,0) = u(0));
  (A(1,0) = u(1));
  (A(2,0) = u(2));
  (A(0,1) = v(0));
  (A(1,1) = v(1));
  (A(2,1) = v(2));

  TDecompSVD svd(A, 0);
  svd.Solve(b);
  TVector2 coords(b(0),b(1));
  return coords;
}


TVector2 PndTpcClustVis::getCoordsInPlane(TVector3 p, GFDetPlane plane) {
  getCoordsInPlane(p, plane.getO(), plane.getU(), plane.getV());
}


void PndTpcClustVis::makeGui() {
  TEveBrowser* browser = gEve->GetBrowser();
  browser->StartEmbedding(TRootBrowser::kLeft);

  TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
  frmMain->SetWindowName("Pandoras Playground");
  frmMain->SetCleanup(kDeepCleanup);

  TGPictureButton* b = 0;
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
    guiDoClustering->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
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
    guiSingeDigiClAmpCut = new TGNumberEntry(hf, ClSingeDigiClAmpCut, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2000);
    hf->AddFrame(guiSingeDigiClAmpCut);
    guiSingeDigiClAmpCut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetClusterfinderParams()");
    lbl = new TGLabel(hf, "Single Digi-Cluster Amp cut");
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
    guiDrawDigis =  new TGCheckButton(hf, "Draw Digis");
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
    lbl = new TGLabel(hf, "\n Pattern Recognition");
      hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiDoPR =  new TGCheckButton(hf, "Do Pattern Recognition");
    if(doPR) guiDoPR->Toggle();
    hf->AddFrame(guiDoPR);
    guiDoPR->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetDrawParams()");
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
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
  frmMain->AddFrame(hf);

  // sorting parameters
  hf = new TGHorizontalFrame(frmMain); {
    guisorting = new TGNumberEntry(hf, _sorting, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 4);
    hf->AddFrame(guisorting);
    guisorting->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Sorting Mode");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiinteractionZ = new TGNumberEntry(hf, _interactionZ, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          -80, 100);
    hf->AddFrame(guiinteractionZ);
    guiinteractionZ->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Z-position of interaction point (for sorting Mode 4)");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guisortingMode =  new TGCheckButton(hf, "Use sorting of riemann tracker");
    if(_sortingMode) guisortingMode->Toggle();
    hf->AddFrame(guisortingMode);
    guisortingMode->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain->AddFrame(hf);

  // Trackfinder Parameters
  hf = new TGHorizontalFrame(frmMain); {
    guiminpoints = new TGNumberEntry(hf, _minpoints, 6,999, TGNumberFormat::kNESInteger,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 200);
    hf->AddFrame(guiminpoints);
    guiminpoints->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "min points for sz-/plane-fit");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiplanecut = new TGNumberEntry(hf, _planecut, 6,999, TGNumberFormat::kNESRealFour,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2);
    hf->AddFrame(guiplanecut);
    guiplanecut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Planecut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
    guiriproxcut = new TGNumberEntry(hf, _riproxcut, 6, 999, TGNumberFormat::kNESRealFour,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 2);
    hf->AddFrame(guiriproxcut);
    guiriproxcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Riemann proximity cut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiszcut = new TGNumberEntry(hf, _szcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiszcut);
    guiszcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "sz cut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiproxcut = new TGNumberEntry(hf, _proxcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiproxcut);
    guiproxcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "Proximity cut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  // Trackmerger Parameters
  hf = new TGHorizontalFrame(frmMain); {
    guiDoMerge =  new TGCheckButton(hf, "Do TrackMerging");
    if(doMerge) guiDoMerge->Toggle();
    hf->AddFrame(guiDoMerge);
    guiDoMerge->Connect("Toggled(Bool_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiTTproxcut = new TGNumberEntry(hf, _TTproxcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTTproxcut);
    guiTTproxcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT Proximity cut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  hf = new TGHorizontalFrame(frmMain); {
    guiTTplanecut = new TGNumberEntry(hf, _TTplanecut, 6,999, TGNumberFormat::kNESRealFour,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 0.99);
    hf->AddFrame(guiTTplanecut);
    guiTTplanecut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT plane cut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
    guiTTszcut = new TGNumberEntry(hf, _TTszcut, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTTszcut);
    guiTTszcut->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "TT sz cut");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);

  hf = new TGHorizontalFrame(frmMain); {
    guiTTscale = new TGNumberEntry(hf, fRiemannScale, 6,999, TGNumberFormat::kNESRealThree,
                          TGNumberFormat::kNEANonNegative,
                          TGNumberFormat::kNELLimitMinMax,
                          0, 99);
    hf->AddFrame(guiTTscale);
    guiTTscale->Connect("ValueSet(Long_t)", "PndTpcClustVis", fh, "guiSetTrackingParams()");
    lbl = new TGLabel(hf, "RiemannScale");
        hf->AddFrame(lbl);
  }
  frmMain->AddFrame(hf);
  frmMain->MapSubwindows();
  frmMain->Resize();
  frmMain->MapWindow();

  browser->StopEmbedding();
  browser->SetTabTitle("Event Control", 0);
}

void PndTpcClustVis::guiGoto(){
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  Long_t n = guiEvent->GetNumberEntry()->GetIntNumber();
  fh->gotoEvent(n);
}

void PndTpcClustVis::guiSetClusterfinderParams(){
  ClMode = guiMode->GetNumberEntry()->GetIntNumber();
  ClTimeslice = giuTimeslice->GetNumberEntry()->GetIntNumber();
  ClTimecut = giuTimecut->GetNumberEntry()->GetIntNumber();
  ClSingeDigiClAmpCut = guiSingeDigiClAmpCut->GetNumberEntry()->GetIntNumber();
  
  if (guiSimpleCl->IsOn()) ClSimpleCl=true;
  else ClSimpleCl=false;
  
  ClSimpleTimeslice = giuSimpleTimeslice->GetNumberEntry()->GetIntNumber();
  
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}

void PndTpcClustVis::guiSetTrackingParams(){
  PRNHits = guiPRNHits->GetNumberEntry()->GetIntNumber();
  _sorting = guisorting->GetNumberEntry()->GetIntNumber();
  _interactionZ = guiinteractionZ->GetNumberEntry()->GetNumber();

  if (guisortingMode->IsOn()) _sortingMode=true;
  else _sortingMode=false;

  _minpoints = guiminpoints->GetNumberEntry()->GetIntNumber();
  _planecut = guiplanecut->GetNumberEntry()->GetNumber();
  _riproxcut = guiriproxcut->GetNumberEntry()->GetNumber();
  _szcut = guiszcut->GetNumberEntry()->GetNumber();
  _proxcut = guiproxcut->GetNumberEntry()->GetNumber();
  _TTproxcut = guiTTproxcut->GetNumberEntry()->GetNumber();
  _TTplanecut = guiTTplanecut->GetNumberEntry()->GetNumber();
  _TTszcut = guiTTszcut->GetNumberEntry()->GetNumber();
  fRiemannScale=guiTTscale->GetNumberEntry()->GetNumber();

  if (guiDoMerge->IsOn()) doMerge=true;
  else doMerge=false;
  
  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}

void PndTpcClustVis::guiSetDrawParams(){

  if (guiInstantRedraw->IsOn()) instantRedraw=true;
  else instantRedraw=false;
  
  if (guiDoClustering->IsOn()) doClustering=true;
  else doClustering=false;

  if (guiDrawTpc->IsOn()) drawTpc=true;
  else drawTpc=false;

  if (guiDrawDigis->IsOn()) drawDigis=true;
  else drawDigis=false;

  if (guiDrawClusters->IsOn()) drawClusters=true;
  else drawClusters=false;

  if (guiDrawClustersErrors->IsOn()) drawClusterErrors=true;
  else drawClusterErrors=false;

  if (guiDoPR->IsOn()) doPR=true;
  else doPR=false;

  PndTpcClustVis*  fh = PndTpcClustVis::getInstance();
  if(instantRedraw) fh->gotoEvent(fEventId);
}


ClassImp(PndTpcClustVis)
