#/bin/bash

par=""
# check which parameters are set
if test "$1" != ""; then
  par=$1
fi

echo $par

# -------------------------------------------------------------------
# Supermodul              |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
# num Alveolen in theta   |  1  |  3  |  3  |  3  |  3  |  3  |  2  |
# coverage from 22 to     |140.0|133.4|113.8| 94.1| 74.4| 54.8| 35.1|
# -------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------
# SIGNAL
# ---------------------------------------------------------------------------------------------------------------------------------

#
# phi -> K+ K-; pbp -> 2 phi (SIGNAL)
#
./resub_arr.pl "sbatch -a1-10 -Jpha1 jobfsim_kronos.sh M2phi_A_EMC1 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-140] ana 0" $par
./resub_arr.pl "sbatch -a1-10 -Jpha2 jobfsim_kronos.sh M2phi_A_EMC2 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-133] ana 1" $par
./resub_arr.pl "sbatch -a1-10 -Jpha3 jobfsim_kronos.sh M2phi_A_EMC3 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-114] ana 2" $par
./resub_arr.pl "sbatch -a1-10 -Jpha4 jobfsim_kronos.sh M2phi_A_EMC4 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-94]  ana 3" $par
./resub_arr.pl "sbatch -a1-10 -Jpha5 jobfsim_kronos.sh M2phi_A_EMC5 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-74]  ana 4" $par
./resub_arr.pl "sbatch -a1-10 -Jpha6 jobfsim_kronos.sh M2phi_A_EMC6 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-55]  ana 5" $par
./resub_arr.pl "sbatch -a1-10 -Jpha7 jobfsim_kronos.sh M2phi_A_EMC7 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-35]  ana 6" $par
./resub_arr.pl "sbatch -a1-10 -Jpha8 jobfsim_kronos.sh M2phi_A_EMC8 10000  decfiles/pp_2phi.dec  -2.3 SetupA:Filter2:EMC[22-20]  ana 7" $par

./resub_arr.pl "sbatch -a1-10 -Jphb1 jobfsim_kronos.sh M2phi_B_EMC1 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-140] ana 20" $par
./resub_arr.pl "sbatch -a1-10 -Jphb2 jobfsim_kronos.sh M2phi_B_EMC2 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-133] ana 21" $par
./resub_arr.pl "sbatch -a1-10 -Jphb3 jobfsim_kronos.sh M2phi_B_EMC3 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-114] ana 22" $par
./resub_arr.pl "sbatch -a1-10 -Jphb4 jobfsim_kronos.sh M2phi_B_EMC4 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-94]  ana 23" $par
./resub_arr.pl "sbatch -a1-10 -Jphb5 jobfsim_kronos.sh M2phi_B_EMC5 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-74]  ana 24" $par
./resub_arr.pl "sbatch -a1-10 -Jphb6 jobfsim_kronos.sh M2phi_B_EMC6 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-55]  ana 25" $par
./resub_arr.pl "sbatch -a1-10 -Jphb7 jobfsim_kronos.sh M2phi_B_EMC7 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-35]  ana 26" $par
./resub_arr.pl "sbatch -a1-10 -Jphb8 jobfsim_kronos.sh M2phi_B_EMC8 10000  decfiles/pp_2phi.dec  -2.3 SetupB:Filter2:EMC[22-20]  ana 27" $par

# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> e+ e-; pbp -> J/psi pi+ pi- (SIGNAL)
#
./resub_arr.pl "sbatch -a1-10 -Jjea1 jobfsim_kronos.sh MJee_A_EMC1 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-140] ana 100" $par
./resub_arr.pl "sbatch -a1-10 -Jjea2 jobfsim_kronos.sh MJee_A_EMC2 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-133] ana 101" $par
./resub_arr.pl "sbatch -a1-10 -Jjea3 jobfsim_kronos.sh MJee_A_EMC3 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-114] ana 102" $par
./resub_arr.pl "sbatch -a1-10 -Jjea4 jobfsim_kronos.sh MJee_A_EMC4 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-94]  ana 103" $par
./resub_arr.pl "sbatch -a1-10 -Jjea5 jobfsim_kronos.sh MJee_A_EMC5 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-74]  ana 104" $par
./resub_arr.pl "sbatch -a1-10 -Jjea6 jobfsim_kronos.sh MJee_A_EMC6 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-55]  ana 105" $par
./resub_arr.pl "sbatch -a1-10 -Jjea7 jobfsim_kronos.sh MJee_A_EMC7 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-35]  ana 106" $par
./resub_arr.pl "sbatch -a1-10 -Jjea8 jobfsim_kronos.sh MJee_A_EMC8 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-20]  ana 107" $par
              
