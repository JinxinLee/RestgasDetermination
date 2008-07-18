// $Id: PndPsi3770MuMu.C,v 1.1 2003/09/24 10:09:30 ritman Exp $
//
// J.Ritman    Sept 2003
//
// Macro for the PANDA event generator. It samples the reaction
// pbar(6.6 GeV/c) + p -->  Psi(3770) -> mu+ mu-
//
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


gRandom.SetSeed(123456);

// use cm not mm
if (!(TDatabasePDG::Instance())->GetParticle(997))
     (TDatabasePDG::Instance())->AddParticle
("Psi(3770) -> mu- mu+","Title",3.770,0,1.0,0,"Unknown",997);

Int_t array[2]={-13,13};
((TDatabasePDG::Instance())->GetParticle(997))-> AddDecayChannel(1, 1.0, 2, array);

TLorentzVector Vertex(0.0,0.0,0.0), Beam;
Beam.  SetXYZM(0.,0.,6.6,((TDatabasePDG::Instance()) -> GetParticle(997)) -> Mass());


TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(997,1,0,0,0,0,Beam,Vertex);

PndReaction  reaction( BeamTarget, VtxRes, "Gen_PsiMuMu.root" );

// reaction.UsePoleMass();

 reaction.Loop(1001);

}


