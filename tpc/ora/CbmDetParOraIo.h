#ifndef CBMDETPARORAIO_H
#define CBMDETPARORAIO_H

#include "FairDetParIo.h"

class FairOraConn;
class FairRtdbRun;

class CbmParOraSet : public TNamed {
public:
  Int_t contextId;         // Id of context
  Double_t versDate[2];    // Actual date range of parameters
  CbmParOraSet(const char* pName);
  ~CbmParOraSet() {}
  void clearVersDate();      
  ClassDef(CbmParOraSet,0)
};

class FairDetParOraIo : public FairDetParIo {
protected:
  FairOraConn* pConn;        // pointer to Oracle connection class
  FairRtdbRun* actContVers;  // pointer to the actual list of container versions
  Int_t actRunId;           // actual runId (can be -1 if there are no data in Oracle)
  TList* containerList;     // list of parameter containers
public:
  FairDetParOraIo(FairOraConn* p=0);
  virtual ~FairDetParOraIo(void);
  void commit(void);
  void rollback(void);
  void showSqlError(const char*);
  Int_t getActRunId(void) { return actRunId; }
  Int_t getRunStart(FairParSet* pPar=0);
  Int_t getPredefVersion(FairParSet*);
  const char* getHistoryDate();
protected:
  void setChanged(FairParSet*);
  CbmParOraSet* getOraSet(FairParSet*);
  Int_t getContextId(const char*,const char*);
private:
  ClassDef(FairDetParOraIo,0) // base class for the detector interface to Oracle
};

#endif  /* !CBMDETPARORAIO_H */
