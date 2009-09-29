//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class dbgstream
//      see dbgstream.hh for details
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
#ifndef f_CINT__
// This Class' Header ------------------
#include "dbgstream.h"

// C/C++ Headers ----------------------
#include <sstream>
#include <algorithm>
#include <fstream>
#include "string.h"
using std::streambuf;
using std::string;
using std::istream;
using std::ostream;

// Collaborating Class Headers --------


// Class Member definitions -----------


odbgstream dbgstrm;

std::ofstream devnull("/dev/null");

dbgstrmbuf::dbgstrmbuf(streambuf* sb) 
  : streambuf(), fsbuf(sb), fcache(EOF),
    fprfx(NULL),flen(0),fnewline(true),
    factive(true)
{
  setp(0,0); // no buffering by this buffer
  setg(0,0,0);
}

void
dbgstrmbuf::setprfx(const string& p){
  flen=p.size();
  fprfx=strcpy(new char[flen+1],p.c_str());
}

void 
dbgstrmbuf::setbuf(streambuf* sb){
  fsbuf=sb;
}

int
dbgstrmbuf::underflow(){
  if(fcache==EOF){
    fcache = fsbuf->sbumpc();
    return fcache;
  }
  else return fcache;
}

int
dbgstrmbuf::uflow(){
  if(fcache==EOF){
    int rc= fsbuf->sbumpc();
    return rc;
  }
  else {
    int rc=fcache;
    fcache = EOF;
    return rc;
  }
}

int
dbgstrmbuf::overflow(int c){
  if(!factive)return 0;
  if(c!=EOF){
    if(fnewline)
      if(fsbuf->sputn(fprfx,flen)!=flen)
	return EOF;
      else fnewline=false;

    int rc= fsbuf->sputc(c);
    if(c == '\n') fnewline=true;
    return rc;
  }
  return 0;
}

// ------------------------------------------------------------------------
//int
//dbgstrmbuf::sync(){
//  fsbuf->sync();
//  return 0;
//}


idbgstream::idbgstream(streambuf* sb)
  : istream(new dbgstrmbuf(sb))
{}

odbgstream::odbgstream()
  : ostream(new dbgstrmbuf(devnull.rdbuf())), fminlevel(0)
{
  addbuffer(devnull.rdbuf());
  addbuffer(std::cout.rdbuf());
}

odbgstream::odbgstream(streambuf* sb)
  : ostream(new dbgstrmbuf(sb))
{
  addbuffer(sb);
}

idbgstream::~idbgstream()
{
  delete rdbuf();
}


// ------------------------------------------------------------------------
odbgstream::~odbgstream()
{
  if(rdbuf()!=NULL) delete rdbuf();
}

unsigned int 
odbgstream::addbuffer(streambuf* sb){
  fslots.push_back(sb);
  return fslots.size()-1;
}

odbgstream& 
odbgstream::setlevel(unsigned int l){
  flevel=l;
  dbgstrmbuf* mybuf=dynamic_cast<dbgstrmbuf*>(rdbuf());
  mybuf->setbuf(fslots.at(slot(fpath,fline,flevel)));
  //flevelblock=l<fminlevel;
  //mybuf->off(block());
  return *this;
}

odbgstream& 
odbgstream::setpos(string f, unsigned int l){
  fpath=f;
  ffile=f.substr(f.find_last_of("/")+1);
  fline=l;

  dbgstrmbuf* mybuf=dynamic_cast<dbgstrmbuf*>(rdbuf());
  mybuf->setbuf(fslots.at(slot(fpath,fline,flevel)));
  
  

  std::stringstream prfx;
  prfx<<ffile<<"("<<fline<<"): ";
 
  mybuf->setprfx(prfx.str());
  return *this;
}

unsigned int 
odbgstream::slot(string path,unsigned int line, unsigned int level)
{
  if(level<fminlevel)return 0;
  unsigned int slot=0;
  unsigned int nr=frules.size();
  //std::cout<<"checking "<<nr<<" rules. ";
  for(int i=0;i<nr;++i){
    slot=std::max(slot,frules[i]->rule(path,line,level));
  }
  //std::cout<<"("<<path<<","
  //	   <<line<<") -> selected slot "<<slot<<std::endl;
  if(slot<fslots.size())return slot;
  else return 0;
}



// -----------------------------------------------------------------------


odbgstream&
level::operator()(odbgstream& os) const {
  os.setlevel(fl);
  return os;
}

odbgstream &operator<<(odbgstream &out, level al)
{
    return al(out);
}


logrule::logrule(string path,unsigned int minline,unsigned int maxline,
		 unsigned int output)
  : ffile(path), fmin(minline), fmax(maxline), foutput(output)
{}

unsigned int 
logrule::rule(string path,unsigned int line, unsigned int level){
  if(path.find(ffile)!=string::npos && fmin<=line && line<=fmax)
    return foutput;
  else return 0;
}




#endif
