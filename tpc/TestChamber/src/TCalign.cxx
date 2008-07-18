#include "TCalign.h"
#include<cstdio>
#include<ctime>
#include<vector>
#include<fstream>
#include<sstream>

TCalign* TCalign::inst = NULL; 



TVector3 TCalign::XYZtoUVW(int id,TVector3 xyz){
  if(T[id]==NULL) quit(id);
  if(R[id]==NULL) quit(id);
  TVector3 uvw = xyz-(*(T[id]));
  uvw = (*(R[id]))*uvw;
  return uvw;
}

TVector3 TCalign::dirXYZtoUVW(int id,TVector3 xyz){
  if(R[id]==NULL) quit(id);
  TVector3 uvw = (*(R[id]))*xyz;
  return uvw;
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
      double M00,M01,M02,M10,M11,M12,M20,M21,M22;   
      input.getline(line,199); 
      std::istringstream istr2(line);
      istr2>>s;
      istr2>>M00>>M01>>M02;
      input.getline(line,199);
      std::istringstream istr3(line);
      istr3>>s;
      istr3>>M10>>M11>>M12;
      input.getline(line,199);
      std::istringstream istr4(line);
      istr4>>s;
      istr4>>M20>>M21>>M22;
      input.getline(line,199);
      std::istringstream istr5(line);
      double pitch;
      istr5>>s;
      istr5>>pitch;
      TVector3 trans(x,y,z);
      TMatrixT<double> M(3,3);
      M[0][0]=M00;    M[0][1]=M01;    M[0][2]=M02;
      M[1][0]=M10;    M[1][1]=M11;    M[1][2]=M12;
      M[2][0]=M20;    M[2][1]=M21;    M[2][2]=M22;
      setConv(id,trans,M,pitch);
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
    fprintf(file,"detId       %d\n",ids.at(i));
    fprintf(file,"translation %.7E %.7E %.7E\n",T[ids.at(i)]->X(),T[ids.at(i)]->Y(),T[ids.at(i)]->Z());
    fprintf(file,"rotationL1  %.7E %.7E %.7E\n",(*R[ids.at(i)])[0][0],(*R[ids.at(i)])[0][1],(*R[ids.at(i)])[0][2]);
    fprintf(file,"rotationL2  %.7E %.7E %.7E\n",(*R[ids.at(i)])[1][0],(*R[ids.at(i)])[1][1],(*R[ids.at(i)])[1][2]);
    fprintf(file,"rotationL3  %.7E %.7E %.7E\n",(*R[ids.at(i)])[2][0],(*R[ids.at(i)])[2][1],(*R[ids.at(i)])[2][2]);
    fprintf(file,"pitch       %.7E\n",*(P[ids.at(i)]));
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
}
