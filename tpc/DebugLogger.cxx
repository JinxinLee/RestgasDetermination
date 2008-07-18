//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class DebugLogger
//      see DebugLogger.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "DebugLogger.h"

// Collaborating Class Headers --------
#include "dbgstream.h"
#include "TH1D.h"
#include "TH2D.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <fstream>
using std::string;
// Class Member definitions -----------



ClassImp(DebugLogger)

DebugLogger* 
DebugLogger::_instance=NULL;

DebugLogger*
DebugLogger::Instance() {
  if(_instance==0){
    _instance = new DebugLogger();
  }
  return _instance;
}

DebugLogger::DebugLogger() : TNamed()
{
  if(_instance!=NULL)throw;
  _instance=this;
  _outfile=new TFile("log.root","RECREATE");
}

DebugLogger::~DebugLogger()
{
  std::cout<<"Destructing DebugLogger"<<std::endl;
  if(_outfile!=NULL){
    _outfile->Close();
    delete _outfile;
  }
}

// handling histos
void
DebugLogger::Histo(string name, double value, 
		   double x0, double x1, int bins){
  if(_histomap[name]==NULL){
    _histomap[name]=new TH1D(name.c_str(),name.c_str(),bins,x0,x1);
  }
  _histomap[name]->Fill(value);
  return;
}

void
DebugLogger::Histo2D(string name, double xvalue, double yvalue, 
	    double xmin, double xmax, int xbins,
	    double ymin, double ymax, int ybins){
  if(_histomap2D[name]==NULL){
    _histomap2D[name]=new TH2D(name.c_str(),name.c_str(),xbins,xmin,xmax,
			       ybins,ymin,ymax);
  }
  _histomap2D[name]->Fill(xvalue,yvalue);
  return;
}



void 
DebugLogger::WriteFiles(){
  _outfile->cd();
  std::map<string, TH1*>::iterator it=_histomap.begin();
  while(it!=_histomap.end()){
    std::cout<<"DebugLogger::Writing histogram "<<it->first<<std::endl;
    it->second->Write();
    delete it->second;
    ++it;
  }
  std::map<string, TH2*>::iterator it2=_histomap2D.begin();
  while(it2!=_histomap2D.end()){
    std::cout<<"DebugLogger::Writing histogram "<<it2->first<<std::endl;
    it2->second->Write();
    delete it2->second;
    ++it2;
  }

  if(_outfile!=NULL){
    _outfile->Close();
    delete _outfile;
  }
}

// Streering the debug-stream
void
DebugLogger::addRule(abslogrule* rule){
  dbgstrm.addrule(rule);
  //std::cout<<"number of rules="<<dbgstrm._rules.size()<<std::endl;
}

void 
DebugLogger::addRule(string file, string function, unsigned int output){
  unsigned int startline=0;
  unsigned int endline=99999;
  if(findFuncInSource(file,function,
		      startline,endline)){
    logrule* rule=new logrule(file,startline,endline,output);
    addRule(rule);
  }
  else std::cout<<"DebugLogger::Rule not created!"<<std::endl;
}


unsigned int 
DebugLogger::addOutput(ostream& os){
  //std::cout<<"number of slots="<<dbgstrm._slots.size()+1<<std::endl;
  return dbgstrm.addbuffer(os.rdbuf());
}

void
DebugLogger::setLevel(unsigned int l){
  dbgstrm.minlevel(l);
}


bool 
DebugLogger::findFuncInSource(string file,string function,
			      unsigned int& startline, unsigned int& endline)
{
  std::ifstream infile(file.c_str(),std::fstream::in);
  int count=0;
  startline=0;
  bool found=false;
  string line;
  char openpar;openpar='{';
  char closepar;closepar='}';
  int numopenpar=0;
  int numclosepar=0;
  while(!found && infile.good()){
    while(infile.good()){// search start of function
      getline(infile,line);
      ++count;
      // check if function is in this line
      string::size_type loc = line.find(function);
      if( loc != string::npos ) { // found expression
	found=true;
	startline=count;
	break;
      }
    } // end loop search start of function
    if(!found)return false;
    while(infile.good()){// search start parenthesis
      // check if parenthesis is opened before semicolon
      string::size_type loc1=line.find('{');
      string::size_type loc2=line.find(';');
      if(loc1!=string::npos && loc2!=string::npos && loc2<loc1)
	{found=false;break;}
      if(loc1==string::npos && loc2!=string::npos)
	{found=false;break;}
      if(loc1!=string::npos && loc2==string::npos)
	{found=true;break;}
      getline(infile,line);
      ++count;
    }// end loop search start parenthesis
  } // end loop search start of function

    while(infile.good()){ // search end-parenthesis
    // check if function is in this line
    numopenpar+=std::count(line.begin(),line.end(),openpar);
    numclosepar+=std::count(line.begin(),line.end(),closepar);
    if(numopenpar>0 && numopenpar==numclosepar){
      endline=count;
      break;
    }
    ++count;
    getline(infile,line);
  } // end loop search end-parenthesis
  if(endline==0)return false;
  return true;
}
