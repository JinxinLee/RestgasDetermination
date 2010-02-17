//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Pad plane in the PndTpc
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef TPCPADPLANE_HH
#define TPCPADPLANE_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include "PndTpcPad.h"
#include <vector>
#include <map>
#include "math.h"
#include <stdio.h>

// Collaborating Class Declarations --
class PndTpcPadShapePool;

class PndTpcPadPlane {
public:

  // Constructors/Destructors ---------
  PndTpcPadPlane(int nx, 
	      int ny, 
	      double xbin,
	      double ybin,
	      double x0,
	      double y0);
  PndTpcPadPlane(const char* const filename,
	      const PndTpcPadShapePool* const pspool);
  ~PndTpcPadPlane();

  // Operators
  friend std::ostream& operator<< (std::ostream& s, const PndTpcPadPlane& me);

  // Accessors -----------------------
  void GetPadList(const double x, 
		   const double y, 
		   const double r, 
		   std::vector<PndTpcPad*>& buffer) const;
  void GetPadXY(const unsigned int PadID, 
		double& x, double& y) const;
  PndTpcPad* GetPad(const unsigned int PadID) const;
  unsigned int GetNPads() const;//the number of pads

  const std::vector<PndTpcPad*>& GetPads() const {return PadReferences;}

  const unsigned int GetNSectors() const {return fnSectors;}
  std::map<unsigned int,PndTpcPad*>* GetSectorList(unsigned int sectorId) {return fSectorLists[sectorId];}
  std::vector<unsigned int> GetSectorIds()const;


  // a simple bounding box
  struct bounding_box {
    bounding_box() : minx(999),maxx(-999),miny(999),maxy(-999) {}
    double minx,maxx,miny,maxy;
  };
  
  bounding_box& GetSector(unsigned int i) {return fSectors[i];}

  const int GetNX() const {return nx;}
  const int GetNY() const {return nx;}
  const double GetXBin() const {return xbin;}
  const double GetYBin() const {return ybin;}
  const double GetX0() const {return x0;}
  const double GetY0() const {return y0;}

  // Modifiers -----------------------

  // Pad Plane will take Ownership!
  void AddPad(PndTpcPad* pad);
  void ReadFromFile(const char* const filename,
		    const PndTpcPadShapePool* const pspool);

  // Reserves place in the Mapping vector PadReferences
  void AllocatePadReferences(const int n){PadReferences.resize(n,0);}
  
  // Operations ----------------------
  struct PadIndexer {
    PadIndexer(int fnx,
	       int fny,
	       double fxbin,
	       double fybin,
	       double fx0,
	       double fy0)
      : nx(fnx),ny(fny),xbin(fxbin),ybin(fybin),x0(fx0),y0(fy0){;}

    int operator()(PndTpcPad* p) const
    {
      double res=floor((p->x()-x0)/xbin) + floor((p->y()-y0)/ybin)*nx;
      return (int)res;
    }
    int operator()(double x, double y) const
    {
      double res=floor((x-x0)/xbin) + floor((y-y0)/ybin)*nx;
      return (int)res;
    }
    int nx;
    int ny;
    double xbin;
    double ybin;
    double x0;
    double y0;
  };


  void WriteToStream(std::ostream& stream) const;

private:

  // Private Data Members ------------
  int nx;      // Number of Regions in x direction
  int ny;      // Number of Regions in y direction
  double xbin; // bin width in x
  double ybin; // bin width in y
  double x0;   // (x0,y0) Origin of plane
  double y0;

  

  PadIndexer Indexer;

  std::vector< std::vector<PndTpcPad*>* > Regions; // regions are just 
                                                // for fast searching

  std::vector<PndTpcPad*> PadReferences;


  std::map<unsigned int,bounding_box> fSectors;  
  std::map<unsigned int,std::map<unsigned int,PndTpcPad*>* > fSectorLists;
  unsigned int fnSectors; // assume Sectors are enumerated 0...(n-1)
                          // a sector represents the collection of pads
                          // that is read out by one front end processor


  // Private Methods -----------------
  bool ReadLine(char* const line,
		const PndTpcPadShapePool* const pspool);
};

// exceptions thrown by this class
#include <exception>
struct unknown_padID : std::exception{
  virtual const char* what() const throw() {
    return "Unkown Pad ID";
  }
};
struct superior_unknown_padId : std::exception{
  superior_unknown_padId(const int padId) : fPadId(padId) {};
  virtual const char* what() const throw() {
    char* bla = new char[50];
    sprintf(bla, "Unknown Pad ID %d", fPadId);
    return bla;
  }
  private:
  int fPadId;
};
struct used_padID : std::exception{
  virtual const char* what() const throw() {
    return "The Pad ID is already assigned.";
  }
}; 



#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
