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
     (TDatabasePDG::Instance())->AddParticle("D+ -> K- pi+ pi+","Title",1.869,0,0.197/317/1e10,0,"Unknown",997);

Int_t array[3]={-321,211,211};
((TDatabasePDG::Instance())->GetParticle(997))-> AddDecayChannel(1, 1.0, 3, array);

if (!(TDatabasePDG::Instance())->GetParticle(998))
     (TDatabasePDG::Instance())->AddParticle("D- -> K+ pi- pi-","Title",1.869,0,0.197/317/1e10,0,"Unknown",998);

Int_t array[3]={321,-211,-211};
((TDatabasePDG::Instance())->GetParticle(998))-> AddDecayChannel(1, 1.0, 3, array);

/*
 { // test code
TLorentzVector Vertex, Beam, Target, s;
Beam.  SetXYZM(0.,0.,1.869,1.869);
TVector3  VtxRes(0,0,0);  // what target smearing do you want???
Vertex.SetXYZT(0,0,0,0);  // zero by default
TParticle BeamTarget(998,1,0,0,0,0,Beam,Vertex);
PndReaction  reaction( BeamTarget, VtxRes, "Gen_DpDm.root");
reaction.Loop(1000);
 }
*/

if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);


Int_t array[2]={997,998};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1, 1.0, 2, array);

TLorentzVector Vertex, Beam, Target, s;
Beam.  SetXYZM(0.,0.,6.6,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

 cout << s.M() << endl;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

PndReaction  reaction( BeamTarget, VtxRes );

 reaction.Loop(10000);

}
