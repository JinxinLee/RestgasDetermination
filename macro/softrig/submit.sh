#!/bin/bash

# simple script, which allows you to submit a bunch of jobs to prometheus
# Parameters:
#   <energy>: 24 (2.4GeV), 38 (3.77 GeV), 45 (4.5GeV), 55 (5.5GeV)
#   <sigmim>: Minimum job number
#   <sigmax>: Maximum job number; if not given, jobs array submitted from 0 - <bgmin>
#   <nevt>  : Number of events per job, default = 2000

energy=24
sigmin=1
sigmax=0
nevt=2000

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

echo "SIG jobs: "$sigmin"-"$sigmax

case $energy in
# E_cm = 2.4 GeV, pbarmom = 1.913547
24)
    pmom=1.913547
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof.sge 24000 $nevt M000_2phi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof.sge 24300 $nevt M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 24320 $nevt M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 24340 $nevt M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof.sge 24400 $nevt M400_LamLamb.dec $pmom $res


	;;

# E_cm = 3.77 GeV, pbarmom = 6.56903
38)
    pmom=6.56903
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof.sge 38000 $nevt M000_2phi.dec $pmom $res

	# D0 modes
	qsub -t $sigmin-$sigmax job_sof.sge 38100 $nevt M100_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38101 $nevt M101_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38102 $nevt M102_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38103 $nevt M103_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38104 $nevt M104_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38105 $nevt M105_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38106 $nevt M106_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38107 $nevt M107_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38108 $nevt M108_D0stD0stb.dec $pmom $res

	# D+- modes
	qsub -t $sigmin-$sigmax job_sof.sge 38120 $nevt M120_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38121 $nevt M121_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38122 $nevt M122_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38123 $nevt M123_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38124 $nevt M124_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38125 $nevt M125_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38126 $nevt M126_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38127 $nevt M127_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38128 $nevt M128_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38129 $nevt M129_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38130 $nevt M130_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38131 $nevt M131_DpstDmst.dec $pmom $res

	# J/psi modes
	qsub -t $sigmin-$sigmax job_sof.sge 38200 $nevt M200_Jpsi2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38201 $nevt M201_Jpsi2pi.dec $pmom $res

	# eta_c modes
	qsub -t $sigmin-$sigmax job_sof.sge 38220 $nevt M220_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38221 $nevt M221_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38222 $nevt M222_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38223 $nevt M223_etac2pi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof.sge 38300 $nevt M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38320 $nevt M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 38340 $nevt M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof.sge 38400 $nevt M400_LamLamb.dec $pmom $res
	;;

# E_cm = 4.5 GeV, pbarmom = 9.808065
45)
    pmom=9.808065
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof.sge 45000 $nevt M000_2phi.dec $pmom $res

	# D0 modes
	qsub -t $sigmin-$sigmax job_sof.sge 45100 $nevt M100_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45101 $nevt M101_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45102 $nevt M102_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45103 $nevt M103_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45104 $nevt M104_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45105 $nevt M105_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45106 $nevt M106_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45107 $nevt M107_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45108 $nevt M108_D0stD0stb.dec $pmom $res

	# D+- modes
	qsub -t $sigmin-$sigmax job_sof.sge 45120 $nevt M120_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45121 $nevt M121_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45122 $nevt M122_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45123 $nevt M123_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45124 $nevt M124_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45125 $nevt M125_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45126 $nevt M126_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45127 $nevt M127_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45128 $nevt M128_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45129 $nevt M129_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45130 $nevt M130_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45131 $nevt M131_DpstDmst.dec $pmom $res

	# Ds modes
	qsub -t $sigmin-$sigmax job_sof.sge 45140 $nevt M140_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45141 $nevt M141_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45142 $nevt M142_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45143 $nevt M143_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45144 $nevt M144_DspstDsmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45145 $nevt M145_DspstDsmst.dec $pmom $res

	# J/psi modes
	qsub -t $sigmin-$sigmax job_sof.sge 45200 $nevt M200_Jpsi2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45201 $nevt M201_Jpsi2pi.dec $pmom $res

	# eta_c modes
	qsub -t $sigmin-$sigmax job_sof.sge 45220 $nevt M220_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45221 $nevt M221_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45222 $nevt M222_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45223 $nevt M223_etac2pi.dec $pmom $res

	# chi_c0 modes
	qsub -t $sigmin-$sigmax job_sof.sge 45240 $nevt M240_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45241 $nevt M241_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45242 $nevt M242_chic02pi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof.sge 45300 $nevt M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45320 $nevt M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 45340 $nevt M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof.sge 45400 $nevt M400_LamLamb.dec $pmom $res
	;;

# E_cm = 5.5 GeV, pbarmom = 15.152765
55)
    pmom=15.152765
    res="pbarpSystem0"

	# phi mode
	qsub -t $sigmin-$sigmax job_sof.sge 55000 $nevt M000_2phi.dec $pmom $res

	# D0 modes
	qsub -t $sigmin-$sigmax job_sof.sge 55100 $nevt M100_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55101 $nevt M101_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55102 $nevt M102_D0D0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55103 $nevt M103_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55104 $nevt M104_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55105 $nevt M105_D0stD0b.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55106 $nevt M106_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55107 $nevt M107_D0stD0stb.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55108 $nevt M108_D0stD0stb.dec $pmom $res

	# D+- modes
	qsub -t $sigmin-$sigmax job_sof.sge 55120 $nevt M120_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55121 $nevt M121_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55122 $nevt M122_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55123 $nevt M123_DpDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55124 $nevt M124_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55125 $nevt M125_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55126 $nevt M126_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55127 $nevt M127_DpstDm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55128 $nevt M128_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55129 $nevt M129_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55130 $nevt M130_DpstDmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55131 $nevt M131_DpstDmst.dec $pmom $res

	# Ds modes
	qsub -t $sigmin-$sigmax job_sof.sge 55140 $nevt M140_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55141 $nevt M141_DspDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55142 $nevt M142_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55143 $nevt M143_DspstDsm.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55144 $nevt M144_DspstDsmst.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55145 $nevt M145_DspstDsmst.dec $pmom $res

	# J/psi modes
	qsub -t $sigmin-$sigmax job_sof.sge 55200 $nevt M200_Jpsi2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55201 $nevt M201_Jpsi2pi.dec $pmom $res

	# eta_c modes
	qsub -t $sigmin-$sigmax job_sof.sge 55220 $nevt M220_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55221 $nevt M221_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55222 $nevt M222_etac2pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55223 $nevt M223_etac2pi.dec $pmom $res

	# chi_c0 modes
	qsub -t $sigmin-$sigmax job_sof.sge 55240 $nevt M240_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55241 $nevt M241_chic02pi.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55242 $nevt M242_chic02pi.dec $pmom $res

	# electro-magnetic modes
	qsub -t $sigmin-$sigmax job_sof.sge 55300 $nevt M300_ee.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55320 $nevt M320_mumu.dec $pmom $res
	qsub -t $sigmin-$sigmax job_sof.sge 55340 $nevt M340_gamgam.dec $pmom $res

	# Lambda mode
	qsub -t $sigmin-$sigmax job_sof.sge 55400 $nevt M400_LamLamb.dec $pmom $res

	# Lambda_c mode
	qsub -t $sigmin-$sigmax job_sof.sge 55420 $nevt M420_LamcLamcb.dec $pmom $res
	;;

*)
	echo "Unknown energy!"
esac

