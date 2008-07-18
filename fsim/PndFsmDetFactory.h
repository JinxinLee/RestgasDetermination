//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: FsmDetFactory.hh,v 1.5 2006/07/24 14:27:17 steinke Exp $
//
// Description:
//      Class FsmDetFactory
//      
//  Factory  for Fast Sim Detectors 
//
//  This software was developed for the PANDA collaboration.  If you
//  use all or part of it, please give an appropriate acknowledgement.
//
// Author List:
//      Klaus Goetzen                    Original Author
//
// Copyright Information:
//      Copyright (C) 2006              GSI
//
//------------------------------------------------------------------------

#ifndef PNDFSMDETFACTORY_H
#define PNDFSMDETFACTORY_H

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating class Headers --
//-------------------------------

//---------------
// C++ Headers --
//---------------
#include <string>
#include "ArgList.h"
//--------------------------------------------
// Collaborating class forward declarations --
// -------------------------------------------

class PndFsmAbsDet;

class PndFsmDetFactory
{
public:

  //--------------------
  // Public interface --
  //--------------------

  //
  // Constructors
  //

  // Default ctor.
  PndFsmDetFactory();
  
  // Destructor
  ~PndFsmDetFactory();
  
  // Accessors to contained information
  PndFsmAbsDet* create(std::string &name,ArgList &par);
  PndFsmAbsDet* create(std::string &name, std::string par);
  
  //
  //  Modifiers
  //
     
private:
  
};

#endif
