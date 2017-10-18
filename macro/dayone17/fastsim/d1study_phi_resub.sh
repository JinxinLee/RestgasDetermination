#/bin/bash

par=""
# check which parameters are set
if test "$1" != ""; then
  par=$1
fi

echo $par

#
# -------------------------------------------------------------------
# 16 phi slices, added symmetrically
# -------------------------------------------------------------------
# ---------------------------------------------------------------------------------------------------------------------------------
# SIGNAL
# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> e+ e-; pbp -> J/psi pi+ pi- (SIGNAL)
#

./resub_arr.pl "sbatch -a1-10 -Jsjea1 jobfsim_kronos.sh MJee_A_SLC1 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-180] ana 100" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea2 jobfsim_kronos.sh MJee_A_SLC2 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-158] ana 101" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea3 jobfsim_kronos.sh MJee_A_SLC3 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-135] ana 102" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea4 jobfsim_kronos.sh MJee_A_SLC4 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-113] ana 103" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea5 jobfsim_kronos.sh MJee_A_SLC5 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-90]  ana 104" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea6 jobfsim_kronos.sh MJee_A_SLC6 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-68]  ana 105" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea7 jobfsim_kronos.sh MJee_A_SLC7 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-45]  ana 106" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea8 jobfsim_kronos.sh MJee_A_SLC8 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:SLC[0-23]  ana 107" $par
./resub_arr.pl "sbatch -a1-10 -Jsjea9 jobfsim_kronos.sh MJee_A_SLC9 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupA:Filter0:EMC[22-20] ana 108" $par

./resub_arr.pl "sbatch -a1-10 -Jsjeb1 jobfsim_kronos.sh MJee_B_SLC1 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-180] ana 120" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb2 jobfsim_kronos.sh MJee_B_SLC2 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-158] ana 121" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb3 jobfsim_kronos.sh MJee_B_SLC3 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-135] ana 122" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb4 jobfsim_kronos.sh MJee_B_SLC4 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-113] ana 123" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb5 jobfsim_kronos.sh MJee_B_SLC5 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-90]  ana 124" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb6 jobfsim_kronos.sh MJee_B_SLC6 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-68]  ana 125" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb7 jobfsim_kronos.sh MJee_B_SLC7 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-45]  ana 126" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb8 jobfsim_kronos.sh MJee_B_SLC8 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:SLC[0-23]  ana 127" $par
./resub_arr.pl "sbatch -a1-10 -Jsjeb9 jobfsim_kronos.sh MJee_B_SLC9 10000  decfiles/pp_Jpsi2pi_Jpsi_ee.dec  -3.872 SetupB:Filter0:EMC[22-20] ana 128" $par


# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> mu+ mu-; pbp -> J/psi pi+ pi- (SIGNAL)
#

./resub_arr.pl "sbatch -a1-10 -Jsjma1 jobfsim_kronos.sh MJmm_A_SLC1 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-180] ana 200" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma2 jobfsim_kronos.sh MJmm_A_SLC2 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-158] ana 201" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma3 jobfsim_kronos.sh MJmm_A_SLC3 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-135] ana 202" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma4 jobfsim_kronos.sh MJmm_A_SLC4 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-113] ana 203" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma5 jobfsim_kronos.sh MJmm_A_SLC5 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-90]  ana 204" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma6 jobfsim_kronos.sh MJmm_A_SLC6 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-68]  ana 205" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma7 jobfsim_kronos.sh MJmm_A_SLC7 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-45]  ana 206" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma8 jobfsim_kronos.sh MJmm_A_SLC8 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:SLC[0-23]  ana 207" $par
./resub_arr.pl "sbatch -a1-10 -Jsjma9 jobfsim_kronos.sh MJmm_A_SLC9 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupA:Filter1:EMC[22-20] ana 208" $par
                                                                                                                                                      
