//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPadPlane
//      see PndTpcPadPlane.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// This Class' Header ------------------
#include "PndTpcPadPlane.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <exception>
#include <fstream>
#include "string.h"
#include "stdlib.h"
// Collaborating Class Headers --------
#include "TError.h"
#include "TString.h"
#include "PndTpcPadShapePool.h"
#include "PndTpcAbsPadShape.h"

// Class Member definitions -----------


PndTpcPadPlane::PndTpcPadPlane(int fnx, 
			 int fny, 
			 double fxbin,
			 double fybin,
			 double fx0,
			 double fy0)
  : nx(fnx),ny(fny),xbin(fxbin),ybin(fybin),x0(fx0),y0(fy0), 
    fnSectors(0), Indexer(fnx,fny,fxbin,fybin,fx0,fy0)
{
  int nRegions=nx*ny;
  for(int iRegion=0; iRegion<nRegions; ++iRegion){
    Regions.push_back(new std::vector<PndTpcPad*>);
  }
}

PndTpcPadPlane::PndTpcPadPlane(const char* const filename,
			 const PndTpcPadShapePool* const pspool)
  : fnSectors(0), Indexer(0,0,0.,0.,0.,0.)
{
  if(pspool==NULL)std::cout<<"WARNING: no padshapes defined!"
			   <<" PadPlane will not be usable!"<<std::endl;
  //open file to construct Indexer
  std::ifstream infile(filename, std::fstream::in);
  TString meh(filename);
  Warning("PndTpcPadPlane::PndTpcPadPlane","Reading file..."+meh);
  if (!infile.good())
    Fatal("PndTpcPadPlane::PndTpcPadPlane","The file can not be found.");
  //search the file for the line describing the Indexer
  while (infile.good())
  {
    char line[256];
    infile.getline(line, 256);
    //cancel comments
    char* com=strchr(line, '*');
    if (com != 0)
      *com = '\0';
    //the line has to begin with "PadPlaneSubDivision"
    char* ppSub = strstr(line, "PadPlaneSubDivision");
    if (ppSub == 0) 
      continue;
    //reading the values
    ppSub = &(ppSub[strlen("PadPlaneSubDivision")]);
    char* rest;
    int fnx = (int) strtol(ppSub, &rest, 10);
    int fny = (int) strtol(rest, &rest, 10);
    double fxbin = strtod(rest,&rest);
    double fybin = strtod(rest,&rest);
    double fx0 = strtod(rest,&rest);
    char* end;
    double fy0 = strtod(rest,&end);
    if (rest == end)//It was impossible to read double fy0
      Fatal("PndTpcPadPlane::PndTpcPadPlane","Too less arguments in input file.");

    //Create the Indexer and the regions (like in  the simple constructor)
    Indexer.nx = nx =fnx;
    Indexer.ny = ny =fny;
    Indexer.xbin = xbin =fxbin;
    Indexer.ybin = ybin =fybin;
    Indexer.x0 = x0 =fx0;
    Indexer.y0 = y0 =fy0;
    int nRegions=nx*ny;
    for(int iRegion=0; iRegion<nRegions; ++iRegion)
      Regions.push_back(new std::vector<PndTpcPad*>);
    
    infile.unget();//if it was the last line=>eof=>without this !infile.good()
    break;
  }
  if (!infile.good())//e.g. eof
    Fatal("PndTpcPadPlane::PndTpcPadPlane","Error searching a line of the form:\n PadPlaneSubDivision int int double double double double \n Pad Plane can't be build.");
  infile.close();
  ReadFromFile(filename, pspool);
}

// PadPlane Takes Ownership over Pads!
PndTpcPadPlane::~PndTpcPadPlane(){
  int n=PadReferences.size();
  for(int i=0;i<n;++i){
    delete PadReferences[i];
  }
  for(int ir=0; ir<nx*ny; ++ir){
    delete Regions[ir];
  }
}

