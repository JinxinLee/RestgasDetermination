#include <iostream>
#include <stdlib.h>
#include <vector>

#include "Ifd.h"
#include "IfdSimpleProxyDict.h"
#include "IfdStrKey.h"
#include "IfdDataProxy.h"
#include "IfdTestClasses.h"
#include "IfdStdVectorProxy.h"

using std::cout;
using std::endl;
using std::vector;

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------



#ifdef IFDTYPEKEY_SPECIALIZE
 IfdTypeKeySpecialize( vector<ListA>, 0 )
#endif



void IfdStdVectorListTest(){

  // Output a sed string to allow us to do regression tests.
  // IfdTypeKeys pick up their values from statics, and different
  // systems init them differently.  Here we make a xlation table
  // for this particular run.

//   IfdTypeKey< Ifd<vector<ListA*> > > tk;

//   cout << "sed "
//        << " -e 's/" << tk <<"/IfdTypeKey[Ifd<vector<ListA*>>]/g' "
//        << " -e 's/0[xX][0-9,a-f,A-F]*/0xbogus/g' "
//        << " << ENDofTEST"
//        << endl;  

  IfdTypeKey< Ifd<vector<B*> > > tk;

  cout << "sed "
       << " -e 's/" << tk <<"/IfdTypeKey[Ifd<vector<B*>>]/g' "
       << " -e 's/0[xX][0-9,a-f,A-F]*/0xbogus/g' "
       << " << ENDofTEST"
       << endl;  

//   Make the ProxyDict to mess around with
  //
  IfdSimpleProxyDict *dict = new IfdSimpleProxyDict;
  dict->print( cout, IfdSimpleProxyDict::noAddresses );

  // Make a std::vector list and a proxy to hold it.
  //
  vector<B*> *l       = new vector<B*>;

  IfdStdVectorProxy<B*>  *p  = new IfdStdVectorProxy<B*> ( l );

  l = 0;

  // Stick it in the dictionary
  
  Ifd< vector<B*> >::put( dict, p );
  dict->print( cout, IfdSimpleProxyDict::noAddresses );

  // Pull it back out (just to show we can)
  //
  l = Ifd< std::vector<B*> >::get(dict);
  if (0==l) {
    cout << "Failed to recover std::vector from the dictionary"
         << endl;
    ::exit(2);
  }


  // Now put some stuff on the list.
  //
  cout << "Adding 10 A objects to the StdVectorList...";
  for( int i=0; i<10; i++) {
    l->push_back( new B(i) );
  }
  cout <<  "done." << endl;
  cout << "Length of the list="<< l->size() << endl;


  cout << "Get the list again and print the content" << endl;
  vector<B*> *g= Ifd< std::vector<B*> >::get(dict);
  if (0==g) {
    cout << "Failed to recover std::vector from the dictionary"
         << endl;
    ::exit(2);
  }

  vector<B*>::const_iterator it=g->begin();
  for (it; it!=g->end();it++)
    {
      (*it)->print();
      cout << endl;
    }

//   // Make the ProxyDict to mess around with
//   //
//   IfdSimpleProxyDict *dict = new IfdSimpleProxyDict;
//   dict->print( cout, IfdSimpleProxyDict::noAddresses );

//   // Make a std::vector list and a proxy to hold it.
//   //
//   vector<ListA*> *l       = new vector<ListA*>;

//   IfdStdVectorProxy<ListA*>  *p  = new IfdStdVectorProxy<ListA*> ( l );

//   l = 0;

//   // Stick it in the dictionary
  
//   Ifd< vector<ListA*> >::put( dict, p );
//   dict->print( cout, IfdSimpleProxyDict::noAddresses );

//   // Pull it back out (just to show we can)
//   //
//   l = Ifd< std::vector<ListA*> >::get(dict);
//   if (0==l) {
//     cout << "Failed to recover std::vector from the dictionary"
//          << endl;
//     ::exit(2);
//   }

//   // Now put some stuff on the list.
//   //
//   ListA::numInstances();  cout << endl;
//   cout << "Adding 10 A objects to the StdVectorList...";
//   for( int i=0; i<10; i++) {
//     l->push_back( new ListA );
//   }
//   cout <<  "done." << endl;
//   cout << "Length of the list="<< l->size() << endl;
//   ListA::numInstances(); cout << endl;


  exit(0);

}
