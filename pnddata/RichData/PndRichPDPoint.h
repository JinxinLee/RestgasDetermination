#ifndef PNDRICHPDPOINT_H
#define PNDRICHPDPOINT_H 1


#include "FairMCPoint.h"

#include "TObject.h"
#include "TVector3.h"

class PndRichPDPoint : public FairMCPoint
{

  public:

    /** Default constructor **/
    PndRichPDPoint();


    /** Constructor with arguments
     *@param trackID  Index of MCTrack
     *@param detID    Detector ID
     *@param pos      Ccoordinates at entrance to active volume [cm]
     *@param mom      Momentum of track at entrance [GeV]
     *@param tof      Time since event start [ns]
     *@param length   Track length since creation [cm]
     *@param eLoss    Energy deposit [GeV]
     **/
    PndRichPDPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
                   Double_t tof, Double_t length, Double_t eLoss, UInt_t EventId);




    /** Destructor **/
    virtual ~PndRichPDPoint();

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

  private:
    /** Copy constructor **/
    PndRichPDPoint(const PndRichPDPoint& point);
    PndRichPDPoint operator=(const PndRichPDPoint& point);

    ClassDef(PndRichPDPoint,1)

};

#endif
