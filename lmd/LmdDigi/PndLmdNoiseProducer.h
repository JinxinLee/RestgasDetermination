// -------------------------------------------------------------------------
// -----                PndLmdNoiseProducer source file                -----
// -----                  Created 04/02/10  by M.Michel                -----
// -----              Based on PndMvdNoiseProducer by R.Kliemt         -----
// -------------------------------------------------------------------------


/** PndLmdNoiseProducer.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The Noise Producer adds fake hits to silicon sensor channels (strips and
 ** Pixels)
 **/


#ifndef PNDLMDNOISEPRODUCER_H
#define PNDLMDNOISEPRODUCER_H

#include "PndSdsNoiseProducer.h"

class TClonesArray;

class PndLmdNoiseProducer : public PndSdsNoiseProducer
{
 public:

  /** Default constructor **/
  PndLmdNoiseProducer();


  /** Destructor **/
  virtual ~PndLmdNoiseProducer();

  /** Virtual method SetBranchNames **/
  virtual void SetBranchNames(TString inBranchNameStrip, TString inBranchNamePixel, TString folderName);
  virtual void SetBranchNames();

  //This method shall be defined by usr
  virtual void SetMCPointType();

  ClassDef(PndLmdNoiseProducer,2);

};

#endif
