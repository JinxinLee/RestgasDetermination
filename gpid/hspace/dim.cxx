#include "dim.h"
#include <iostream>
#include <string>
dim::dim()
{
cout<<"new dim"<<endl;
}

dim::~dim()
{
 destroy();
}

dim::dim(string st, int dno,double min,double max,int seg)
{
 nSEG = seg;
 fMIN = min;
 fMAX = max;
 sNAME = st;
 fDNO = dno;
}


inline bool dim::operator==(const dim& other )const
{
 if ( fDNO == other.fDNO ) return true;

 return false;
}

inline bool dim::operator<(const dim& other )const
{
 if ( fDNO < other.fDNO ) return true;

 return false;
}

void  dim::operator= (dim const &other) 
{
  if (this != &other)
  {
    destroy();
    copy(other);
  }

}
/*
  cout<<"inside ="<<endl;
  nSEG = other.nSEG;
  cout<<"inside ="<<endl;
  fMIN = other.fMIN;
  fMAX = other.fMAX;
  fDNO = other.fDNO;
  cout<<"inside ="<<endl;
  sNAME = other.sNAME;
  cout<<"inside ="<<endl;
*/


void dim::print()
{
 cout<<"no of segmentation  :"<<nSEG<<endl;
 cout<<"               Min  :"<<fMIN<<endl;
 cout<<"               Max  :"<<fMAX<<endl;
 cout<<"          dim name  :"<<sNAME<<endl;
 cout<<"            dim no  :"<<nSEG<<endl;
}

void dim::copy( const dim &other)
{
  cout<<"inside copy"<<endl;
//  nSEG = other.nSEG;
  fMIN = other.fMIN;
  fMAX = other.fMAX;
  fDNO = other.fDNO;
  sNAME = other.sNAME;
  cout<<"inside copy"<<endl;
}

void dim::destroy()
{
}


dim::dim(dim const &other)
{
  copy(other); 
}



