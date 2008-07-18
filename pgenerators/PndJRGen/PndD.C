// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


//gRandom.SetSeed(123456);

// use cm not mm
if (!(TDatabasePDG::Instance())->GetParticle(997))
     (TDatabasePDG::Instance())->AddParticle("D+ -> K- pi+ pi+","Title",1.869,0,1.0,0,"Unknown",997);

Int_t array[3]={-321,211,211};
((TDatabasePDG::Instance())->GetParticle(997))-> AddDecayChannel(1, 1.0, 3, array);

TLorentzVector Vertex(0.1,0.1,0.4), Beam;
Beam.  SetXYZM(0.,0.,3.3,((TDatabasePDG::Instance()) -> GetParticle(997)) -> Mass());


TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(997,1,0,0,0,0,Beam,Vertex);

PndReaction  reaction( BeamTarget, VtxRes, "Gen_D.root");

 reaction.UsePoleMass();

 reaction.Loop(1001);

}
