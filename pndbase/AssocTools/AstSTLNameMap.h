#ifndef AstSTLNameMap_HH
#define AstSTLNameMap_HH
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstSTLNameMap.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//	Class AstSTLNameMap
//
//      A set of std::string stored both as a 
//         map<string,int> and vector<string> for fast lookup in either 
//         direction
//
// Environment:
//	Software developed for BaBar expirment @ SLAC B-Factory
//
// Author List:
//      Eric Charles
//
// Copyright Information:
//	Copyright (C) 2003              LBNL
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------


//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

#include <map>
#include <vector>
#include <string>
#include <iostream>

//		---------------------
// 		-- Class Interface --
//		---------------------

class AstSTLNameMap {

public:

  // typedefs
  typedef std::map<std::string,unsigned int>       map;
  typedef map::const_iterator                      map_const_iterator;
  typedef std::vector<std::string>                 vector;
  typedef vector::const_iterator                   const_iterator;


  // 0xffffffff is used to indicate a null value 
  // when a requested name is on the the map
  inline static unsigned int null() { return 0xffffffff; }

  // Constructors

  AstSTLNameMap();
  
  // Copy Constructor
  AstSTLNameMap( const AstSTLNameMap& other );

  
  // Destructor
  virtual ~AstSTLNameMap( );

  // Operators
  const AstSTLNameMap& operator=( const AstSTLNameMap& other) {
    if ( this == &other) return *this;
    _vect = other._vect;
    _map = other._map;
    return *this;
  }

  // Selectors (const)

  // begin iterator
  inline const_iterator begin() const { return _vect.begin(); }
  
  // end iterator
  inline const_iterator end() const { return _vect.end(); }

  // return the number of names in this map
  inline unsigned int size() const { return _vect.size(); }  

  // return the index of a particular name
  // is used to indicate that a given name is not in the map  
  inline unsigned int nameNum(const std::string& st) const {
    map_const_iterator itr = _map.find(st);
    return itr != _map.end() ? itr->second : null();
  }

  // get a name for the map by index
  // returns index if name was found, null() otherwize
  inline unsigned int getName(std::string& st, unsigned int num) const {
    if ( num >= _vect.size() ) return null();
    st = _vect[num];
    return num;
  }
  
  // return a name a const char*
  // null pointer is used to indicate search failure
  inline const char* name(unsigned int num) const {
    std::string st;
    unsigned int idx = getName(st,num);
    return idx != null() ? st.c_str() : 0;
  }
  
  // Modifiers

  // clear the map
  inline void clear() { 
    _vect.clear();
    _map.clear();
  }

  // add a name to the map
  // returns the index of the name
  unsigned int add(const std::string& sname);

  // add a name to the map
  // returns the index of the name  
  inline unsigned int add(const char* cname) {
    std::string st(cname);
    return add(st);    
  }

  // print the map
  void print(std::ostream& o) const;

private:

  // Data members
  map    _map;
  vector _vect;

};

#endif
