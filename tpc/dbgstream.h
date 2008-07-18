//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      streambuffer for debugging streams
//
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      adapted from http://www.inf.uni-konstanz.de/~kuehl/iostream
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef DEBUGSTRMBUF_HH
#define DEBUGSTRMBUF_HH

// Base Class Headers ----------------


// Collaborating Class Headers -------
#include <iostream> // remove if you do not need streaming op
#include <iosfwd>
#include <vector>
#include <string>

// Collaborating Class Declarations --

//typedef std::streambuf std::basic_streambuf<char, std::char_traits<char> >;


class dbgstrmbuf: public std::streambuf {
public:

  // Constructors/Destructors ---------
  dbgstrmbuf(std::streambuf *sb);
  ~dbgstrmbuf(){}

  // Accessors -----------------------


  // Modifiers -----------------------
  void setprfx(const std::string& p);
  void setbuf(std::streambuf *sb);
  // Operations ----------------------
  void off(bool flag=true){_active=!flag;}
  void on(bool flag=true){_active=flag;}

protected:
  
  int overflow(int);
  int underflow();
  int uflow();
  //int sync();

private:

  // Private Data Members ------------
  std::streambuf *_sbuf;
  int _cache; // cache a read character
  char* _prfx;
  unsigned int _len;
  bool _newline;
  bool _active;

  // Private Methods -----------------

};

// ------------------------------------------------------------------------
// stream classes
class abslogrule;

class idbgstream: public std::istream {
public:
  idbgstream(std::streambuf* sb);
  ~idbgstream();
};


class odbgstream: public std::ostream {
public:
  odbgstream(std::streambuf* sb);
  odbgstream();
  ~odbgstream();

  odbgstream& setlevel(unsigned int l);
  
  odbgstream& setpos(std::string file,unsigned int line);

  

  friend class DebugLogger; // a singleton interface to configure the stream

protected:
  void addrule(abslogrule* rule){_rules.push_back(rule);}
  unsigned int addbuffer(std::streambuf* sb);
  void minlevel(unsigned int l){_minlevel=l;}

private:
  std::vector<std::streambuf*> _slots; //buffers to choose from
  std::vector<abslogrule*> _rules;
  std::string _file;
  std::string _path;
  unsigned int _line;
  unsigned int _minlevel;
  unsigned int _level;
  void setslot(unsigned int i); // choose buffer;
  bool _fileblock;
  bool _levelblock;
  
  unsigned int slot(std::string path,unsigned int line, unsigned int level);

  bool block(){return _fileblock || _levelblock;}
};

extern odbgstream dbgstrm;

#ifndef __CINT__
#define LOG dbgstrm.setpos(__FILE__,__LINE__)<<level(0)
#endif

// ------------------------------------------------------------------------
// manupilators 

class level {
public:
  level(unsigned int l): _l(l){}
  odbgstream& operator()(odbgstream&) const;
private:
  unsigned int _l;
};

odbgstream &operator<<(odbgstream &out, level al);




// ------------------------------------------------------------------------
// helper class to encapsule rules for logging

class abslogrule {
public:
  // returns the id of the slot to use for output. slot0 is /dev/null
  virtual ~abslogrule(){}
  virtual unsigned int rule(std::string path,unsigned int line,unsigned int level)=0;
};

class logrule : public abslogrule {
public:
  logrule(std::string path,
	  unsigned int minline,unsigned int maxline, 
	  unsigned int output);
  unsigned int rule(std::string path,unsigned int line,unsigned int level);

private:
  std::string _file; // file to log
  unsigned int _min; // linerange:
  unsigned int _max;
  unsigned int _output;
};



#endif


//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
