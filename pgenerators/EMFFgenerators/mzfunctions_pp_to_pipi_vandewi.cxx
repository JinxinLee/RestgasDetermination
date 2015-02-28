/**
 * @author Manuel Zambrana <zambrana@kph.uni-mainz.de> and Dmitry Khaneftd <khaneftd@kph.uni-mainz.de>
 *
 * Mainz, May 2011
**/

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

//mz for pp_to_pipi_vandewi
#include "pipisigmas.h"
#include "mzfunctions_pp_to_pipi_vandewi.h"

using namespace std;

void mz_pp_to_pipi_vandewi_init(int seed, double P, 
                                double cos_theta_min,
                                double cos_theta_max)
{

  double p_a=pz[0];        //momentum range
  double p_b=pz[19];
  double p_c=pz[20];
  double p_d=pz[38];

  double P_PANDA_THRESHOLD=1.5;    //pbar momentum threshold in PANDA

  double M=mp;             //proton mass

  double p_p[4];           //lab frame: 4-mom p    (proton target)
  double pbar_p[4];        //lab frame: 4-mom pbar (anti-proton beam)
  double pbarp_p[4];       //lab frame: 4-mom pbarpsystem
  double E;                //lab frame: energy pbar
  double S;                //invariant mass square pbarpsystem
  double sqrt_S;           //invariant mass pbarpsystem

  int p_flag;              //momentum extrapolation flag
  int i_min;               //lower site in p-lattice for input momentum P

  //checking parameter range...
  printf("\n\n");
  printf("###########################################################\n");

  if( !( (P>=p_a)&&(P<=p_d) ) ){
    printf("ERROR: P (anti-proton momentum) out of range \n");
    printf("    => P should be contained in the interval [%4.2f,%5.2f] GeV\n",p_a,p_d);
    printf("\n\n");
    exit(1);
  }


  if(P<P_PANDA_THRESHOLD){
    printf("WARNING: P (anti-proton momentum) smaller than PANDA threshold %4.2f GeV \n", P_PANDA_THRESHOLD);
    printf("\n\n");
  }


  if( !( (cos_theta_min>=-1.0)&&(cos_theta_max>cos_theta_min)&&(cos_theta_max<=1.0) ) ){
    printf("ERROR: cos(theta*) range NOT allowed \n");
    printf("    => cos(theta*) range [cos(theta*) min, cos(theta*) max]\n");
    printf("       should be contained within [-1.0,1.0]\n");
    printf("\n\n");
    exit(1);
  }


  if( ((P>=p_a)&&(P<p_c))&&( (cos_theta_min<-0.94)||(cos_theta_max>0.94)) ){
    printf("WARNING: in the momentum range [%4.2f,%5.2f) GeV,\n",p_a,p_c);
    printf("         it is recommended to keep cos(theta*) range within the interval [-0.94,0.94]\n");
    printf("         to avoid potentially large extrapolations in the cross section\n");
    printf("\n\n");
  }



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

  //print out kinematics
  printf("Initial state kinematics\n");
  printf("------------------------\n");
  printf("\n");                                
  printf("=> CM energy square:        S = %5.2f GeV^2 \n", S);
  printf("   CM energy:         sqrt(S) = %5.2f GeV \n", sqrt_S);
  printf("\n\n\n\n");



  //determining kinematic range and cross section parametrization...

  printf("Kinematic regime and cross section parametrization\n");
  printf("--------------------------------------------------\n");
  printf("\n");

  if( (P>=p_a)&&(P<=p_b)  ){
    printf("%5.2f < P < %5.2f GeV => low energy regime\n", p_a, p_b);
    printf("cross section parametrization: Legendre polynomial series\n");
    printf("\n\n");
  }

  if( (P>p_b)&&(P<p_c)  ){
    printf("%5.2f < P < %5.2f GeV => GAP energy regime\n", p_b, p_c);
    printf("cross section parametrization: extrapolation Legendre polynomial series / Regge description\n");
    printf("\n\n");
  }

  if( (P>=p_c)&&(P<=pz[24])  ){
    printf("%5.2f < P < %5.2f GeV => transition energy regime\n", p_c, pz[24]);
    printf("cross section parametrization: Regge description\n");
    printf("\n\n");
  }


  if( (P>pz[24])&&(P<=p_d)  ){
    printf("%5.2f < P < %5.2f GeV => high energy regime\n", pz[24],p_d);
    printf("cross section parametrization: Regge description\n");
    printf("\n\n");
  }


  //determining if some cross section extrapolation is needed...

  mz_pp_to_pipi_get_p_lattice_site(P, i_min, p_flag);


  if(p_flag==0){
    printf("P at lattice site: no cross section extrapolation needed \n");
    printf("\n\n\n\n");
  }

  if(p_flag==1){
    printf("P NOT at lattice site: extrapolating cross section between momenta %5.2f and %5.2f GeV\n", pz[i_min], pz[i_min+1]);
    printf("\n\n\n\n");
  }

  printf("###########################################################\n");
  printf("\n\n");

  //initialize RANLUX
  rlxd_init(1,seed);
}