./resub_arr.pl "sbatch -a1-10 -Jjeb1 jobfsim_kronos.sh MJee_B_EMC1 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-140] ana 120" $par 
./resub_arr.pl "sbatch -a1-10 -Jjeb2 jobfsim_kronos.sh MJee_B_EMC2 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-133] ana 121" $par
./resub_arr.pl "sbatch -a1-10 -Jjeb3 jobfsim_kronos.sh MJee_B_EMC3 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-114] ana 122" $par
./resub_arr.pl "sbatch -a1-10 -Jjeb4 jobfsim_kronos.sh MJee_B_EMC4 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-94]  ana 123" $par
./resub_arr.pl "sbatch -a1-10 -Jjeb5 jobfsim_kronos.sh MJee_B_EMC5 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-74]  ana 124" $par
./resub_arr.pl "sbatch -a1-10 -Jjeb6 jobfsim_kronos.sh MJee_B_EMC6 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-55]  ana 125" $par
./resub_arr.pl "sbatch -a1-10 -Jjeb7 jobfsim_kronos.sh MJee_B_EMC7 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-35]  ana 126" $par
./resub_arr.pl "sbatch -a1-10 -Jjeb8 jobfsim_kronos.sh MJee_B_EMC8 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-20]  ana 127" $par

# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> mu+ mu-; pbp -> J/psi pi+ pi- (SIGNAL)
#
./resub_arr.pl "sbatch -a1-10 -Jjma1 jobfsim_kronos.sh MJmm_A_EMC1 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-140] ana 200" $par
./resub_arr.pl "sbatch -a1-10 -Jjma2 jobfsim_kronos.sh MJmm_A_EMC2 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-133] ana 201" $par
./resub_arr.pl "sbatch -a1-10 -Jjma3 jobfsim_kronos.sh MJmm_A_EMC3 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-114] ana 202" $par
./resub_arr.pl "sbatch -a1-10 -Jjma4 jobfsim_kronos.sh MJmm_A_EMC4 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-94]  ana 203" $par
./resub_arr.pl "sbatch -a1-10 -Jjma5 jobfsim_kronos.sh MJmm_A_EMC5 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-74]  ana 204" $par
./resub_arr.pl "sbatch -a1-10 -Jjma6 jobfsim_kronos.sh MJmm_A_EMC6 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-55]  ana 205" $par
./resub_arr.pl "sbatch -a1-10 -Jjma7 jobfsim_kronos.sh MJmm_A_EMC7 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-35]  ana 206" $par
./resub_arr.pl "sbatch -a1-10 -Jjma8 jobfsim_kronos.sh MJmm_A_EMC8 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-20]  ana 207" $par
                                  
./resub_arr.pl "sbatch -a1-10 -Jjmb1 jobfsim_kronos.sh MJmm_B_EMC1 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-140] ana 220" $par 
./resub_arr.pl "sbatch -a1-10 -Jjmb2 jobfsim_kronos.sh MJmm_B_EMC2 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-133] ana 221" $par
./resub_arr.pl "sbatch -a1-10 -Jjmb3 jobfsim_kronos.sh MJmm_B_EMC3 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-114] ana 222" $par
./resub_arr.pl "sbatch -a1-10 -Jjmb4 jobfsim_kronos.sh MJmm_B_EMC4 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-94]  ana 223" $par
./resub_arr.pl "sbatch -a1-10 -Jjmb5 jobfsim_kronos.sh MJmm_B_EMC5 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-74]  ana 224" $par
./resub_arr.pl "sbatch -a1-10 -Jjmb6 jobfsim_kronos.sh MJmm_B_EMC6 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-55]  ana 225" $par
./resub_arr.pl "sbatch -a1-10 -Jjmb7 jobfsim_kronos.sh MJmm_B_EMC7 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-35]  ana 226" $par
./resub_arr.pl "sbatch -a1-10 -Jjmb8 jobfsim_kronos.sh MJmm_B_EMC8 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-20]  ana 227" $par


