//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TProbab				    				//
//                                                                      //
// Calculate probability from chisquare and degrees of freedom		//
//                                                                      //
// Author: Marcel Kunze, RUB, Nov. 99					//
// Copyright (C) 1999-2001, Ruhr-University Bochum.			//
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "RhoMath/TProbab.h"
#include "TMath.h"

extern double TProbab(const int& ndof, const double& chisq){
    
    //constants
    static real srtopi=2.0/TMath::Sqrt(2.0*TMath::Pi());
    static real upl=100.0;
    
    real prob=0.0;
    if(ndof<=0) {return prob;}
    if(chisq<0.0) {return prob;}
    if(ndof<=60) {
	//full treatment
	if(chisq>upl) {return prob;}
	real sum=TMath::Exp(-0.5*chisq);
	real term=sum;
	
	int m=ndof/2;
	if(2*m==ndof){
	    if(m==1){return sum;}
	    for(int i=2; i<=m;i++){
		term=0.5*term*chisq/(i-1);
		sum+=term;
	    }//(int i=2; i<=m)
	    return sum;
	    //even
	    
	}else{
	    //odd
	    real srty=TMath::Sqrt(chisq);
	    real temp=srty/1.414;
	    prob=TMath::Erfc(temp);
	    if(ndof==1) {return prob;}
	    if(ndof==3) {return (srtopi*srty*sum+prob);}
	    m=m-1;
	    for(int i=1; i<=m; i++){
		term=term*chisq/(2*i+1);
		sum+=term;
	    }//(int i=1; i<=m; i++)
	    return (srtopi*srty*sum+prob);
	    
	}//(2*m==ndof)
	
    }else{
	//asymtotic Gaussian approx
	real srty=TMath::Sqrt(chisq)-TMath::Sqrt(ndof-0.5);
	if(srty<12.0) {prob=0.5*TMath::Erfc(srty);};
	
    }//ndof<30
    
    return prob;
}//endof probab