./resub_arr.pl "sbatch -a1-10 -Jsjmb1 jobfsim_kronos.sh MJmm_B_SLC1 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-180] ana 220" $par 
./resub_arr.pl "sbatch -a1-10 -Jsjmb2 jobfsim_kronos.sh MJmm_B_SLC2 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-158] ana 221" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb3 jobfsim_kronos.sh MJmm_B_SLC3 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-135] ana 222" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb4 jobfsim_kronos.sh MJmm_B_SLC4 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-113] ana 223" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb5 jobfsim_kronos.sh MJmm_B_SLC5 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-90]  ana 224" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb6 jobfsim_kronos.sh MJmm_B_SLC6 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-68]  ana 225" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb7 jobfsim_kronos.sh MJmm_B_SLC7 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-45]  ana 226" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb8 jobfsim_kronos.sh MJmm_B_SLC8 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:SLC[0-23]  ana 227" $par
./resub_arr.pl "sbatch -a1-10 -Jsjmb9 jobfsim_kronos.sh MJmm_B_SLC9 10000  decfiles/pp_Jpsi2pi_Jpsi_mm.dec  -3.872 SetupB:Filter1:EMC[22-20] ana 228" $par


# ---------------------------------------------------------------------------------------------------------------------------------

#
# ppb -> eta_c1 eta; eta_c1 -> chi_c1 pi0 pi0; chi_c1 -> J/psi gamma; J/psi -> e+ e- (SIGNAL)
#

./resub_arr.pl "sbatch -a1-10 -Jseta1 jobfsim_kronos.sh Metac1_A_SLC1 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-180] ana 300" $par
./resub_arr.pl "sbatch -a1-10 -Jseta2 jobfsim_kronos.sh Metac1_A_SLC2 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-158] ana 301" $par
./resub_arr.pl "sbatch -a1-10 -Jseta3 jobfsim_kronos.sh Metac1_A_SLC3 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-135] ana 302" $par
./resub_arr.pl "sbatch -a1-10 -Jseta4 jobfsim_kronos.sh Metac1_A_SLC4 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-113] ana 303" $par
./resub_arr.pl "sbatch -a1-10 -Jseta5 jobfsim_kronos.sh Metac1_A_SLC5 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-90]  ana 304" $par
./resub_arr.pl "sbatch -a1-10 -Jseta6 jobfsim_kronos.sh Metac1_A_SLC6 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-68]  ana 305" $par
./resub_arr.pl "sbatch -a1-10 -Jseta7 jobfsim_kronos.sh Metac1_A_SLC7 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-45]  ana 306" $par
./resub_arr.pl "sbatch -a1-10 -Jseta8 jobfsim_kronos.sh Metac1_A_SLC8 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:SLC[0-23]  ana 307" $par
./resub_arr.pl "sbatch -a1-10 -Jseta9 jobfsim_kronos.sh Metac1_A_SLC9 10000  decfiles/pp_etactilde.dec  -5.5 SetupA:Filter4:EMC[22-20] ana 308" $par
                                                                                                                                                
./resub_arr.pl "sbatch -a1-10 -Jsetb1 jobfsim_kronos.sh Metac1_B_SLC1 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-180] ana 320" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb2 jobfsim_kronos.sh Metac1_B_SLC2 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-158] ana 321" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb3 jobfsim_kronos.sh Metac1_B_SLC3 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-135] ana 322" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb4 jobfsim_kronos.sh Metac1_B_SLC4 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-113] ana 323" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb5 jobfsim_kronos.sh Metac1_B_SLC5 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-90]  ana 324" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb6 jobfsim_kronos.sh Metac1_B_SLC6 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-68]  ana 325" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb7 jobfsim_kronos.sh Metac1_B_SLC7 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-45]  ana 326" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb8 jobfsim_kronos.sh Metac1_B_SLC8 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:SLC[0-23]  ana 327" $par
./resub_arr.pl "sbatch -a1-10 -Jsetb9 jobfsim_kronos.sh Metac1_B_SLC9 10000  decfiles/pp_etactilde.dec  -5.5 SetupB:Filter4:EMC[22-20] ana 328" $par



# ---------------------------------------------------------------------------------------------------------------------------------
# BACKGROUND
# ---------------------------------------------------------------------------------------------------------------------------------

# Full EMC simulations first

./resub_arr.pl "sbatch -a1-100 -Jsbeta1 jobfsim_kronos.sh DPMetac1_A_SLC1 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-180] ana 1300" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb1 jobfsim_kronos.sh DPMetac1_B_SLC1 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-180] ana 1320" $par

