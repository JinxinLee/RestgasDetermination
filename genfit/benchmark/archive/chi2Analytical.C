//f(chi2) = (chi2)^(n/2 - 1) e^(-chi2 / 2 ) 2^(-n/2) / gamma(n/2)
double Ndeg;
double func(double* x, double *p){
  double X = x[0];
  double N = Ndeg;
  X*=N;

  return p[0]*5000*pow(X,N/2.-1.)*exp(X/(-2.))*pow(2.,N/(-2.))/TMath::Gamma(N/2.);
}

TF1* getFunc(double n){
  Ndeg=n;
  TF1* f = new TF1("mychi2",func,0.,10.,1);
  f->SetParameter(0,1.);
  return f;
}