void 
PndTpcPadPlane::GetPadList(const double x, 
			const double y, 
			const double r, 
			std::vector<PndTpcPad*>& buffer) const {
  
  for(int iy=-1; iy<=1; ++iy){
    for(int ix=-1; ix<=1; ++ix){
      double xin = x+ix*xbin;
      double yin = y+iy*ybin;
      if (xin<x0 || xin>nx*xbin+x0 || yin<y0 ||  yin>ny*ybin+y0) continue;
      //Get Regions
      int iRegion=Indexer(xin,yin);
      int nPads=Regions[iRegion]->size();
      for(int iPad=0; iPad<nPads; ++iPad){
	//Look which Pads ly inside range
	PndTpcPad* aPad=(*Regions[iRegion])[iPad];
	if(aPad->CircleIntersection(x,y,r))buffer.push_back(aPad);
      }
    }
  }
}

void 
PndTpcPadPlane::GetPadXY(const unsigned int PadID, 
		      double& x, double& y) const
{
  PndTpcPad* pad=GetPad(PadID);
  x=pad->x();
  y=pad->y();
}

PndTpcPad*
PndTpcPadPlane::GetPad(const unsigned int PadID) const
{
  if (PadID >= PadReferences.size()) throw superior_unknown_padId(PadID);
  PndTpcPad* pad=PadReferences[PadID];
  if(pad==NULL) throw superior_unknown_padId(PadID);
  return(pad);
}

unsigned int
PndTpcPadPlane::GetNPads() const
{
  unsigned int np=0;
  for (unsigned int i=0; i<PadReferences.size(); ++i)
    if (PadReferences[i] != 0)
      np++;
  return(np);
}

// Pad Plane will take Ownership!
void 
PndTpcPadPlane::AddPad(PndTpcPad* pad)
{
  //std::cout<<*pad<<std::endl;
  int i=Indexer(pad);
  try {
    Regions.at(i)->push_back(pad);
    fSectors[pad->sectorId()].minx=std::min(fSectors[pad->sectorId()].minx,pad->x());
    fSectors[pad->sectorId()].miny=std::min(fSectors[pad->sectorId()].miny,pad->y());
    fSectors[pad->sectorId()].maxx=std::max(fSectors[pad->sectorId()].maxx,pad->x());
    fSectors[pad->sectorId()].maxy=std::max(fSectors[pad->sectorId()].maxy,pad->y());
    // this assumes continually nummbered sectors:
    if(pad->sectorId()>=fnSectors)fnSectors=pad->sectorId()+1;
  } 
  catch (std::exception& e) {
    std::cerr<<"Index="<<i<<std::endl;
    std::cerr<<e.what()<<std::endl;
    throw;
  }

  unsigned int ID=pad->id();
  try {
    GetPad(ID);
  }
  catch (unknown_padID& e)//the desired case
  {
    if(PadReferences.size()<=ID)PadReferences.resize(ID+1,0);
    PadReferences[ID]=pad;
    if(fSectorLists[pad->sectorId()]==0)fSectorLists[pad->sectorId()]=new std::map<unsigned int, PndTpcPad*>;
    (*fSectorLists[pad->sectorId()])[pad->padId()]=pad;
    return;
  }
  catch (superior_unknown_padId& e)//the desired case
  {
    if(PadReferences.size()<=ID)PadReferences.resize(ID+1,0);
    PadReferences[ID]=pad;
    if(fSectorLists[pad->sectorId()]==0)fSectorLists[pad->sectorId()]=new std::map<unsigned int, PndTpcPad*>;
    (*fSectorLists[pad->sectorId()])[pad->padId()]=pad;
    return;
  }
  //the padID is not unknown=> some other pad has this ID
  Regions.at(i)->pop_back();
  throw used_padID();
}

void
PndTpcPadPlane::ReadFromFile(const char* const filename,
			  const PndTpcPadShapePool* const pspool)
{
  bool noproblem = true;
  std::ifstream infile(filename, std::fstream::in);
  Warning("PndTpcPadPlane::ReadFromFile","Reading file %s",filename);
  if (!infile.good())
    Fatal("PndTpcPadPlane::ReadFromFile","The file %s can not be found.",filename);
  while (infile.good() && noproblem)
  {
    char line[256];
    infile.getline(line, 256);
    noproblem = ReadLine(line, pspool);
  }
  // if (!infile.eof())
  // Fatal("PndTpcPadPlane::ReadFromFile","The file %s can not be read.",filename);
  infile.close();
  if (noproblem == false)
    Fatal("PndTpcPadPlane::ReadFromFile","The file %s does not correspond to the required format.",filename);
}