#
# ppb -> eta_c1 eta; eta_c1 -> chi_c1 pi0 pi0; chi_c1 -> J/psi gamma; J/psi -> e+ e- (SIGNAL)
#

./resub_arr.pl "sbatch -a1-10 -Jeta1 jobfsim_kronos.sh Metac1_A_EMC1 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-140] ana 300" $par
./resub_arr.pl "sbatch -a1-10 -Jeta2 jobfsim_kronos.sh Metac1_A_EMC2 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-133] ana 301" $par
./resub_arr.pl "sbatch -a1-10 -Jeta3 jobfsim_kronos.sh Metac1_A_EMC3 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-114] ana 302" $par
./resub_arr.pl "sbatch -a1-10 -Jeta4 jobfsim_kronos.sh Metac1_A_EMC4 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-94]  ana 303" $par
./resub_arr.pl "sbatch -a1-10 -Jeta5 jobfsim_kronos.sh Metac1_A_EMC5 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-74]  ana 304" $par
./resub_arr.pl "sbatch -a1-10 -Jeta6 jobfsim_kronos.sh Metac1_A_EMC6 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-55]  ana 305" $par
./resub_arr.pl "sbatch -a1-10 -Jeta7 jobfsim_kronos.sh Metac1_A_EMC7 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-35]  ana 306" $par
./resub_arr.pl "sbatch -a1-10 -Jeta8 jobfsim_kronos.sh Metac1_A_EMC8 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-20]  ana 307" $par
                                                                                                                                              
./resub_arr.pl "sbatch -a1-10 -Jetb1 jobfsim_kronos.sh Metac1_B_EMC1 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-140] ana 320" $par
./resub_arr.pl "sbatch -a1-10 -Jetb2 jobfsim_kronos.sh Metac1_B_EMC2 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-133] ana 321" $par
./resub_arr.pl "sbatch -a1-10 -Jetb3 jobfsim_kronos.sh Metac1_B_EMC3 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-114] ana 322" $par
./resub_arr.pl "sbatch -a1-10 -Jetb4 jobfsim_kronos.sh Metac1_B_EMC4 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-94]  ana 323" $par
./resub_arr.pl "sbatch -a1-10 -Jetb5 jobfsim_kronos.sh Metac1_B_EMC5 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-74]  ana 324" $par
./resub_arr.pl "sbatch -a1-10 -Jetb6 jobfsim_kronos.sh Metac1_B_EMC6 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-55]  ana 325" $par
./resub_arr.pl "sbatch -a1-10 -Jetb7 jobfsim_kronos.sh Metac1_B_EMC7 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-35]  ana 326" $par
./resub_arr.pl "sbatch -a1-10 -Jetb8 jobfsim_kronos.sh Metac1_B_EMC8 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-20]  ana 327" $par


# ---------------------------------------------------------------------------------------------------------------------------------
# BACKGROUND
# ---------------------------------------------------------------------------------------------------------------------------------

# Full EMC simulations first

#./resub_arr.pl "sbatch -a1-200 -Jbeta1 jobfsim_kronos.sh DPMetac1_A_EMC1 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-140] ana 1300" $par
#./resub_arr.pl "sbatch -a1-200 -Jbetb1 jobfsim_kronos.sh DPMetac1_B_EMC1 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-140] ana 1320" $par
               
#./resub_arr.pl "sbatch -a1-700 -Jbjea1 jobfsim_kronos.sh DPMJmm_A_EMC1 100000  DPM  -3.872 SetupA:Filter1:EMC[22-140] ana 1200" $par
#./resub_arr.pl "sbatch -a1-700 -Jbjeb1 jobfsim_kronos.sh DPMJmm_B_EMC1 100000  DPM  -3.872 SetupB:Filter1:EMC[22-140] ana 1220" $par
               
