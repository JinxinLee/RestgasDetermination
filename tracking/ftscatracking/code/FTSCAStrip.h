  //-*- Mode: C++ -*-
  // *****************************************************************************
  //                                                                             *
  // @Autors: I.Kulakov; M.Pugach; M.Zyzak; I.Kisel                                        *
  // @e-mail: I.Kulakov@gsi.de; M.Pugach@gsi.de; M.Zyzak@gsi.de; I.Kisel@compeng.uni-frankfurt.de *
  //                                                                             *
  // *****************************************************************************

#ifndef FTSCASTRIP_H
#define FTSCASTRIP_H

class FTSCAStrip {
 public:
  FTSCAStrip():fS(0),fIsUsed(false){}
  
  FTSCAStrip(float f):fS(f),fIsUsed(false){}
  
  operator float() const { return fS; }

  bool IsUsed() const { return fIsUsed; }

  void SetAsUsed() { fIsUsed = true; }
  
 private:
  float fS;

  bool fIsUsed;
};

#endif
  
