#include "TCalign.h"
#include<cstdio>
#include<ctime>
#include<cmath>
#include<vector>
#include<fstream>
#include<sstream>

TCalign* TCalign::inst = NULL; 

TMatrixT<double> TCalign::eulerMatrix(double theta, double phi, double psi){
      TMatrixT<double> tM(3,3);
      TMatrixT<double> phiM(3,3);
      TMatrixT<double> psiM(3,3);
      /*
        Building euler matrixes using the x-convention:
        temporary is theta and phi switched. ie theta has means the angle phi usualy points to.
        see:
        http://mathworld.wolfram.com/EulerAngles.html
        quote:
        "The so-called "x-convention," illustrated above, is the most common definition. 
        In this convention, the rotation given by Euler angles (phi,theta,psi), 
        where the first rotation is by an angle phi about the z-axis, 
        the second is by an angle theta in [0,pi] about the x-axis, 
        and the third is by an angle psi about the z-axis (again)"
      */
      tM[0][0]=cos(theta);     tM[0][1]=sin(theta);    tM[0][2]=0;
      tM[1][0]=-sin(theta);    tM[1][1]=cos(theta);    tM[1][2]=0;
      tM[2][0]=0;              tM[2][1]=0;             tM[2][2]=1;

      phiM[0][0]=1;            phiM[0][1]=0;           phiM[0][2]=0;
      phiM[1][0]=0;            phiM[1][1]=cos(phi);    phiM[1][2]=sin(phi);
      phiM[2][0]=0;            phiM[2][1]=-sin(phi);   phiM[2][2]=cos(phi);

      psiM[0][0]=cos(psi);     psiM[0][1]=sin(psi);    psiM[0][2]=0;
      psiM[1][0]=-sin(psi);    psiM[1][1]=cos(psi);    psiM[1][2]=0;
      psiM[2][0]=0;            psiM[2][1]=0;           psiM[2][2]=1;
      
      TMatrixT<double> M(3,3);
      M=psiM*(phiM*tM);
      return M;
      
}

TVector3 TCalign::XYZtoUVW(int id,TVector3 xyz){
  if(T[id]==NULL) quit(id);
  if(R[id]==NULL) quit(id);
  TVector3 uvw = xyz-(*(T[id])); //
				 //dereferencing, the minus sign is
				 //the same in my code
  uvw = (*(R[id]))*uvw; //dereferencing then multiplication4
  return uvw;
}

//for momentum vectors
TVector3 TCalign::notranslXYZtoUVW(int id,TVector3 xyz){
  if(R[id]==NULL) quit(id);
  return (*(R[id]))*xyz;
}

TVector3 TCalign::notranslUVWtoXYZ(int id,TVector3 uvw){
  if(R[id]==NULL) quit(id);
  TMatrixT<double> M = (*(R[id]));
  M.T();//Transposing since Tinverse = T-transpose
  return  M*uvw;
}


TVector3 TCalign::UVWtoXYZ(int id,TVector3 uvw){
  if(T[id]==NULL) quit(id);
  if(R[id]==NULL) quit(id);
  TMatrixT<double> M = (*(R[id]));
  M.T();
  TVector3 xyz = M*uvw;
  xyz = xyz + (*(T[id]));
  return xyz;
}

std::vector<int> TCalign::getLoadedIDs(){
    std::vector<int> ids;

    if(Theta.empty()==true){
	return ids;
    }
    for(std::map<int, double*>::const_iterator it = Theta.begin(); it != Theta.end(); ++it)
    {
	ids.push_back(it->first);
	//std::cout<<"id map: "<<it->first<<std::endl;
    }

    return ids;
}

