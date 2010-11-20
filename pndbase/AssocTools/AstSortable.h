//--------------------------------------------------------------------------
//
// Environment:
//      This software was developed for the BaBar collaboration.  If you
//      use all or part of it, please give an appropriate acknowledgement.
//
// Description:
// minimal templated class for Key Value pair that complies with the requirements of sortability via the Value type
//
//
// class K requires
//   copy c'tor
//   operator =
//
// class V requires
//   copy c'tor
//   operator =
//   operator ==
//   operator <
//
// Author
//     Rolf Dubitzky
//
// Copyright Information:
//
// Bertram Kopf (RUB)  migrated to PandaRoot
//------------------------------------------------------------------------

#ifndef AstSortable_HH
#define AstSortable_HH


//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//              ---------------------
//              -- Class Interface --
//              ---------------------

template <class K, class V> class AstSortable
{

//--------------------
// Declarations     --
//--------------------

    // Typedefs, consts, and enums

//--------------------
// Instance Members --
//--------------------


  public:
    // C'tors
    AstSortable() {};  // required for sorted containers (STL & RW)
    AstSortable( const K k, const V v ) : _k( k ), _v( v ) {};
    AstSortable( const AstSortable& other ) : _k( other.key() ), _v( other.value() ) {};
    // D'tor
    ~AstSortable() {};
    // assignment
    AstSortable& operator =  (const AstSortable& other ) { _k = other.key(); _v = other.value(); return *this; };
    // required relations (STL & RW)
    bool operator == (const AstSortable& other ) const { return _v==other.value(); };
    bool operator <  (const AstSortable& other ) const { return _v<other.value(); };
    // getters
    const K& key()   const { return _k; }
    const V& value() const { return _v; }
    // setters
    void setKey( const K k ) { _k = k; }
    void setValue( const V v ) { _v = v; }

  private:

    // Data members
    K _k;
    V _v;

};


#endif 

