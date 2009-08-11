// $Id: PndPhiPhiKK.C,v 1.2 2003/12/09 17:12:57 carsten Exp $ 
// 
//C.Schwarz, GSI, Darmstadt, Germany. 28-Apr-2003 
// 
// Macro for the PndJRGen generator. It samples the reaction 
// pbar + p --> phi + phi at 8.5 GeV 
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");


gRandom.SetSeed(123475);

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
  Int_t array[2]={-211,211}; //Pi- Pi+
  ((TDatabasePDG::Instance())->GetParticle(999))
    -> AddDecayChannel(1, 1.0, 2, array);

  TLorentzVector Vertex, Beam, Target, s;
  Beam.  SetXYZM(0.,0.,4.0,((TDatabasePDG::Instance()) 
			    -> GetParticle(-2212)) -> Mass());
  Target.SetXYZM(0.,0.,0.0,((TDatabasePDG::Instance()) 
			    -> GetParticle( 2212)) -> Mass());
  s = Beam+Target;

  // mean location of the target
  Vertex.SetXYZT(0,0,0,0);  // zero by default

  // smearing of the primary vertex (Gauss with the above mean, and this sigma)
  //TVector3  VtxRes(.1,.1,.2);  // what target smearing do you want???
  TVector3 VtxRes(0,0,0);
  TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);

  PndReaction  reaction( BeamTarget, VtxRes, "Gen_PipPim.root");
  
  reaction.Loop(1001);

}
