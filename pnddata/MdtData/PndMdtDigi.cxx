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
PndMdtDigi::PndMdtDigi(Int_t detID, TVector3& pos, Int_t evtNo)
{
  fDetectorID = detID;
  fLabPos = pos;
  fEvtNo  = evtNo;
}

/** Destructor **/
PndMdtDigi::~PndMdtDigi() 
{
} 

PndMdtDigi::PndMdtDigi(const PndMdtDigi& copy):
  fDetectorID(copy.fDetectorID)
  ,fLabPos(copy.fLabPos)
  ,fPointList(copy.fPointList)
  ,fEvtNo(copy.fEvtNo)
{
  SetTimeStamp(copy.GetTimeStamp());
  SetTimeStampError(copy.GetTimeStampError());
  SetLinks(FairMultiLinkedData("MdtPoint", copy.fPointList));
}
bool PndMdtDigi::operator < (const PndMdtDigi& rValue) const
{
  return (GetTimeStamp() < rValue.GetTimeStamp());
}

ClassImp(PndMdtDigi)
