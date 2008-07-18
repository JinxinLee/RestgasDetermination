######################################################
#                                                    #
#filter for event B0->D(*)pi(with cpv) and  B0->D*lnu#
#eff=2,5%                                            #
######################################################

module talk GefSelectFilter

#B->D*lnu
  BooNew   mupFilter =  GefPdtList
  BooObjects mupFilter and mu+ nu_mu D*- 
  BooObjects mupFilter p 1.25 5.5

  BooNew   mumFilter =  GefPdtList
  BooObjects mumFilter and mu- anti-nu_mu D*+ 
  BooObjects mumFilter p 1.25 5.5

  BooNew   epFilter =  GefPdtList
  BooObjects epFilter and e+ nu_e D*- 
  BooObjects epFilter p 1.25 5.5

  BooNew   emFilter =  GefPdtList
  BooObjects emFilter and e- anti-nu_e D*+ 
  BooObjects emFilter p 1.25 5.5


#2D* or (1D*and 1D+)  
  BooNew Dstarpm = GefPdtList
  BooObjects Dstarpm  and D*+ D*-

  BooNew Dstarpp = GefPdtList
  BooObjects Dstarpp  and D*+ D*+

  BooNew Dstarmm = GefPdtList
  BooObjects Dstarmm  and D*- D*-

  BooNew DstarpDm = GefPdtList
  BooObjects DstarpDm and D*+ D-

  BooNew DstarpDp = GefPdtList
  BooObjects DstarpDp and D*+ D+

  BooNew DstarmDm = GefPdtList
  BooObjects DstarmDm and D*- D-

  BooNew DstarmDp = GefPdtList
  BooObjects DstarmDp and D*- D+

  BooCompose twoDstarFilter = or  Dstarpm Dstarpp Dstarmm DstarpDm DstarpDp DstarmDm DstarmDp 

  BooCompose leptonFilter =  or mupFilter mumFilter epFilter emFilter
  BooCompose lpiCPVFilter = and leptonFilter twoDstarFilter
  beforeFilter set  lpiCPVFilter

exit