./resub_arr.pl "sbatch -a1-700 -Jsbjea1 jobfsim_kronos.sh DPMJee_A_SLC1 100000  DPM  -3.872 SetupA:Filter0:SLC[0-180] ana 1100" $par
./resub_arr.pl "sbatch -a1-700 -Jsbjeb1 jobfsim_kronos.sh DPMJee_B_SLC1 100000  DPM  -3.872 SetupB:Filter0:SLC[0-180] ana 1120" $par

./resub_arr.pl "sbatch -a1-700 -Jsbjma1 jobfsim_kronos.sh DPMJmm_A_SLC1 100000  DPM  -3.872 SetupA:Filter1:SLC[0-180] ana 1200" $par
./resub_arr.pl "sbatch -a1-700 -Jsbjmb1 jobfsim_kronos.sh DPMJmm_B_SLC1 100000  DPM  -3.872 SetupB:Filter1:SLC[0-180] ana 1220" $par


# ---------------------------------------------------------------------------------------------------------------------------------

#
# ppb -> eta_c1 eta; eta_c1 -> chi_c1 pi0 pi0; chi_c1 -> J/psi gamma; J/psi -> e+ e- (BACKGROUND)
#

#sbatch -a1-100 -Jsbeta1 jobfsim_kronos.sh DPMetac1_A_SLC1 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-180] ana 1300
./resub_arr.pl "sbatch -a1-100 -Jsbeta2 jobfsim_kronos.sh DPMetac1_A_SLC2 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-158] ana 1301" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta3 jobfsim_kronos.sh DPMetac1_A_SLC3 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-135] ana 1302" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta4 jobfsim_kronos.sh DPMetac1_A_SLC4 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-113] ana 1303" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta5 jobfsim_kronos.sh DPMetac1_A_SLC5 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-90]  ana 1304" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta6 jobfsim_kronos.sh DPMetac1_A_SLC6 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-68]  ana 1305" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta7 jobfsim_kronos.sh DPMetac1_A_SLC7 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-45]  ana 1306" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta8 jobfsim_kronos.sh DPMetac1_A_SLC8 1000000  DPM  -5.5 SetupA:Filter4:SLC[0-23]  ana 1307" $par
./resub_arr.pl "sbatch -a1-100 -Jsbeta9 jobfsim_kronos.sh DPMetac1_A_SLC9 1000000  DPM  -5.5 SetupA:Filter4:EMC[22-20] ana 1308" $par
                                                                                   
#sbatch -a1-100 -Jsbetb1 jobfsim_kronos.sh DPMetac1_B_SLC1 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-180] ana 1320
./resub_arr.pl "sbatch -a1-100 -Jsbetb2 jobfsim_kronos.sh DPMetac1_B_SLC2 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-158] ana 1321" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb3 jobfsim_kronos.sh DPMetac1_B_SLC3 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-135] ana 1322" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb4 jobfsim_kronos.sh DPMetac1_B_SLC4 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-113] ana 1323" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb5 jobfsim_kronos.sh DPMetac1_B_SLC5 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-90]  ana 1324" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb6 jobfsim_kronos.sh DPMetac1_B_SLC6 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-68]  ana 1325" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb7 jobfsim_kronos.sh DPMetac1_B_SLC7 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-45]  ana 1326" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb8 jobfsim_kronos.sh DPMetac1_B_SLC8 1000000  DPM  -5.5 SetupB:Filter4:SLC[0-23]  ana 1327" $par
./resub_arr.pl "sbatch -a1-100 -Jsbetb9 jobfsim_kronos.sh DPMetac1_B_SLC9 1000000  DPM  -5.5 SetupB:Filter4:EMC[22-20] ana 1328" $par


