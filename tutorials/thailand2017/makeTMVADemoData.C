#include "TFile.h"
#include "TNtuple.h"
#include "TRandom.h"
#include <iostream>

using std::cout;
using std::endl;

void makeTMVADemoData(int S = 10000, int B = 100000, double fac=3.0)
{
	TFile *f = new TFile("demodata.root","RECREATE");
	
	TNtuple *ntp = new TNtuple("ntp","demo data","m:ev:v1:v2:v3:v4:v5:signal");
	
	double m0 = 1.864, sig0 = 0.02;
	double mmin = m0 - 0.3, mmax = m0 + 0.3;
	
	double sv1m = 2, sv1s = 0.2;
	double bv1m = 3, bv1s = 0.5;

	double sv2m = 4, sv2s = 0.7;
	double bv2m = 3, bv2s = 0.5;

	double sv3m = 4, sv3s = 1.0;
	double bv3m = 3, bv3s = 0.8;

	double sv4m = 1, sv4s = 0.5;
	double bv4m = 3, bv4s = 0.8;

	double sv5m = 2, sv5s = 0.6;
	double bv5m = 2, bv5s = 0.3;
		
	for (int i=0;i<S;++i)
	{
		if (i%1000 == 0) cout <<i<<endl;
		double m  = gRandom->Gaus(m0,sig0);
		
		double c1 = gRandom->Exp(3); // correlation of v2 v3
		double c2 = gRandom->Gaus(1,2); // correlation of v2 v3
		
		double v1 = gRandom->Gaus(sv1m,sv1s*fac);		
		double v2 = gRandom->Gaus(sv2m+c1,sv2s*fac);
		double v3 = gRandom->Gaus(sv3m-c1,sv3s*fac);
		
		double v4 = gRandom->Gaus(sv4m-c2,sv4s*fac);
		double v5 = gRandom->Gaus(sv5m*c2-2*c1,sv5s*fac);
		
		ntp->Fill(m, i, v1, v2, v3, v4, v5, 1.0);
	}
	
	for (int i=0;i<B;++i)
	{
		if (i%1000 == 0) cout <<i<<endl;
		double m  = gRandom->Rndm()*(mmax-mmin) + mmin;
		double c1 = gRandom->Gaus(0,1); // correlation of v2 v3
		double c2 = gRandom->Gaus(3,3); // correlation of v2 v3

		double v1 = gRandom->Gaus(bv1m,bv1s*fac);
		double v2 = gRandom->Gaus(bv2m+c1+c2,bv2s*fac);
		double v3 = gRandom->Exp(bv3s*fac+c1);
		double v4 = gRandom->Gaus(bv4m+(c2*c1),bv4s*fac);
		double v5 = gRandom->Gaus(bv5m*c1-c2,bv5s*fac);
		
		ntp->Fill(m, i+S, v1, v2, v3, v4, v5, 0.0);
	}
	
	f->Write();
	f->Close();
	
}
