// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


//gRandom.SetSeed(123456);

if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);

// 2pi0+2eta
Int_t array[4]={111,111,221,221};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1, 1.0, 4, array);

TLorentzVector Vertex, Beam, Target, s;
Beam.  SetXYZM(0.,0.,8.,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction  reaction( BeamTarget, VtxRes, "Gen_2Pi0eEta.root");

  reaction.Loop(1001);

}
