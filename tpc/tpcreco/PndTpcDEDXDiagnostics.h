//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      can be used to draw the sequence of clusters in a track
//	   with the position of the clusters	
// 	   the idea behind is to investigate the influence of diffusion on dx
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCDEDXDIAGNOSTICS_HH
#define PNDTPCDEDXDIAGNOSTICS_HH

// Base Class Headers ----------------
#include "TObject.h"
#include "PndTpcDEDXDiagnosticPoint.h"
#include <vector>

class TPolyMarker3D;
class TPolyLine3D;
class TPolyLine;

class PndTpcDEDXDiagnostics : public TObject
{
public:
	PndTpcDEDXDiagnostics(){}
	virtual ~PndTpcDEDXDiagnostics()	{}
	void SetPoint(PndTpcDEDXDiagnosticPoint point);
	TPolyMarker3D* GetMarker() const;
	TPolyLine3D* GetConnectedLine() const;
	TPolyLine* GetLine2D() const;
private:
	std::vector<PndTpcDEDXDiagnosticPoint> fPoints;
public:
  	ClassDef(PndTpcDEDXDiagnostics,3)
};

#endif
