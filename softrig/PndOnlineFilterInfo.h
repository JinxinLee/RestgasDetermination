#ifndef PNDONLINEFILTERINFO_H
#define PNDONLINEFILTERINFO_H
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndOnlineFilterInfo	                                                //
//                                                                      //
// Information Container for Online Filter Results                      //
//                                                                      //
// Author: Klaus Goetzen, GSI, 2013                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "TObject.h"
#include "TBuffer.h"

#define OFIMAXMODES 50


class PndOnlineFilterInfo : public TObject 
{

 public:

	PndOnlineFilterInfo();
	~PndOnlineFilterInfo();

	// *** any tag?
	bool Tagged() const {return fTag;}
	
	// *** total number of tags of all triggers
	int  GetNTagTotal() const { return fNTagTotal; } 
	
	// *** accessor to number of tags for mode
	int GetNTag(int mode);
	
	// *** modifier for number of tags for mode
	void SetNTag(int mode, const int tag);
	
	void Reset();
	void Print();

	// custom streamer; needed to build map
	//virtual void Streamer(TBuffer &R__b);

 protected:
	 
	bool fTag;
	int fNTagTotal;
	
	int fNModes;
	int fMode[OFIMAXMODES];
	int fNTag[OFIMAXMODES];
		
    ClassDef(PndOnlineFilterInfo,2);
    
};


#endif                                           


