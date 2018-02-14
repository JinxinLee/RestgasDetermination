// $Id: TProtoUnpackEvent.h 478 2009-10-29 12:26:09Z linev $
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#ifndef TProtoEVENT_H
#define TProtoEVENT_H

#include "TGo4EventElement.h"
#include "ProtoParams.h"

class TProtoUnpackEvent : public TGo4EventElement {
 public:
  TProtoUnpackEvent() ;
  TProtoUnpackEvent(const char* name) ;
  virtual ~TProtoUnpackEvent() ;
  
  /**
   * Method called by the event owner (analysis step) to clear the
   * event element.
   */
  void Clear(Option_t *t="");
  
  Double_t amplitude[ConfiguredProtoChannels];
  Double_t energy[ConfiguredProtoChannels];
  Double_t time[ConfiguredProtoChannels];
  Int_t waveform[ConfiguredProtoChannels][256];
  Int_t trigger[ConfiguredProtoChannels/16+1];
  Int_t rawData[1024];
  Int_t rawDataLength;

  ClassDef(TProtoUnpackEvent,3);
};

#endif //TProtoEVENT_H



