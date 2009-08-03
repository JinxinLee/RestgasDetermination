#include <TFile.h>
#include <iostream>
#include <TVector3.h>
#include <TMath.h>
#include <TGeoMatrix.h>
void coordinates()
{
  //************************************************************
  //
  //Optical survey coordinates
  //
  //************************************************************
  double tg1P1[4]={-37.8873,35.6630,0,1};
  double tg1P2[4]={-.2745,.0640,0,1};

  double tg2P1[4]={-37.8945,35.6770,0,1};
  double tg2P2[4]={-.3065,.0480,0,1};
  
  TVector3 transTtg1;  
  transTtg1.SetXYZ(tg1P2[0],tg1P2[1],tg1P2[2]);
  TVector3 transTtg2;
  transTtg2.SetXYZ(tg2P2[0],tg2P2[1],tg2P2[2]);
  //making new base
  double __x[3];
  __x[0]=-1;__x[1]=0;__x[2]=0;
  double __y[3];
  __y[0]=0;__y[1]=1;__y[2]=0;
  double __z[3];
  __z[0]=0;__z[1]=0;__z[2]=1;
  double invTransMatTG1[4][4];
  invTransMatTG1[0][0]=__x[0];    invTransMatTG1[0][1]=__y[0];    invTransMatTG1[0][2]=__z[0]; invTransMatTG1[0][3]=transTtg1.x();
  invTransMatTG1[1][0]=__x[1];    invTransMatTG1[1][1]=__y[1];    invTransMatTG1[1][2]=__z[1]; invTransMatTG1[1][3]=transTtg1.y();
  invTransMatTG1[2][0]=__x[2];    invTransMatTG1[2][1]=__y[2];    invTransMatTG1[2][2]=__z[2]; invTransMatTG1[2][3]=transTtg1.z();
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
  invTransMatTG2[0][0]=__x[0];    invTransMatTG2[0][1]=__y[0];    invTransMatTG2[0][2]=__z[0]; invTransMatTG2[0][3]=transTtg2.x();
  invTransMatTG2[1][0]=__x[1];    invTransMatTG2[1][1]=__y[1];    invTransMatTG2[1][2]=__z[1]; invTransMatTG2[1][3]=transTtg2.y();
  invTransMatTG2[2][0]=__x[2];    invTransMatTG2[2][1]=__y[2];    invTransMatTG2[2][2]=__z[2]; invTransMatTG2[2][3]=transTtg2.z();
  invTransMatTG2[3][0]=0;        invTransMatTG2[3][1]=0;        invTransMatTG2[3][2]=0;     invTransMatTG2[3][3]=1;
  TMatrixT<double> transformationTG2(4,4,&invTransMatTG2[0][0]);
  cout<<"determinant "<<transformationTG2.Determinant()<<endl;
  cout<<"***********************************************************************************"<<endl;
  transformationTG2.Print();
  cout<<"***********************************************************************************"<<endl;
  transformationTG2.Invert();
  transformationTG2.Print();
  cout<<"***********************************************************************************"<<endl;
  
   
  vector< TMatrixT<double> > tg1Q;
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
    tg1Q.at(i+4).Print();
    tg2Q.at(i+4).Print();
  }
  //TMatrixT<double> tg1Q[i+4],tg2Q[] i-{1,4}
  // has coordinates of strips
  // q3q4=direction of "y-strips", q1 position of first strip x
  // q1q2=direction of "x-strips", q4 position of first strip x
  //********************************************************

  
  //*************************************************************
  TVector3 scint1(0.104971,0.330293,-0.607805);
  TVector3 scint2(0.0516079,0.334447,1.16535);

  TVector3 sil1p1(0.108478,0.342911,-0.390464);
  TVector3 sil1p2(0.0892391,0.343151,-0.39061);
  TVector3 sil1p3(0.0893495,0.323319,-0.390696);
  TVector3 sil1p4(0.108353,0.323307,-0.390897);

  TVector3 sil2p1(0.0961556,0.342195,0.0521152);
  TVector3 sil2p2(0.0773385,0.342169,0.0521324);
  TVector3 sil2p3(0.0772431,0.322718,0.0521289);
  TVector3 sil2p4(0.096172,0.322429,0.05206);

  TVector3 tg01p1(0.256584,0.452425,-0.473861);
  TVector3 tg01p2(-0.118592,0.0989069,-0.476781);
  TVector3 tg02p1(0.255164,0.453678,0.14172);
  TVector3 tg02p2(-0.122005,0.098039,0.147322);

  TVector3 siBox1p1(0.206856,0.502544,-0.336813);
  TVector3 siBox1p2(-0.00814888,0.502742,-0.33478);
  TVector3 siBox1p3(-0.00824142,0.241433,-0.33799);
  TVector3 siBox1p4(0.207293,0.241879,-0.338691);

  TVector3 siBox2p1(0.194186,0.500668,-0.00287458);
  TVector3 siBox2p2(-0.0197852,0.501919,-0.00216727);
  TVector3 siBox2p3(-0.0213047,0.241135,-0.00062352);
  TVector3 siBox2p4(0.193671,0.241653,-0.000895106);
  //*********************************************************
  //converting from meters to cm
  //*********************************************************
  scint1*=100;
  scint2*=100;

  sil1p1*=100;
  sil1p2*=100;
  sil1p3*=100;
  sil1p4*=100;
  sil2p1*=100;
  sil2p2*=100;
  sil2p3*=100;
  sil2p4*=100;

  tg01p1*=100;
  tg01p2*=100;
  tg02p1*=100;
  tg02p2*=100;

  siBox1p1*=100;
  siBox1p2*=100;
  siBox1p3*=100;
  siBox1p4*=100;

  siBox2p1*=100;
  siBox2p2*=100;
  siBox2p3*=100;
  siBox2p4*=100;

  //***********************************************************************
  double ascint1[3]={0.104971,0.330293,-0.607805};
  double ascint2[3]={0.0516079,0.334447,1.16535};

  double asil1p1[3]={0.108478,0.342911,-0.390464};
  double asil1p2[3]={0.0892391,0.343151,-0.39061};
  double asil1p3[3]={0.0893495,0.323319,-0.390696};
  double asil1p4[3]={0.108353,0.323307,-0.390897};

  double asil2p1[3]={0.0961556,0.342195,0.0521152};
  double asil2p2[3]={0.0773385,0.342169,0.0521324};
  double asil2p3[3]={0.0772431,0.322718,0.0521289};
  double asil2p4[3]={0.096172,0.322429,0.05206};

  double atg01p1[3]={0.256584,0.452425,-0.473861};
  double atg01p2[3]={-0.118592,0.0989069,-0.476781};
  double atg02p1[3]={0.255164,0.453678,0.14172};
  double atg02p2[3]={-0.122005,0.098039,0.147322};

  double asiBox1p1[3]={0.206856,0.502544,-0.336813};
  double asiBox1p2[3]={-0.00814888,0.502742,-0.33478};
  double asiBox1p3[3]={-0.00824142,0.241433,-0.33799};
  double asiBox1p4[3]={0.207293,0.241879,-0.338691};

  double asiBox2p1[3]={0.194186,0.500668,-0.00287458};
  double asiBox2p2[3]={-0.0197852,0.501919,-0.00216727};
  double asiBox2p3[3]={-0.0213047,0.241135,-0.00062352};
  double asiBox2p4[3]={0.193671,0.241653,-0.000895106};
  //**********************************************************************   
  ascint1[0]=100*ascint1[0];  ascint1[1]=100*ascint1[1];  ascint1[2]=100*ascint1[2];
  ascint2[0]=100*ascint2[0];  ascint2[1]=100*ascint2[1];  ascint2[2]=100*ascint2[2];

  asil1p1[0]=100*asil1p1[0];  asil1p1[1]=100*asil1p1[1];  asil1p1[2]=100*asil1p1[2];
  asil1p2[0]=100*asil1p2[0];  asil1p2[1]=100*asil1p2[1];  asil1p2[2]=100*asil1p2[2];
  asil1p3[0]=100*asil1p3[0];  asil1p3[1]=100*asil1p3[1];  asil1p3[2]=100*asil1p3[2];
  asil1p4[0]=100*asil1p4[0];  asil1p4[1]=100*asil1p4[1];  asil1p4[2]=100*asil1p4[2];

  asil2p1[0]=100*asil2p1[0];  asil2p1[1]=100*asil2p1[1];  asil2p1[2]=100*asil2p1[2];
  asil2p2[0]=100*asil2p2[0];  asil2p2[1]=100*asil2p2[1];  asil2p2[2]=100*asil2p2[2];
  asil2p3[0]=100*asil2p3[0];  asil2p3[1]=100*asil2p3[1];  asil2p3[2]=100*asil2p3[2];
  asil2p4[0]=100*asil2p4[0];  asil2p4[1]=100*asil2p4[1];  asil2p4[2]=100*asil2p4[2];

  atg01p1[0]=100*atg01p1[0];  atg01p1[1]=100*atg01p1[1];  atg01p1[2]=100*atg01p1[2];
  atg01p2[0]=100*atg01p2[0];  atg01p2[1]=100*atg01p2[1];  atg01p2[2]=100*atg01p2[2];
  atg02p1[0]=100*atg02p1[0];  atg02p1[1]=100*atg02p1[1];  atg02p1[2]=100*atg02p1[2];
  atg02p2[0]=100*atg02p2[0];  atg02p2[1]=100*atg02p2[1];  atg02p2[2]=100*atg02p2[2];

  asiBox1p1[0]=100*asiBox1p1[0];  asiBox1p1[1]=100*asiBox1p1[1];  asiBox1p1[2]=100*asiBox1p1[2];
  asiBox1p2[0]=100*asiBox1p2[0];  asiBox1p2[1]=100*asiBox1p2[1];  asiBox1p2[2]=100*asiBox1p2[2];
  asiBox1p3[0]=100*asiBox1p3[0];  asiBox1p3[1]=100*asiBox1p3[1];  asiBox1p3[2]=100*asiBox1p3[2];
  asiBox1p4[0]=100*asiBox1p4[0];  asiBox1p4[1]=100*asiBox1p4[1];  asiBox1p4[2]=100*asiBox1p4[2];

  asiBox2p1[0]=100*asiBox2p1[0];  asiBox2p1[1]=100*asiBox2p1[1];  asiBox2p1[2]=100*asiBox2p1[2];
  asiBox2p2[0]=100*asiBox2p2[0];  asiBox2p2[1]=100*asiBox2p2[1];  asiBox2p2[2]=100*asiBox2p2[2];
  asiBox2p3[0]=100*asiBox2p3[0];  asiBox2p3[1]=100*asiBox2p3[1];  asiBox2p3[2]=100*asiBox2p3[2];
  asiBox2p4[0]=100*asiBox2p4[0];  asiBox2p4[1]=100*asiBox2p4[1];  asiBox2p4[2]=100*asiBox2p4[2];

  //**********************************************************************  
  TVector3 siBox1p1p2=siBox1p2-siBox1p1;
  TVector3 siBox1p1p4=siBox1p4-siBox1p1;
  
  TVector3 transT=scint1;
  
  cout<<"***********************************************************************************"<<endl;
  transT.Print();
  cout<<"***********************************************************************************"<<endl;
  //vectors spanning new reference plane. Silicon station one defines it.
  TVector3 newBaseX=-siBox1p1p2*(1/siBox1p1p2.Mag());
  TVector3 newBaseY=-siBox1p1p4*(1/siBox1p1p4.Mag());
  //creating an orthonormal basis for the new reference system
  double _x[3];
  _x[0]=newBaseX.x();_x[1]=newBaseX.y();_x[2]=newBaseX.z();
  double _y[3];
  _y[0]=newBaseY.x();_y[1]=newBaseY.y();_y[2]=newBaseY.z();
  double _z[3];
  TMath::Cross(&_x[0],&_y[0],&_z[0]);//creating z component, is normal to x and y
  TMath::Cross(&_z[0],&_x[0],&_y[0]);//make new y component, to make shure its orthogonal to x and z
  TMath::Normalize(&_x[0]);  TMath::Normalize(&_y[0]);  TMath::Normalize(&_z[0]);
  //inserting it into a geomatrix
  //4x4 matrix containing rotation and translation
  //r11 r12 r13 tx
  //r21 r22 r23 ty
  //r31 r32 r33 tz
  //  0   0   0  1
  // giving the transformation M_vec=T * L_vec
  // with Master as the coordinate system of the given
  // we start with the inverse transform
  double invTransMat[4][4];
  invTransMat[0][0]=_x[0];    invTransMat[0][1]=_y[0];    invTransMat[0][2]=_z[0]; invTransMat[0][3]=transT.x();
  invTransMat[1][0]=_x[1];    invTransMat[1][1]=_y[1];    invTransMat[1][2]=_z[1]; invTransMat[1][3]=transT.y();
  invTransMat[2][0]=_x[2];    invTransMat[2][1]=_y[2];    invTransMat[2][2]=_z[2]; invTransMat[2][3]=transT.z();
  invTransMat[3][0]=0;        invTransMat[3][1]=0;        invTransMat[3][2]=0;     invTransMat[3][3]=1;
  TMatrixT<double> transformation(4,4,&invTransMat[0][0]);
  cout<<"***********************************************************************************"<<endl;
  transformation.Print();
  cout<<"***********************************************************************************"<<endl;
  transformation.Invert();
  transformation.Print();
  cout<<"***********************************************************************************"<<endl;

  double rotMat[3][3];
  rotMat[0][0]=transformation[0][0];  rotMat[0][1]=transformation[0][1];  rotMat[0][2]=transformation[0][2];
  rotMat[1][0]=transformation[1][0];  rotMat[1][1]=transformation[1][1];  rotMat[1][2]=transformation[1][2];
  rotMat[2][0]=transformation[2][0];  rotMat[2][1]=transformation[2][1];  rotMat[2][2]=transformation[2][2];
  transT.SetXYZ(transformation[0][3],transformation[1][3],transformation[2][3]);
  TGeoRotation rot("rot");
  rot.SetMatrix(&rotMat[0][0]);
  rot.Print();
  TMatrixT<double> rotT(3,3,&rotMat[0][0]);
  cout<<"det: "<<rotT.Determinant()<<endl;
  cout<<"***********************************************************************************"<<endl;
  TGeoTranslation trans(transT.x(),transT.y(),transT.z());
  TGeoCombiTrans geoTransformation(trans,rot);
  //checking the transformation, local to master should give the zero vector  
  double bla[3]={999,999,999};
  geoTransformation.MasterToLocal(ascint1,bla);
  cout<<"transform Master to local"<<endl;
  cout<<bla[0]<<", "<<bla[1]<<", "<<bla[2]<<endl;
  bla[0]=999;bla[1]=999;bla[2]=999;
  geoTransformation.LocalToMaster(ascint1,bla);
  cout<<"transform local to master"<<endl;
  cout<<bla[0]<<", "<<bla[1]<<", "<<bla[2]<<endl;
  cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
  
  
  //TMatrixT<double> tg1Q[i+4],tg2Q[] i-{1,4} relative to p2
  // has coordinates of strips
  // q4q3=direction of "x-strips", q4 position of first strip x
  // q1q2=direction of "y-strips", q1 position of first strip y
  //********************************************************
  //transforming the positions of the detectors to my new frame of reference.
  double gem1p1[3];
  double gem1p2[3];
  double gem2p1[3];
  double gem2p2[3];

  double si1p1[3];
  double si1p2[3];
  double si1p3[3];
  double si1p4[3];

  double si2p1[3];
  double si2p2[3];
  double si2p3[3];
  double si2p4[3];

  double _scint1[3];
  double _scint2[3];


  geoTransformation.LocalToMaster(atg01p1,gem1p1);
  geoTransformation.LocalToMaster(atg01p2,gem1p2);
  geoTransformation.LocalToMaster(atg02p1,gem2p1);
  geoTransformation.LocalToMaster(atg02p2,gem2p2);

  geoTransformation.LocalToMaster(asil1p1,si1p1);
  geoTransformation.LocalToMaster(asil1p2,si1p2);
  geoTransformation.LocalToMaster(asil1p3,si1p3);
  geoTransformation.LocalToMaster(asil1p4,si1p4);

  geoTransformation.LocalToMaster(asil2p1,si2p1);
  geoTransformation.LocalToMaster(asil2p2,si2p2);
  geoTransformation.LocalToMaster(asil2p3,si2p3);
  geoTransformation.LocalToMaster(asil2p4,si2p4);

  geoTransformation.LocalToMaster(ascint1,_scint1);
  geoTransformation.LocalToMaster(ascint2,_scint2);

  cout<<"*******************************************************"<<endl;
  cout<<"***** GEM01 ******"<<endl;
  cout<<gem1p1[0]<<", "<<gem1p1[1]<<", "<<gem1p1[2]<<endl;
  cout<<gem1p2[0]<<", "<<gem1p2[1]<<", "<<gem1p2[2]<<endl;
  cout<<gem1p1[0]-gem1p2[0]<<", "<<gem1p1[1]-gem1p2[1]<<", "<<gem1p1[2]-gem1p2[2]<<endl;
  cout<<"*******************************************************"<<endl;
  cout<<"***** GEM02 ******"<<endl;
  cout<<gem2p1[0]<<", "<<gem2p1[1]<<", "<<gem2p1[2]<<endl;
  cout<<gem2p2[0]<<", "<<gem2p2[1]<<", "<<gem2p2[2]<<endl;
  cout<<gem2p1[0]-gem2p2[0]<<", "<<gem2p1[1]-gem2p2[1]<<", "<<gem2p1[2]-gem2p2[2]<<endl;

  cout<<"*******************************************************"<<endl;
  cout<<"***** SI01 ******"<<endl;
  cout<<"p1: "<<si1p1[0]<<", "<<si1p1[1]<<", "<<si1p1[2]<<endl;
  cout<<"p2: "<<si1p2[0]<<", "<<si1p2[1]<<", "<<si1p2[2]<<endl;
  cout<<"p3: "<<si1p3[0]<<", "<<si1p3[1]<<", "<<si1p3[2]<<endl;
  cout<<"p4: "<<si1p4[0]<<", "<<si1p4[1]<<", "<<si1p4[2]<<endl;

  cout<<"*******************************************************"<<endl;
  cout<<"***** SI02 ******"<<endl;
  cout<<"p1: "<<si2p1[0]<<", "<<si2p1[1]<<", "<<si2p1[2]<<endl;
  cout<<"p2: "<<si2p2[0]<<", "<<si2p2[1]<<", "<<si2p2[2]<<endl;
  cout<<"p3: "<<si2p3[0]<<", "<<si2p3[1]<<", "<<si2p3[2]<<endl;
  cout<<"p4: "<<si2p4[0]<<", "<<si2p4[1]<<", "<<si2p4[2]<<endl;

  cout<<(si2p3[0]+si2p1[0])/2<<", "<<(si2p3[1]+si2p1[1])/2<<", "<<(si2p3[2]+si2p1[2])/2<<endl;
  cout<<(si2p2[0]+si2p4[0])/2<<", "<<(si2p2[1]+si2p4[1])/2<<", "<<(si2p2[2]+si2p4[2])/2<<endl;
  //
  //Making transformations for SI1
  //
  //************* SI1X *******************
  //taking the middle of SI1 as origin of si1x
  TGeoTranslation si1xTrans("si1xTrans", (si1p2[0]+si1p4[0])/2,(si1p2[1]+si1p4[1])/2,(si1p2[2]+si1p4[2])/2);
  //taking the si1p3p4 as th u vector and p3p2 as the v vector
  double _u[3];
  _u[0]=si1p4[0]-si1p3[0];  _u[1]=si1p4[1]-si1p3[1];  _u[2]=si1p4[2]-si1p3[2];
  double _v[3];
  _v[0]=si1p2[0]-si1p3[0];  _v[1]=si1p2[1]-si1p3[1];  _v[2]=si1p2[2]-si1p3[2];
  double _w[3];
  TMath::Cross(&_u[0],&_v[0],&_w[0]);//creating w component, is normal to u and v
  TMath::Cross(&_w[0],&_u[0],&_v[0]);//make new y component, to make shure its orthogonal to x and z
  TMath::Normalize(&_u[0]);  TMath::Normalize(&_v[0]);  TMath::Normalize(&_w[0]);
  double _rotmat[3][3];
  _rotmat[0][0]=_u[0];  _rotmat[0][1]=_v[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_u[1];  _rotmat[1][1]=_v[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_u[2];  _rotmat[2][1]=_v[2];  _rotmat[2][2]=_w[2];
  TGeoRotation si1xRot("si1xRot");
  si1xRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans si1xCombiTrans(si1xTrans,si1xRot);
  si1xCombiTrans.SetName("si1xCombiTrans");
  cout<<"*******************************************************"<<endl;
  si1xCombiTrans.Print();
  //************* SI1Y *******************
  //taking midle of si1x + w translation of 0.4 cm as center of si1y
  TGeoTranslation si1yTrans("si1yTrans", (si1p2[0]+si1p4[0])/2+0.4*_w[0],(si1p2[1]+si1p4[1])/2+0.4*_w[1],(si1p2[2]+si1p4[2])/2+0.4*_w[2]);
  //using same rotation matrix as si1x with _u=_v and _v=-_u 
  _rotmat[0][0]=_v[0];  _rotmat[0][1]=-_u[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_v[1];  _rotmat[1][1]=-_u[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_v[2];  _rotmat[2][1]=-_u[2];  _rotmat[2][2]=_w[2];
  TGeoRotation si1yRot("si1yRot");
  si1yRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans si1yCombiTrans(si1yTrans,si1yRot);  //
  si1yCombiTrans.SetName("si1yCombiTrans");
  cout<<"*******************************************************"<<endl;
  si1yCombiTrans.Print();
  //Making transformations for SI2x
  //************* SI2X *******************
  //taking the middle of SI2 as origin of si2x
  TGeoTranslation si2xTrans("si2xTrans", (si2p2[0]+si2p4[0])/2,(si2p2[1]+si2p4[1])/2,(si2p2[2]+si2p4[2])/2);
  //taking the si2p3p4 as th u vector and p3p2 as the v vector
  _u[0]=si2p4[0]-si2p3[0];  _u[1]=si2p4[1]-si2p3[1];  _u[2]=si2p4[2]-si2p3[2];
  _v[0]=si2p2[0]-si2p3[0];  _v[1]=si2p2[1]-si2p3[1];  _v[2]=si2p2[2]-si2p3[2];
  TMath::Cross(&_u[0],&_v[0],&_w[0]);//creating w component, is normal to u and v
  TMath::Cross(&_w[0],&_u[0],&_v[0]);//make new y component, to make shure its orthogonal to x and z
  TMath::Normalize(&_u[0]);  TMath::Normalize(&_v[0]);  TMath::Normalize(&_w[0]);
  _rotmat[0][0]=_u[0];  _rotmat[0][1]=_v[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_u[1];  _rotmat[1][1]=_v[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_u[2];  _rotmat[2][1]=_v[2];  _rotmat[2][2]=_w[2];
  TGeoRotation si2xRot("si2xRot");
  si2xRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans si2xCombiTrans(si2xTrans,si2xRot);
  si2xCombiTrans.SetName("si2xCombiTrans");
  cout<<"*******************************************************"<<endl;
  si2xCombiTrans.Print();
  //************* SI2Y *******************
  //taking midle of si2x + w translation of 0.4 cm as center of si2y
  TGeoTranslation si2yTrans("si2yTrans", (si2p2[0]+si2p4[0])/2+0.4*_w[0],(si2p2[1]+si2p4[1])/2+0.4*_w[1],(si2p2[2]+si2p4[2])/2+0.4*_w[2]);
  //using same rotation matrix as si2x with _u=_v and _v=-_u 
  _rotmat[0][0]=_v[0];  _rotmat[0][1]=-_u[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_v[1];  _rotmat[1][1]=-_u[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_v[2];  _rotmat[2][1]=-_u[2];  _rotmat[2][2]=_w[2];
  TGeoRotation si2yRot("si2yRot");
  si2yRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans si2yCombiTrans(si2yTrans,si2yRot);
  si2yCombiTrans.SetName("si2yCombiTrans");
  cout<<"*******************************************************"<<endl;
  si2yCombiTrans.Print();
  //Making transformations for GEM1
  //************** GEM1x ****************
  //
  //u=q4q3
  _u[0]=tg1Q[6][0][0]-tg1Q[7][0][0];  _u[1]=tg1Q[6][1][0]-tg1Q[7][1][0];  _u[2]=tg1Q[6][2][0]-tg1Q[7][2][0];
  _v[0]=tg1Q[5][0][0]-tg1Q[4][0][0];  _v[1]=tg1Q[5][1][0]-tg1Q[4][1][0];  _v[2]=tg1Q[5][2][0]-tg1Q[4][2][0];
  TMath::Cross(&_u[0],&_v[0],&_w[0]);//creating w component, is normal to u and v
  TMath::Cross(&_w[0],&_u[0],&_v[0]);//make new y component, to make shure its orthogonal to x and z
  TMath::Normalize(&_u[0]);  TMath::Normalize(&_v[0]);  TMath::Normalize(&_w[0]);
  _rotmat[0][0]=_u[0];  _rotmat[0][1]=_v[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_u[1];  _rotmat[1][1]=_v[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_u[2];  _rotmat[2][1]=_v[2];  _rotmat[2][2]=_w[2];
  TGeoRotation gm1xRot("gm1xRot");
  gm1xRot.SetMatrix(&_rotmat[0][0]);
  TGeoTranslation gm1xTrans("gm1xTrans", gem1p2[0]+tg1Q[7][0][0]+5.12*_u[0],gem1p2[1]+tg1Q[7][1][0]+5.12*_u[1],gem1p2[2]+tg1Q[7][2][0]+5.12*_u[2]);
  TGeoCombiTrans gm1xCombiTrans(gm1xTrans,gm1xRot);
  gm1xCombiTrans.SetName("gm1xCombiTrans");
  cout<<"*******************************************************"<<endl;
  gm1xCombiTrans.Print();
  //************* GM1Y *******************
  //using same rotation matrix as gm1x with _u=_v and _v=-_u 
  //using same translation vector
  _rotmat[0][0]=_v[0];  _rotmat[0][1]=-_u[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_v[1];  _rotmat[1][1]=-_u[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_v[2];  _rotmat[2][1]=-_u[2];  _rotmat[2][2]=_w[2];
  TGeoRotation gm1yRot("gm1yRot");
  gm1yRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans gm1yCombiTrans(gm1xTrans,gm1yRot);
  gm1yCombiTrans.SetName("gm1yCombiTrans");
  cout<<"*******************************************************"<<endl;
  gm1yCombiTrans.Print();
  //Making transformations for GEM2
  //************** GEM2x ****************
  //
  //u=q4q3
  _u[0]=tg2Q[6][0][0]-tg2Q[7][0][0];  _u[1]=tg2Q[6][1][0]-tg2Q[7][1][0];  _u[2]=tg2Q[6][2][0]-tg2Q[7][2][0];
  _v[0]=tg2Q[5][0][0]-tg1Q[4][0][0];  _v[1]=tg2Q[5][1][0]-tg1Q[4][1][0];  _v[2]=tg2Q[5][2][0]-tg1Q[4][2][0];
  TMath::Cross(&_u[0],&_v[0],&_w[0]);//creating w component, is normal to u and v
  TMath::Cross(&_w[0],&_u[0],&_v[0]);//make new y component, to make shure its orthogonal to x and z
  TMath::Normalize(&_u[0]);  TMath::Normalize(&_v[0]);  TMath::Normalize(&_w[0]);
  _rotmat[0][0]=_u[0];  _rotmat[0][1]=_v[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_u[1];  _rotmat[1][1]=_v[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_u[2];  _rotmat[2][1]=_v[2];  _rotmat[2][2]=_w[2];
  TGeoRotation gm2xRot("gm2xRot");
  gm2xRot.SetMatrix(&_rotmat[0][0]);
  TGeoTranslation gm2xTrans("gm2xTrans", gem2p2[0]+tg2Q[7][0][0]+5.12*_u[0],gem2p2[1]+tg2Q[7][1][0]+5.12*_u[1],gem2p2[2]+tg2Q[7][2][0]+5.12*_u[2]);
  TGeoCombiTrans gm2xCombiTrans(gm2xTrans,gm2xRot);
  gm2xCombiTrans.SetName("gm2xCombiTrans");
  cout<<"*******************************************************"<<endl;
  gm2xCombiTrans.Print();
  //************* GEM@Y *******************
  //using same rotation matrix as gm2x with _u=_v and _v=-_u 
  //using same translation vector
  _rotmat[0][0]=_v[0];  _rotmat[0][1]=-_u[0];  _rotmat[0][2]=_w[0];
  _rotmat[1][0]=_v[1];  _rotmat[1][1]=-_u[1];  _rotmat[1][2]=_w[1];
  _rotmat[2][0]=_v[2];  _rotmat[2][1]=-_u[2];  _rotmat[2][2]=_w[2];
  TGeoRotation gm2yRot("gm2yRot");
  gm2yRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans gm2yCombiTrans(gm2xTrans,gm2yRot);
  gm2yCombiTrans.SetName("gm2yCombiTrans");
  cout<<"*******************************************************"<<endl;
  gm2yCombiTrans.Print();
  TGeoTranslation scint1Trans("scint1Trans",_scint1[0],_scint1[1],_scint1[2]);
  TGeoTranslation scint2Trans("scint2Trans",_scint2[0],_scint2[1],_scint2[2]);
  _rotmat[0][0]=TMath::Sqrt(2)/2;   _rotmat[0][1]=TMath::Sqrt(2)/2;  _rotmat[0][2]=0;
  _rotmat[1][0]=-TMath::Sqrt(2)/2;  _rotmat[1][1]=TMath::Sqrt(2)/2;  _rotmat[1][2]=0;
  _rotmat[2][0]=0;                  _rotmat[2][1]=0;                 _rotmat[2][2]=1;
  TGeoRotation scintRot("scintRot");
  scintRot.SetMatrix(&_rotmat[0][0]);
  TGeoCombiTrans scint1CombiTrans(scint1Trans,scintRot);
  scint1CombiTrans.SetName("scint1Combitrans");
  TGeoCombiTrans scint2CombiTrans(scint2Trans,scintRot);
  scint2CombiTrans.SetName("scint2Combitrans");
 
 TFile* file = new TFile("transformations.root","RECREATE");
  scint1Trans.Write();
  scint2Trans.Write();
  scintRot.Write();
  scint1CombiTrans.Write();
  scint2CombiTrans.Write();
    
  si1xTrans.Write();
  si1yTrans.Write();
  si2xTrans.Write();
  si2yTrans.Write();

  gm1xTrans.Write();
  //gm1yTrans.Write();
  gm2xTrans.Write();
  //  gm2yTrans.Write();
  si1xRot.Write();
  si1yRot.Write();
  si2xRot.Write();
  si2yRot.Write();

  gm1xRot.Write();
  gm1yRot.Write();
  gm2xRot.Write();
  gm2yRot.Write();

  si1xCombiTrans.Write();
  si1yCombiTrans.Write();
  si2xCombiTrans.Write();
  si2yCombiTrans.Write();

  gm1xCombiTrans.Write();
  gm1yCombiTrans.Write();
  gm2xCombiTrans.Write();
  gm2yCombiTrans.Write();
  file->Close();
  delete file;
}