void mz_pp_to_pipi_get_p_lattice_site(double P, int& i_min, int& iflag)
{
  /**
   * function mz_pp_to_pipi_get_p_lattice_site
   *
   * INPUT:
   * 
   *  P -> pbar momentum lab frame [GeV]
   *
   * OUTPUT:
   *
   *  i_min   -> lower site in momentum lattice for input momentum P
   *  iflag   -> momentum extrapolation flag
   *             iflag=0: input momentum P at lattice site, i.e. no extrapolation needed
   *             iflag=1: input momentum P NOT at lattice site, i.e. extrapolation needed
   *
   */

  for(int i=0; i < dim_pz; i++){
    if(P==pz[i]){ 
      iflag=0;
      i_min=i;
    }
    if(i<(dim_pz-1)){      
      if( (P>pz[i])&&(P<pz[i+1])  ){ 
        iflag=1;
        i_min=i;
      }
    }
  }

}

void mz_pp_to_pipi_get_costheta_lattice_site(double cos_theta, int& k_min, int& kflag)
{
  /**
   * function mz_pp_to_pipi_get_costheta_lattice_site
   *
   * INPUT:
   *
   *  cos_theta -> cos(theta*) 
   *
   * OUTPUT:
   *
   *  k_min   -> lower site in cos(theta*) lattice for input cos(theta*)
   *  kflag   -> cos(theta*) extrapolation flag
   *             kflag=0: input cos(theta*) at lattice site, i.e. no extrapolation needed
   *             iflag=1: input cos(theta*) NOT at lattice site, i.e. extrapolation needed  
   *
   */

  double a=(2.0)/(200.0);        //lattice spacing in cos(theta*) 
  double diff;

  //checking arguments range...

  if( !((cos_theta>=-1.0)&&(cos_theta<=1.0)) ){
    printf("ERROR: function mz_pp_to_pipi_get_costheta_lattice_site\n");
    printf("       called with argument cos_theta out of range \n");
    printf("    => cos_theta should be contained in the interval [-1.0,1.0]\n");
    printf("\n\n");
    exit(1);
  }


  //calculate lower cos(theta*) lattice site for input cos_theta
  k_min=int( (cos_theta+1.0)/(a) );

  //decide whether or not extrapolation is needed
  kflag=0;
  diff=cos_theta -1.0*(-1.0 +a*(double)(k_min));
  if(diff>0.0) kflag=1;
}

double mz_pp_to_pipi_vandewi_maximum_sigma(double P)
{
  /**
   * funtion mz_pp_to_pipi_vandewi_maximum_sigma
   *
   * INPUT:
   * 
   *  P -> pbar input momentum lab frame [GeV]
   *
   * RETURNS:
   *
   *  cross section maximum for momentum P in full cos(theta*) range   
   *
   */

  double max;
  int i_min, p_flag;

  if( (P>=pz[0])&&(P<pz[1]) )    max=300.0;               // legendre zone
  if( (P>=pz[1])&&(P<pz[2]) )    max=300.0;
  if( (P>=pz[2])&&(P<pz[3]) )    max=150.0;
  if( (P>=pz[3])&&(P<pz[4]) )    max=150.0;
  if( (P>=pz[4])&&(P<pz[5]) )    max=120.0;
  if( (P>=pz[5])&&(P<pz[6]) )    max=160.0;
  if( (P>=pz[6])&&(P<pz[7]) )    max=160.0;
  if( (P>=pz[7])&&(P<pz[8]) )    max=100.0;
  if( (P>=pz[8])&&(P<pz[9]) )    max=130.0;
  if( (P>=pz[9])&&(P<pz[10]) )   max=130.0;
  if( (P>=pz[10])&&(P<pz[11]) )  max=90.0;
  if( (P>=pz[11])&&(P<pz[12]) )  max=90.0;
  if( (P>=pz[12])&&(P<pz[13]) )  max=65.0;
  if( (P>=pz[13])&&(P<pz[14]) )  max=65.0;
  if( (P>=pz[14])&&(P<pz[15]) )  max=65.0;
  if( (P>=pz[15])&&(P<pz[16]) )  max=70.0;
  if( (P>=pz[16])&&(P<pz[17]) )  max=70.0;
  if( (P>=pz[17])&&(P<pz[18]) )  max=65.0;
  if( (P>=pz[18])&&(P<=pz[19]) ) max=23.0;
  //
  if( (P>pz[19])&&(P<pz[20]) ) max=50.0;                   // legendre/vandewi extrapolation zone
  //
  if( (P>=pz[20])&&(P<=pz[38]) ){                           // vandewi zone
       mz_pp_to_pipi_get_p_lattice_site(P, i_min, p_flag);
       max=sigmas_dt[i_min-20][200]*1.10;
  }

  return max;
}

