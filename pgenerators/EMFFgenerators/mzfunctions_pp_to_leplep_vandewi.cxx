//c/c++
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

//ranlux
#include "ranlxs.h"
#include "ranlxd.h"

void ranlxs(float r[], int n);
void ranlxd(double r[], int n);

void rlxs_init(int level, int seed);
void rlxd_init(int level, int seed);


//mz general
#include "mzparameters.h"
#include "mzfunctions.h"

//mz for pp_to_leplep_vandewi
#include "mzfunctions_pp_to_leplep_vandewi.h"

using namespace std;



void mz_pp_to_leplep_vandewi_init(int seed, int particle_flag,
                                  double P, double GE_to_GM,
                                  double cos_theta_min, double cos_theta_max)
{
  /**
   * function mz_pp_to_leplep_vandewi_init
   * 
   * INPUT:
   *
   *  seed            -> RANLUX seed
   *  particle_flag   -> 0=electron, 1=muon, 2=tau
   *  P               -> anti-proton momentum (lab frame) [GeV]
   *  GE_to_GM        -> ratio |G_E|/|G_M|
   *  cos_theta_min   -> cos(theta*) minimum; theta*=angle(lep-,pbar) CMS frame
   *  cos_theta_max   -> cos_theta_max: cos(theta*) maximum
   *
   * OUTPUT:
   *
   *  without output
   *
   *
   * DESCRIPTION:
   *
   *  function  mz_pp_to_leplep_vandewi_init checks the values of all arguments, 
   *  killing the job if at least one value lies outside its allowed range.
   *  In case of tau production, it determines compatibility with the input antiproton momentum,
   *  killing the job if the proton-antiproton center-of-mass energy does not reach the threshold
   *  for tau production. Kinematic information is printed out to the log file. 
   *  RANLUX is initialised.
   *
   */

  double P_PANDA_THRESHOLD=1.5;        //pbar momentum threshold in PANDA
  double P_PANDA_UP=15.0;              //pbar momentum upper limit in PANDA

  double M=mp;             //proton mass

  double E;                //lab frame: pbar energy
  double S;                //center-of-mass energy squared of pbarpsystem
  double sqrt_S;           //center-of-mass energy of pbarpsystem
  double S_min;            //threshold for tau+tau- production: center-of-mass energy squared of pbarpsystem 
  double E_min;            //threshold for tau+tau- production: (lab frame) pbar energy 
  double P_min;            //threshold for tau+tau- production: (lab frame) pbar momentum

  //checking parameter range...

  if( (seed<1)||(seed>2147483647) ){   
   printf("ERROR: RANLUX seed out of range: \n");
   printf("    => RANLUX seed should be contained within [1,2^31-1]\n");
   printf("    => job killed!!! \n");
   printf("\n\n");
   exit(1);
  }


  if( !( ( particle_flag==0)||( particle_flag==1)||( particle_flag==2) ) ){
    printf("ERROR: particle flag out of range: \n");
    printf("    => particle flag should be 0 (=electron), 1 (=muon) or 2 (=tau)\n");
    printf("    => job killed!!! \n");
    printf("\n\n");
    exit(1);
  }


  if(P<0.0){
    printf("ERROR: P (anti-proton momentum) out of range: \n");
    printf("    => P should be a positive real number \n");
    printf("    => job killed!!! \n");
    printf("\n\n");
    exit(1);
  }


  if(P<P_PANDA_THRESHOLD){
    printf("WARNING: P (anti-proton momentum) smaller than PANDA threshold %4.2f GeV \n", 
           P_PANDA_THRESHOLD);
    printf("\n\n");
  }


  if(P>P_PANDA_UP){
    printf("WARNING: P (anti-proton momentum) larger than PANDA upper limit %4.2f GeV \n", 
           P_PANDA_UP);
    printf("\n\n");
  }


  if(GE_to_GM<0.0){
    printf("ERROR: |G_E|/|G_M|  out of range: \n");
    printf("    => |G_E|/|G_M|  should be a positive real number \n");
    printf("    => job killed!!! \n");
    printf("\n\n");
    exit(1);
  }


  if( !( (cos_theta_min>=-1.0)&&(cos_theta_max>cos_theta_min)&&(cos_theta_max<=1.0) ) ){
    printf("ERROR: cos(theta*) range NOT allowed \n");
    printf("    => cos(theta*) range [cos(theta*) min, cos(theta*) max]\n");
    printf("       should be contained within [-1.0,1.0]\n");
    printf("    => job killed!!! \n");
    printf("\n\n");
    exit(1);
  }



  //initial state kinematics


  //antiproton energy
  E=pow((P*P+M*M),0.5);


  //center-of-mass energy squared and center-of-mass energy of pbarpsystem
  S=2.0*M*(M+E);
  sqrt_S=pow(S,0.5);


  //kinematic thershold for tau production
  S_min=4.0*mtau*mtau;
  E_min=S_min/(2.0*M)-M;
  P_min=pow((E_min*E_min-M*M),0.5);


  //check if tau production is possible
  if( (particle_flag==2)&&(S<=S_min) ){
   printf("ERROR: center of mass energy below threshold for tau+tau- production \n");
   printf("    => tau+tau- production requires: \n");
   printf("           center-of-mass energy squared   s > 4.0*m_{tau}^2 = %4.2f GeV^2, i.e. \n", S_min);
   printf("           antiproton energy               E > %4.2f GeV, i.e. \n", E_min);
   printf("           antiproton momentum             P > %4.2f GeV \n", P_min);
   printf("    => job killed!!! \n");
   exit(1);
   printf("\n\n");
  }

  //print out kinematics
  printf("Initial state kinematics\n");
  printf("------------------------\n");
  printf("\n");                                
  printf("=> CM energy square:        S = %5.2f GeV^2 \n", S);
  printf("   CM energy:         sqrt(S) = %5.2f GeV \n", sqrt_S);
  printf("\n\n\n\n");


  //initialize RANLUX
  rlxd_init(1,seed);

}



