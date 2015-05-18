// -------------------------------------------------------------------------
// -----                PndMvdNoiseProducer header file                -----
// -----                  Created 01.07.08  by R.Kliemt                -----
// -------------------------------------------------------------------------


/** PndLmdNoiseProducer.h
 * based on PndMvdNoiseProducer with adaptation to lmd specific calls
 *original @author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The Noise Producer adds fake hits to silicon sensor channels (strips and
 ** Pixels)
 **/


#ifndef PNDLMDNOISEPRODUCER_H
#define PNDLMDNOISEPRODUCER_H

#include "PndMvdNoiseProducer.h"
//#include "FairTask.h"

class PndLmdNoiseProducer : public PndMvdNoiseProducer
{
 public:
	PndLmdNoiseProducer():PndMvdNoiseProducer(){};

  virtual InitStatus Init();

  void FillSensorLists();

  void SetParContainers();

  void Exec(Option_t* opt);

 private:

  std::vector<Int_t> fPixelIds;

  ClassDef(PndLmdNoiseProducer,1);

};

#endif
