root -l -b -q 'merge.C("ntp1","analysis/ntp1_slc_Jee_A.root","data/MJee_A_SLC*_ana.root","data/DPMJee_A_SLC*_ana.root")'
root -l -b -q 'merge.C("ntp1","analysis/ntp1_slc_Jee_B.root","data/MJee_B_SLC*_ana.root","data/DPMJee_B_SLC*_ana.root")'

root -l -b -q 'merge.C("ntp1","analysis/ntp1_slc_Jmm_A.root","data/MJmm_A_SLC*_ana.root","data/DPMJmm_A_SLC*_ana.root")'
root -l -b -q 'merge.C("ntp1","analysis/ntp1_slc_Jmm_B.root","data/MJmm_B_SLC*_ana.root","data/DPMJmm_B_SLC*_ana.root")'

root -l -b -q 'merge.C("ntp5","analysis/ntp5_slc_etac1_A.root","data/Metac1_A_SLC*_ana.root","data/DPMetac1_A_SLC*_ana.root")'
root -l -b -q 'merge.C("ntp5","analysis/ntp5_slc_etac1_B.root","data/Metac1_B_SLC*_ana.root","data/DPMetac1_B_SLC*_ana.root")'



#root -l -b -q 'merge.C("ntp1","analysis/ntp1_Jee_A.root","data/MJee_A_EMC*_ana.root","data/DPMJee_A_EMC*_ana_*.root")'  # CAUTION: this operates in analysis output created with anasub.pl, not the direct one
#root -l -b -q 'merge.C("ntp1","analysis/ntp1_Jee_B.root","data/MJee_B_EMC*_ana.root","data/DPMJee_B_EMC*_ana_*.root")'  # CAUTION: this operates in analysis output created with anasub.pl, not the direct one

#root -l -b -q 'merge.C("ntp1","analysis/ntp1_Jmm_A.root","data/MJmm_A_EMC*_ana.root","data/DPMJmm_A_EMC*_ana_*.root")'  # CAUTION: this operates in analysis output created with anasub.pl, not the direct one
#root -l -b -q 'merge.C("ntp1","analysis/ntp1_Jmm_B.root","data/MJmm_B_EMC*_ana.root","data/DPMJmm_B_EMC*_ana_*.root")'  # CAUTION: this operates in analysis output created with anasub.pl, not the direct one

#root -l -b -q 'merge.C("ntp5","analysis/ntp5_etac1_A.root","data/Metac1_A_EMC*_ana.root","data/DPMetac1_A_EMC*_ana_*.root")'  # CAUTION: this operates in analysis output created with anasub.pl, not the direct one
#root -l -b -q 'merge.C("ntp5","analysis/ntp5_etac1_B.root","data/Metac1_B_EMC*_ana.root","data/DPMetac1_B_EMC*_ana_*.root")'  # CAUTION: this operates in analysis output created with anasub.pl, not the direct one