double mz_pp_to_leplep_vandewi_sigma(int particle_flag, 
                                     double P, double GE, double GM, 
                                     double cos_theta){
  /**
   * function mz_pp_to_leplep_vandewi_sigma
   *
   * INPUT:
   *   particle_flag:  0=electron, 1=muon, 2=tau
   *   P: anti-proton momentum (lab frame) [GeV]
   *   GE: |G_E|
   *   GM: |G_M|
   *   cos_theta: cos(theta*), theta*=angle(lep-,pbar) CMS frame
   *
   *
   * RETURNS:
   *
   *   value of the cross section d\sigma / d cos(theta*) [fb]
   *   for antiproton momentum P, electric and magnetic form factors GE_to_GM 
   *   at the point cos_theta
   *
   */

  double sigma;

  double hc2_units=hc2*1000000000000.0;    //(hbar*c)^2 [fb*GeV^2]

  double PI=acos(-1.0);

  double M=mp;             //proton mass
  double mlepton;          //lepton mass
 
  double E;                //lab frame: pbar energy
  double S;                //center-of-mass-energy squared of pbarpsystem

  double tau;              // q^2/(4M^2)                M: proton mass
  double beta_l;           // sqrt{1-4m^2/s}            m: lepton mass
  double beta_p;           // sqrt{1-4M^2/s}

  double A_1, A_2, A_3, A_4;  //kinematic factors 

  double S_min;   //threshold for tau+tau- production: center-of-mass energy squared of pbarpsystem

  //set the lepton mass
  if(particle_flag==0) mlepton=me;
  if(particle_flag==1) mlepton=mmu;
  if(particle_flag==2) mlepton=mtau;

  //antiproton energy
  E=pow((P*P+M*M),0.5);

  //center-of-mass-energy squared of pbarpsystem
  S=2.0*M*(M+E);

  //kinematic factors
  tau=S/(4.0*M*M);
  beta_l=pow(1.0-4.0*mlepton*mlepton/S,0.5);
  beta_p=pow(1.0-4.0*M*M/S,0.5);

  A_1 = hc2_units*PI*alpha_QED*alpha_QED/2.0;

  A_2 = beta_l/(beta_p*S);

  A_3 = (2.0 -beta_l*beta_l +beta_l*beta_l*cos_theta*cos_theta)*GM*GM;

  A_4 = (1.0/tau)*(1.0 -beta_l*beta_l*cos_theta*cos_theta)*GE*GE;

  //calculate cross section
  sigma = A_1*A_2*(A_3+A_4);

  //return null value for tau production below threshold
  S_min=4.0*mtau*mtau;
  if( (particle_flag==2)&&(S<=S_min) ) sigma=0.0;

  return sigma;
}





