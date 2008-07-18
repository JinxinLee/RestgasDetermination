#include "EvtGenBase/EvtPatches.hh"
/*
 * Parse decay file, construct amplitude and compare
 * integrals of amplitude squared terms. 
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "EvtGenBase/EvtMultiChannelParser.hh"
#include "EvtGenBase/EvtPto3PAmpFactory.hh"
#include "EvtGenBase/EvtPDL.hh"
#include "EvtGenBase/EvtDecayMode.hh"
#include "EvtGenBase/EvtAmpPdf.hh"
#include "EvtGenBase/EvtAmpAmpPdf.hh"
#include "EvtGenBase/EvtDalitzFlatPdf.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtRandom.hh"

void usage(const char* name) {

  printf("Usage: %s -f decay_file -i nItg -m nFindMax -e nEff -p -z\n",name);
  exit(0);
}

int main(int argc, char* argv[]) 
{
  typedef EvtDalitzPoint P;

  // Setup

  EvtPDL pdl;
  pdl.readPDT("../EvtGenBase/evt.pdl");
  EvtRandom::setRandomEngine(new EvtRandomEngine());
  

  // Parse command line options

  char file[256] = "";
  char model[256] = "PTO3P";
  int nItg = 100;
  int nFindMax = 100;
  int nEff = 100;
  bool polecomp = false;
  bool intfterms = false;
  extern int optind;
  extern char* optarg; 
  int c;
  while ( (c = getopt(argc, argv, "pzi:m:e:f:h")) != EOF ) {
    switch (c) {
    case 'p': polecomp = true; break;	     
    case 'z': intfterms = true; break;	     
    case 'i': nItg = atoi(optarg); break;	     
    case 'm': nFindMax = atoi(optarg); break;	     
    case 'e': nEff = atoi(optarg); break;	     
    case 'f': strcpy(file,optarg); break; 
    case 'h': { usage(argv[0]); exit(0); }
    }
  }
  
  // Parse decay file - build factory

  EvtMultiChannelParser parser;
  parser.parse(file,model);
  EvtDecayMode mode = parser.getDecayMode(file);
  EvtDalitzPlot plot(mode);
  EvtDalitzFlatPdf flat(plot);
  flat.getItg(1000);
  EvtPto3PAmpFactory fact(plot);
  fact.build(parser,nItg);

  
  // Full PDF
  
  EvtAmplitudeSum<P>* ampSum = fact.getAmp();
  EvtAmpPdf<P> pdf(*ampSum);
  EvtPdfSum<P>* pc = fact.getPC();

  
  // Find maximum by scanning
  
  if(parser.nScan() > 0) {

    EvtPdfMax<P> max;
    if(!polecomp) max = pdf.findMax(flat,parser.nScan());
    else {      
      EvtPdfDiv<P> pdfdiv(pdf,*pc);
      max = pdfdiv.findMax(*pc,parser.nScan());
    }
    printf("SCAN %d %f\n",parser.nScan(),max.value());
  }

  EvtValError sum(0.,0.);  
  EvtValError itgTot;
  EvtValError effTot(-1,0.);
  int nt = fact.getAmp()->nTerms();

  // Full PDF

  if(polecomp) {
    itgTot = pdf.compute_mc_integral(*pc,nItg);
    effTot = pdf.findGenEff(*pc,nEff,nFindMax);
  }
  else {
    itgTot = pdf.compute_mc_integral(flat,nItg);
    effTot = pdf.findGenEff(flat,nEff,nFindMax);
  }
  
  char fmt1[256] = "#%20s %8.2f +/- %5.2f %8.2f %8.2f %8.2f %8.3f\n";
  char fmt2[256] = "#%20s %8s +/- %5s %8s %8s %8s %8s\n";
  printf(fmt2,"Channel","I","err","|c|","|c|^2*I","Fract","Eff");
  printf(fmt1,"Total",itgTot.value(),itgTot.error(),1.,itgTot.value(),1.,effTot.value());

  EvtValError itg;
  EvtValError eff;

  int i;
  for(i=0;i<nt;i++) {
    
    EvtAmpPdf<P> pdf(*fact.getAmp(i));
    if(polecomp) {
      EvtPdfSum<P> pcsum;
      pcsum.addTerm(1./flat.getItg().value(),flat);
      EvtPdf<P>* pci = fact.getPC(i);
      pcsum.addTerm(1./pci->getItg().value(),*pci);
      itg = pdf.compute_mc_integral(pcsum,nItg);
      eff = pdf.findGenEff(pcsum,nEff,nFindMax);
    }
    else {
      itg = pdf.compute_mc_integral(flat,nItg);
      eff = pdf.findGenEff(flat,nEff,nFindMax);
    }
    sum += abs2(fact.getCoeff(i))*itg;
    double cf = fact.getTermCoeff(i);
    double I = itg.value();
    printf(fmt1,fact.compName(i),I,itg.error(),sqrt(cf),cf*I,cf*I/itgTot.value(),eff.value());
  }
   
  // Interference terms

    if(intfterms) {
      int j,k;
      for(i=0;i<nt;i++) {
	for(j=i+1;j<nt;j++) {
	  for(k=0;k<4;k++) {
	    
	    EvtAmpAmpPdf<P> pdf(k,*fact.getAmp(i),*fact.getAmp(j));
	    if(polecomp) {
	      EvtPdfSum<P> pcsum;
	      EvtPdf<P>* pci = fact.getPC(i);
	      EvtPdf<P>* pcj = fact.getPC(j);
	      pcsum.addTerm(1./flat.getItg().value(),flat);
	      pcsum.addTerm(1./pci->getItg().value(),*pci);
	      pcsum.addTerm(1./pcj->getItg().value(),*pcj);
	      itg = pdf.compute_mc_integral(pcsum,nItg);
	      eff = pdf.findGenEff(pcsum,nEff,nFindMax);
	    }
	    else {
	      itg = pdf.compute_mc_integral(flat,nItg);
	      eff = pdf.findGenEff(flat,nEff,nFindMax);
	    }
	    
	    char name[256]; 
	    sprintf(name,"%s_%s_%d",fact.compName(i),fact.compName(j),k);
	    double cf = fact.getTermCoeff(k,i,j);
	    double I = itg.value();
	    printf(fmt1,name,I,itg.error(),sqrt(fabs(cf)),cf*I,cf*I/itgTot.value(),eff.value());
	  }
	}
      }
    }
    
  EvtValError frac = sum/itgTot;
  printf("# AmpSq sum %f +/- %f\n",frac.value(),frac.error());
      
  
  return 0;
}