# ---------------------------------------------------------------------------------------------------------------------------------
# FIRST ROUND
# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> e+ e-; pbp -> J/psi pi+ pi- (BACKGROUND)
#
#sbatch -a1-350 -Jsbjea1 jobfsim_kronos.sh DPMJee_A_SLC1 100000  DPM  -3.872 SetupA:Filter0:SLC[0-180] ana 1100
./resub_arr.pl "sbatch -a1-350 -Jsbjea2 jobfsim_kronos.sh DPMJee_A_SLC2 100000  DPM  -3.872 SetupA:Filter0:SLC[0-158] ana 1101" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea3 jobfsim_kronos.sh DPMJee_A_SLC3 100000  DPM  -3.872 SetupA:Filter0:SLC[0-135] ana 1102" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea4 jobfsim_kronos.sh DPMJee_A_SLC4 100000  DPM  -3.872 SetupA:Filter0:SLC[0-113] ana 1103" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea5 jobfsim_kronos.sh DPMJee_A_SLC5 100000  DPM  -3.872 SetupA:Filter0:SLC[0-90]  ana 1104" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea6 jobfsim_kronos.sh DPMJee_A_SLC6 100000  DPM  -3.872 SetupA:Filter0:SLC[0-68]  ana 1105" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea7 jobfsim_kronos.sh DPMJee_A_SLC7 100000  DPM  -3.872 SetupA:Filter0:SLC[0-45]  ana 1106" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea8 jobfsim_kronos.sh DPMJee_A_SLC8 100000  DPM  -3.872 SetupA:Filter0:SLC[0-23]  ana 1107" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjea9 jobfsim_kronos.sh DPMJee_A_SLC9 100000  DPM  -3.872 SetupA:Filter0:EMC[22-20] ana 1108" $par
                                                                                                                               
#sbatch -a1-350 -Jsbjeb1 jobfsim_kronos.sh DPMJee_B_SLC1 100000  DPM  -3.872 SetupB:Filter0:SLC[0-180] ana 1120                
./resub_arr.pl "sbatch -a1-350 -Jsbjeb2 jobfsim_kronos.sh DPMJee_B_SLC2 100000  DPM  -3.872 SetupB:Filter0:SLC[0-158] ana 1121" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb3 jobfsim_kronos.sh DPMJee_B_SLC3 100000  DPM  -3.872 SetupB:Filter0:SLC[0-135] ana 1122" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb4 jobfsim_kronos.sh DPMJee_B_SLC4 100000  DPM  -3.872 SetupB:Filter0:SLC[0-113] ana 1123" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb5 jobfsim_kronos.sh DPMJee_B_SLC5 100000  DPM  -3.872 SetupB:Filter0:SLC[0-90]  ana 1124" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb6 jobfsim_kronos.sh DPMJee_B_SLC6 100000  DPM  -3.872 SetupB:Filter0:SLC[0-68]  ana 1125" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb7 jobfsim_kronos.sh DPMJee_B_SLC7 100000  DPM  -3.872 SetupB:Filter0:SLC[0-45]  ana 1126" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb8 jobfsim_kronos.sh DPMJee_B_SLC8 100000  DPM  -3.872 SetupB:Filter0:SLC[0-23]  ana 1127" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjeb9 jobfsim_kronos.sh DPMJee_B_SLC9 100000  DPM  -3.872 SetupB:Filter0:EMC[22-20] ana 1128" $par


# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> mu+ mu-; pbp -> J/psi pi+ pi- (BACKGROUND)
#
#sbatch -a1-350 -Jsbjma1 jobfsim_kronos.sh DPMJmm_A_SLC1 100000  DPM  -3.872 SetupA:Filter1:SLC[0-180] ana 1200
./resub_arr.pl "sbatch -a1-350 -Jsbjma2 jobfsim_kronos.sh DPMJmm_A_SLC2 100000  DPM  -3.872 SetupA:Filter1:SLC[0-158] ana 1201" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma3 jobfsim_kronos.sh DPMJmm_A_SLC3 100000  DPM  -3.872 SetupA:Filter1:SLC[0-135] ana 1202" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma4 jobfsim_kronos.sh DPMJmm_A_SLC4 100000  DPM  -3.872 SetupA:Filter1:SLC[0-113] ana 1203" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma5 jobfsim_kronos.sh DPMJmm_A_SLC5 100000  DPM  -3.872 SetupA:Filter1:SLC[0-90]  ana 1204" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma6 jobfsim_kronos.sh DPMJmm_A_SLC6 100000  DPM  -3.872 SetupA:Filter1:SLC[0-68]  ana 1205" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma7 jobfsim_kronos.sh DPMJmm_A_SLC7 100000  DPM  -3.872 SetupA:Filter1:SLC[0-45]  ana 1206" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma8 jobfsim_kronos.sh DPMJmm_A_SLC8 100000  DPM  -3.872 SetupA:Filter1:SLC[0-23]  ana 1207" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjma9 jobfsim_kronos.sh DPMJmm_A_SLC9 100000  DPM  -3.872 SetupA:Filter1:EMC[22-20] ana 1208" $par
                                                                                                                               
