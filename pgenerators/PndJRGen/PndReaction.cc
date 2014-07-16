
#include "PndReaction.hh"


ClassImp(PndReaction)


///////////////////////////////////////////////////////////
//
// constructor, default value for OutFile is in the hh file
//
///////////////////////////////////////////////////////////
PndReaction:: PndReaction( TParticle BT, TVector3 VR, char * OutFile ) 
{
    filename = OutFile;
    BeamTarget = BT;
    TLorentzVector v;
    BeamTarget.Momentum(v);
    BeamTarget.SetCalcMass(v.M()); // CalcMass is the actual (here sqrt(s)) not the pole mass!!

    PrimaryVtx.SetXYZ( BT.Vx(), BT.Vy(), BT.Vz() );
    VtxResolution = VR;
    VtxResFlag    = (VtxResolution.Mag()>1e-5) ? 1 : 0;

    Elastic = StaticBR = PoleMass = 0;
    PWindex = numHistos = 0;
    PStabindex = 0;

    SetStable(130); // K0L          
    SetStable(310); // K0S          
    SetStable(211); // charged pion 
    SetStable(321); // charged kaons

    PolarIndex = 0;
    SetDecayAsymmetry( 3122, 1,  0.642);// Lambda -> p pi-
    SetDecayAsymmetry(-3122, 1, -0.642);
    SetDecayAsymmetry( 3122, 2,  0.65); // Lambda -> n pi0
    SetDecayAsymmetry(-3122, 2, -0.65);
    SetDecayAsymmetry( 3222, 1, -0.98); // Sigma+ -> p pi0
    SetDecayAsymmetry(-3222, 1,  0.98);
    SetDecayAsymmetry( 3222, 2,  0.068);// Sigma+ -> n pi+
    SetDecayAsymmetry(-3222, 2, -0.068);
    SetDecayAsymmetry( 3222, 3, -0.76); // Sigma+ -> p gamma
    SetDecayAsymmetry(-3222, 3,  0.76);


}

///////////////////////////////////////////////////////////
//// main event loop
//
///////////////////////////////////////////////////////////
void PndReaction::  Loop(Int_t nEvts)
{
  // initialize the output
  TFile f1(filename,"RECREATE","ROOT_Tree");
//  FILE *ascii_output = fopen("JRoutput.asc","w");

  Double_t seed;

  TTree* fTree = new TTree("data","PandaEvtGen");
  fEvt=new TClonesArray("TParticle",100);
  activeCnt=0;
  fTree->Branch("Npart",&activeCnt,"Npart/I");
  fTree->Branch("Weigth",&weight,"Weight/D");
  fTree->Branch("Seed",&seed,"Seed/D");
  fTree->Branch("Particles",&fEvt,32000);

  for (Int_t i=0; i<nEvts; i++){

    seed = gRandom->GetSeed();
    if ((i%100)==0 || i==1)
      std::cout << i << " events finished" << std::endl; 
    fEvt->Clear();
    weight = 1;
    activeCnt = 0;

    if ( VtxResFlag ) 
	BeamTarget.SetProductionVertex(
	    gRandom->Gaus( PrimaryVtx.X() , VtxResolution.X() ) ,
	    gRandom->Gaus( PrimaryVtx.Y() , VtxResolution.Y() ) ,
	    gRandom->Gaus( PrimaryVtx.Z() , VtxResolution.Z() ) ,  0 );

    Decay(&BeamTarget);
//    fprintf(ascii_output,"%u %u %f %f %f\n",fEvt->GetEntries(),
//	    i,
//	    BeamTarget.Vx(),
//	    BeamTarget.Vy(),
//	    BeamTarget.Vz());


    for (Int_t ii = 0; ii<fEvt->GetEntries(); ii++) {

//    TParticle *temp = (TParticle*)fEvt->At(ii);
    
//    fprintf(ascii_output, "%d ", temp->GetPdgCode());
//    fprintf(ascii_output, "%f %f %f\n",temp->Px(),temp->Py(),temp->Pz());

    }

    fTree->Fill();
  }
  f1.Write();
  f1.Close();
//  fclose(ascii_output);
//   delete fTree;
}


