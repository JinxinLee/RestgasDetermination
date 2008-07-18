// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");

gRandom.SetSeed(123456);



if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);


Int_t array[2]={22,22};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1, 1.0, 2, array);

double m_prot=.938272;
double m_etac=2.9798;
double E_pbar=(m_etac*m_etac-2*m_prot*m_prot)/(2*m_prot);
double pz_pbar=sqrt(E_pbar*E_pbar-m_prot*m_prot);



TLorentzVector Vertex, Beam, Target, s;
Beam.  SetXYZM(0.,0.,pz_pbar,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

// // mean location of the target
// //Vertex.SetXYZT(0,0,0,0);  // zero by default

// // smearing of the primary vertex (Gauss with the above mean, and this sigma)
// TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???

//TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

 cout << s.M() << endl;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???

TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction reaction( BeamTarget, VtxRes, "Gen_etac_2gamma.root" );

 reaction.Loop(10000);

}
