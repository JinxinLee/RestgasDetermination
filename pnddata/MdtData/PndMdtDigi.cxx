#include "PndMdtDigi.h"

// -----   Default constructor   -------------------------------------------
PndMdtDigi::PndMdtDigi()
{
  fDetectorID = -1;
  fLabPos.SetXYZ(0,0,0);
}


PndMdtDigi::PndMdtDigi(Int_t detID, TVector3& pos, std::vector<Int_t> pointList)
{
  fDetectorID = detID;
  fLabPos = pos;
  fPointList = pointList;
  SetLinks(FairMultiLinkedData("MdtPoint", pointList));
}

/** Destructor **/
PndMdtDigi::~PndMdtDigi() 
{
} 

ClassImp(PndMdtDigi)
