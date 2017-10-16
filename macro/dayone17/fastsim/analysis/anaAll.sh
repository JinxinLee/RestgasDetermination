mv anaJ.root anaJ_old.root

root -l -b -q 'analyse_J.C("ntp1_Jee_A.root","xd0d0pide>0.85&&xd0d1pide>0.85&&chi24c<50",100,"Setup A, ppb#rightarrowJ/#psi(e^{+}e^{-})#pi^{+}#pi^{-}")'
root -l -b -q 'analyse_J.C("ntp1_Jee_B.root","xd0d0pide>0.85&&xd0d1pide>0.85&&chi24c<50",120,"Setup B, ppb#rightarrowJ/#psi(e^{+}e^{-})#pi^{+}#pi^{-}")'

root -l -b -q 'analyse_J.C("ntp1_Jmm_A.root","abs(f4cxd0m-3.097)<0.1&&esfw1<-0.05&&xd0d0muoiron>25&&xd0d1muoiron>25&&chi24c<50",200,"Setup A, ppb#rightarrowJ/#psi(#mu^{+}#mu^{-})#pi^{+}#pi^{-}")'
root -l -b -q 'analyse_J.C("ntp1_Jmm_B.root","abs(f4cxd0m-3.097)<0.1&&esfw1<-0.04&&chi24c<50&&xd0oang<2.5&&xd0d0thtcm>0.65",220,"Setup B, ppb#rightarrowJ/#psi(#mu^{+}#mu^{-})#pi^{+}#pi^{-}")'

root -l -b -q  combinePlotsJ.C

#------------

mv ana_etac.root ana_etac_old.root

root -l -b -q  'analyse_etac1.C("ntp5_etac1_A.root","chi24c<50",300, "Setup A, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")'
root -l -b -q  'analyse_etac1.C("ntp5_etac1_B.root","chi24c<50",320, "Setup B, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")'

root -l -b -q  combinePlotsEtac.C

#------------

mv  anaPhi.root anaPhi_old.root

root -l -b -q  'analyse_phi_base.C("ntp1_2phi_A.root","abs(f4cxd0m-1.02)<0.012&&abs(f4cxd1m-1.02)<0.012&chi24c<50", 0, "Setup A, #bar{p}p#rightarrow#phi#phi")'
root -l -b -q  'analyse_phi_base.C("ntp1_2phi_B.root","abs(f4cxd0m-1.02)<0.012&&abs(f4cxd1m-1.02)<0.012&chi24c<50",20, "Setup B, #bar{p}p#rightarrow#phi#phi")'
#root -l -b -q  'analyse_phi_base.C("ntp1_2phi_A.root","abs(f4cxd0m+f4cxd1m-2.15)<0.15&&abs(f4cxd0m-1.02)<0.012&&abs(f4cxd1m-1.02)<0.012&chi24c<50", 0, "Setup A, #bar{p}p#rightarrow#phi#phi")'
#root -l -b -q  'analyse_phi_base.C("ntp1_2phi_B.root","abs(f4cxd0m+f4cxd1m-2.15)<0.15&&abs(f4cxd0m-1.02)<0.012&&abs(f4cxd1m-1.02)<0.012&chi24c<50",20, "Setup B, #bar{p}p#rightarrow#phi#phi")'

root -l -b -q  combinePlotsPhi.C
