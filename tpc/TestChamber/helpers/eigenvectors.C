{


  TMatrixT<double> M(2,2);

  M[0][0]=1.;
  M[1][1]=2.;

  M[1][0]=0.;
  M[0][1]=0.;

  double theta=45.;

  theta=theta/180. * TMath::Pi();

  TMatrixT<double> rot(2,2);
  rot[0][0]=TMath::Cos(theta);
  rot[0][1]=-1.*TMath::Sin(theta);
  rot[1][0]=TMath::Sin(theta);
  rot[1][1]=TMath::Cos(theta);

  TMatrixT<double> rotTransp = rot;
  rotTransp.T();

  M = rotTransp*(M*rot);

  TVectorT<double> EVA(2);
  TMatrixT<double> EVE(2,2);
  EVE=M.EigenVectors(EVA);

  M.Print();
  EVA.Print();
  EVE.Print();








}
