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
  Double_t GetP()       {return _p;}
  TVector3 GetMom()     {return _mom;}

  const std::vector<double>* GetResX() {return &_PndTpcResX;}
  const std::vector<double>* GetResY() {return &_PndTpcResY;}
  const std::vector<double>* GetResZ() {return &_PndTpcResZ;}
  const std::vector<double>* GetChi2X() {return &_PndTpcChi2X;}
  const std::vector<double>* GetChi2Y() {return &_PndTpcChi2Y;}
  const std::vector<double>* GetChi2Z() {return &_PndTpcChi2Z;}
  const std::vector<unsigned int>* GetClSizes() {return &_PndTpcClusterSize;}
  const std::vector<unsigned int>* Get2DClSizes() {return &_PndTpc2DClusterSize;}
  const std::vector<double>* GetAmps() {return &_PndTpcClusterAmp;}
  const std::vector<TVector3>* GetProjectionPoints() {return &_PndTpcProj;}
  const std::vector<double>* GetHitPositionsX() {return &_PndTpcPosX;}
  const std::vector<double>* GetHitPositionsY() {return &_PndTpcPosY;}
  const std::vector<double>* GetHitPositionsZ() {return &_PndTpcPosZ;}


  // Modifiers -----------------------

  void fillPndTpcResX(const std::vector<double>& r){_PndTpcResX=r;}
  void fillPndTpcResY(const std::vector<double>& r){_PndTpcResY=r;}
  void fillPndTpcResZ(const std::vector<double>& r){_PndTpcResZ=r;}
  void fillPndTpcChi2X(const std::vector<double>& r){_PndTpcChi2X=r;}
  void fillPndTpcChi2Y(const std::vector<double>& r){_PndTpcChi2Y=r;}
  void fillPndTpcChi2Z(const std::vector<double>& r){_PndTpcChi2Z=r;}
  void fillPndTpcClusterSize(const std::vector<unsigned int>& s){_PndTpcClusterSize=s;}
  void fillPndTpc2DClusterSize(const std::vector<unsigned int>& s){_PndTpc2DClusterSize=s;}
  void fillPndTpcClusterAmp(const std::vector<double>& s){_PndTpcClusterAmp=s;}
  void fillPndTpcProjPoints(const std::vector<TVector3>& s){_PndTpcProj=s;}
  void fillPndTpcHitPositionsX(const std::vector<double>& s){_PndTpcPosX=s;}
  void fillPndTpcHitPositionsY(const std::vector<double>& s){_PndTpcPosY=s;}
  void fillPndTpcHitPositionsZ(const std::vector<double>& s){_PndTpcPosZ=s;}
  
  void addFailedHits(int NFH) {_failedHits+=NFH;}
  void setNDF(unsigned int ndf) {_NDF=ndf;}
  void setChi2(double chi2) {_ChiSqu=chi2;}

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

  unsigned int getFailedHits() {return _failedHits;}
  unsigned int getNDF() const {return _NDF;}    //NDF from fit
  double getChi2() const {return _ChiSqu;}      //ChiSqu from fit
  

private:

  // Private Data Members ------------
  std::vector<double> _PndTpcResX;
  std::vector<double> _PndTpcResY;
  std::vector<double> _PndTpcResZ;
  std::vector<double> _PndTpcChi2X;
  std::vector<double> _PndTpcChi2Y;
  std::vector<double> _PndTpcChi2Z;
  std::vector<unsigned int> _PndTpcClusterSize;
  std::vector<unsigned int> _PndTpc2DClusterSize;
  std::vector<double> _PndTpcClusterAmp;
  std::vector<TVector3> _PndTpcProj;
  std::vector<double> _PndTpcPosX;
  std::vector<double> _PndTpcPosY;
  std::vector<double> _PndTpcPosZ;

  double _p;
  double _pmc;
  double _pstart;
  TVector3 _mom;
  double _charge;
  double _mccharge;
  double _sigp;
  Int_t _nPDG;
  Int_t _motherID;

  unsigned int _failedHits;   //stores the number of failed hits (cov was zero)
  
  bool _hasMc;
  double _ChiSqu;
  unsigned int _NDF;

  // Private Methods -----------------

public:
  ClassDef(TrackFitStat,10)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
//Version 8 : Chi2 info added