#./resub_arr.pl "sbatch -a1-700 -Jbjma1 jobfsim_kronos.sh DPMJee_A_EMC1 100000  DPM  -3.872 SetupA:Filter0:EMC[22-140] ana 1100" $par
#./resub_arr.pl "sbatch -a1-700 -Jbjmb1 jobfsim_kronos.sh DPMJee_B_EMC1 100000  DPM  -3.872 SetupB:Filter0:EMC[22-140] ana 1120" $par

#./resub_arr.pl "sbatch -a1-1000 -Jbpha1 jobfsim_kronos.sh DPM2phi_A_EMC1 500000  DPM  -2.3 SetupA:Filter2:EMC[22-140] ana 1000" $par
#./resub_arr.pl "sbatch -a1-1000 -Jbphb1 jobfsim_kronos.sh DPM2phi_B_EMC1 500000  DPM  -2.3 SetupB:Filter2:EMC[22-140] ana 1020" $par

# ---------------------------------------------------------------------------------------------------------------------------------

#
# ppb -> eta_c1 eta; eta_c1 -> chi_c1 pi0 pi0; chi_c1 -> J/psi gamma; J/psi -> e+ e- (BACKGROUND)
#

./resub_arr.pl "sbatch -a1-200 -Jbeta1 jobfsim_kronos.sh DPMetac1_A_EMC1 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-140] ana 1300" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta2 jobfsim_kronos.sh DPMetac1_A_EMC2 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-133] ana 1301" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta3 jobfsim_kronos.sh DPMetac1_A_EMC3 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-114] ana 1302" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta4 jobfsim_kronos.sh DPMetac1_A_EMC4 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-94]  ana 1303" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta5 jobfsim_kronos.sh DPMetac1_A_EMC5 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-74]  ana 1304" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta6 jobfsim_kronos.sh DPMetac1_A_EMC6 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-55]  ana 1305" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta7 jobfsim_kronos.sh DPMetac1_A_EMC7 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-35]  ana 1306" $par
./resub_arr.pl "sbatch -a1-200 -Jbeta8 jobfsim_kronos.sh DPMetac1_A_EMC8 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-20]  ana 1307" $par

./resub_arr.pl "sbatch -a1-200 -Jbetb1 jobfsim_kronos.sh DPMetac1_B_EMC1 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-140] ana 1320" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb2 jobfsim_kronos.sh DPMetac1_B_EMC2 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-133] ana 1321" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb3 jobfsim_kronos.sh DPMetac1_B_EMC3 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-114] ana 1322" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb4 jobfsim_kronos.sh DPMetac1_B_EMC4 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-94]  ana 1323" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb5 jobfsim_kronos.sh DPMetac1_B_EMC5 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-74]  ana 1324" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb6 jobfsim_kronos.sh DPMetac1_B_EMC6 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-55]  ana 1325" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb7 jobfsim_kronos.sh DPMetac1_B_EMC7 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-35]  ana 1326" $par
./resub_arr.pl "sbatch -a1-200 -Jbetb8 jobfsim_kronos.sh DPMetac1_B_EMC8 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-20]  ana 1327" $par

# ---------------------------------------------------------------------------------------------------------------------------------

#
# phi -> K+ K-; pbp -> 2 phi (BACKGROUND)
#
./resub_arr.pl "sbatch -a1-1000 -Jbpha1 jobfsim_kronos.sh DPM2phi_A_EMC1 500000  DPM  -2.3 SetupA:Filter2:EMC[22-140] ana 1000" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha2 jobfsim_kronos.sh DPM2phi_A_EMC2 500000  DPM  -2.3 SetupA:Filter2:EMC[22-134] ana 1001" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha3 jobfsim_kronos.sh DPM2phi_A_EMC3 500000  DPM  -2.3 SetupA:Filter2:EMC[22-114] ana 1002" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha4 jobfsim_kronos.sh DPM2phi_A_EMC4 500000  DPM  -2.3 SetupA:Filter2:EMC[22-94]  ana 1003" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha5 jobfsim_kronos.sh DPM2phi_A_EMC5 500000  DPM  -2.3 SetupA:Filter2:EMC[22-74]  ana 1004" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha6 jobfsim_kronos.sh DPM2phi_A_EMC6 500000  DPM  -2.3 SetupA:Filter2:EMC[22-55]  ana 1005" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha7 jobfsim_kronos.sh DPM2phi_A_EMC7 500000  DPM  -2.3 SetupA:Filter2:EMC[22-35]  ana 1006" $par
./resub_arr.pl "sbatch -a1-1000 -Jbpha8 jobfsim_kronos.sh DPM2phi_A_EMC8 500000  DPM  -2.3 SetupA:Filter2:EMC[22-20]  ana 1007" $par
                                
