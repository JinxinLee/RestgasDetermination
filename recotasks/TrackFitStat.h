//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Data object carrying infomration about a fitted track
//      (residuals, chi2,...)
//      This class is meant to serve as a container which can be
//      used witht the ROOT TreeViewer to produce plots
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TRACKFITSTAT_HH
#define TRACKFITSTAT_HH

// Base Class Headers ----------------
#include "TObject.h"
#include "TVector3.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <vector>
// Collaborating Class Declarations --



class TrackFitStat : public TObject{
public:

  // Constructors/Destructors ---------
  TrackFitStat();
  virtual ~TrackFitStat(){}

  // Operators
  
  // Accessors -----------------------
  Int_t GetPdgCode()	{ return _nPDG; }
  Int_t GetMotherID()	{ return _motherID; }
  Double_t GetMCP()		{ return _pmc;	}

  // Modifiers -----------------------

  void fillPndTpcResX(const std::vector<double>& r){_PndTpcResX=r;}
  void fillPndTpcResY(const std::vector<double>& r){_PndTpcResY=r;}
  void fillPndTpcClusterSize(const std::vector<unsigned int>& s){_PndTpcClusterSize=s;}
  void fillPndTpcClusterAmp(const std::vector<unsigned int>& s){_PndTpcClusterAmp=s;}

  // Operations ----------------------

  void setp(double p){_p=p;}
  void setmom(TVector3 p){_mom=p;}
  void setpmc(double p){_pmc=p;_hasMc=true;}
  void setpstart(double pstart){_pstart=pstart;}
  void setsigp(double s){_sigp=s;}
  void setcharge(double q){_charge=q;}
  void setmccharge(double q){_mccharge=q;}
  void setpdg(Int_t pdg){_nPDG=pdg;}
  void setmotherid(Int_t motherID)	{_motherID=motherID;}
  unsigned int nhits() const {return _PndTpcResX.size();}

private:

  // Private Data Members ------------
  std::vector<double> _PndTpcResX;
  std::vector<double> _PndTpcResY;
  std::vector<unsigned int> _PndTpcClusterSize;
  std::vector<unsigned int> _PndTpcClusterAmp;

  double _p;
  double _pmc;
  double _pstart;
  TVector3 _mom;
  double _charge;
  double _mccharge;
  double _sigp;
  Int_t _nPDG;
  Int_t _motherID;
  
  bool _hasMc;

  // Private Methods -----------------

public:
  ClassDef(TrackFitStat,7)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