#sbatch -a1-350 -Jsbjmb1 jobfsim_kronos.sh DPMJmm_B_SLC1 100000  DPM  -3.872 SetupB:Filter1:SLC[0-180] ana 1220                
./resub_arr.pl "sbatch -a1-350 -Jsbjmb2 jobfsim_kronos.sh DPMJmm_B_SLC2 100000  DPM  -3.872 SetupB:Filter1:SLC[0-158] ana 1221" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb3 jobfsim_kronos.sh DPMJmm_B_SLC3 100000  DPM  -3.872 SetupB:Filter1:SLC[0-135] ana 1222" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb4 jobfsim_kronos.sh DPMJmm_B_SLC4 100000  DPM  -3.872 SetupB:Filter1:SLC[0-113] ana 1223" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb5 jobfsim_kronos.sh DPMJmm_B_SLC5 100000  DPM  -3.872 SetupB:Filter1:SLC[0-90]  ana 1224" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb6 jobfsim_kronos.sh DPMJmm_B_SLC6 100000  DPM  -3.872 SetupB:Filter1:SLC[0-68]  ana 1225" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb7 jobfsim_kronos.sh DPMJmm_B_SLC7 100000  DPM  -3.872 SetupB:Filter1:SLC[0-45]  ana 1226" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb8 jobfsim_kronos.sh DPMJmm_B_SLC8 100000  DPM  -3.872 SetupB:Filter1:SLC[0-23]  ana 1227" $par
./resub_arr.pl "sbatch -a1-350 -Jsbjmb9 jobfsim_kronos.sh DPMJmm_B_SLC9 100000  DPM  -3.872 SetupB:Filter1:EMC[22-20] ana 1228" $par



# ---------------------------------------------------------------------------------------------------------------------------------
# SECOND ROUND
# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> e+ e-; pbp -> J/psi pi+ pi- (BACKGROUND)
#
#sbatch -a351-700 -Jsbjea1 jobfsim_kronos.sh DPMJee_A_SLC1 100000  DPM  -3.872 SetupA:Filter0:SLC[0-180] ana 1100
./resub_arr.pl "sbatch -a351-700 -Jsbjea2 jobfsim_kronos.sh DPMJee_A_SLC2 100000  DPM  -3.872 SetupA:Filter0:SLC[0-158] ana 1101" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea3 jobfsim_kronos.sh DPMJee_A_SLC3 100000  DPM  -3.872 SetupA:Filter0:SLC[0-135] ana 1102" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea4 jobfsim_kronos.sh DPMJee_A_SLC4 100000  DPM  -3.872 SetupA:Filter0:SLC[0-113] ana 1103" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea5 jobfsim_kronos.sh DPMJee_A_SLC5 100000  DPM  -3.872 SetupA:Filter0:SLC[0-90]  ana 1104" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea6 jobfsim_kronos.sh DPMJee_A_SLC6 100000  DPM  -3.872 SetupA:Filter0:SLC[0-68]  ana 1105" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea7 jobfsim_kronos.sh DPMJee_A_SLC7 100000  DPM  -3.872 SetupA:Filter0:SLC[0-45]  ana 1106" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea8 jobfsim_kronos.sh DPMJee_A_SLC8 100000  DPM  -3.872 SetupA:Filter0:SLC[0-23]  ana 1107" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjea9 jobfsim_kronos.sh DPMJee_A_SLC9 100000  DPM  -3.872 SetupA:Filter0:EMC[22-20] ana 1108" $par
                                                                                                                                 
#sbatch -a351-700 -Jsbjeb1 jobfsim_kronos.sh DPMJee_B_SLC1 100000  DPM  -3.872 SetupB:Filter0:SLC[0-180] ana 1120                
./resub_arr.pl "sbatch -a351-700 -Jsbjeb2 jobfsim_kronos.sh DPMJee_B_SLC2 100000  DPM  -3.872 SetupB:Filter0:SLC[0-158] ana 1121" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb3 jobfsim_kronos.sh DPMJee_B_SLC3 100000  DPM  -3.872 SetupB:Filter0:SLC[0-135] ana 1122" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb4 jobfsim_kronos.sh DPMJee_B_SLC4 100000  DPM  -3.872 SetupB:Filter0:SLC[0-113] ana 1123" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb5 jobfsim_kronos.sh DPMJee_B_SLC5 100000  DPM  -3.872 SetupB:Filter0:SLC[0-90]  ana 1124" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb6 jobfsim_kronos.sh DPMJee_B_SLC6 100000  DPM  -3.872 SetupB:Filter0:SLC[0-68]  ana 1125" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb7 jobfsim_kronos.sh DPMJee_B_SLC7 100000  DPM  -3.872 SetupB:Filter0:SLC[0-45]  ana 1126" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb8 jobfsim_kronos.sh DPMJee_B_SLC8 100000  DPM  -3.872 SetupB:Filter0:SLC[0-23]  ana 1127" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjeb9 jobfsim_kronos.sh DPMJee_B_SLC9 100000  DPM  -3.872 SetupB:Filter0:EMC[22-20] ana 1128" $par