double mz_pp_to_pipi_vandewi_sigma_legendre(int i, double cos_theta)
{
  /**
   * function mz_pp_to_pipi_vandewi_sigma_legendre
   *
   * INPUT:
   *
   *  i         -> momentum lattice site
   *  cos_theta -> cos(theta*)
   *
   * RETURNS:
   * 
   *  cross section (legendre polinomial series parametrization) 
   *  at cos(theta*) for the pbar momentum corresponding  to the i-th lattice site
   *
   */

  double sigma;
  double sum=0.0;

  //checking arguments range...

  if( !((i>=0)&&(i<=19)) ){
    printf("ERROR: function mz_pp_to_pipi_vandewi_sigma_legendre\n");
    printf("       called with argument i out of range \n");
    printf("    => i should be contained in the interval [0,19]\n");
    printf("\n\n");
    exit(1);
  }


  //calculate the cross section using the fit coeficients
  for(int k=0; k<11; k++){
    sum=sum+fitlegendre[i][k]*mz_legendre_polynomial(k,cos_theta);
  }

  sigma=sum;

  return sigma;
}

double mz_pp_to_pipi_sigma(double P, double cos_theta)
{
  /**
   * function mz_pp_to_pipi_sigma
   *
   * INPUT:
   *
   *  P         -> anti-proton momentum (lab frame) [GeV]
   *  cos_theta -> cos(theta*)
   *
   *
   * RETURNS:
   *
   *  value of cross section for the pbar momentum P at the point cos_theta
   */

  double sigma;

  int i_min, iflag;
  int k_min, kflag;
  double x1,y1,x2,y2;
  double factor=2.77207297; //conversion dsigma/d(cos\theta*) -> dsigma/dt at p=2.43 GeV 

  double sigma_1,sigma_2;
  double a=(2.0)/(200.0);        //lattice spacing in cos(theta*)

  //checking arguments range...

  if( !((cos_theta>=-1.0)&&(cos_theta<=1.0)) ){
    printf("ERROR: function mz_pp_to_pipi_vandewi_sigma\n");
    printf("       called with argument cos_theta out of range \n");
    printf("    => cos_theta should be contained in the interval [-1.0,1.0]\n");
    printf("\n\n");
    exit(1);
  }


  //get momentum-lattice site
  mz_pp_to_pipi_get_p_lattice_site(P, i_min, iflag);

  //get cos(theta*) lattice site
  mz_pp_to_pipi_get_costheta_lattice_site(cos_theta, k_min, kflag);



  //legendre zone
  if( (P>=pz[0])&&(P<=pz[19]) ){
    if(iflag==0){                      //NO p-extrapolation
      sigma=mz_pp_to_pipi_vandewi_sigma_legendre(i_min,cos_theta);
    }
    if(iflag==1){                      //YES p_extrapolation
      x1=pz[i_min];
      y1=mz_pp_to_pipi_vandewi_sigma_legendre(i_min,cos_theta);
      x2=pz[i_min+1];
      y2=mz_pp_to_pipi_vandewi_sigma_legendre(i_min+1,cos_theta);
      sigma=mz_linear_extrapolation(x1,y1,x2,y2,P);
    }
  }


  //legendre/vandewi extrapolation zone
  if( (P>pz[19])&&(P<pz[20]) ){
    if(kflag==0){                 //NO cos(theta*) extrapolation
      x1=pz[19];
      y1=mz_pp_to_pipi_vandewi_sigma_legendre(19,cos_theta)*factor;
      x2=pz[20];
      y2=sigmas_dt[0][k_min];
      sigma=mz_linear_extrapolation(x1,y1,x2,y2,P);
    }
    //
    if(kflag==1){                 //YES cos(theta*) extrapolation
      sigma_1=mz_pp_to_pipi_vandewi_sigma_legendre(19,cos_theta)*factor;
      //
      x1=-1.0+a*(double)(k_min);
      y1=sigmas_dt[0][k_min];
      x2=-1.0+a*(double)(k_min+1);
      y2=sigmas_dt[0][k_min+1];
      sigma_2=mz_linear_extrapolation(x1,y1,x2,y2,cos_theta);
      //
      x1=pz[19];
      y1=sigma_1;
      x2=pz[20];
      y2=sigma_2;
      sigma=mz_linear_extrapolation(x1,y1,x2,y2,P);
    }
  }


  //vandewi zone
  if( (P>=pz[20])&&(P<=pz[38]) ){
    if(  (iflag==0)&&(kflag==0)  ){       //NO p-extrapolation NO cos(theta*)-extrapolation
      sigma=sigmas_dt[i_min-20][k_min];
    }
    //
    if(  (iflag==0)&&(kflag==1)  ){       //NO p-extrapolation YES cos(theta*)-extrapolation
      x1=-1.0+a*(double)(k_min);
      y1=sigmas_dt[i_min-20][k_min];
      x2=-1.0+a*(double)(k_min+1);
      y2=sigmas_dt[i_min-20][k_min+1];
      sigma=mz_linear_extrapolation(x1,y1,x2,y2,cos_theta);
    }
    //
    if(  (iflag==1)&&(kflag==0)  ){       //YES p-extrapolation NO cos(theta*)-extrapolation
      x1=pz[i_min];
      y1=sigmas_dt[i_min-20][k_min];
      x2=pz[i_min+1];
      y2=sigmas_dt[i_min+1-20][k_min];
      sigma=mz_linear_extrapolation(x1,y1,x2,y2,P);
    }
    //
    if(  (iflag==1)&&(kflag==1)  ){       //YES p-extrapolation YES cos(theta*)-extrapolation
      x1=-1.0+a*(double)(k_min);
      y1=sigmas_dt[i_min-20][k_min];
      x2=-1.0+a*(double)(k_min+1);
      y2=sigmas_dt[i_min-20][k_min+1];
      sigma_1=mz_linear_extrapolation(x1,y1,x2,y2,cos_theta);
      //
      x1=-1.0+a*(double)(k_min);
      y1=sigmas_dt[i_min+1-20][k_min];
      x2=-1.0+a*(double)(k_min+1);
      y2=sigmas_dt[i_min+1-20][k_min+1];
      sigma_2=mz_linear_extrapolation(x1,y1,x2,y2,cos_theta);
      //
      x1=pz[i_min];
      y1=sigma_1;
      x2=pz[i_min+1];
      y2=sigma_2;
      sigma=mz_linear_extrapolation(x1,y1,x2,y2,P);
    }
  }

  return sigma;
}