///////////////////////////////////////////////////////////
//
// determine if we have pbar p elastic scattering
//
///////////////////////////////////////////////////////////
Int_t    PndReaction::  UseElasticDXC()
{
  Elastic = 0;
  if ( BeamTarget.GetMass() < 2* (((TDatabasePDG::Instance()) -> GetParticle(2212)) -> Mass())+0.01) {
      std::cout << "Error in PndReaction::UseElasticDXC(): not enough energy" << std::endl;
      return 1;
  }

  TParticlePDG *BTPDG = BeamTarget.GetPDG();
  if (  BTPDG -> NDecayChannels() != 1){
      std::cout << "Error in PndReaction::UseElasticDXC(): incorrect decay path, too many decay channels" << std::endl;
      return 1;
  }

  TDecayChannel * channel = BTPDG -> DecayChannel(0);
  if ( ! (
          (channel->DaughterPdgCode(0)== 2212  && channel->DaughterPdgCode(1)==-2212) ||
          (channel->DaughterPdgCode(0)==-2212  && channel->DaughterPdgCode(1)== 2212) 
         )  ) {
      std::cout << "Error in PndReaction::UseElasticDXC(): incorrect decay path, not pbar p" << std::endl;
      return 1;
  }

  Elastic = 1;
  return 0;
}




///////////////////////////////////////////////////////////
//
// decay a particle
//
///////////////////////////////////////////////////////////
Int_t PndReaction::  Decay(TParticle *mother)
{

  if (Elastic) {
    doElastic();
    return 0;
  }
  //
  // select a decay channel
  //
  TDecayChannel * channel = SelectChannel(mother);

  if ( channel==0 )   return 0; // return if it is a stable particle

  for (Int_t i=0; i<channel->NDaughters(); i++) 
    if (  !((TDatabasePDG::Instance()) -> GetParticle(channel->DaughterPdgCode(i)))  )
      std::cout << " Error in PndReaction::Decay  illegal daughter particle number " 
		<< channel->DaughterPdgCode(i) << std::endl;

  //
  // select the masses of the daughters
  //
  Double_t masses[channel->NDaughters()], msum;
  Int_t j=0;
  do {
    msum=0;
    for (Int_t i=0; i<channel->NDaughters(); i++) {
      msum += masses[i] = SampleMass( channel->DaughterPdgCode(i) );

//       std::cout << masses[i] << "  " ;
    }
//      std::cout << "    sum = " << msum << std::endl;
    if ( msum < mother->GetCalcMass() ) break; // energetically allowed decay

  } while( j++<100 && PoleMass!=1 ); // repeat in case only part of the mass range is allowed

  if ( j > 99 ) {
    std::cerr << "Error in  PndReaction::Decay, insufficient energy for the decay" << std::endl;
    return 0; 
  }


  //.. 
  // look to see if there is an entry in the asymetry list.
  //
  Int_t IsAsym=0, sign=((mother->GetPdgCode()>0)?1:-1);

  Float_t Palpha=0;

  Long_t Pippo = sign*(DC+1)*10000000 + (mother->GetPdgCode());

  for(j=0;j<PolarIndex;j++){
    if ( Pippo == PolarID[j] ) {
      Palpha = Polar[j]*Asym[j];
      IsAsym=1;
      break;
    }
  }

  //
  // setup the decay
  //

  TGenPhaseSpace event;
  TLorentzVector W;
  mother->Momentum(W);
  event.SetDecay( W, channel->NDaughters(), masses );

  //
  // do the decay
  // 
  float wt;
  // This do-while loop means that we do not need to use the weight to correct
  // The reason is that different decays will have different MEAN weights

  TVector3 n;
  mother->GetPolarisation(n);

  TLorentzVector P;
  do {
      wt = event.Generate()/event.GetWtMax();

      if (IsAsym){
	P = *event.GetDecay(0);
	P.Boost(-W.BoostVector());	

	Float_t CosTheta = cos( n.Angle(P.Vect()) );

	wt *= (1.0+Palpha*CosTheta) / (1.0 + fabs(Palpha));
      }

      wt *= PWWeight(&event,mother->GetPdgCode(),channel->Number(),0);

  } while ( wt < gRandom->Rndm() ); // 


  //
  // calculate the location of the decay vertex
  //
  TLorentzVector V;
  mother->ProductionVertex(V);

  if (TotalWidth(mother)){
    Double_t tau = SampleLifetime(mother);
    TLorentzVector Delta(W.BoostVector(),1);
    V = V + ( c * tau * W.Gamma() * Delta );
  }
  //
  // store results
  //
  TLorentzVector *p;
  for (Int_t i=0; i<channel->NDaughters(); i++) {
    p = event.GetDecay(i);
    TParticle temp(((int) channel->DaughterPdgCode(i)),1,0,0,0,0,*p,V);
    temp.SetCalcMass( p->M() ); 

    //
    // find out if the daughter is polarizable
    //
    static TVector3 a,b,n;
    for(j=0;j<PolarIndex;j++){
      Int_t ttttt=PolarID[j];
      while (ttttt> 10000000) {ttttt-=10000000;}
      while (ttttt<-10000000) {ttttt+=10000000;}
      if ( ttttt == channel->DaughterPdgCode(i) ) {
	a=W.Vect();
	b=p->Vect();
	n=a.Cross(b);
	n=n.Unit();
	temp.SetPolarisation(n);
      }
    }


    if ( ! Decay(&temp) )  // recursion, only store those particles that do not decay
      {
      new( (*fEvt)[activeCnt++] ) TParticle(temp);
      }



  }
  return channel->NDaughters();

}


