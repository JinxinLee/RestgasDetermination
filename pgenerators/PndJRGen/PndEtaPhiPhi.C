// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


gRandom.SetSeed(2537664832);

// Redefinition of eta0
if (!(TDatabasePDG::Instance())->GetParticle(998))
     (TDatabasePDG::Instance())->AddParticle("eta0","Title",0.54775,0,0.00000129,0,"Unknown",998);
Int_t array1[2]={22,22};
((TDatabasePDG::Instance())->GetParticle(998))-> AddDecayChannel(1,1.0,2,array1);


     
// Redefinition of Phi
if (!(TDatabasePDG::Instance())->GetParticle(997))
     (TDatabasePDG::Instance())->AddParticle("Phi","Title",1.0195,0,0.00426,0,"Unknown",997);
Int_t array2[2]={321,-321};
((TDatabasePDG::Instance())->GetParticle(997))->AddDecayChannel(1,1.0,2,array2);
     

// Fireball = beam + target
if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);


// p+pbar -> eta0+2Phi
Int_t array[3]={998,997,997};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1,1.0,3,array);

//

TLorentzVector Vertex, Beam, Target, s;
Beam.SetXYZM(0.,0.,2.45,((TDatabasePDG::Instance())->GetParticle(-2212))->Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???

TVector3  VtxRes(0.,0.,0.);

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction reaction( BeamTarget, VtxRes, "PndEta2Phi.root" );

  reaction.Loop(10001);

}
