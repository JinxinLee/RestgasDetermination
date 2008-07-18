// $Id: PndJPsiPhi.C,v 1.4 2003/07/15 12:03:58 carsten Exp $
//
// C.Schwarz, GSI, Darmstadt, Germany. 2-Apr-2003
//
// Macro for the PANDA event generator. It samples the reaction
// pbar + p --> J/psi + phi at 8.5 GeV
//
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

  Int_t array[2]={443,333}; // J/psi phi
  ((TDatabasePDG::Instance())->GetParticle(999))
    -> AddDecayChannel(1, 1.0, 2, array);

  TLorentzVector Vertex, Beam, Target, s;
  Beam.  SetXYZM(0.,0.,8.5,((TDatabasePDG::Instance()) 
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

  PndReaction  reaction( BeamTarget, VtxRes, "Gen_JPsiPhi.root");
  
  reaction.Loop(10001);

}
// $Log: PndJPsiPhi.C,v $
// Revision 1.4  2003/07/15 12:03:58  carsten
// 100000 evts
//
// Revision 1.3  2003/04/10 09:20:07  carsten
// direct definition of J/psi phi
//
// Revision 1.2  2003/04/01 15:33:50  carsten
// added log
//
