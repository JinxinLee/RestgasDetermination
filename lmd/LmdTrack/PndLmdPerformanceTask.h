// -------------------------------------------------------------------------
// -----              PndLmdPerformance Task header file               -----
// -----                     Based on PndLmdGeaneTask                  -----
// -----                   Author: Prometeusz Jasinski                 -----
// -------------------------------------------------------------------------


/** PndLmdPerformanceTask.h
 *@author P.Jasinski <jasinski@kph.uni-mainz.de>
 **
 ** Dedicated to luminosity monitor performance checks
 **/

#ifndef PndLmdPerformanceTask_H
#define PndLmdPerformanceTask_H

// framework includes
#include "FairTask.h"
#include "TH1.h"
#include "TH2.h"
#include <TRotation.h>
#include <TVector3.h>
#include "GFTrackCand.h"
#include "FairGeanePro.h"
#include "TFile.h"
#include "TString.h"

//#include "PndSdsGeoHandling.h"
#include "PndGeoHandling.h"
#include "FairGeanePro.h"
#include "TGeoManager.h"
#include "PndLmdDim.h"
#include <vector>
#include <map>

class TClonesArray;
class PndSdsCluster;
class TGeant3;

// some design constants of the LUMI detector
const int nplanes(4);
const int nsensors_per_plane(8);//(12);

class PndLmdPerformanceTask: public FairTask {
public:

	/** Default constructor **/
	PndLmdPerformanceTask();

	///Set up beam momuntum value and position of interaction point
	PndLmdPerformanceTask(Double_t pBeam, TVector3 IP);

	/** Destructor **/
	~PndLmdPerformanceTask();

	/** Virtual method Init **/
	virtual void SetParContainers();
	virtual InitStatus Init();

	/** Propagate one track back to the initial point by the closest approach **/
	/*void Propagate(TVector3 pos, // position of the measured track
			TVector3 mom, // direction and momentum of the measured track
			int pdgcode, // particle type to be propagated
			TVector3 ip, // initial point assumed to be the origin of the track
			TVector3& initpos, // position after back tracking via distance of closest approach to ip
			TVector3& initdir // initial momentum (assumed) will be changed to direction and momentum after back tracking
			);*/

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	virtual void Finish();

	// to save the output histograms into a file that you prefere
	// it is mandatory to provide that filename
	void SetHistFilename(TString filename);

private:

	TClonesArray* fTrackParGeane;
	TClonesArray* fTrackParIni;
	TClonesArray* fTrackParFinal;
	TClonesArray* fDetName;

	FairGeanePro* fPro;
	PndGeoHandling* fGeoH;
	TGeoManager* fgGeoMan;
	PndLmdDim* lmddim;
	int fEventNr;
	bool fUseMVDPoint;
	Double_t fPbeam;
	Int_t fPDGid;
	TVector3 vtx;

	std::map<int, std::vector<int> > fTrackPixHitIdMap; //Track -> PixHitId
	std::map<int, std::vector<int> > fTrackStripHitIdMap; //Track -> StripHitId

	// hits per each plane are stored event wise in this map
	std::map<int, std::vector<TVector3> > fTrackPlaneHitMap;
	// momenta of possibly more than one anti proton at
	std::map<int, std::vector<TVector3> > fTrackMCVectMapIn; // the a plane of the detector
	std::vector<TVector3> fTrackMCVectMapInit; // the initial point at the target

	// Input Data ----------
	TClonesArray* fMCHits;
	/* TClonesArray* fMCTracks; */

	TClonesArray* fTracks; //Real tracks
	TClonesArray* fHits; //Real hits


	//--- MC info -----------------------------------------------------------------
	TClonesArray* true_tracks;
	TClonesArray* true_points;
	//----------------------------------------------------------------------------------


	//--- Digitization info ------------------------------------------------------------
	TClonesArray* fStripClusterArray;
	TClonesArray* fStripDigiArray;
	//----------------------------------------------------------------------------------

	//--- Real Hits --------------------------------------------------------------------
	TClonesArray* rechit_array;
	//----------------------------------------------------------------------------------