///////////////////////////////////////////////////////////
//
//  randomly select a decay branch. 
//
///////////////////////////////////////////////////////////
TDecayChannel * PndReaction::  SelectChannel( TParticle *mother)
{
  TParticlePDG *motherPDG = mother -> GetPDG();

  if ( Stable( motherPDG->PdgCode() ) )  return 0; 
  //
  // Select a channel
  //
  Double_t rnd = gRandom->Rndm(), temp=0;

  for (Int_t i=0; i < motherPDG -> NDecayChannels() ; i++) 
    if ( rnd < ( temp += BranchRatio( mother, i ) ) ) {
      DC = i;
      return motherPDG -> DecayChannel(i);
    }
  //
  //  catch an error that should never happen
  //
  std::cerr << "Error in PndReaction:: SelectChannel" << std::endl;
  return 0;
  
}


/////////////////////////////////////////////////////////////
//
// return the branching ratio for a given channel. This is either the static value
// (good at the pole mass) or use the mass dependent widths.
//
///////////////////////////////////////////////////////////
Double_t PndReaction::  BranchRatio(  TParticle *mother , Int_t channel ) 
{
  Double_t totalwidth;

  if ( StaticBR || ((totalwidth=TotalWidth(mother))<1e-4) ){
    //
    // Determine the normalization of the branching ratios
    //
    Double_t  sum=0;
    for (Int_t i=0; i < ( mother -> GetPDG() ) -> NDecayChannels() ; i++) 
      sum += ( ( mother -> GetPDG() ) -> DecayChannel(i) ) -> BranchingRatio();

    return ( ( mother -> GetPDG() ) -> DecayChannel(channel) ) -> BranchingRatio() / sum;
   
  }

  Double_t partw = PartialWidth(mother,channel);

  return partw/totalwidth;
}


///////////////////////////////////////////////////////////
//
//  return the total decay width of the particle
//
///////////////////////////////////////////////////////////
Double_t PndReaction:: TotalWidth( TParticle *mother ) 
{
  if ( PoleMass || Stable(mother->GetPdgCode())) 
      return  PoleWidth(mother->GetPdgCode());
      // don't use the following because the database sets this to 0 sometimes...
      // return ( ( mother -> GetPDG() ) -> DecayChannel(channel) ) -> Width();


  Double_t Width=0;

  for (Int_t i=0; i < ( mother -> GetPDG() ) -> NDecayChannels() ; i++) {
      Width += PartialWidth(mother,i);
  }
  
  return Width;
}