bool
PndTpcPadPlane::ReadLine(char* const line,
		      const PndTpcPadShapePool* const pspool)
{
  //remove comments at end of line
  char* com=strchr(line, '*');//comment
  if (com != 0)
    *com = '\0';
  if (line[0] == '\0')
    return(true);
  //Is either read by the right constructor or ignored
  if (strstr(line, "PadPlaneSubDivision") != 0)
    return(true);
  char* rest;
  unsigned int ID;
  unsigned int sectorID;
  unsigned int shapeID;
  double x;
  double y;
  double angle;
  ID = (unsigned int) strtoul(line, &rest, 10);
  sectorID = (unsigned int) strtoul(rest, &rest, 10);
  shapeID = (unsigned int) strtoul(rest, &rest, 10);
  x = strtod(rest,&rest);
  y = strtod(rest,&rest);
  char* end;
  angle = strtod(rest,&end);
  if (rest == line)//No reading happened (e.g. only blanks)
    return(true);
  if (rest == end)//It was impossible to read angle
  {  
    Warning("PndTpcPadPlane::ReadLine","In line: %s \n Too less arguments.",line);
    return(false);
  }

  PndTpcAbsPadShape* shape=0; 
  if(pspool!=NULL){
    shape=pspool->GetPadShape(shapeID);
    if (shape == 0)//shapeId does not exist
      {
	Warning("PndTpcPadPlane::ReadLine","In line: %s \n There is no PndTpcPadShape with ID= %i",line,shapeID);
	return(false);
      }
  }
  

  PndTpcPad* pad = new PndTpcPad(x, y, angle,
			   shape, sectorID, ID);
  // read in neighbour ids if available
  char* end2;
  unsigned int nid= (unsigned int) strtoul(end, &end2, 10);
  while(end!=end2){
    pad->addNeighbour(nid);
    end=end2;
    nid= (unsigned int) strtoul(end, &end2, 10);
  }
  try {
    AddPad(pad);
  }
  catch (std::exception& e)
  {
    Warning("PndTpcPadPlane::ReadLine","The pad from line: %s \ncould not be added.Reason: \n %s",line,e.what());
    return(false);
  }
  return(true);
}

 

std::vector<unsigned int> 
PndTpcPadPlane::GetSectorIds()const {
  std::map<unsigned int,std::map<unsigned int,PndTpcPad*>*>::const_iterator sIt=fSectorLists.begin();
  std::vector<unsigned int> ids;
  while(sIt!=fSectorLists.end()){
    ids.push_back(sIt->first);
    ++sIt;
  }
  return ids;
}




// Operators
std::ostream& operator<< (std::ostream& s, const PndTpcPadPlane& me){
  return s << "PndTpcPadPlane:\n"
	   << "      nx="<<me.GetNX()
	   << "      ny="<<me.GetNY()<<"\n"
	   << "      nRegions="<<me.Regions.size()<<"\n"
	   << "      Covered Area: ("<<me.GetX0()<<","<<me.GetY0()
	   << ")->("<<me.GetNX()*me.GetXBin()+me.GetX0()
	   << ","<<me.GetNY()*me.GetYBin()+me.GetY0()<< ")\n"
	   << "      nPads="<<me.PadReferences.size()<<"\n";
}

void 
PndTpcPadPlane::WriteToStream(std::ostream& stream) const {
  stream << "PadPlaneSubDivision "
	 <<GetNX()<<" "
	 <<GetNY()<<" "
	 <<GetXBin()<<" "
	 <<GetYBin()<<" "
	 <<GetX0()<<" "	 
	 <<GetY0()<<std::endl;

  unsigned int npads=GetNPads();
  for(unsigned int ipad=0;ipad<npads;++ipad){
    PndTpcPad* pad=GetPad(ipad);
    stream << pad->id() <<" "
	   << pad->sectorId() <<" "
	   << pad->shape()->GetID()<<" "
	   << pad->x()<<" "
	   << pad->y()<<" "
	   << pad->angle()<<" ";
          
    unsigned int nn=pad->nNeighbours();
    for(unsigned int in=0;in<nn;++in){
      stream << pad->getNeighbour(in) <<" ";
    }

    stream << std::endl;
  }
  
  stream.flush();
}

ClassImp(PndTpcPadPlane)
