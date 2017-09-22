root -l -b -q 'merge.C("ntp1","ntp1_2phi_A.root","data/M2phi_B_EMC*_ana.root","data/DPM2phi__EMC*_ana_*.root")'

root -l -b -q 'merge.C("ntp1","ntp1_Jee_A.root","data/MJee_A_EMC*_ana.root","data/DPMJee_A_EMC*_ana_*.root")'
root -l -b -q 'merge.C("ntp1","ntp1_Jee_B.root","data/MJee_B_EMC*_ana.root","data/DPMJee_B_EMC*_ana_*.root")'

root -l -b -q 'merge.C("ntp1","ntp1_Jmm_A.root","data/MJmm_A_EMC*_ana.root","data/DPMJmm_A_EMC*_ana_*.root")'
root -l -b -q 'merge.C("ntp1","ntp1_Jmm_B.root","data/MJmm_B_EMC*_ana.root","data/DPMJmm_B_EMC*_ana_*.root")'

root -l -b -q 'merge.C("ntp5","ntp5_etac1_A.root","data/Metac1_A_EMC*_ana.root","data/DPMetac1_A_EMC*_ana_*.root")'
root -l -b -q 'merge.C("ntp5","ntp5_etac1_B.root","data/Metac1_B_EMC*_ana.root","data/DPMetac1_B_EMC*_ana_*.root")'
