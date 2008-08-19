#include "space.h"
#include "dim.h"

space::space()
{
 nNDIM = 0;
}

space::~space()
{
}

void space::AddDim(dim D)
{
 fDIM.push_back(D);
 nNDIM = nNDIM+1;
}

void space::CreateSpace()
{
 hcube box;
 cout<<nNDIM<<endl;
 vector<int> vecInt (nNDIM, 1);
 box.setCoord(vecInt);
 vector<int>::iterator it;
 cout<<vecInt.size()<<endl;
 for ( int it = 0; it < vecInt.size(); it++)
 {
 cout<<"hai"<<it<<endl;
  dim d =  fDIM[it];
  
  for (int j = 1; j <= d.getSEG(); j++)
   {
    cout<<"bai"<<j<<endl;
    vecInt[it] = j;
    box.setCoord(vecInt);
    box.setDummy(it*j);
    fSPACE.push_back(box);
   }
 }
 cout<<fSPACE.size()<<endl;
}


void space::CS()
{
 vector< vector<int> >  myspace;
 dim d = fDIM[1];
 for (int i=1; i <= d.getSEG() ; i++ )
 {
  vector<int> vecInt(nNDIM);
  vecInt[1]=i; 
  vecInt[2]=1;
  myspace.push_back(vecInt);  
 }

 for (int j = 2; j <= nNDIM; j++)
 {
  dim d1 = fDIM[j];
  vector<vector<int> >sub_space;
  sub_apace = myspace;
  for ( int i = 0; i <  len; i++  )
  {
   
 //  for ( int k = 0;  )
   vector<int> vecInt;
   
   vecInt[]
  } 
 }
}

