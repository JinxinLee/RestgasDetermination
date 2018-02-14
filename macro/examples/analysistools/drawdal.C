// ****************************************************************
// drawdal.C(double m0, double m1, double m2, double m3, TString opt="Lsame", int n=200)
// ****************************************************************
// 
// Simple macro to draw Dalitz Plot border (m0+m2)^2:(m1+m2)^2 above a histogram
// USAGE: .x drawdal.C(M, m0, m1, m2, <opt>, <n>)
//   M         : Mass of mother system
//   m0,m1,m2  : masses of three daughters
//   <opt>     : draw option for the TGraph; default: "Lsame"
//   <n>       : number of points in TGraph: default: 200
//
// ****************************************************************
//  K.Goetzen, GSI, 10/2015
// ****************************************************************

Double_t simin(Int_t i, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t result = 0.;
	
	switch (i)
	{
	case 1:  result = (m2+m3)*(m2+m3);
		break;
	case 2:  result = (m1+m3)*(m1+m3);
		break;
	case 3:  result = (m1+m2)*(m1+m2);
		break;
	}
	return result;
}

Double_t simax(Int_t i, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t result = 0.;
	
	switch (i)
	{
	case 1:  result = (m0-m1)*(m0-m1);
		break;
	case 2:  result = (m0-m2)*(m0-m2);
		break;
	case 3:  result = (m0-m3)*(m0-m3);
		break;
	}
	return result;
}

Double_t lambda(Double_t x, Double_t y, Double_t z)
{
  return x*x + y*y + z*z - 2.*x*y - 2.*y*z - 2.*z*x; 
}

Double_t s2min(Double_t s1, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s1,s,m1*m1) ) * sqrt( lambda(s1, m2*m2, m3*m3) );
	 
	Double_t result  = m1*m1 + m3*m3 + ( (s-s1-m1*m1)*(s1-m2*m2+m3*m3) - lamterm )/(2.*s1);
	
	return result;
}

Double_t s2max(Double_t s1, Double_t m0, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s      = m0*m0;
	Double_t lamterm = sqrt( lambda(s1,s,m1*m1) ) * sqrt( lambda(s1, m2*m2, m3*m3) );
	 
	Double_t result  = m1*m1 + m3*m3 + ( (s-s1-m1*m1)*(s1-m2*m2+m3*m3) + lamterm )/(2.*s1);
	
	return result;
}


void CreateGraph(TGraph* g, Int_t n, Double_t m, Double_t m1, Double_t m2, Double_t m3)
{
	Double_t s1minl = simin(1, m, m1, m2, m3);
	Double_t s1maxl = simax(1, m, m1, m2, m3);
	
	Double_t binw = (s1maxl-s1minl)/(Double_t)(n);
	
	Int_t i=0;
	
	Double_t ymin,ymax,x;
	
	for (i=0; i<n; i++)
	{
		x = i*binw + s1minl;
		if (x<=s1minl) x=s1minl+binw/20;
		if (x>=s1maxl) x=s1maxl-binw/20;
		
		ymin = s2min(x, m, m1, m2, m3);
		ymax = s2max(x, m, m1, m2, m3);
		
		g->SetPoint(i,x,ymin);
		g->SetPoint(n*2-i,x,ymax);
	}

	g->SetPoint(n,s1maxl,s2min(s1maxl-binw/20,m,m1,m2,m3));
	g->GetPoint(0,x,ymin);
	g->SetPoint(n*2,x,ymin);
	
}

drawdal(double m0, double m1, double m2, double m3, TString opt="Lsame", int n=200)
{
	TGraph *g=new TGraph(n);
	g->SetLineColor(2);
	g->SetLineStyle(2);
	g->SetLineWidth(2);
	CreateGraph(g,n,m0,m1,m2,m3);
	g->Draw(opt);
}
