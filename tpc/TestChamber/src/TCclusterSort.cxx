#include"TCclusterSort.h"
#include<cmath>

bool TCclusterSortU::operator()(TCcluster c1, TCcluster c2){
  return c1.posUVW().X() < c2.posUVW().X();
}


bool TCclusterSortUVW::operator()(TCcluster c1, TCcluster c2){
  unsigned char u=0,v=0,w=0;
  static const double eps =1.E-10;
  if(c1.posUVW().X() < (c2.posUVW().X()-eps)){//u1<u2
    u=1;
  }
  else if(c1.posUVW().X() > (c2.posUVW().X()+eps)){//u1>u2
    u=2;
  }
  else {//u1==u2
    u=3;
  }
  if(c1.posUVW().Y() < (c2.posUVW().Y()-eps)){//v1<v2
    v=1;
  }
  else if(c1.posUVW().Y() > (c2.posUVW().Y()+eps)){//v1>v2
    v=2;
  }
  else {//v1==v2
    v=3;
  }
  if(c1.posUVW().Z() < (c2.posUVW().Z()-eps)){//w1<w2
    w=1;
  }
  else if(c1.posUVW().Z() > (c2.posUVW().Z()+eps)){//w1>w2
    w=2;
  }
  else {//w1==w2
    w=3;
  }
  
  if(u==1) return true;
  if(u==2) return false;
  if(u==3){
    if(v==1) return true;
    if(v==2) return false;
    if(v==3) {
      if(w==1) return true;
      if(w==2) return false;
      if(w==3) return false;//this operator does LT not LE	  
    }
  }
}



bool TCclusterSortXYZ::operator()(TCcluster c1, TCcluster c2){
  unsigned char x=0,y=0,z=0;
  static const double eps =1.E-10;
  if(c1.posXYZ().X() < (c2.posXYZ().X()-eps)){//x1<x2
    x=1;
  }
  else if(c1.posXYZ().X() > (c2.posXYZ().X()+eps)){//x1>x2
    x=2;
  }
  else {//x1==x2
    x=3;
  }
  if(c1.posXYZ().Y() < (c2.posXYZ().Y()-eps)){//y1<y2
    y=1;
  }
  else if(c1.posXYZ().Y() > (c2.posXYZ().Y()+eps)){//y1>y2
    y=2;
  }
  else {//y1==y2
    y=3;
  }
  if(c1.posXYZ().Z() < (c2.posXYZ().Z()-eps)){//z1<z2
    z=1;
  }
  else if(c1.posXYZ().Z() > (c2.posXYZ().Z()+eps)){//z1>z2
    z=2;
  }
  else {//z1==z2
    z=3;
  }

  //std::cout << "sortXYZ" << std::endl;
  //c1.posXYZ().Print();
  //c2.posXYZ().Print();
  //std::cout << "result " << x << " " << y << " " << z << std::endl;
  
  if(x==1) return true;
  if(x==2) return false;
  if(x==3){
    if(y==1) return true;
    if(y==2) return false;
    if(y==3) {
      if(z==1) return true;
      if(z==2) return false;
      if(z==3) return false;//this operator does LT not LE	  
    }
  }
}
