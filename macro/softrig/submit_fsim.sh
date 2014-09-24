#!/bin/bash

# simple script, which allows you to submit a bunch of jobs to prometheus
# Parameters:
#   <energy>: 24 (2.4GeV), 38 (3.77 GeV), 45 (4.5GeV), 55 (5.5GeV)
#   <sigmim>: Minimum job number
#   <sigmax>: Maximum job number; if not given, jobs array submitted from 0 - <bgmin>
#   <nevt>  : Number of events per job, default = 2000

energy=24
sigmin=1
sigmax=1
nevt=100000

if test "$1" != ""; then
  energy=$1
fi

if test "$2" != ""; then
  sigmin=$2
fi

if test "$3" != ""; then
  sigmax=$3
fi

if test "$4" != ""; then
  nevt=$4
fi

if [ "$sigmax" -lt "$sigmin" ]; then
  sigmax=$sigmin
  sigmin=1
fi

echo "Fsim SIG jobs: "$sigmin"-"$sigmax

case $energy in
# E_cm = 2.4 GeV, pbarmom = 1.913547
24)
    pmom=1.913547
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 24000 $nevt decfiles/M000_2phi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 24300 $nevt decfiles/M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 24320 $nevt decfiles/M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 24340 $nevt decfiles/M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 24400 $nevt decfiles/M400_LamLamb.dec $pmom $res


	;;

# E_cm = 3.77 GeV, pbarmom = 6.56903
38)
    pmom=6.56903
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38000 $nevt decfiles/M000_2phi.dec $pmom $res

	# D0 modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38100 $nevt decfiles/M100_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38101 $nevt decfiles/M101_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38102 $nevt decfiles/M102_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38109 $nevt decfiles/M109_D0D0b.dec $pmom $res

	# D+- modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38120 $nevt decfiles/M120_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38121 $nevt decfiles/M121_DpDm.dec $pmom $res
#	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38122 $nevt decfiles/M122_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38123 $nevt decfiles/M123_DpDm.dec $pmom $res

	# J/psi modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38200 $nevt decfiles/M200_Jpsi2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38201 $nevt decfiles/M201_Jpsi2pi.dec $pmom $res

	# eta_c modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38220 $nevt decfiles/M220_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38221 $nevt decfiles/M221_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38222 $nevt decfiles/M222_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38223 $nevt decfiles/M223_etac2pi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38300 $nevt decfiles/M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38320 $nevt decfiles/M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38340 $nevt decfiles/M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 38400 $nevt decfiles/M400_LamLamb.dec $pmom $res
	;;

# E_cm = 4.5 GeV, pbarmom = 9.808065
45)
    pmom=9.808065
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45000 $nevt decfiles/M000_2phi.dec $pmom $res

	# D0 modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45100 $nevt decfiles/M100_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45101 $nevt decfiles/M101_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45102 $nevt decfiles/M102_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45103 $nevt decfiles/M103_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45104 $nevt decfiles/M104_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45105 $nevt decfiles/M105_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45106 $nevt decfiles/M106_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45107 $nevt decfiles/M107_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45108 $nevt decfiles/M108_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45109 $nevt decfiles/M109_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45110 $nevt decfiles/M110_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45111 $nevt decfiles/M111_D0stD0stb.dec $pmom $res

	# D+- modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45120 $nevt decfiles/M120_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45121 $nevt decfiles/M121_DpDm.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45122 $nevt decfiles/M122_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45123 $nevt decfiles/M123_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45124 $nevt decfiles/M124_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45125 $nevt decfiles/M125_DpstDm.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45126 $nevt decfiles/M126_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45127 $nevt decfiles/M127_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45128 $nevt decfiles/M128_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45129 $nevt decfiles/M129_DpstDmst.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45130 $nevt decfiles/M130_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45131 $nevt decfiles/M131_DpstDmst.dec $pmom $res

	# Ds modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45140 $nevt decfiles/M140_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45141 $nevt decfiles/M141_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45142 $nevt decfiles/M142_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45143 $nevt decfiles/M143_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45144 $nevt decfiles/M144_DspstDsmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45145 $nevt decfiles/M145_DspstDsmst.dec $pmom $res

	# J/psi modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45200 $nevt decfiles/M200_Jpsi2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45201 $nevt decfiles/M201_Jpsi2pi.dec $pmom $res

	# eta_c modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45220 $nevt decfiles/M220_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45221 $nevt decfiles/M221_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45222 $nevt decfiles/M222_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45223 $nevt decfiles/M223_etac2pi.dec $pmom $res

	# chi_c0 modes
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45240 $nevt decfiles/M240_chic02pi.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45241 $nevt decfiles/M241_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45242 $nevt decfiles/M242_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45243 $nevt decfiles/M243_chic02pi.dec $pmom $res

	# h_c modes
 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45260 $nevt decfiles/M260_hc2pi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45300 $nevt decfiles/M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45320 $nevt decfiles/M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45340 $nevt decfiles/M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45400 $nevt decfiles/M400_LamLamb.dec $pmom $res
	;;

