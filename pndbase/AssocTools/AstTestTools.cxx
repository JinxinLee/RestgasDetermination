//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstTestTools.cc,v 1.1.1.1 2005/03/29 16:49:06 steinke Exp $
//
// Description:
//	AstTestTools:  simply utility classes and global functions for
//      use with AssocTools regression tests (testAst*).
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother                     Original Author
//      Michael Kelsey <kelsey@slac.stanford.edu>
//
// Copyright Information:
//	Copyright (C) 2000  Princeton University
//
//------------------------------------------------------------------------



#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "PndStdDeleteObject.h"
#include "String.h"

#include "AstTestTools.h"
using std::endl;
using std::cout;
using std::ostream;

// Generic container class with RWTPtrXxx semantics

bool SimpleClass::operator==(const SimpleClass &other) const {
  return _myInt == other._myInt;
}

bool SimpleClass::operator<(const SimpleClass &other) const {
  return _myInt < other._myInt;
}

bool simpleClassLessThan(const SimpleClass &sc1, const SimpleClass &sc2) {
  if (sc1.getInt() < sc2.getInt()) return true;
  return false;
}


// Hashing function for RW string pointers

unsigned stringHashFunction(std::string * const &hashThis) {
   return panda::String::rwHash(*hashThis);
}

unsigned strValHashFunction(const std::string& hashThis) {
   return panda::String::rwHash(hashThis);
}


// Function to exercise function registration

void AstSTLTestCleaner(std::vector<std::string*> &theVec) {
  std::for_each(theVec.begin(), theVec.end(), PndStdDeleteObject());
  theVec.clear();
}


// Display delimited message at start of test

void AstTestBanner(std::string name) {
  cout << "***************************************************" << endl
     << "** Tests for " << name << endl
     << "***************************************************" << endl;
}


void outputInOrder(ostream &os, const std::set<std::string>::iterator & from, 
		   const std::set<std::string>::iterator &to)
{
  std::set<std::string>::iterator iter = from;
  while (iter != to) {
    os << *iter << endl;
    ++iter;
  }
}

void outputInOrder(ostream &os, const std::set<std::string*>::iterator & from, 
		   const std::set<std::string*>::iterator &to)
{
  std::set<std::string*>::iterator iter = from;
  while (iter != to) {
    os << *(*iter) << endl;
    ++iter;
  }
}

void outputInOrder(ostream &os, const std::vector<std::string*> & vec)
{
  std::vector<std::string*>::const_iterator iter = vec.begin();
  while (iter != vec.end()) {
    os << *(*iter) << endl;
    ++iter;
  }
}

