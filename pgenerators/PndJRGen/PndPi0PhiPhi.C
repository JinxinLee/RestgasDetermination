// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");

gRandom.SetSeed(554339);

// Redefinition of pi0
if (!(TDatabasePDG::Instance())->GetParticle(998))
(TDatabasePDG::Instance())->AddParticle("pi0","Title",0.1349766,0,0.000000008,0,"Unknown",998);
Int_t array1[2]={22,22};
((TDatabasePDG::Instance())->GetParticle(998))->AddDecayChannel(1,1.0,2,array1);
// Redefinition of Phi
if (!(TDatabasePDG::Instance())->GetParticle(997))
(TDatabasePDG::Instance())->AddParticle("Phi","Title",1.0194,0,0.00426,0,"Unknown",997);
Int_t array2[2]={321,-321};
((TDatabasePDG::Instance())->GetParticle(997))->AddDecayChannel(1,1.0,2,array2);
     
// Fireball = beam + target
if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);


// p+pbar -> pi0+2Phi
Int_t array[3]={998,997,997};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1,1.0,3,array);

//

TLorentzVector Vertex, Beam, Target, s;
Beam.SetXYZM(0.,0.,15.0,((TDatabasePDG::Instance())->GetParticle(-2212))->Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
TVector3  VtxRes(0.0,0.0,0.0);

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction reaction(BeamTarget, VtxRes, "Gen_Pi02Phi.root" );

  reaction.Loop(10001);

}
