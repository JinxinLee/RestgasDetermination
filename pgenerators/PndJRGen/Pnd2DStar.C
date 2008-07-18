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

// create a new particle D0 to have only one decay branch
if (!(TDatabasePDG::Instance())->GetParticle(996))
     (TDatabasePDG::Instance())->AddParticle("D0 -> K- pi+","Title",1.8645,0,0.197/123.7/1e10,0,"Unknown",996);
Int_t array[3]={-321,211};
((TDatabasePDG::Instance())->GetParticle(996))-> AddDecayChannel(1, 1.0, 2, array);

// D0bar
if (!(TDatabasePDG::Instance())->GetParticle(997))
     (TDatabasePDG::Instance())->AddParticle("D0 -> K+ pi-","Title",1.8645,0,0.197/123.7/1e10,0,"Unknown",997);
Int_t array[3]={321,-211};
((TDatabasePDG::Instance())->GetParticle(997))-> AddDecayChannel(1, 1.0, 2, array);

// D*+
if (!(TDatabasePDG::Instance())->GetParticle(998))
     (TDatabasePDG::Instance())->AddParticle("D*+ -> D0 pi+","Title",2.01,0,1e-15,0,"Unknown",998);
Int_t array[3]={996,211};
((TDatabasePDG::Instance())->GetParticle(998))-> AddDecayChannel(1, 1.0, 2, array);

// D*-
if (!(TDatabasePDG::Instance())->GetParticle(999))
     (TDatabasePDG::Instance())->AddParticle("D*- -> D0bar pi-","Title",2.01,0,1e-15,0,"Unknown",999);
Int_t array[3]={997,-211};
((TDatabasePDG::Instance())->GetParticle(999))-> AddDecayChannel(1, 1.0, 2, array);






if (!(TDatabasePDG::Instance())->GetParticle(995))
     (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",995);
Int_t array[2]={998,999};
((TDatabasePDG::Instance())->GetParticle(995))-> AddDecayChannel(1, 1.0, 2, array);

TLorentzVector Vertex, Beam, Target, s;
Beam.  SetXYZM(0.,0.,8,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0.,0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;

 cout << s.M() << endl;

// mean location of the target
//Vertex.SetXYZT(0,0,0,0);  // zero by default

// smearing of the primary vertex (Gauss with the above mean, and this sigma)
//TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
TVector3  VtxRes(0,0,0);  // what target smearing do you want???

TParticle BeamTarget(995,1,0,0,0,0,s,Vertex);

PndReaction  reaction( BeamTarget, VtxRes, "Gen_2DStar.root");

reaction.UsePoleMass();


 reaction.Loop(1001);

}