./resub_arr.pl "sbatch -a1-1000 -Jbphb1 jobfsim_kronos.sh DPM2phi_B_EMC1 500000  DPM  -2.3 SetupB:Filter2:EMC[22-140] ana 1020" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb2 jobfsim_kronos.sh DPM2phi_B_EMC2 500000  DPM  -2.3 SetupB:Filter2:EMC[22-134] ana 1021" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb3 jobfsim_kronos.sh DPM2phi_B_EMC3 500000  DPM  -2.3 SetupB:Filter2:EMC[22-114] ana 1022" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb4 jobfsim_kronos.sh DPM2phi_B_EMC4 500000  DPM  -2.3 SetupB:Filter2:EMC[22-94]  ana 1023" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb5 jobfsim_kronos.sh DPM2phi_B_EMC5 500000  DPM  -2.3 SetupB:Filter2:EMC[22-74]  ana 1024" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb6 jobfsim_kronos.sh DPM2phi_B_EMC6 500000  DPM  -2.3 SetupB:Filter2:EMC[22-55]  ana 1025" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb7 jobfsim_kronos.sh DPM2phi_B_EMC7 500000  DPM  -2.3 SetupB:Filter2:EMC[22-35]  ana 1026" $par
./resub_arr.pl "sbatch -a1-1000 -Jbphb8 jobfsim_kronos.sh DPM2phi_B_EMC8 500000  DPM  -2.3 SetupB:Filter2:EMC[22-20]  ana 1027" $par



# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> e+ e-; pbp -> J/psi pi+ pi- (BACKGROUND)
#
./resub_arr.pl "sbatch -a1-500 -Jbjea1  jobfsim_kronos.sh DPMJee_A_EMC1 200000  DPM  -3.872 SetupA:Filter0:EMC[22-140] ana 1100" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea2  jobfsim_kronos.sh DPMJee_A_EMC2 100000  DPM  -3.872 SetupA:Filter0:EMC[22-133] ana 1101" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea3  jobfsim_kronos.sh DPMJee_A_EMC3 100000  DPM  -3.872 SetupA:Filter0:EMC[22-114] ana 1102" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea4  jobfsim_kronos.sh DPMJee_A_EMC4 100000  DPM  -3.872 SetupA:Filter0:EMC[22-94]  ana 1103" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea5  jobfsim_kronos.sh DPMJee_A_EMC5 100000  DPM  -3.872 SetupA:Filter0:EMC[22-74]  ana 1104" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea6  jobfsim_kronos.sh DPMJee_A_EMC6 100000  DPM  -3.872 SetupA:Filter0:EMC[22-55]  ana 1105" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea7  jobfsim_kronos.sh DPMJee_A_EMC7 100000  DPM  -3.872 SetupA:Filter0:EMC[22-35]  ana 1106" $par
./resub_arr.pl "sbatch -a1-700 -Jbjea8  jobfsim_kronos.sh DPMJee_A_EMC8 100000  DPM  -3.872 SetupA:Filter0:EMC[22-20]  ana 1107" $par
                               
