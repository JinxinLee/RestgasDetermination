// $Id: PndPP.C,v 1.1 2003/08/14 15:11:51 ritman Exp $
//
// J.Ritman U. Giessen 15.8.03
//
// Macro for the PANDA event generator. 
// pbar + p elastic scattering
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
    (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);

  Int_t array[2]={-2212,2212};
  ((TDatabasePDG::Instance())->GetParticle(999)) -> AddDecayChannel(1, 1.0, 2, array);


  TLorentzVector Vertex, Beam, Target, s;
  Beam.  SetXYZM(0.,0.,8.0,((TDatabasePDG::Instance())
                            -> GetParticle(-2212)) -> Mass());
  Target.SetXYZM(0.,0.,0.0,((TDatabasePDG::Instance())
                            -> GetParticle( 2212)) -> Mass());
  TVector3  VtxRes(0,0,0);  // what target smearing do you want???
  s = Beam+Target;
  TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);


  PndReaction  reaction( BeamTarget,VtxRes, "Gen_PPbar.root" );

  reaction.UseElasticDXC();
  reaction.Loop(10001);

}


