//-------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstIndexedObject.hh,v 1.1.1.1 2005/03/29 16:49:05 steinke Exp $
//
// Description:
//      Simple class to hold an object and its index (in a vector
//      for example) in the same class
//   
// Author List:
//      Phil Strother               March 2000
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//-------------------------------------------------------------------
#ifndef ASTINDEXEDOBJECT_HH
#define ASTINDEXEDOBJECT_HH

//-------------
// C Headers --
//-------------

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//              ---------------------
//              -- Class Interface --
//              ---------------------


template <class T>
class AstIndexedObject{
  
public:

  AstIndexedObject(int, T*);
  
  virtual ~AstIndexedObject();

  inline int getInt() const {return _myInt;}
  inline T* getT() const {return _myT;}

  bool operator==(const AstIndexedObject<T> &other) const;
private:

  AstIndexedObject();
  AstIndexedObject(const AstIndexedObject<T> &);
  AstIndexedObject<T> &operator=(const AstIndexedObject<T> &);

  int _myInt;
  T* _myT;
};

#include  "AstIndexedObject.cxx"

#endif // ASTINDEXEDOBJECT_HH





