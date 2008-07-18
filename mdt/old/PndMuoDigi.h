#ifndef PNDMUODIGI_HH
#define PNDMUODIGI_HH

#include "CbmHit.h"
#include "TVector3.h"

class PndMuoDigi : public CbmHit {

  //class PndMuoDigi {
public:
 
  
  PndMuoDigi();
  PndMuoDigi(Int_t trackID, Int_t eventID, Short_t sec, Short_t pla, Short_t strip, Short_t box, Short_t tub);
  virtual ~PndMuoDigi();
  
  /** Output to screen (not yet implemented) **/
  virtual void Print(const Option_t* opt = 0) const {;}


  /** Public method Clear
   ** Resets the flag to -1
   **/
  void Clear();

  /** Accessors **/
  Int_t GetFlag() const { return fFlag; }; 


  /** Modifiers **/
  void SetFlag (Int_t flag) { fFlag = flag; };

  
  // set
  
  void setSector(Short_t sec) {nSector = sec;}
  void setPlane(Short_t pla) {nPlane = pla;}
  void setStrip(Short_t strip) {nStrip = strip;}
  void setBox(Short_t box) {nBox = box;}
  void setTube(Short_t tub) {nTube = tub;}
  //void setMult(Short_t mult) {nMult = mult;}  
  
  // get
  int GetTrackID() const {return fTrackID;}
  int GetEventID() const {return fEventID;}

  Short_t getSector() {return nSector;}
  Short_t getPlane()  {return nPlane;} 
  Short_t getStrip()  {return nStrip;}
  Short_t getBox()    {return nBox;}
  Short_t getTube()   {return nTube;} 
  //Short_t getMult()   {return nMult;}

  Short_t getX() {return nSector;}
  Short_t getY()  {return nPlane;}
  
 protected: 
  Int_t fFlag;
  

 private:
 
  Int_t fTrackID; 
  Int_t fEventID;
 
  Short_t nSector;
  Short_t nPlane;
  Short_t nStrip;
  Short_t nBox;
  Short_t nTube; 
  //Short_t nMult;

  ClassDef(PndMuoDigi,1);
};

#endif