double mz_pp_to_leplep_vandewi_sigma_nonorm(int particle_flag, 
                                            double P, double GE_to_GM, 
                                            double cos_theta){
  /**
   * function mz_pp_to_leplep_vandewi_sigma_nonorm
   *
   * INPUT:
   *   particle_flag:  0=electron, 1=muon, 2=tau
   *   P: anti-proton momentum (lab frame) [GeV]
   *   GE_to_GM: ratio |G_E|/|G_M|
   *   cos_theta: cos(theta*), theta*=angle(lep-,pbar) CMS frame
   *
   * RETURNS:
   *
   *   value of the non-normalized cross section d\sigma / d cos(theta*)
   *   for antiproton momentum P and and ratio GE_to_GM at the point cos_theta
   *
   * the non-normalized cross section is
   *
   * d sigma / d cos(theta*) ~ 1 + A*cos(theta*), with
   *
   *  A = beta_l^2[1-R^2/tau] / ( 2-beta_l^2 + R^2/tau ), where
   *
   *  beta_l =  sqrt{1-4m^2/s}            m: lepton mass 
   *  tau    =  q^2/(4M^2)                M: proton mass 
   */

  double sigma;

  double M=mp;             //proton mass
  double mlepton;          //lepton mass
 
  double E;                //lab frame: pbar energy
  double S;                //center-of-mass-energy squared of pbarpsystem

  double tau;              // q^2/(4M^2)                M: proton mass
  double beta_l;           // sqrt{1-4m^2/s}            m: lepton mass
  double R;                // |G_E|/|G_M|
  double A;                // beta_l^2[1-R^2/tau] / ( 2-beta_l^2 + R^2/tau )


  //set the lepton mass
  if(particle_flag==0) mlepton=me;
  if(particle_flag==1) mlepton=mmu;
  if(particle_flag==2) mlepton=mtau;

  //antiproton energy
  E=pow((P*P+M*M),0.5);

  //center-of-mass-energy squared of pbarpsystem
  S=2.0*M*(M+E);

  //kinematic factors
  tau=S/(4.0*M*M);
  beta_l=pow(1.0-4.0*mlepton*mlepton/S,0.5);
  R=GE_to_GM;
  A=( beta_l*beta_l*(1.0-R*R/tau) )/( 2.0-beta_l*beta_l + R*R/tau );  

  //cross section
  sigma = 1.0 +A*cos_theta*cos_theta;

  return sigma;
}





