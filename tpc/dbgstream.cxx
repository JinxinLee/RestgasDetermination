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
#ifndef __CINT__
// This Class' Header ------------------
#include "dbgstream.h"

// C/C++ Headers ----------------------
#include <sstream>
#include <algorithm>
#include <fstream>

using std::streambuf;
using std::string;
using std::istream;
using std::ostream;

// Collaborating Class Headers --------


// Class Member definitions -----------


odbgstream dbgstrm;

std::ofstream devnull("/dev/null");

dbgstrmbuf::dbgstrmbuf(streambuf* sb) 
  : streambuf(), _sbuf(sb), _cache(EOF),
    _prfx(NULL),_len(0),_newline(true),
    _active(true)
{
  setp(0,0); // no buffering by this buffer
  setg(0,0,0);
}

void
dbgstrmbuf::setprfx(const string& p){
  _len=p.size();
  _prfx=strcpy(new char[_len+1],p.c_str());
}

void 
dbgstrmbuf::setbuf(streambuf* sb){
  _sbuf=sb;
}

int
dbgstrmbuf::underflow(){
  if(_cache==EOF){
    _cache = _sbuf->sbumpc();
    return _cache;
  }
  else return _cache;
}

int
dbgstrmbuf::uflow(){
  if(_cache==EOF){
    int rc= _sbuf->sbumpc();
    return rc;
  }
  else {
    int rc=_cache;
    _cache = EOF;
    return rc;
  }
}

int
dbgstrmbuf::overflow(int c){
  if(!_active)return 0;
  if(c!=EOF){
    if(_newline)
      if(_sbuf->sputn(_prfx,_len)!=_len)
	return EOF;
      else _newline=false;

    int rc= _sbuf->sputc(c);
    if(c == '\n') _newline=true;
    return rc;
  }
  return 0;
}

// ------------------------------------------------------------------------
//int
//dbgstrmbuf::sync(){
//  _sbuf->sync();
//  return 0;
//}


idbgstream::idbgstream(streambuf* sb)
  : istream(new dbgstrmbuf(sb))
{}

odbgstream::odbgstream()
  : ostream(new dbgstrmbuf(devnull.rdbuf())), _minlevel(0)
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
  _slots.push_back(sb);
  return _slots.size()-1;
}

odbgstream& 
odbgstream::setlevel(unsigned int l){
  _level=l;
  dbgstrmbuf* mybuf=dynamic_cast<dbgstrmbuf*>(rdbuf());
  mybuf->setbuf(_slots.at(slot(_path,_line,_level)));
  //_levelblock=l<_minlevel;
  //mybuf->off(block());
  return *this;
}

odbgstream& 
odbgstream::setpos(string f, unsigned int l){
  _path=f;
  _file=f.substr(f.find_last_of("/")+1);
  _line=l;

  dbgstrmbuf* mybuf=dynamic_cast<dbgstrmbuf*>(rdbuf());
  mybuf->setbuf(_slots.at(slot(_path,_line,_level)));
  
  

  std::stringstream prfx;
  prfx<<_file<<"("<<_line<<"): ";
 
  mybuf->setprfx(prfx.str());
  return *this;
}

unsigned int 
odbgstream::slot(string path,unsigned int line, unsigned int level)
{
  if(level<_minlevel)return 0;
  unsigned int slot=0;
  unsigned int nr=_rules.size();
  //std::cout<<"checking "<<nr<<" rules. ";
  for(int i=0;i<nr;++i){
    slot=std::max(slot,_rules[i]->rule(path,line,level));
  }
  //std::cout<<"("<<path<<","
  //	   <<line<<") -> selected slot "<<slot<<std::endl;
  if(slot<_slots.size())return slot;
  else return 0;
}



// -----------------------------------------------------------------------


odbgstream&
level::operator()(odbgstream& os) const {
  os.setlevel(_l);
  return os;
}

odbgstream &operator<<(odbgstream &out, level al)
{
    return al(out);
}


logrule::logrule(string path,unsigned int minline,unsigned int maxline,
		 unsigned int output)
  : _file(path), _min(minline), _max(maxline), _output(output)
{}

unsigned int 
logrule::rule(string path,unsigned int line, unsigned int level){
  if(path.find(_file)!=string::npos && _min<=line && line<=_max)
    return _output;
  else return 0;
}




#endif
