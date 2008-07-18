#include "TTree.h"
#include "TClonesArray.h"
#include "tpc/PndTpcMCTracklet.h"
#include <vector>
#include "TGraph.h"
#include <iostream>
#include "TFile.h"

void plotMCEvtMixQuality(TTree* tree){

  TClonesArray* trkl=new TClonesArray("PndTpcMCTracklet");
  tree->SetBranchAddress("DeconvTrkl",&trkl);

  // define variables
  int Ntrue=0;  // total number of physics tracks
  int NV0=0;    // total number of V0s in tpc
  // gridsize:
  int nt0=20;double t0start=20; double t0step=20;
  int nmvdres=20;double resstart=0.1;double resstep=0.05;
  int nnmvd=4;int nstart=1;
  int nv0=1;double v0start=0.1; double v0step=0.05;
  int nxy=1;double xystart=0.1; double xystep=0.4;
  // define parameter sets:
  int ngrid=nt0*nmvdres*nnmvd*nv0*nxy;
  std::vector<int> phys(ngrid,0); // number of survived physics track
  std::vector<int> bkg(ngrid,0);  // number of survived bkkg tracks
  std::vector<int> v0phys(ngrid,0); // number of suvived v0s
  std::vector<int> v0bkg(ngrid,0);
  // parameters for scan:
  std::vector<double> t0cut(ngrid);
  std::vector<double> mvdrescut(ngrid);
  std::vector<int> nmvdcut(ngrid);
  std::vector<double> v0cut(ngrid);
  std::vector<double> xycut(ngrid);

  // loop over parameters
  double t0=t0start;
  // register parameter sets:
  int it=0;
  for(int it0=0; it0<nt0; ++it0){ // loop over t0 parameters
    double res=resstart;
    for(int imvd=0; imvd<nmvdres; ++imvd){ // loop over mvd residuals
       double v0=v0start;
       for(int iv0=0; iv0<nv0; ++iv0){ // loop over V0 residuals
	 double xy=xystart;
	 for(int ixy=0; ixy<nxy; ++ixy){ // loop over xy cut
	   int nmin=nstart;
	   for(int inmvd=0; inmvd<nnmvd; ++inmvd){
	     // register values
	     t0cut[it]=t0;
	     mvdrescut[it]=res;
	     nmvdcut[it]=nmin;
	     v0cut[it]=v0;
	     xycut[it]=xy;
	     std::cout<<"Set"<<it
		      <<"    t0="<<t0
		      <<"    mvdres="<<res
		      <<"    nmvd="<<nmin
		      <<"    v0res="<<v0
		      <<"    xycut="<<xy<<std::endl;
	     
	     ++it; // increase paramter set counter
	     ++nmin;
	   }// end loop over nmvd cut
	   xy+=xystep;
	 }// end loop over xy cut;
	v0+=v0step;
      } // end loop over V0 residusl
      res+=resstep;
    }// end loop over mvdresidualscut
    t0+=t0step;
  } // end loop over t0
  

  int nevt=tree->GetEntriesFast();
  for(int i=0; i<nevt; ++i){ // loop over events
    tree->GetEntry(i);
    
    int ntrkl=trkl->GetEntriesFast();
    for(int itrkl=0; itrkl<ntrkl;++itrkl){
      PndTpcMCTracklet* trk=(PndTpcMCTracklet*)trkl->At(itrkl);
      bool isphy=(trk->mcid().mceventID()==0) && (trk->status()==0);
      if(isphy){
	if(trk->q()!=0)++Ntrue;
	else if(trk->pos().Perp()>16)++NV0; // only inside TPC!
      }
      // loop over parameter sets
      for(int ipar=0; ipar<ngrid; ++ipar){
	if(fabs(trk->t0())<t0cut[ipar] && trk->zpoca().Perp()<xycut[ipar]){
	  if(trk->q()!=0 && trk->nmvdAcc(mvdrescut[ipar])>=nmvdcut[ipar]){
	    if(isphy)phys[ipar]=phys[ipar]+1;
	    else bkg[ipar]=bkg[ipar]+1;
	  }
	  else if(trk->q()==0 && fabs(trk->V0res())<v0cut[ipar] && trk->pos().Perp()>16 ){
	    if(isphy)v0phys[ipar]=v0phys[ipar]+1;
	    else v0bkg[ipar]=v0bkg[ipar]+1;
	  }
	}// end cutting on paramter set
      } // end loop over parameter sets
    } // end loop over tracklets
  } // end loop over events

  TFile* outfile=TFile::Open("deconvQual.root","RECREATE");
  TTree* outtree=new TTree("qual","qual");

  double _eff;
  double _pur;
  double _v0eff;
  double _v0pur;
  double _t0;
  double _res;
  int _nmvd;
  double _v0res;
  double _xyres;
  outtree->Branch("eff",&_eff,"eff/D");
  outtree->Branch("pur",&_pur,"pur/D");
  outtree->Branch("v0eff",&_v0eff,"v0eff/D");
  outtree->Branch("v0pur",&_v0pur,"v0pur/D");
  outtree->Branch("t0",&_t0,"t0/D");
  outtree->Branch("res",&_res,"res/D");
  outtree->Branch("v0res",&_v0res,"v0res/D");
  outtree->Branch("xyres",&_xyres,"xyres/D");
  outtree->Branch("nmvd",&_nmvd,"nmvd/I");
  

  std::cout<<Ntrue<<" physics tracks in total."<<std::endl;
  // print parameter sets:
  TGraph* hQual=new TGraph(ngrid);
  for(int i=0;i<ngrid; ++i){
    std::cout<<"Set"<<i<<std::endl;
    _eff=(double)phys[i]/(double)Ntrue;
    _pur=1.-(double)bkg[i]/(double)(Ntrue+bkg[i]);
    _v0eff=(double)v0phys[i]/(double)NV0;
    _v0pur=1.-(double)v0bkg[i]/(double)(NV0+v0bkg[i]);
    std::cout<<"efficiency: "<<_eff<<std::endl;
    std::cout<<"purity: "<<_pur<<std::endl;
    std::cout<<"V0efficiency: "<<_v0eff<<std::endl;
    std::cout<<"V0purity: "<<_v0pur<<std::endl;
    _t0=t0cut[i];
    _res=mvdrescut[i];
    _nmvd=nmvdcut[i];
    _v0res=v0cut[i];
    _xyres=xycut[i];
    hQual->SetPoint(i,_eff,_pur);
    outtree->Fill();
  }
  hQual->SetMarkerStyle(20);
  hQual->Draw("AP");
  outtree->Write();
  outfile->Close();
}
