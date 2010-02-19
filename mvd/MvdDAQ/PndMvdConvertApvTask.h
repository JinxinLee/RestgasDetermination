// -------------------------------------------------------------------------
// -----                 PndMvdConvertApvTask header file              -----
// -----                  Created 12/01/09  by L.Ackermann             -----
// -------------------------------------------------------------------------


/** PndMvdConvertApvTask.h
 *@author L.Ackermann <lars.ackermann@physik.tu-dresden.de>, 
 *        R.Kliemt <ralf.kliemt@hiskp.uni-bonn.de>
 **
 ** Converting Task from ascci hit file to clones array of PndMvdDigiStrip
 **/


#ifndef PNDMVDAPVCONVERTTASK_H
#define PNDMVDAPVCONVERTTASK_H


// framework includes
#include "FairTask.h"
#include "PndMvdConvertApv.h"
#include "PndMvdMapApv.h"
#include "PndMvdGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndMvdDigiStrip;

/**   
 @class PndMvdConvertApvTask : public FairTask
 @brief Convert Task of data
 
 Task to convert data from real measurement with strip sensors
 @author Lars Ackermann
 @date 11.03.2009
*/
class PndMvdConvertApvTask : public FairTask
{
 public:

	/** Default constructor **/  
	PndMvdConvertApvTask(PndMvdConvertApv* Apvconvert, PndMvdMapApv* Apvmapper);

	/** Destructor **/
	~PndMvdConvertApvTask();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	/**
	@fn virtual InitStatus Init()
	init task
	*/
	virtual InitStatus Init();

	/**
	@fn virtual void Finish()
	finish task
	@return void
	*/
	virtual void Finish();
  
 private:
	/// class which convert the read in and hold the streams
	PndMvdConvertApv* fApvConvert;
  PndMvdMapApv* fApvMapper;
	/// array of results
	TClonesArray* fStripArray;
  ///Geohandler for name string compression
  PndMvdGeoHandling* fGeoH;
	/// number of strips in this event
	Int_t iStrip;

	/**
	@fn void Register()
	@return void
	*/
	void Register();

	/**
	@fn void Reset()
	@return void
	*/
	void Reset();

  ClassDef(PndMvdConvertApvTask,1);

};

#endif