///////////////////////////////////////////////////////////
//
//  return the partial width of the particle for a given channel
//
///////////////////////////////////////////////////////////
Double_t PndReaction:: PartialWidth( TParticle *mother, Int_t DecChan ) 
{
  
  if ( StaticBR || PoleMass || (PoleWidth(mother->GetPdgCode()) < 0.0001) || 
       mother -> GetPdgCode()==999 ) {
    return PoleWidth( mother->GetPdgCode() ) * 
	  ( ( mother -> GetPDG() ) -> DecayChannel(DecChan) ) -> BranchingRatio();   
  }
  //
  // find the correct histogram
  //

  Int_t AbsPDG =  abs( mother->GetPdgCode() );
  Int_t index;

  for ( index=0; index < numHistos; index++)
    if ( PID[index] ==  AbsPDG ) break;

  Double_t calcMass = mother->GetCalcMass();
  if (! (index<numHistos) ) index = InitHisto( AbsPDG );

  return  PWHistos[index][DecChan] -> 
      GetBinContent( PWHistos[index][DecChan]->FindBin(calcMass) );
}

///////////////////////////////////////////////////////////
//
//  do the actual calculation of the partial width
//
///////////////////////////////////////////////////////////
Double_t PndReaction:: PW( TParticle *mother, Int_t DecChan ) 
{
  //  The partial width is given by:
  //  the ( total width times the branching ratio ) at the resonance pole mass
  //       times
  //  a correction for the relative phase space of the decay channel
  //  at the given resonannce mass compared to the pole mass

  // partial width at pole mass
  Double_t GammaRes = PoleWidth( mother->GetPdgCode() );
  GammaRes *= ( ( mother -> GetPDG() ) -> DecayChannel(DecChan) ) -> BranchingRatio();
  if ( StaticBR || PoleMass ) return GammaRes;
  //
  //  Now determine the relative phase space factor
  // 
  // fill an array with the pdgcode of the daughters 
  //
  TDecayChannel * channel = ( mother -> GetPDG() ) -> DecayChannel(DecChan);

  Int_t array[20]; 
  if (channel->NDaughters() > 20){
      std::cerr << "Error in  PndReaction:: PartialWidth, too many daughters" << std::endl;
      return 0;
  }
  Int_t i;
  for (i=0; i<channel->NDaughters(); i++) 
    array[i] = channel->DaughterPdgCode(i);
  array[i] = mother -> GetPdgCode();

  Double_t ps0 = PhaseSpace( (mother->GetPDG())->Mass(),array, channel->NDaughters()+1,
			     DecChan);
  if ( ps0>0 )
    GammaRes *= PhaseSpace(  mother->GetCalcMass(), array, channel->NDaughters()+1,
			     DecChan) / ps0;
  else
    GammaRes=0;

  return GammaRes;
}


