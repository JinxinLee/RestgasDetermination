mv anaJ_slc.root anaJ_slc_old.root

root -l -b -q 'analyse_J_slc.C("ntp1_slc_Jee_A.root","xd0d0pide>0.85&&xd0d1pide>0.85&&chi24c<50",100,"Setup A, ppb#rightarrowJ/#psi(e^{+}e^{-})#pi^{+}#pi^{-}")'
root -l -b -q 'analyse_J_slc.C("ntp1_slc_Jee_B.root","xd0d0pide>0.85&&xd0d1pide>0.85&&chi24c<50",120,"Setup B, ppb#rightarrowJ/#psi(e^{+}e^{-})#pi^{+}#pi^{-}")'

root -l -b -q 'analyse_J_slc.C("ntp1_slc_Jmm_A.root","abs(f4cxd0m-3.097)<0.1&&esfw1<-0.05&&xd0d0muoiron>25&&xd0d1muoiron>25&&chi24c<50",200,"Setup A, ppb#rightarrowJ/#psi(#mu^{+}#mu^{-})#pi^{+}#pi^{-}")'
root -l -b -q 'analyse_J_slc.C("ntp1_slc_Jmm_B.root","abs(f4cxd0m-3.097)<0.1&&esfw1<-0.04&&chi24c<50&&xd0oang<2.5&&xd0d0thtcm>0.65",220,"Setup B, ppb#rightarrowJ/#psi(#mu^{+}#mu^{-})#pi^{+}#pi^{-}")'

root -l -b -q  combinePlotsJ_slc.C

#------------

mv ana_etac_slc.root ana_etac_slc_old.root

root -l -b -q  'analyse_etac1_slc.C("ntp5_slc_etac1_A.root","chi24c<50",300, "Setup A, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")'
root -l -b -q  'analyse_etac1_slc.C("ntp5_slc_etac1_B.root","chi24c<50",320, "Setup B, #bar{p}p#rightarrow#tilde{#eta_{c1}}#eta")'

root -l -b -q  combinePlotsEtac_slc.C

