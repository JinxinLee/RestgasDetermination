//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Singleton class to configure the odgbstream
//      
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef DEBUGLOGGER_HH
#define DEBUGLOGGER_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op
#include <string>
#include <map>

#include "TFile.h"
#include "TNamed.h"

// Collaborating Class Declarations --
class abslogrule;
class TH1;
class TH2;

class DebugLogger : public TNamed{
public:

  // Constructors/Destructors ---------
  static DebugLogger* Instance();
  DebugLogger();
  ~DebugLogger();

  // Operators
  

  // Accessors -----------------------


  // Modifiers -----------------------
  void addRule(abslogrule* rule);
  void addRule(std::string file, std::string function, unsigned int output);
  unsigned int addOutput(std::ostream&);
  //unsigned int addOutFile(std::string filename);  // not implemented
  void setLevel(unsigned int l);
  void Histo(std::string name, double value, 
	     double x0=0, double x1=10, int bins=100);
  void Histo2D(std::string name, double xvalue, double yvalue, 
	    double xmin=0, double xmax=10, int xbins=100,
	    double ymin=0, double ymax=10, int ybins=100);

  // Operations ----------------------
  void WriteFiles();
  TH1* GetHisto(std::string name) {return fhistomap[name];}
  TH2* GetHisto2D(std::string name) {return fhistomap2D[name];}


protected:
  DebugLogger(const DebugLogger&); // forbidden


private:
  // The singleton instance
  static DebugLogger* finstance;

  // Private Methods -----------------
bool findFuncInSource(std::string file,std::string function,
		      unsigned int& startline, unsigned int& endline);
  // Private Data Members ------------
  TFile* foutfile;
  std::map<std::string, TH1*> fhistomap;
  std::map<std::string, TH2*> fhistomap2D;

public:
  ClassDef(DebugLogger,1)
};



#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