///////////////////////////////////////////////////////////
//
//  return a number proportional to the volume of phase space
//
///////////////////////////////////////////////////////////
Double_t PndReaction:: PhaseSpace( Double_t ecm, Int_t * array, Int_t nPart,
				   Int_t ch, Int_t indx, Int_t S)
{
//  Double_t ecm2=ecm*ecm;
  Double_t  m0, m1, mLow, maxM2, deltaM2, sum=0;
  Double_t prob0;

  // don't sample masses with width<100keV
  Int_t i0 = array[0] ;
  Int_t n0 = ( Stable(i0) || PoleWidth(i0) < 0.0001 ) ? 1 : numM; 
  Int_t i1 = array[1] ;
  Int_t n1 = ( Stable(i1) || PoleWidth(i1) < 0.0001 ) ? 1 : numM; 
  

  // determine the partial wave
  Int_t L=0;
  for (Int_t i=0; i<PWindex; i++)
    if ( PartialWaves[i][0] == array[nPart-1] && PartialWaves[i][1] == ch)
      L = PartialWaves[i][2+indx];
  if (S) L=0;// force to S-Wave

  // integrate over the width of wide particles
  for (Int_t j=0; j<n0; j++) {
    m0 = ((TDatabasePDG::Instance()) -> GetParticle(array[0])) -> Mass() +
	   PoleWidth(i0) *  MassWindow / n0 * (0.5+j-n0/2.0) ;
    prob0 = Prob(array[0],m0);

    //
    // phase space is proportional to |p|^(2L+1) / m^2 for a 2-body decay
    //
    if ( nPart-1 == 2) {
      for (Int_t k=0; k<n1; k++) {
	m1 = ((TDatabasePDG::Instance()) -> GetParticle(array[1])) -> Mass() +
	    PoleWidth(i1) * ( MassWindow / n1 * (0.5+k-n1/2.0) );
	if (m0+m1 > ecm) continue;
        sum += prob0*Prob(array[1],m1) * pow(PCM(ecm, m0, m1),2*L+1) / ecm;
      }
    }
  //
  // Use a recursion for decays with more daughters.
  //
    else {
      // Determine the minimum mass squared of particles 2,3,...nPart 
      mLow = 0;
      for (Int_t k=1; k<nPart-1; k++) {
	mLow += ((TDatabasePDG::Instance()) -> GetParticle(array[k])) -> Mass();
	if (  ! (Stable(array[k]) || PoleWidth(array[k]) < 0.0001 ) )
	  mLow -= PoleWidth(array[k]) *  MassWindow;
      }
      mLow *= mLow; // minimum invariant mass squared for the 2nd to nth particle
      maxM2 = (ecm - m0)*(ecm-m0); // max Minv squared for particles 2...nPart
      deltaM2 = (maxM2 - mLow ) / 20.; // don't hardcode this to 20 mass steps...
      mLow += deltaM2/2.; // do not select masses directly on the threshold;

      do {
	sum += prob0 * pow(PCM(ecm, m0, sqrt(mLow)),2*L+1) / ecm * 
	    PhaseSpace( sqrt(mLow), array+1, nPart-1, ch, indx+1) * deltaM2;
      } while ( (mLow+=deltaM2) < maxM2);

    } // end of nPart>2
  }  // end of loop over mass of first particle
  return sum;

}


///////////////////////////////////////////////////////////
//
// return the cm momentum of 2 particles with masses m1 & m2 when
// they have a total cm energy of Ecm
//
///////////////////////////////////////////////////////////
Double_t  PndReaction:: PCM   ( Double_t Ecm, Double_t m1, Double_t m2 ){

    Double_t ecm2=Ecm*Ecm;
    Double_t plus  = m1 + m2,  minus = m1 - m2;

    return sqrt( (ecm2-plus*plus)*(ecm2-minus*minus) ) / 2. / Ecm;
}


///////////////////////////////////////////////////////////
//
// return the probability that a particle has a given mass
// sample a relativistic BreitWigner for unstable particles 
//
///////////////////////////////////////////////////////////
Double_t PndReaction:: Prob( Int_t PDGCode, Double_t Mass)
{
  // 
  // short circuit this routine if the flag is set.
  //  (1e-6 is because we are comparing floats, not integers)
  // do not sample if the width is too small
  //
  if ( PDGCode==999 ) return 1; // beam-target

  Double_t m0 = ((TDatabasePDG::Instance()) -> GetParticle(PDGCode)) -> Mass();

  if (PoleMass || PoleWidth(PDGCode) < 0.0001 ) 
    return (fabs( Mass - m0 ) > 1e-6 )?0:1;

  //
  // find the correct histogram
  //
  Int_t index;
  Int_t AbsPDG = abs( PDGCode );
  for ( index=0; index < numHistos; index++) 
    if ( PID[index] ==  AbsPDG ) break;
  // create it if it does not yet exist
  if (! (index<numHistos) ) 
      index = InitHisto( AbsPDG );

  if ( index<0 ) return 0;  // error if it can not be created

  return  MassHistos[index]->GetBinContent( MassHistos[index]->FindBin(Mass) );

}