./resub_arr.pl "sbatch -a1-700 -Jbjeb1  jobfsim_kronos.sh DPMJee_B_EMC1 100000  DPM  -3.872 SetupB:Filter0:EMC[22-140] ana 1120" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb2  jobfsim_kronos.sh DPMJee_B_EMC2 100000  DPM  -3.872 SetupB:Filter0:EMC[22-133] ana 1121" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb3  jobfsim_kronos.sh DPMJee_B_EMC3 100000  DPM  -3.872 SetupB:Filter0:EMC[22-114] ana 1122" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb4  jobfsim_kronos.sh DPMJee_B_EMC4 100000  DPM  -3.872 SetupB:Filter0:EMC[22-94]  ana 1123" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb5  jobfsim_kronos.sh DPMJee_B_EMC5 100000  DPM  -3.872 SetupB:Filter0:EMC[22-74]  ana 1124" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb6  jobfsim_kronos.sh DPMJee_B_EMC6 100000  DPM  -3.872 SetupB:Filter0:EMC[22-55]  ana 1125" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb7  jobfsim_kronos.sh DPMJee_B_EMC7 100000  DPM  -3.872 SetupB:Filter0:EMC[22-35]  ana 1126" $par
./resub_arr.pl "sbatch -a1-700 -Jbjeb8  jobfsim_kronos.sh DPMJee_B_EMC8 100000  DPM  -3.872 SetupB:Filter0:EMC[22-20]  ana 1127" $par


# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> mu+ mu-; pbp -> J/psi pi+ pi- (BACKGROUND)
#
./resub_arr.pl "sbatch -a1-700 -Jbjma1  jobfsim_kronos.sh DPMJmm_A_EMC1 100000  DPM  -3.872 SetupA:Filter1:EMC[22-140] ana 1200" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma2  jobfsim_kronos.sh DPMJmm_A_EMC2 100000  DPM  -3.872 SetupA:Filter1:EMC[22-133] ana 1201" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma3  jobfsim_kronos.sh DPMJmm_A_EMC3 100000  DPM  -3.872 SetupA:Filter1:EMC[22-114] ana 1202" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma4  jobfsim_kronos.sh DPMJmm_A_EMC4 100000  DPM  -3.872 SetupA:Filter1:EMC[22-94]  ana 1203" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma5  jobfsim_kronos.sh DPMJmm_A_EMC5 100000  DPM  -3.872 SetupA:Filter1:EMC[22-74]  ana 1204" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma6  jobfsim_kronos.sh DPMJmm_A_EMC6 100000  DPM  -3.872 SetupA:Filter1:EMC[22-55]  ana 1205" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma7  jobfsim_kronos.sh DPMJmm_A_EMC7 100000  DPM  -3.872 SetupA:Filter1:EMC[22-35]  ana 1206" $par
./resub_arr.pl "sbatch -a1-700 -Jbjma8  jobfsim_kronos.sh DPMJmm_A_EMC8 100000  DPM  -3.872 SetupA:Filter1:EMC[22-20]  ana 1207" $par
                                   
./resub_arr.pl "sbatch -a1-700 -Jbjmb1  jobfsim_kronos.sh DPMJmm_B_EMC1 100000  DPM  -3.872 SetupB:Filter1:EMC[22-140] ana 1220" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb2  jobfsim_kronos.sh DPMJmm_B_EMC2 100000  DPM  -3.872 SetupB:Filter1:EMC[22-133] ana 1221" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb3  jobfsim_kronos.sh DPMJmm_B_EMC3 100000  DPM  -3.872 SetupB:Filter1:EMC[22-114] ana 1222" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb4  jobfsim_kronos.sh DPMJmm_B_EMC4 100000  DPM  -3.872 SetupB:Filter1:EMC[22-94]  ana 1223" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb5  jobfsim_kronos.sh DPMJmm_B_EMC5 100000  DPM  -3.872 SetupB:Filter1:EMC[22-74]  ana 1224" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb6  jobfsim_kronos.sh DPMJmm_B_EMC6 100000  DPM  -3.872 SetupB:Filter1:EMC[22-55]  ana 1225" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb7  jobfsim_kronos.sh DPMJmm_B_EMC7 100000  DPM  -3.872 SetupB:Filter1:EMC[22-35]  ana 1226" $par
./resub_arr.pl "sbatch -a1-700 -Jbjmb8  jobfsim_kronos.sh DPMJmm_B_EMC8 100000  DPM  -3.872 SetupB:Filter1:EMC[22-20]  ana 1227" $par




