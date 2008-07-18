// macro for the  PndJRGen event generator
// modified by A.Galoyan

{

  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");

  // define beam - target system

  if (!(TDatabasePDG::Instance())->GetParticle(999))
    (TDatabasePDG::Instance())->AddParticle("BeamTarget",
					    "Title",
					    4.0,
					    0,
					    4.0,
					    0,
					    "Unknown",
					    999);

  // define how the compound object "BeamTarget" should decay
  // here: 100% branching ratio into four charged pions

  Int_t array[4]={211,-211,211,-211}; // 4 charged pions
  ((TDatabasePDG::Instance())->GetParticle(999))
    -> AddDecayChannel(1, 1.0, 4, array);

  TLorentzVector Vertex, Beam, Target, s;

  // beam: antiproton (-2212)
  Beam.  SetXYZM(0.,0.,10.,((TDatabasePDG::Instance()) 
			    -> GetParticle(-2212)) -> Mass());

  // target: proton (2212)
  Target.SetXYZM(0.,0.,0.0,((TDatabasePDG::Instance()) 
			    -> GetParticle( 2212)) -> Mass());

  s = Beam+Target;

  // mean location of the target
  Vertex.SetXYZT(0,0,0,0);  // zero by default

  // smearing of the primary vertex (Gauss with the above mean, and this sigma)
  TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
  //  TVector3 VtxRes(0,0,0);

  TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

  // output root file

  PndReaction  reaction( BeamTarget, VtxRes, "Gen_2Pi+2Pi-.root" );
  
  // number of events
  reaction.Loop(1001);

}

