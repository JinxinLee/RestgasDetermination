// -------------------------------------------------------------------------
// -----                 PndSdsCell header file                   -----
// -----                  Created by A. Karavdina            -----
// -------------------------------------------------------------------------

/** PndSdsCell.h
 *@author A.Karavdina <karavdin@kph.uni-mainz.de>
 **
 ** A container for a cell (straight track's segment)
 **/

#ifndef PNDSDSCELL_H
#define PNDSDSCELL_H


#include <stdio.h>
#include <iostream>
#include "TObject.h"

class PndSdsCell : public TObject
{
 
 public:

  /** Default constructor (not for use) **/
  PndSdsCell();

  PndSdsCell(int hitUp, int hitDw);

  /** Destructor **/
  virtual ~PndSdsCell();


  /** Accessors **/
  
  void SetHitUp(int &ih){fhit_upstream = ih;}
  void SetHitDw(int &ih){fhit_downstream = ih;}
  void SetPV(int &ipv){fpv = ipv;}
  int GetHitUp(){ return fhit_upstream; }
  int GetHitDw(){ return fhit_downstream; }
  int GetPV(){return fpv;}

 private:

  int fhit_upstream;     // hit upstream
  int fhit_downstream; // hit downstream
  int fpv;// cell position in evolution

  /* Int_t fsecMC; ///< unique sensor ID */
  /* bool fisMerged; */
  ClassDef(PndSdsCell,1);

};

#endif