void TCalign::read(std::string filename){
  clear();
  std::ifstream input(filename.c_str());
  char line[200];
  int j=0;
  while(input.getline(line,199)) {
    std::istringstream istr(line);
    std::string s;
    istr >> s;
    if(s.substr(0,1)=="#" || s.substr(0,1)=="") continue;
    int id;
    if(s=="detId") {
      istr >> id;
      input.getline(line,199);
      std::istringstream istr1(line);
      istr1>>s;
      double x,y,z;
      istr1>>x>>y>>z;
//       double M00,M01,M02,M10,M11,M12,M20,M21,M22;   
//       input.getline(line,199); 
//       std::istringstream istr2(line);
//       istr2>>s;
//       istr2>>M00>>M01>>M02;
//       input.getline(line,199);
//       std::istringstream istr3(line);
//       istr3>>s;
//       istr3>>M10>>M11>>M12;
//       input.getline(line,199);
//       std::istringstream istr4(line);
//       istr4>>s;
//       istr4>>M20>>M21>>M22;
      input.getline(line,199);
      std::istringstream istr5(line);
      double pitch;
      istr5>>s;
      istr5>>pitch;
      input.getline(line,199);
      std::istringstream istr6(line);
      double resolution;
      istr6>>s;
      istr6>>resolution;
      input.getline(line,199);
      std::istringstream istr7(line);
      double theta,phi,psi;
      istr7>>s;
      istr7>>theta>>phi>>psi;
      TVector3 trans(x,y,z);

      
      TMatrixT<double> M(3,3);
      M=eulerMatrix(theta,phi,psi);
      /*
        M[0][0]=M00;    M[0][1]=M01;    M[0][2]=M02;
        M[1][0]=M10;    M[1][1]=M11;    M[1][2]=M12;
        M[2][0]=M20;    M[2][1]=M21;    M[2][2]=M22;
      */
      setConv(id,trans,M,pitch,theta,phi,psi,resolution);
    }
  }
}


void TCalign::write(std::string filename){
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  

  FILE *file = fopen (filename.c_str(),"w");
  fprintf(file,"#alignement file written at %s\n",asctime (timeinfo) );

//   std::map<int,TVector3>::iterator iter;   
//   for( iter = T.begin(); iter != T.end(); iter++ ) {
//     std::cout << "word: " << iter->first << ", count: " << iter->second << std::endl;
//   }
  
  std::vector<int> ids;
  std::map<int,TVector3*>::iterator it=T.begin();
  while(it!=T.end()){
    if(it->second != NULL){
      ids.push_back(it->first);
    }
    ++it;
  }

  for(unsigned int i=0;i<ids.size();++i){
    fprintf(file,"#####################################################\n");
    fprintf(file,"detId       %d\n",ids.at(i));
    fprintf(file,"translation %.7E %.7E %.7E\n",T[ids.at(i)]->X(),T[ids.at(i)]->Y(),T[ids.at(i)]->Z());
    fprintf(file,"pitch       %.7E\n",*(P[ids.at(i)]));
    fprintf(file,"Resolution  %.7E\n",*(Resolution[ids.at(i)]));
    fprintf(file,"euler       %.7E %.7E %.7E\n",(*(Theta[ids.at(i)])),(*(Phi[ids.at(i)])),(*(Psi[ids.at(i)])));
  }
  
  fclose(file);
}

void TCalign::clear(){
  std::map<int,TVector3*>::iterator itT=T.begin();
  while(itT!=T.end()){
    delete itT->second;
    ++itT;
  }
  T.clear();
  std::map<int,TMatrixT<double>* >::iterator itR=R.begin();
  while(itR!=R.end()){
    delete itR->second;
    ++itR;
  }
  R.clear();
  std::map<int,double*>::iterator itP=P.begin();
  while(itP!=P.end()){
    delete itP->second;
    ++itP;
  }
  P.clear();
  std::map<int,double*>::iterator itTheta=Theta.begin();
  while(itTheta!=Theta.end()){
    delete itTheta->second;
    ++itTheta;
  }
  Theta.clear();
  std::map<int,double*>::iterator itPhi=Phi.begin();
  while(itPhi!=Phi.end()){
    delete itPhi->second;
    ++itPhi;
  }
  Phi.clear();
  std::map<int,double*>::iterator itPsi=Psi.begin();
  while(itPsi!=Psi.end()){
    delete itPsi->second;
    ++itPsi;
  }
  Psi.clear();
  std::map<int,double*>::iterator itRes=Resolution.begin();
  while(itRes!=Resolution.end()){
    delete itRes->second;
    ++itRes;
  }
  Resolution.clear();
}
