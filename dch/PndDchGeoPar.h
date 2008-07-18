// -------------------------------------------------------------------------
// -----                PndDchGeoPar header file                       -----
// -----                Created 15/05/07  by P.Hawranek                -----
// -------------------------------------------------------------------------

/** \class PndDchGeoPar
 *  \author P.Hawranek
 *  \date 15/05/07
 *  \brief 
 * 
 * 
 * 
 **/

#ifndef PNDDCHGEOPAR_H
#define PNDDCHGEOPAR_H

#include "CbmParGenericSet.h"
#include "TObjArray.h"

class PndDchGeoPar : public CbmParGenericSet {
 public:
  TObjArray        *fGeoSensNodes; ///< List of CbmGeoNodes for sensitive  volumes
  TObjArray      *fGeoPassNodes;   ///< List of CbmGeoNodes for passive  volumes

  /** Standard constructor **/ 
  PndDchGeoPar(const char* name="PndDchGeoPar",
             const char* title="Dch Geometry Parameters",
             const char* context="TestDefaultContext");

  /** Destructor **/
  ~PndDchGeoPar(void);

  /** Public method clear() **/
  void clear(void);

  /** Public method putParams(...) **/
  void putParams(CbmParamList*);

  /** Public method gutParams(...) **/
  Bool_t getParams(CbmParamList*);

  /** Public method GetGeoSensitiveNodes(...) **/
  TObjArray* GetGeoSensitiveNodes(){return fGeoSensNodes;}

  /** Public method GetGeoPassiveNodes(...) **/
  TObjArray* GetGeoPassiveNodes(){return fGeoPassNodes;}

  ClassDef(PndDchGeoPar,1)
};

#endif /* !PNDDCHGEOPAR_H */