	//--- Track Candidate ---------------------------------------------------------------
	TClonesArray* trkcand_array;
	//-----------------------------------------------------------------------------------

	//--- Real tracks -------------------------------------------------------------------
	TClonesArray* rec_trk;
	//----------------------------------------------------------------------------------
	// Output Data -------

	// performance histograms
	TH2* hist_angular_distr_gen;
	TH2* hist_angular_distr_acc;

	// spatial acceptance in x and y at the first lumi plane
	TH2* hist_spatial_distr_gen;
	TH2* hist_spatial_distr_acc;

	// theta over momentum acceptance
	TH2* hist_theta_over_mom_gen;
	TH2* hist_theta_over_mom_acc;

	// phi over momentum acceptance
	TH2* hist_phi_over_mom_gen;
	TH2* hist_phi_over_mom_acc;

	TTree* tree_results;
	// initial momenta of the antiprotons
	double px_init;
	double py_init;
	double pz_init;
	double ptheta_init;
	double pphi_init;
	// momenta at the entry of a detector plane
	// in the reference system of the LUMI
	double px_in;
	double py_in;
	double pz_in;
	double ptheta_in;
	double pphi_in;
	// position at the entry of a detector plane
	// in the reference system of the LUMI
	double x_in;
	double y_in;
	double z_in;
	// detector and sensor id of the mc hits
	int det_id;
	int sens_id;
	// calculated plane and sensor in plane id
	int plane;
	int sensor;
	// variables due to new HVMAPS setup
	int ihalf, iplane, imodule, iside, idie, isensor;
	double x_in_mod; // reference frame on the module surface
	double y_in_mod;
	double z_in_mod;
	double x_in_sens; // reference frame on the sensor
	double y_in_sens;
	double z_in_sens;
	double x_in_sens_al; // aligned reference frame on the sensor
	double y_in_sens_al;
	double z_in_sens_al;
	double x_in_aligned; // reference frame on the module surface
	double y_in_aligned; // aligned coordinates in the lumi frame
	double z_in_aligned;
	double theta_prop;
	double theta_prop_aligned;
	bool missed;

	// Drawing directly from a tree is elegant but slow as every Draw call
	// loops over the whole tree of events
	// Better it is to create the desired histograms in advance
	// and to fill those on event by event basis simultaneously

	// histograms per plane
	TH2* hist_xy[nplanes];
	TH2* hist_theta_init[nplanes];
	TH2* hist_theta_in[nplanes];
	TH2* hist_theta_rec[nplanes];
	TH2* hist_theta_diff[nplanes];
	TH2* hist_theta_diff_rel[nplanes];
	TH2* hist_theta_rec_diff[nplanes];
	TH2* hist_theta_rec_diff_rel[nplanes];

	// histograms per sensor
	TH2* hists_xy[nplanes][nsensors_per_plane];
	TH2* hists_xy_local[nplanes][nsensors_per_plane];
	TH2* hists_xy_local_cvd[nplanes][nmdules_per_plane];
	TH1* hists_theta_init[nplanes][nsensors_per_plane];
	TH1* hists_theta_in[nplanes][nsensors_per_plane];
	TH1* hists_theta_diff[nplanes][nsensors_per_plane];
	TH1* hists_theta_diff_rel[nplanes][nsensors_per_plane];
	TH1* hists_theta_rec_diff[nplanes][nsensors_per_plane];
	TH1* hists_theta_rec_diff_rel[nplanes][nsensors_per_plane];

	// histograms for geane tests
	TH2* hist_theta_diff_prop_true;
	TH2* hist_theta_diff_prop_true_o_theta;

	TRotation inv_lmdrotation;
	TVector3 inv_lmdtranslation;

	TFile* hist_output_file;

	void Register();

	void Reset();

	void ProduceHits();

	double last_percent;
	unsigned int fnevents;
	unsigned int fievent;
	// draw a progress bar only when the length changes significantly
	void DrawProgressBar(int len, double percent);

	std::map<int, std::vector<int> > AssignHitsToTracks();

ClassDef(PndLmdPerformanceTask,1)
	;

};

#endif