# ---------------------------------------------------------------------------------------------------------------------------------

#
# J/psi -> mu+ mu-; pbp -> J/psi pi+ pi- (BACKGROUND)
#
#sbatch -a351-700 -Jsbjma1 jobfsim_kronos.sh DPMJmm_A_SLC1 100000  DPM  -3.872 SetupA:Filter1:SLC[0-180] ana 1200
./resub_arr.pl "sbatch -a351-700 -Jsbjma2 jobfsim_kronos.sh DPMJmm_A_SLC2 100000  DPM  -3.872 SetupA:Filter1:SLC[0-158] ana 1201" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma3 jobfsim_kronos.sh DPMJmm_A_SLC3 100000  DPM  -3.872 SetupA:Filter1:SLC[0-135] ana 1202" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma4 jobfsim_kronos.sh DPMJmm_A_SLC4 100000  DPM  -3.872 SetupA:Filter1:SLC[0-113] ana 1203" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma5 jobfsim_kronos.sh DPMJmm_A_SLC5 100000  DPM  -3.872 SetupA:Filter1:SLC[0-90]  ana 1204" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma6 jobfsim_kronos.sh DPMJmm_A_SLC6 100000  DPM  -3.872 SetupA:Filter1:SLC[0-68]  ana 1205" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma7 jobfsim_kronos.sh DPMJmm_A_SLC7 100000  DPM  -3.872 SetupA:Filter1:SLC[0-45]  ana 1206" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma8 jobfsim_kronos.sh DPMJmm_A_SLC8 100000  DPM  -3.872 SetupA:Filter1:SLC[0-23]  ana 1207" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjma9 jobfsim_kronos.sh DPMJmm_A_SLC9 100000  DPM  -3.872 SetupA:Filter1:EMC[22-20] ana 1208" $par
                                                                                                                                 
#sbatch -a351-700 -Jsbjmb1 jobfsim_kronos.sh DPMJmm_B_SLC1 100000  DPM  -3.872 SetupB:Filter1:SLC[0-180] ana 1220                
./resub_arr.pl "sbatch -a351-700 -Jsbjmb2 jobfsim_kronos.sh DPMJmm_B_SLC2 100000  DPM  -3.872 SetupB:Filter1:SLC[0-158] ana 1221" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb3 jobfsim_kronos.sh DPMJmm_B_SLC3 100000  DPM  -3.872 SetupB:Filter1:SLC[0-135] ana 1222" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb4 jobfsim_kronos.sh DPMJmm_B_SLC4 100000  DPM  -3.872 SetupB:Filter1:SLC[0-113] ana 1223" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb5 jobfsim_kronos.sh DPMJmm_B_SLC5 100000  DPM  -3.872 SetupB:Filter1:SLC[0-90]  ana 1224" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb6 jobfsim_kronos.sh DPMJmm_B_SLC6 100000  DPM  -3.872 SetupB:Filter1:SLC[0-68]  ana 1225" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb7 jobfsim_kronos.sh DPMJmm_B_SLC7 100000  DPM  -3.872 SetupB:Filter1:SLC[0-45]  ana 1226" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb8 jobfsim_kronos.sh DPMJmm_B_SLC8 100000  DPM  -3.872 SetupB:Filter1:SLC[0-23]  ana 1227" $par
./resub_arr.pl "sbatch -a351-700 -Jsbjmb9 jobfsim_kronos.sh DPMJmm_B_SLC9 100000  DPM  -3.872 SetupB:Filter1:EMC[22-20] ana 1228" $par