///////////////////////////////////////////////////////////
//
// Initialize the histogram
//
///////////////////////////////////////////////////////////
Int_t PndReaction:: InitHisto( Int_t PDGCode )
{
  
  Double_t m0 = ((TDatabasePDG::Instance()) -> GetParticle(PDGCode)) -> Mass();
  if ( numHistos > 100 ) {
    std::cerr << "Error in PndReaction::InitHisto, numHistos too large"<< std::endl;
    return -1; // return -1 on purpose 
  }

  if ( PoleWidth(PDGCode) < 1e-6 ) {
    std::cerr << "Error in PndReaction::InitHisto, you should never arrive here"<< std::endl;
    return -1;
  }

  Int_t index = numHistos;   numHistos++;
  PID[index] = abs( PDGCode );

  TLorentzVector t;
  TParticle tempPart(PDGCode,1,0,0,0,0,t,t);

  //
  // histograms of the partial widths
  //
//  TH1D * tempHist[ (tempPart.GetPDG())->NDecayChannels() ];
//  PWHistos[index] = tempHist;
  
  for (Int_t i=0; i < (tempPart.GetPDG())->NDecayChannels() ; i++) {
    char* name = new char[1024];
    sprintf(name,"PWHistoPID%05d_%02d",PID[index],i);
    PWHistos[index][i] = new TH1D (name,name,nBins, m0 - PoleWidth(PDGCode) * MassWindow / 2. ,
				                    m0 + PoleWidth(PDGCode) * MassWindow / 2. );

    for (Int_t j=0; j<nBins; j++){
      tempPart.  SetCalcMass ( m0+PoleWidth(PDGCode)*(MassWindow/nBins*(0.5+j-nBins/2.0)) );
      PWHistos[index][i] -> SetBinContent( j+1, PW(&tempPart,i) );
    }
  }

  //
  // histograms of the mass distribution
  //
  char* name = new char[1024];
  sprintf(name,"MassHistoPID%05d",PID[index]);

  MassHistos[index] = new TH1D (name,name,nBins, m0 - PoleWidth(PDGCode) * MassWindow / 2. ,
				m0 + PoleWidth(PDGCode) * MassWindow / 2. );
  for (int i=0; i<nBins; i++){
      tempPart.SetCalcMass( m0 + PoleWidth(PDGCode) * ( MassWindow / nBins * (0.5+i-nBins/2.0) ));
      (MassHistos[index]) -> SetBinContent( i+1, 
	         BreitWigner( m0, tempPart.GetCalcMass(), TotalWidth(&tempPart) ) );
     
  }

  return  index; 
}


///////////////////////////////////////////////////////////
//
// Relativistic Breit-Wigner, m0=polemass
//
///////////////////////////////////////////////////////////
Double_t PndReaction:: BreitWigner(Double_t m0, Double_t m, Double_t Gamma)
{
    Double_t m2 = m*m, m02 = m0*m0;
    return   m2*Gamma / ((m02-m2)*(m02-m2)+m2*Gamma*Gamma);

}


///////////////////////////////////////////////////////////
//
// random sampling of the particles CM lifetime (in seconds)
//
///////////////////////////////////////////////////////////
Double_t PndReaction::  SampleLifetime( TParticle *mother )
{
  Float_t tau = TotalWidth(mother);

  if ( tau > 0 ) tau = hbar / tau;

  if (tau==0) 
    std::cout << "Error in PndReaction::SampleLifetime:, the lifetime is 0 for particle "
	      <<  mother -> GetPdgCode() <<  std::endl;

  return gRandom->Exp( tau );
}


///////////////////////////////////////////////////////////
//
// sampling of the particle's mass
// if PoleMass==1 then use the particle's pole mass
// else make a random sample of the mass taking the available phase space into account
//
///////////////////////////////////////////////////////////
Double_t  PndReaction::  SampleMass(Int_t PDGCode)
{

  if ( PoleMass || (PoleWidth(PDGCode) < 0.0001) )
    return ((TDatabasePDG::Instance()) -> GetParticle(PDGCode)) -> Mass();

  //
  // find the correct histogram
  //
  Int_t AbsPDG = abs( PDGCode );
  Int_t index;
  for ( index=0; index < numHistos; index++)
    if ( PID[index] ==  AbsPDG ) break;

  if (! (index<numHistos) ) index = InitHisto( AbsPDG );
  return MassHistos[index]->GetRandom();
 
}



