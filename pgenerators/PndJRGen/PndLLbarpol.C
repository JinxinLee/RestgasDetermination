// $Id: PndLLbarpol.C,v 1.3 2004/11/02 17:25:20 alex Exp $
//
// M.Destefanis    Jan 2004
//
// Macro for the PANDA event generator. It samples the reaction
// pbar(4.0 GeV/c) + p -->  L Lbar
//
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


gRandom.SetSeed(123456);


//Lambda
if (!(TDatabasePDG::Instance())->GetParticle(997))
     (TDatabasePDG::Instance())->AddParticle("L -> p pi-","Title",1.115684,0,0.197/78900/1e9,0,"Unknown",997);

Int_t array1[2]={2212,-211};
((TDatabasePDG::Instance())->GetParticle(997))-> AddDecayChannel(1, 1.0, 2, array1);


//Lambda bar
if (!(TDatabasePDG::Instance())->GetParticle(998))
     (TDatabasePDG::Instance())->AddParticle("L -> pbar pi+","Title",1.115684,0,0.197/78900/1e9,0,"Unknown",998);

Int_t array2[2]={-2212,211};
((TDatabasePDG::Instance())->GetParticle(998))-> AddDecayChannel(1, 1.0, 2, array2);


if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);


Int_t array[2]={997,998};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1, 1.0, 2, array);

TLorentzVector Vertex, Beam, Target, s;
Beam.  SetXYZM(0.,0.,4.,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

 cout << s.M() << endl;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction reaction( BeamTarget, VtxRes, "Gen_LLbarpol.root" );


reaction.SetDecayAsymmetry(997,1,1);
reaction.SetPolarization(997,1,1);
reaction.SetDecayAsymmetry(998,1,-1);
reaction.SetPolarization(998,1,1);

 reaction.Loop(10005);

}