void mz_pp_to_pipi_vandewi_event(double P, double cos_theta_min, double cos_theta_max, double* piplus_p, double* piminus_p)
{
  /**
   * function mz_pp_to_pipi_vandewi_event
   *
   * INPUT:
   *
   *   P: anti-proton momentum (lab frame) [GeV]
   *   cos_theta_min: cos(theta*) minimum; theta*=angle(pi-,pbar) CMS frame
   *   cos_theta_max: cos(theta*) maximum
   *
   * OUTPUT:
   *
   *   piplus_p: 4-momentum pi- lab frame
   *   piminus_p: 4-momentum pi+ lab frame
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

  int flag;
  double x,y;                              
  double max;
  double prob;
  double costheta, theta, phi;

  double piplus_pcm[4];    //CMS frame: 4-mom pi+
  double piminus_pcm[4];   //CMS frame: 4-mom pi-



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


  //get upper bound to cross section for pbar momentum P
  max=mz_pp_to_pipi_vandewi_maximum_sigma(P);


  flag=0;

  while(flag==0){
    x=mzrnd(cos_theta_min,cos_theta_max);   //random cos(theta*) in range [cos_theta_min,cos_theta_max]  
    y=mzrnd(0.0,max);                       //random in range [0.0,upper bound to the cross section]
    prob=mz_pp_to_pipi_sigma(P,x);          //cross section at generated cos(theta*)

    if(y<prob){                             //accept event
      flag=1;

      costheta=x;
      if(costheta > 1.0) costheta=1.0;      //just make sure about range...
      if(costheta < -1.0) costheta=-1.0;
      theta=acos(costheta);                 //theta* pi-
      phi=mzrnd(0.0,2.0*PI);                //phi pi-
 
      //build pi- 4-momentum in CMS frame
      piminus_pcm[0]=sqrt_S/2.0;
      piminus_pcm[1]=pow(S/4.0-mpi*mpi,0.5)*sin(theta)*cos(phi);
      piminus_pcm[2]=pow(S/4.0-mpi*mpi,0.5)*sin(theta)*sin(phi);
      piminus_pcm[3]=pow(S/4.0-mpi*mpi,0.5)*cos(theta);

      //build pi+ 4-momentum in CMS frame
      piplus_pcm[0]=piminus_pcm[0];
      piplus_pcm[1]=-1.0*piminus_pcm[1];
      piplus_pcm[2]=-1.0*piminus_pcm[2];
      piplus_pcm[3]=-1.0*piminus_pcm[3];

      //boost pi- and pi+ 4-momenta to lab frame
      mzboost(1,pbarp_p,piplus_pcm,piplus_p);
      mzboost(1,pbarp_p,piminus_pcm,piminus_p);
    }

  }

}

