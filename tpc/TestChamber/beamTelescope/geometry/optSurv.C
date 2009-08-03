#include <TVector3.h>
#include <iostream>
void optSurv()
{
  double tg1P1[4]={-37.8873,35.6630,0,1};
  double tg1P2[4]={-.2745,.0640,0,1};

  double tg2P1[4]={-37.8945,35.6770,0,1};
  double tg2P2[4]={-.3065,.0480,0,1};
  
  TVector3 transTtg1;  
  transTtg1.SetXYZ(tg1P2[0],tg1P2[1],tg1P2[2]);
  TVector3 transTtg2;
  transTtg2.SetXYZ(tg2P2[0],tg2P2[1],tg2P2[2]);
  //making new base
  double _x[3];
  _x[0]=-1;_x[1]=0;_x[2]=0;
  double _y[3];
  _y[0]=0;_y[1]=1;_y[2]=0;
  double _z[3];
  _z[0]=0;_z[1]=0;_z[2]=1;
  double invTransMatTG1[4][4];
  invTransMatTG1[0][0]=_x[0];    invTransMatTG1[0][1]=_y[0];    invTransMatTG1[0][2]=_z[0]; invTransMatTG1[0][3]=transTtg1.x();
  invTransMatTG1[1][0]=_x[1];    invTransMatTG1[1][1]=_y[1];    invTransMatTG1[1][2]=_z[1]; invTransMatTG1[1][3]=transTtg1.y();
  invTransMatTG1[2][0]=_x[2];    invTransMatTG1[2][1]=_y[2];    invTransMatTG1[2][2]=_z[2]; invTransMatTG1[2][3]=transTtg1.z();
  invTransMatTG1[3][0]=0;        invTransMatTG1[3][1]=0;        invTransMatTG1[3][2]=0;     invTransMatTG1[3][3]=1;
  TMatrixT<double> transformationTG1(4,4,&invTransMatTG1[0][0]);
  cout<<"determinant "<<transformationTG1.Determinant()<<endl;
  cout<<"***********************************************************************************"<<endl;
  transformationTG1.Print();
  cout<<"***********************************************************************************"<<endl;
  transformationTG1.Invert();
  transformationTG1.Print();
  cout<<"***********************************************************************************"<<endl;
  
  double invTransMatTG2[4][4];
  invTransMatTG2[0][0]=_x[0];    invTransMatTG2[0][1]=_y[0];    invTransMatTG2[0][2]=_z[0]; invTransMatTG2[0][3]=transTtg2.x();
  invTransMatTG2[1][0]=_x[1];    invTransMatTG2[1][1]=_y[1];    invTransMatTG2[1][2]=_z[1]; invTransMatTG2[1][3]=transTtg2.y();
  invTransMatTG2[2][0]=_x[2];    invTransMatTG2[2][1]=_y[2];    invTransMatTG2[2][2]=_z[2]; invTransMatTG2[2][3]=transTtg2.z();
  invTransMatTG2[3][0]=0;        invTransMatTG2[3][1]=0;        invTransMatTG2[3][2]=0;     invTransMatTG2[3][3]=1;
  TMatrixT<double> transformationTG2(4,4,&invTransMatTG2[0][0]);
  cout<<"determinant "<<transformationTG2.Determinant()<<endl;
  cout<<"***********************************************************************************"<<endl;
  transformationTG2.Print();
  cout<<"***********************************************************************************"<<endl;
  transformationTG2.Invert();
  transformationTG2.Print();
  cout<<"***********************************************************************************"<<endl;
  
   
  std::vector< TMatrixT<double> > tg1Q;
  double tmp[4]={-22.7065,17.2470,0,1};
  tg1Q.push_back(TMatrixT<double> (4,1,&tmp[0] ));
  double tmp2[4]={-22.6895,27.4520,0,1};
  tg1Q.push_back(TMatrixT<double> (4,1,&tmp2[0] ));
  double tmp3[4]={-27.7796,22.3760,0,1};
  tg1Q.push_back(TMatrixT<double> (4,1,&tmp3[0] ));
  double tmp4[4]={-17.5856,22.3560,0,1};
  tg1Q.push_back(TMatrixT<double> (4,1,&tmp4[0] ));

  vector< TMatrixT<double> > tg2Q;
  double tmp21[4]={-22.7165,17.3199,0,1};
  tg2Q.push_back(TMatrixT<double> (4,1,&tmp21[0] ));
  double tmp22[4]={-22.7126,27.5270,0,1};
  tg2Q.push_back(TMatrixT<double> (4,1,&tmp22[0] ));
  double tmp23[4]={-27.7965,22.4400,0,1};
  tg2Q.push_back(TMatrixT<double> (4,1,&tmp23[0] ));
  double tmp24[4]={-17.6035,22.4370,0,1};
  tg2Q.push_back(TMatrixT<double> (4,1,&tmp24[0] ));


  TMatrixT<double> tg1P1M(4,1,&tg1P1[0]);
  TMatrixT<double> tg1P2M(4,1,&tg1P2[0]);
  TMatrixT<double> tg2P1M(4,1,&tg2P1[0]);
  TMatrixT<double> tg2P2M(4,1,&tg2P2[0]);
  cout<<"*************************************************************************************"<<endl;
  (transformationTG1*tg1P1M).Print();
  (transformationTG1*tg1P2M).Print();
  (transformationTG2*tg2P1M).Print();
  (transformationTG2*tg2P2M).Print();
  cout<<"*************************************************************************************"<<endl;
  //**************************************
  //
  //New coordinates stored in i + 4
  //
  //*****************************************
  for(int i=0;i<4;++i){
    tg1Q.push_back(transformationTG1*tg1Q.at(i));
    tg2Q.push_back(transformationTG2*tg2Q.at(i));
    cout<<"*** Q"<<i<<" *****************************"<<endl;
    // tg1Q.at(i+4).Print();
    //tg2Q.at(i+4).Print();
  }
}
