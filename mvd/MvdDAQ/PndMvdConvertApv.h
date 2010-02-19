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

/**   
 @class PndMvdConvertApv
 @brief Convert meassured data into PndMvdDigiStrip
 
 Load calibration parameter and keeps strean to meassured data. Event wise read in and converting to PndMvdDigiStrip
 @author Lars Ackermann
 @date 11.03.2009
*/
class PndMvdConvertApv
{
   public :
	/** default constructor **/
	PndMvdConvertApv() {;}

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

	/**
	@fn Bool_t Init()
	Initialize global geometry
	@return success
	*/
	Bool_t Init();

	/**
	@fn void SetFakePair(Int_t TopModuleID, Int_t BottomModuleID)
	Set two moduleIDs to merge them as one double sided sensor
	@param TopModuleID moduleID for top side of fake sensor
	@param BottomModuleID moduleID for bottom side of fake sensor
	@return void
	*/
	void SetFakePair(Int_t TopModuleID, Int_t BottomModuleID);

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

	/// stored fake module top side
	Int_t fTopModuleID;

	/// stored fake module bottom side
	Int_t fBottomModuleID;

	/// knows if fake is allowed
	Bool_t fFake;

  PndMvdGeoHandling* fGeoH;	     //! Gives Access to the Path info of a hit

private:
	ClassDef(PndMvdConvertApv,1);
};

#endif
