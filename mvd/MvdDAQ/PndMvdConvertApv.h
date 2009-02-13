// --------------------------------------------------------
// ----     PndMvdConvertApv header file                   ---
// ----     Created 06.01.09 by Lars Ackermann          ---
// ----     on the basic of the ana tool of dts         ---
// --------------------------------------------------------

/** PndMvdConvertApv.h
*@author L.Ackermann <lars.ackermann@physik.tu-dresden.de>
**/

#ifndef PNDMVDCONVERTAPV_H
#define PNDMVDCONVERTAPV_H

#include "TString.h"
#include "TObject.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include "PndMvdApvHit.h"
#include "PndMvdGeoHandling.h"
//! Class for digitising strip of Apv-Sensors to digtil hits

// class PndMvdApv;
#include "PndMvdDigiStrip.h"

class PndMvdConvertApv
{
   public :

        /**
        main constructor, call all function to be ready for converting hits from hitfile
        @param CalibFileName name of file where the calibration is stored
        @param HitFileName name of hitfile
        */
	PndMvdConvertApv(const TString& CalibFileName, const TString& HitFileName);

	/** Destructor **/
	~PndMvdConvertApv()
	{ fDataFile.close(); }

	/**
	@fn long int GetNofEvents()

	@return long integer of last eventID from hitfile
	*/
        long int GetNofEvents();

	/**
	@fn std::vector<PndMvdDigiStrip> ReadAll()

	read all events from hitfile
	@return vector of PndMvdDigiStrip of the event
	*/
	std::vector<PndMvdDigiStrip> ReadAll();

	/**
	@fn std::vector<PndMvdDigiStrip> ReadNext()

	read the next event from hitfile
	@return vector of PndMvdDigiStrip of the event
	*/
	std::vector<PndMvdDigiStrip> ReadNext();

   private :
	/**
	@fn void ModulChecker(Int_t moduleID, std::vector<Int_t>& modules)

	check the vector of modules if the moduleID is known, if not mind it
	@param moduleID moduleID which could be unknown
	@param modules vector of moduleIDs of modules in the hitfile
	@return void
	*/
        void ModulChecker(Int_t moduleID, std::vector<Int_t>& modules);

	/**
	@fn void LoadCalibration(TString CalibFileName, std::vector<Int_t> modules)

	Read the calibration of the modules
	@param CalibFileName Filename where the calibration of the modules are listed
	@param modules vector of moduleIDs of modules in the hitfile
	@return void
	@Todo load calibration from database
	*/
	void LoadCalibration(TString CalibFileName, std::vector<Int_t> modules);

	/**
	@fn std::vector<PndMvdDigiStrip> Calc(std::vector<PndMvdApvHit> hitlist)

	Convert the readed hit and store them in the PndMvdDigiStrip
	@param hitlist vector of PndMvdApvHit hits of one event from hitfile
	@return list of converted PndMvdDigiStrip
	*/
	std::vector<PndMvdDigiStrip> Calc(std::vector<PndMvdApvHit> hitlist);

	/// Calib Parameters: <moduleID<FE<channel> > >
// 	std::vector<std::map<Int_t, std::vector<double> > > fCalibPars;
	std::map<Int_t, std::map<Int_t, std::map<Int_t, double> > > fCalibPars;

	/// stored last eventID of the hitfile
	long int fNofEvents;

	/// stored current eventID
	long int fEvent;

	/// true if calib was succesfull loaded
	bool fNoCalib;

	/// name of hitfile
	TString fHitFileName;

	/// fstream of hitfile
	std::ifstream fDataFile;

	/// stored readed events
        std::vector<PndMvdApvHit> fhitlist;

  PndMvdGeoHandling* fGeoH;	     //! Gives Access to the Path info of a hit


	ClassDef(PndMvdConvertApv,1);
};

#endif
