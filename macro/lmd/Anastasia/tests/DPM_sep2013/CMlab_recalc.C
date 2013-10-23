// this macro calculate th_min and th_max from CM to LAB frame
// for pbarp elastic scattering case

void CMlab_recalc(){
  double cosTh1 = -0.95;
  double cosTh2 = 0.93;
  // double cosTh1 = -1;
  // double cosTh2 = 0;

  /// fixed params by channel
  double Plab = 1.5; //GeV/c bar p mom in LAB frame
  // double Plab = 2.43; //GeV/c bar p mom in LAB frame
  double M0 = 0.938;//GeV/c barp mass
  //  double gamma = TMath::Sqrt(1+TMath::Power(Plab/M0,2)); //LAB
  double E1 = TMath::Sqrt(M0*M0+Plab*Plab); //LAB
  double E2 = M0; //LAB

  double Ecm = TMath::Sqrt(2*M0*M0+2*E1*E2);
  cout<<"Ecm = "<<Ecm<<endl;
  double Pcm = Plab*M0/Ecm;


  /// calc some useful vars
  double sinTh1 = TMath::Sqrt(1-TMath::Power(cosTh1,2));
  double sinTh2 = TMath::Sqrt(1-TMath::Power(cosTh2,2));
  double dgj = Pcm+TMath::Sqrt(M0*M0+Pcm*Pcm);
  double fdjasf = dgj/M0;
  double chi = TMath::Log(fdjasf);

  /// calc limits
  // double tgTh1_lab = sinTh1/(gamma*(Plab/Pcm+cosTh1));
  // double tgTh2_lab = sinTh2/(gamma*(Plab/Pcm+cosTh2));
  double tgTh1_lab = Pcm*sinTh1/(Pcm*cosTh1*TMath::CosH(chi)+Ecm*TMath::SinH(chi));
  double tgTh2_lab = Pcm*sinTh2/(Pcm*cosTh2*TMath::CosH(chi)+Ecm*TMath::SinH(chi));
  // cout<<"sinTh = "<<sinTh1<<" sinTh2 = "<<sinTh2<<endl;
  // cout<<"chi = "<<chi<<" TMath::CosH(chi) = "<<TMath::CosH(chi)<<" TMath::SinH(chi)="<<TMath::SinH(chi)<<endl;
  // cout<<"Pcm*cosTh1*TMath::CosH(chi)+Ecm*TMath::SinH(chi) = "<<Pcm*cosTh1*TMath::CosH(chi)+Ecm*TMath::SinH(chi)<<endl;
  cout<<"Pcm = "<<Pcm<<endl;
  cout<<"============="<<endl;
  cout<<"In CM costh1 = "<<cosTh1<<" and cosTh2 = "<<cosTh2<<endl;
  cout<<"In LAB Th1 = "<<TMath::ATan(tgTh1_lab)<<" Th2 = "<<TMath::ATan(tgTh2_lab)<<" [rad]"<<endl;
  cout<<"In LAB Th1 = "<<180.*TMath::ATan(tgTh1_lab)/TMath::Pi()<<" Th2 = "<<180.*TMath::ATan(tgTh2_lab)/TMath::Pi()<<" [degree]"<<endl;
  //  cout<<"In LAB tg(Th1) = "<<tgTh1_lab<<" tg(Th2) = "<<tgTh2_lab<<endl;
}
