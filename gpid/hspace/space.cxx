#include "space.h"
#include "dim.h"

space::space()
{
 nNDIM = 0;
}

space::~space()
{
}

void space::AddDim(const dim D)
{
// int dno = D.getDNO();
// fDIM[dno]=D;
  vector<dim>::iterator it; 
  it = fDIM.end();
  fDIM.insert(it,D);
 nNDIM = nNDIM+1;
}


void space::CS()
{
// vector< vector<int> >  myspace;

 vector<vector <int> >::iterator it_space;
// dim dummy = dim("dummy",1,0,10,10);
// dim d = fDIM[0];
// d.print();
// cout<<d.getSEG()<<endl;
 vector<int> vecInt;
 vecInt.resize(nNDIM);
 vector<int>::iterator it;
// it=vecInt.begin();
 for (int i=1; i <= fDIM[0].getSEG() ; i++ )
 {
  vecInt[0]=i; 
  myspace.push_back(vecInt);
 }
/*
 for (int j = 2; j <= nNDIM; j++)
 {
cout<<"before"<<endl;
//  dim d1 = fDIM[j];
// d1.print();
  int k = fDIM[j].getDNO()-1;
  cout<<"  -I-  This dimension no is "<<fDIM[j].getDNO()<<endl;
cout<<"after"<<endl;

  vector<vector<int> > sub_space;
  sub_space = myspace;
  myspace.clear();
  for (it_space=sub_space.begin() ; it_space < sub_space.end() ; it_space++)
    {
       for (int l = 1; l <= fDIM[j].getSEG(); l++ )
       {
         vecInt = *it_space;
         vecInt[k] = l;      
         myspace.push_back(vecInt);

       }
    }
 }

fSPACE.clear();
  for (it_space = myspace.begin() ; it_space < myspace.end() ; it_space++)
   {
    hbox mybox;
    vecInt = *it_space;
    // cout<<"before insertion"<<endl;
    mybox.setCoord(vecInt);
    fSPACE.insert(pair<vector<int>,hbox>(vecInt,mybox));           
   }

 cout<<"  -I-  No of box in the space "<<myspace.size()<<endl;
 cout<<"  -I-  No of box in the space "<<fSPACE.size()<<endl;
*/
}

void space::print()
{
 cout<<"  -I-  No of dimensions "<<nNDIM<<endl;
 vector<dim>::iterator it;
 for ( it = fDIM.begin() ; it < fDIM.end() ; it++)
  {
  cout<<"hai";
  cout<< it->getDNO()<<endl;
  
  }

}



/*
cout<<" my vector contains:";
for ( it=vecInt.begin() ; it < vecInt.end(); it++ )
    cout << " " << *it;
cout<<endl;
 */