void mz_pp_to_leplep_vandewi_event(int particle_flag, 
                                   double P, double GE_to_GM, 
                                   double cos_theta_min, double cos_theta_max, 
                                   double* lepplus_p, double* lepminus_p)
{
  /**
   * function mz_pp_to_leplep_vandewi_event
   *
   * INPUT:
   *   particle_flag:  0=electron, 1=muon, 2=tau
   *   P: anti-proton momentum (lab frame) [GeV]
   *   GE_to_GM: ratio |G_E|/|G_M|
   *   cos_theta_min: cos(theta*) minimum; theta*=angle(lep-,pbar) CMS frame
   *   cos_theta_max: cos(theta*) maximum
   *
   * OUTPUT:
   *
   *   lepplus_p: 4-momentum lep+ lab frame
   *   lepminus_p: 4-momentum lep- lab frame
   *
   */

  double M=mp;             //proton mass
  double PI=acos(-1.0);

  double p_p[4];           //lab frame: 4-mom p    (proton target)
  double pbar_p[4];        //lab frame: 4-mom pbar (anti-proton beam)
  double pbarp_p[4];       //lab frame: 4-mom pbarpsystem
  double E;                //lab frame: energy pbar
  double S;                //invariant mass square pbarpsystem
  double sqrt_S;           //invariant mass pbarpsystem

  double mlepton;          //lepton mass

  double tau;              // q^2/(4M^2)                M: proton mass
  double beta_l;           // sqrt{1-4m^2/s}            m: lepton mass
  double R;                // |G_E|/|G_M|
  double A;                // beta_l^2[1-R^2/tau] / ( 2-beta_l^2 + R^2/tau ) 

  int flag;
  double x,y;                              
  double max;
  double prob;
  double costheta, theta, phi;

  double lepplus_pcm[4];    //CMS frame: 4-mom lepton+
  double lepminus_pcm[4];   //CMS frame: 4-mom lepton-


  //set the lepton mass
  if(particle_flag==0) mlepton=me;
  if(particle_flag==1) mlepton=mmu;
  if(particle_flag==2) mlepton=mtau;


 //initial state kinematic

  //4-mom of p (lab frame)
  p_p[0]=M;
  p_p[1]=0.;
  p_p[2]=0.;
  p_p[3]=0.;

  //4-mom of pbar (lab frame)
  E=pow((P*P+M*M),0.5);

  pbar_p[0]=E;
  pbar_p[1]=0.;
  pbar_p[2]=0.;
  pbar_p[3]=P;

  //4-mom pbarpsystem (lab frame)
  for(int i=0; i<4; i++){
    pbarp_p[i]=p_p[i]+pbar_p[i];
  }

  //invariant mass pbarpsystem (cms energy)
  S=mzvmod2(4,pbarp_p);
  sqrt_S=pow(S,0.5);


  //get upper bound to non-normalized cross section
  tau=S/(4.0*M*M);
  beta_l=pow(1.0-4.0*mlepton*mlepton/S,0.5);
  R=GE_to_GM;
  A=( beta_l*beta_l*(1.0-R*R/tau) )/( 2.0-beta_l*beta_l + R*R/tau );
  max=1.0 + fabs(A);


  flag=0;

  while(flag==0){
    x=mzrnd(cos_theta_min,cos_theta_max);   //random cos(theta*) in range [cos_theta_min,cos_theta_max]  
    y=mzrnd(0.0,max);                       //random in range [0.0,upper bound to the cross section]
    prob=mz_pp_to_leplep_vandewi_sigma_nonorm(particle_flag,P,GE_to_GM,x); //cross section at generated cos(theta*) 

    if(y<prob){                             //accept event
      flag=1;

      costheta=x;
      if(costheta > 1.0) costheta=1.0;      //just make sure about range...
      if(costheta < -1.0) costheta=-1.0;
      theta=acos(costheta);                 //theta* lepton- in [0,PI]
      phi=mzrnd(0.0,2.0*PI);                //phi lepton-    in [0,2.0*PI]
 
      //build lepton- 4-momentum in CMS frame
      lepminus_pcm[0]=sqrt_S/2.0;
      lepminus_pcm[1]=pow(S/4.0-mlepton*mlepton,0.5)*sin(theta)*cos(phi);
      lepminus_pcm[2]=pow(S/4.0-mlepton*mlepton,0.5)*sin(theta)*sin(phi);
      lepminus_pcm[3]=pow(S/4.0-mlepton*mlepton,0.5)*cos(theta);

      //build lepton+ 4-momentum in CMS frame
      lepplus_pcm[0]=lepminus_pcm[0];
      lepplus_pcm[1]=-1.0*lepminus_pcm[1];
      lepplus_pcm[2]=-1.0*lepminus_pcm[2];
      lepplus_pcm[3]=-1.0*lepminus_pcm[3];

      //boost lep+ and lep- 4-momenta to lab frame
      mzboost(1,pbarp_p,lepplus_pcm,lepplus_p);
      mzboost(1,pbarp_p,lepminus_pcm,lepminus_p);
    }

  }


}