# E_cm = 5.5 GeV, pbarmom = 15.152765
55)
    pmom=15.152765
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55000 $nevt decfiles/M000_2phi.dec $pmom $res

	# D0 modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55100 $nevt decfiles/M100_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55101 $nevt decfiles/M101_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55102 $nevt decfiles/M102_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55103 $nevt decfiles/M103_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55104 $nevt decfiles/M104_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55105 $nevt decfiles/M105_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55106 $nevt decfiles/M106_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55107 $nevt decfiles/M107_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55108 $nevt decfiles/M108_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55109 $nevt decfiles/M109_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55110 $nevt decfiles/M110_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55111 $nevt decfiles/M111_D0stD0stb.dec $pmom $res

	# D+- modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55120 $nevt decfiles/M120_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55121 $nevt decfiles/M121_DpDm.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55122 $nevt decfiles/M122_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55123 $nevt decfiles/M123_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55124 $nevt decfiles/M124_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55125 $nevt decfiles/M125_DpstDm.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55126 $nevt decfiles/M126_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55127 $nevt decfiles/M127_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55128 $nevt decfiles/M128_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55129 $nevt decfiles/M129_DpstDmst.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55130 $nevt decfiles/M130_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55131 $nevt decfiles/M131_DpstDmst.dec $pmom $res

	# Ds modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55140 $nevt decfiles/M140_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55141 $nevt decfiles/M141_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55142 $nevt decfiles/M142_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55143 $nevt decfiles/M143_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55144 $nevt decfiles/M144_DspstDsmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55145 $nevt decfiles/M145_DspstDsmst.dec $pmom $res

	# J/psi modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55200 $nevt decfiles/M200_Jpsi2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55201 $nevt decfiles/M201_Jpsi2pi.dec $pmom $res

	# eta_c modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55220 $nevt decfiles/M220_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55221 $nevt decfiles/M221_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55222 $nevt decfiles/M222_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55223 $nevt decfiles/M223_etac2pi.dec $pmom $res

	# chi_c0 modes
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55240 $nevt decfiles/M240_chic02pi.dec $pmom $res
# 	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55241 $nevt decfiles/M241_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55242 $nevt decfiles/M242_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45243 $nevt decfiles/M243_chic02pi.dec $pmom $res

	# h_c modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 45260 $nevt decfiles/M260_hc2pi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55300 $nevt decfiles/M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55320 $nevt decfiles/M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55340 $nevt decfiles/M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55400 $nevt decfiles/M400_LamLamb.dec $pmom $res

	# Lambda_c mode
	qsub -t $sigmin-$sigmax job_sof_fsim.sge 55420 $nevt decfiles/M420_LamcLamcb.dec $pmom $res
	;;

*)
	echo "Unknown energy!"
esac