Bool_t  PndReaction::  Stable     ( Int_t PDGCode ){

    Bool_t stable=0;

    //
    // do not decay the following particles
    //
    TParticlePDG * p = (TDatabasePDG::Instance()) -> GetParticle(PDGCode);
    if (p) {
        if ( p -> NDecayChannels() == 0 )                    stable = 1;  // static particle
	TString Lepton("Lepton");				   
	if ( !Lepton    .CompareTo( p->ParticleClass() ) )   stable = 1; 
	TString GaugeBoson("GaugeBoson");			   
	if ( !GaugeBoson.CompareTo( p->ParticleClass() ) )   stable = 1; 
    }
    //    
    // let all particles with decay branches decay, except those explicitly listed here
    // leptons and gauge bosons are also not allowed to decay
    //
   
    for(int i=0;i<PStabindex;i++){
      if( PDGCode == PartStable[i]){
        stable = 1;
      }
    }

    return stable;

}


Double_t PndReaction:: PoleWidth  ( Int_t PDGCode ){

    Double_t width=0;

    if  ((TDatabasePDG::Instance()) -> GetParticle(PDGCode)) 
	width = ((TDatabasePDG::Instance()) -> GetParticle(PDGCode)) -> Width() ;
    // 
    // The following should decay, but the Width =0 in the Database...
    //
    if (PDGCode==111)  return hbar / 8.4e-17;  // pi0
    if (PDGCode==221)  return 1.18e-6;         // eta
    if (PDGCode==310)  return hbar / 0.893e-10;// K0S
    if (PDGCode==999)  return 4.0;             // BeamTarget
    if (PDGCode==3122) return hbar / 2.632e-10;// Lambda

    return width;
}


///////////////////////////////////////////////////////////
//
// This fills an array with the partial waves of a given decay
// which is recognized by the PDGCode of the mother particle
// and the decay channel.  The array contains N-1 entries where
// N is the number of particles in the decay branch.
// The 1st number is the orbital angular momentum of the 1st 
// particle relative to all the rest, The 2nd number is the orbital
// angular momentum of particle 2 versus 3,4..., and so on.
//
///////////////////////////////////////////////////////////

void  PndReaction::  DefinePartialWaves   ( Int_t PdgCode, Int_t channel, Int_t *array){
  TDecayChannel *DecChan = ((TDatabasePDG::Instance()) -> GetParticle(PdgCode)) -> DecayChannel(channel);
  if ( DecChan->NDaughters() > 20 ){
     std::cerr << "Error in PndReaction::  DefinePartialWaves, increase array size" << std::endl;
     return;
   }

   PartialWaves[PWindex][0] = PdgCode;
   PartialWaves[PWindex][1] = channel;
   for (Int_t i=0; i < DecChan->NDaughters(); i++)
       PartialWaves[PWindex][i+2] = array[i];
   PWindex++;
}


//
// relative weight compared to S-wave
//
Double_t PndReaction:: PWWeight(TGenPhaseSpace *event, Int_t PdgCode, Int_t channel, Int_t index)
{
  Double_t weight=1.0;
  TDecayChannel *DecChan = ((TDatabasePDG::Instance()) -> GetParticle(PdgCode)) -> DecayChannel(channel);

  // end of the recursion
  if ( ( DecChan -> NDaughters() - index ) < 2) return weight; 

  // determine the partial wave
  Int_t L=0;
  for (Int_t i=0; i<PWindex; i++)
    if ( PartialWaves[i][0] == PdgCode && PartialWaves[i][1] == channel)
      L = PartialWaves[i][2+index];



  if (L!=0) {
    Double_t MminRest=0;
    TLorentzVector *p, SumAll, SumRest;
    p = event->GetDecay(index);
    Double_t m0 = p->M();
    SumAll = *p;
    SumRest.SetXYZT(0,0,0,0);

    for (Int_t i=index+1; i<DecChan->NDaughters(); i++) {
      p = event->GetDecay(i);
      SumAll  += *p;
      SumRest += *p;
      MminRest += p->M();  // in principle one should use the minimum mass here, otherwise weight=1 for 2-body decays
    }

    weight = pow( ( PCM(SumAll.M(),m0,SumRest.M())/PCM(SumAll.M(),m0,MminRest) ), 2*L ); 

  }

  // recursive calling
  return weight * PWWeight(event,PdgCode,channel,index+1);

}

