#include <iostream>

#include "AbsArgCast.h"
#include "AbsArg.h"
#include "AbsArgVal.h"
#include "IfdTypeKeyIFace.h"

using std::cout;
using std::endl;

void f( AbsArg& a );

void AbsArgTest(){

  double d=3.1;
  int    i=5;

  cout << "testing AbsArgs..."<< endl;
  cout << endl;

  //cout << "sizeof(AbsArg)        =" << sizeof( AbsArg)          << endl;
  //cout << "sizeof(AbsArgVal<int>)=" << sizeof( AbsArgVal<int> ) << endl;
  //cout << endl;

  cout << "Explicitly create typed-AbsArgVal's" << endl;
  AbsArgVal<double> ad( &d );
  AbsArgVal<int>    ai( &i );
  cout << endl;


  cout << "Call f( AbsArg ) using AbsArgVal<double> and AbsArgVal<int> args.."
       << endl;
  cout << "Expect output of 'got a double' followed by 'not found'" <<endl;
  f( ad );                      // should give "got a double..."
  f( ai );                      // should give "did not find a value..."
  cout << endl;

  cout << "Testing clone method...";
  AbsArg* pd = ad.clone();
  AbsArg* pi = ai.clone();
  if ( ( pd->getTypeKey() == pi->getTypeKey() ) ||
       ( pd->getTypeKey() != ad.getTypeKey() ) ||
       ( pi->getTypeKey() != ai.getTypeKey() ) ||
       ( AbsArgCast<double>::value( pd ) != AbsArgCast<double>::value( ad )) ||
       ( AbsArgCast<int>::value( pi ) != AbsArgCast<int>::value( ai ) )
       ) 
    {
      cout << "Error:  failed clone test." << endl;
    } else {
      cout << "Pass." << endl;
    }

  //  the following converstion does not wok
  //
  //cout << "Test auto converstion of T to AbsArgVal<T> for fn with"
  // << " signature of f( AbsArg)"
  // << endl;
  //
  //f( 4.2 );
  //f( 1 );
  
  exit(0);

}


void
f( AbsArg& a ) {

  double *d = 0;

  d = AbsArgCast<double>::value( a );

  if ( 0 != d ) {
    cout << "f:  got a double arg of " << *d << endl;
  } else {
    cout << "f:  did not find a value" << endl;
  }
}
