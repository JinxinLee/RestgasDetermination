//Last modified: 7:th August 2003 Agnes Lundborg
// macro for the  PndJRGen event generator
// modified by A.Galoyan

{
  // load shared libraries
  gROOT->Reset();
  gSystem->Load("libPhysics.so");
  gSystem->Load("libEG.so");
  gSystem->Load("lib/libPndJRGen.so");

gROOT->Reset();

//----------Update static particle list-------------------
//Add the hybrid to the static particle list
if (!(TDatabasePDG::Instance())->GetParticle(998)) 
  (TDatabasePDG::Instance())->AddParticle("psi_g", "Psig", 4.28, 0, 0.02, 0, "Unknown", 998);
// chi_1c + pi0 + pi0
Int_t psigProd[3]={20443,111,111}; 
((TDatabasePDG::Instance())->GetParticle(998))->AddDecayChannel(1, 1.0, 3, psigProd);

//Add the combinated particle pbarp
if (!(TDatabasePDG::Instance())->GetParticle(999)) 
  (TDatabasePDG::Instance())->AddParticle("BeamTarget","Title",4.0,0,4.0,0,"Unknown",999);
 
// psi_g + pi0 fraga om type, type paverkar ingenting, ej implementerat
Int_t ppProd[2]={998,111}; //Hybrid and pion
((TDatabasePDG::Instance())->GetParticle(999))->AddDecayChannel(1, 1.0, 2, ppProd);

//Arguments: type,decay ratio,number of final state particles, array with final
//state particle ID

//-----------Intial state particle, composite pbarpstate-------
TLorentzVector Vertex, Beam, Target, s;
Beam.SetXYZM(0.,0.,15.,((TDatabasePDG::Instance()) -> GetParticle(-2212)) -> Mass());
Target.SetXYZM(0.,0., 0.,((TDatabasePDG::Instance()) -> GetParticle( 2212)) -> Mass());
s = Beam+Target;
TParticle BeamTarget(999,1,0,0,0,0,s,Vertex);
TVector3 VtxRes(0,0,0);

PndReaction reaction(BeamTarget,VtxRes, "Gen_Hc_ee7G.root");

//reaction.SetStable(998);   //Hybrid
//reaction.SetStable(20443); //chi_c1
//reaction.SetStable(111);   //pion
//reaction.SetStable(443);   //J/Psi
//reaction.UseStaticBR();    
//reaction.UsePoleMass();

Int_t L1[1]={1}; //p-wave between pion and hybrid
reaction.DefinePartialWaves(999,0,L1);
Int_t L2[2]={1,0}; //p-wave between chi_ci and pion pair, s-wave between pions
reaction.DefinePartialWaves(998,0,L2);
reaction.DefinePartialWaves(111,0,L1); //p-wave between photons

//----Take away a decay channel for chi_ci-----------------
TObjArray *a=((TDatabasePDG::Instance())->GetParticle(20443))->DecayList();
a->RemoveAt(1);
(TDatabasePDG::Instance())->GetParticle(20443)->NDecayChannels();
//----Take away a decay channel for J/Psi------------------
TObjArray *b=((TDatabasePDG::Instance())->GetParticle(443))->DecayList();
b->RemoveAt(2);
b->RemoveAt(1);
(TDatabasePDG::Instance())->GetParticle(443)->NDecayChannels();
//----Take away a decay channel for pi0------------------
TObjArray *c=((TDatabasePDG::Instance())->GetParticle(111))->DecayList();
c->RemoveAt(1);
(TDatabasePDG::Instance())->GetParticle(111)->NDecayChannels();


reaction.Loop(10001);

}










