// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


//gRandom.SetSeed(123456);

// create a new particle that only decays to K+ K-

if (!(TDatabasePDG::Instance())->GetParticle(997))
     (TDatabasePDG::Instance())->AddParticle("phi -> K- K+","Title", 1.01940e+00 ,0, 4.43000e-03,0,"Unknown",997);
Int_t array[3]={-321,321};
((TDatabasePDG::Instance())->GetParticle(997))-> AddDecayChannel(1, 1.0, 2, array);


//
// create a new particle for the beam and target
//
if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);


Int_t array[2]={997,997};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1, 1.0, 2, array);

TLorentzVector Vertex, Beam, Target, s;

// use a beam momentum of 4.0 GeV/c in the example below

Beam.  SetXYZM(0.,0.,4.0,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

 cout << s.M() << endl;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction  reaction( BeamTarget, VtxRes, "PhiPhi.root" );

 reaction.Loop(10001);

}
