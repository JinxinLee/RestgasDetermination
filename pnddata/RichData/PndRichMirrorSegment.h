// -------------------------------------------------------------------------
// -----                     PndRichMirrorSegment header file          -----
// -----               Created 22/07/16 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDRICHMIRRORSEGMENT_H
#define PNDRICHMIRRORSEGMENT_H

#include "FairHit.h"
#include <vector>

class PndRichMirrorSegment {

public:    
  
  /** Default constructor **/
  PndRichMirrorSegment();

  PndRichMirrorSegment(TVector3 point,
                       TVector3 dims,
                       TVector3 normal); 
   

/** Copy constructor **/
  PndRichMirrorSegment(const PndRichMirrorSegment& seg) { *this = seg; };

  /** Destructor **/
  virtual ~PndRichMirrorSegment();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  
    
  /** Accessors **/
  virtual TVector3 GetPoint()  {return fMiddleFlatMirrorPoint;}
  virtual TVector3 GetDims()  {return fSizeOfFlatMirror;}
  virtual TVector3 GetNormal()  {return fNormalOfFlatMirror;}
  void SetPoint( TVector3 point ) { fMiddleFlatMirrorPoint = point; }
  void SetDimensions( TVector3 dims ) { fSizeOfFlatMirror = dims; }
  void SetNormal( TVector3 normal ) { fNormalOfFlatMirror = normal; }

 protected:
  
  TVector3 fMiddleFlatMirrorPoint;
  TVector3 fSizeOfFlatMirror;
  TVector3 fNormalOfFlatMirror;
   
  ClassDef(PndRichMirrorSegment,1)
};

#endif //PNDRICHMIRRORSEGMENT_H