void PndReaction:: SetStable (Long_t PdgCode) {
  
           PartStable[PStabindex++] =  PdgCode;
           PartStable[PStabindex++] = -PdgCode; // antiaparticle
  
}


void PndReaction:: SetPolarization (Long_t PdgCode, Long_t channel, Float_t Pol) {

  if (fabs(Pol)>1.0) {
    std::cout << "Error in  PndReaction::SetPolarization, the polarization must be >-1 and < 1"
	      << std::endl;
      return;
  }
  
  Int_t i;
  Int_t sign = (PdgCode>0)?1:-1;
  for ( i=0; i<PolarIndex; i++) {
    if ((PdgCode+sign*channel*10000000)==PolarID[i]) break;
  }

  if (i==PolarIndex) {
    PolarIndex++;
    PolarID[i] = PdgCode+sign*channel*10000000;
  }

  Polar[i] = Pol;
  
}

void PndReaction:: SetDecayAsymmetry (Long_t PdgCode, Long_t channel, Float_t Asy) {
  
  if (fabs(Asy)>1.0) {
    std::cout << " Error in  PndReaction::SetDecayAsymmetry, the asymmetry must be >-1 and < 1"
	      << std::endl;
      return;
  }

  Int_t i;
  Int_t sign = (PdgCode>0)?1:-1;
  for (i=0; i<PolarIndex; i++) {
    if ((sign*channel*10000000+PdgCode)==PolarID[i]) break;
  }

  if (i==PolarIndex) {
    PolarIndex++;
    PolarID[i] = PdgCode+sign*channel*10000000;
  }

  Asym[i] = Asy;
  
}




///////////////////////////////////////////////////////////
//
// do an elastic process with pbar p
//
///////////////////////////////////////////////////////////
void PndReaction::doElastic()
{
  TLorentzVector W;
  BeamTarget.Momentum(W);


//aida  static const
   Float_t ecm = W.M() / 2. ; // total cm energy per (anti)proton
//aida  static const
 Float_t pcm = sqrt( ecm*ecm - 
				   pow(((TDatabasePDG::Instance()) -> GetParticle(2212)) -> Mass(),2) );

  // find the cm azimuthal angle
  Float_t phi_cm   = gRandom->Rndm() * 2.0 * TMath::Pi();

  // find the cm polar angle with the correct distribution
  Float_t theta_cm;
  static Float_t b[2] = {-16.0,0.837};
  {
      Float_t B = b[0] + b[1] * W.M();
      Float_t t = gRandom->Exp(-1./B);
      theta_cm = 2.0*asin(sqrt(t) /2. /pcm);
  }

  //
  // store the results
  //

  // first to a TVector3
  TVector3 V3_1, V3_2; 
  V3_1.SetMagThetaPhi(pcm, theta_cm, phi_cm);
  V3_2 = -V3_1;

  // then to a 4-vector (2 steps because no appropriate method for TLorentzVector)
  TLorentzVector V4_1, V4_2;

  V4_1.SetVectM( V3_1, ((TDatabasePDG::Instance())-> GetParticle(2212))-> Mass() );
  V4_2.SetVectM( V3_2, ((TDatabasePDG::Instance())-> GetParticle(2212))-> Mass() );

  // boost to laboratory system
  V4_1.Boost(W.BoostVector());
  V4_2.Boost(W.BoostVector());


  TLorentzVector V;// this is the production vertex

  // store to output tree

  new( (*fEvt)[activeCnt++] ) TParticle(-2212,1,0,0,0,0,V4_1,V);
  new( (*fEvt)[activeCnt++] ) TParticle( 2212,1,0,0,0,0,V4_2,V);
}


