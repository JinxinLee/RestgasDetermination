#ifndef CBMGENERICPARORAIO_H
#define CBMGENERICPARORAIO_H

#include "FairDetParOraIo.h"

class FairOraConn;
class FairParSet;
class FairParGenericSet;
class CbmParamBinObj;

class FairGenericParOraIo : public FairDetParOraIo {
private:
  class CbmParOraBlob : public TObject {
    public:
      CbmParamBinObj* binaryParam;   // Pointer to object in parameter list
      Int_t           blobId;        // Id of BinaryLargeObject
      CbmParOraBlob(CbmParamBinObj* o=0,Int_t id=0) { binaryParam=o; blobId=id; }
      ~CbmParOraBlob() {}
  };
public:
  FairGenericParOraIo(FairOraConn* p=0);
  ~FairGenericParOraIo() {;}
  Bool_t init(FairParSet*,Int_t*);
  Int_t write(FairParSet*);
  Bool_t read(FairParGenericSet*);
  Int_t writeSet(FairParGenericSet*);
  Bool_t readFromLoadingTable(FairParGenericSet*,Int_t);
private:
  Bool_t readBlob(CbmParamBinObj* obj,Int_t lobId);
  Int_t createParamVers(FairParGenericSet*);
  Int_t storeBlob(UChar_t*, Int_t);
  Bool_t readLoadBlob(CbmParamBinObj*,Int_t);
  ClassDef(FairGenericParOraIo,0) // I/O from Oracle for gemeric parameter containers
};

#endif  /* !CBMGENERICPARORAIO_H */

