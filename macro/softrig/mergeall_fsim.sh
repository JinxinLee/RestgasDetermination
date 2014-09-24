#!/bin/bash

# ../mergeall.sh <energy>
#
# simple script to ../merge the output of the softtrigger task
# Parameters:
#   <energy>: 24 (2.4GeV), 38 (3.77 GeV), 45 (4.5GeV), 55 (5.5GeV)


energy=24

if test "$1" != ""; then
  energy=$1
fi

echo "Merging output for energy: "$energy

case $energy in
# E_cm = 2.4 GeV, pbarmom = 1.913547
24)
	# phi mode
	root -l -b -q '../merge.C("nphi","M24_nphi.root","M24000*_fsim_all.root","M24900*_fsim_all.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M24_n2e.root","M24300*_fsim_all.root","M24900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2mu","M24_n2mu.root","M24320*_fsim_all.root","M24900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2g","M24_n2gam.root","M24340*_fsim_all.root","M24900*_fsim_all.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M24_nlam.root","M24400*_fsim_all.root","M24900*_fsim_all.root")'

	# event tuple
	root -l -b -q '../merge.C("ntpev","M24_ntp.root","M24*_fsim_all.root")'
	;;

38)
	# phi mode
	root -l -b -q '../merge.C("nphi","M38_nphi.root","M38000*_fsim_all.root","M38900*_fsim_all.root")'

	# D0 modes
	root -l -b -q '../merge.C("nd01","M38_nd01.root","M38100*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M38_nd02.root","M38101*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M38_nd03.root","M38102*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M38_nd04.root","M38109*_fsim_all.root","M38900*_fsim_all.root")'

	# D+- modes
	root -l -b -q '../merge.C("ndpm1","M38_ndpm1.root","M38120*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M38_ndpm2.root","M38121*_fsim_all.root","M38900*_fsim_all.root")'
#	root -l -b -q '../merge.C("ndpm3","M38_ndpm3.root","M38122*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M38_ndpm4.root","M38123*_fsim_all.root","M38900*_fsim_all.root")'

	# J/psi modes
	root -l -b -q '../merge.C("njpsi1","M38_njpsi1.root","M38200*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("njpsi2","M38_njpsi2.root","M38201*_fsim_all.root","M38900*_fsim_all.root")'

	# eta_c modes
	root -l -b -q '../merge.C("netac1","M38_netac1.root","M38220*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("netac2","M38_netac2.root","M38221*_fsim_all.root","M38900*_fsim_all.root")'
#	root -l -b -q '../merge.C("netac3","M38_netac3.root","M38222*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("netac4","M38_netac4.root","M38223*_fsim_all.root","M38900*_fsim_all.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M38_n2e.root","M38300*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2mu","M38_n2mu.root","M38320*_fsim_all.root","M38900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2g","M38_n2gam.root","M38340*_fsim_all.root","M38900*_fsim_all.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M38_nlam.root","M38400*_fsim_all.root","M38900*_fsim_all.root")'

	# event tuple
	root -l -b -q '../merge.C("ntpev","M38_ntp.root","M38*_fsim_all.root")'
	;;

45)
	# phi mode
	root -l -b -q '../merge.C("nphi","M45_nphi.root","M45000*_fsim_all.root","M45900*_fsim_all.root")'

	# D0 modes
	root -l -b -q '../merge.C("nd01","M45_nd01.root","M45100*_fsim_all.root","M45103*_fsim_all.root","M45106*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M45_nd02.root","M45101*_fsim_all.root","M45104*_fsim_all.root","M45107*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M45_nd03.root","M45102*_fsim_all.root","M45105*_fsim_all.root","M45108*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M45_nd04.root","M45109*_fsim_all.root","M45110*_fsim_all.root","M45111*_fsim_all.root","M45900*_fsim_all.root")'
	
	# D0 modes single source
	root -l -b -q '../merge.C("nd01","M45100_nd01.root","M45100*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M45101_nd02.root","M45101*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M45102_nd03.root","M45102*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M45109_nd04.root","M45109*_fsim_all.root","M45900*_fsim_all.root")'

	root -l -b -q '../merge.C("nd01","M45103_nd01.root","M45103*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M45104_nd02.root","M45104*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M45105_nd03.root","M45105*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M45110_nd04.root","M45110*_fsim_all.root","M45900*_fsim_all.root")'

	root -l -b -q '../merge.C("nd01","M45106_nd01.root","M45106*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M45107_nd02.root","M45107*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M45108_nd03.root","M45108*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M45111_nd04.root","M45111*_fsim_all.root","M45900*_fsim_all.root")'

	# D+- modes
	root -l -b -q '../merge.C("ndpm1","M45_ndpm1.root","M45120*_fsim_all.root","M45124*_fsim_all.root","M45128*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M45_ndpm2.root","M45121*_fsim_all.root","M45125*_fsim_all.root","M45129*_fsim_all.root","M45900*_fsim_all.root")'
#	root -l -b -q '../merge.C("ndpm3","M45_ndpm3.root","M45122*_fsim_all.root","M45126*_fsim_all.root","M45130*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M45_ndpm4.root","M45123*_fsim_all.root","M45127*_fsim_all.root","M45131*_fsim_all.root","M45900*_fsim_all.root")'

	# D+- modes single source
	root -l -b -q '../merge.C("ndpm1","M45120_ndpm1.root","M45120*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M45121_ndpm2.root","M45121*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M45123_ndpm4.root","M45123*_fsim_all.root","M45900*_fsim_all.root")'

	root -l -b -q '../merge.C("ndpm1","M45124_ndpm1.root","M45124*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M45125_ndpm2.root","M45125*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M45127_ndpm4.root","M45127*_fsim_all.root","M45900*_fsim_all.root")'

	root -l -b -q '../merge.C("ndpm1","M45128_ndpm1.root","M45128*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M45129_ndpm2.root","M45129*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M45131_ndpm4.root","M45131*_fsim_all.root","M45900*_fsim_all.root")'

	# Ds+- modes
	root -l -b -q '../merge.C("nds1","M45_nds1.root","M45140*_fsim_all.root","M45142*_fsim_all.root","M45144*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M45_nds2.root","M45141*_fsim_all.root","M45143*_fsim_all.root","M45145*_fsim_all.root","M45900*_fsim_all.root")'

	# Ds+- modes single source
	root -l -b -q '../merge.C("nds1","M45140_nds1.root","M45140*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M45141_nds2.root","M45141*_fsim_all.root","M45900*_fsim_all.root")'

	root -l -b -q '../merge.C("nds1","M45142_nds1.root","M45142*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M45143_nds2.root","M45143*_fsim_all.root","M45900*_fsim_all.root")'

	root -l -b -q '../merge.C("nds1","M45144_nds1.root","M45144*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M45145_nds2.root","M45145*_fsim_all.root","M45900*_fsim_all.root")'

	# J/psi modes
	root -l -b -q '../merge.C("njpsi1","M45_njpsi1.root","M45200*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("njpsi2","M45_njpsi2.root","M45201*_fsim_all.root","M45900*_fsim_all.root")'

	# eta_c modes
	root -l -b -q '../merge.C("netac1","M45_netac1.root","M45220*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("netac2","M45_netac2.root","M45221*_fsim_all.root","M45900*_fsim_all.root")'
#	root -l -b -q '../merge.C("netac3","M45_netac3.root","M45222*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("netac4","M45_netac4.root","M45223*_fsim_all.root","M45900*_fsim_all.root")'

	# chi_c0 modes
#	root -l -b -q '../merge.C("nchic01","M45_nchic01.root","M45240*_fsim_all.root","M45900*_fsim_all.root")'
#	root -l -b -q '../merge.C("nchic02","M45_nchic02.root","M45241*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nchic03","M45_nchic03.root","M45242*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("nchic04","M45_nchic04.root","M45243*_fsim_all.root","M45900*_fsim_all.root")'

	# h_c modes
	root -l -b -q '../merge.C("nhc","M45_nhc.root","M45260*_fsim_all.root","M45900*_fsim_all.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M45_n2e.root","M45300*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2mu","M45_n2mu.root","M45320*_fsim_all.root","M45900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2g","M45_n2gam.root","M45340*_fsim_all.root","M45900*_fsim_all.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M45_nlam.root","M45400*_fsim_all.root","M45900*_fsim_all.root")'

	# event tuple
	root -l -b -q '../merge.C("ntpev","M45_ntp.root","M45*_fsim_all.root")'
	;;

55)
	# phi mode
	root -l -b -q '../merge.C("nphi","M55_nphi.root","M55000*_fsim_all.root","M55900*_fsim_all.root")'

	# D0 modes
	root -l -b -q '../merge.C("nd01","M55_nd01.root","M55100*_fsim_all.root","M55103*_fsim_all.root","M55106*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M55_nd02.root","M55101*_fsim_all.root","M55104*_fsim_all.root","M55107*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M55_nd03.root","M55102*_fsim_all.root","M55105*_fsim_all.root","M55108*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M55_nd04.root","M55109*_fsim_all.root","M55110*_fsim_all.root","M55111*_fsim_all.root","M55900*_fsim_all.root")'

	# D0 modes single source
	root -l -b -q '../merge.C("nd01","M55100_nd01.root","M55100*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M55101_nd02.root","M55101*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M55102_nd03.root","M55102*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M55109_nd04.root","M55109*_fsim_all.root","M55900*_fsim_all.root")'

	root -l -b -q '../merge.C("nd01","M55103_nd01.root","M55103*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M55104_nd02.root","M55104*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M55105_nd03.root","M55105*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M55110_nd04.root","M55110*_fsim_all.root","M55900*_fsim_all.root")'

	root -l -b -q '../merge.C("nd01","M55106_nd01.root","M55106*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd02","M55107_nd02.root","M55107*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd03","M55108_nd03.root","M55108*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nd04","M55111_nd04.root","M55111*_fsim_all.root","M55900*_fsim_all.root")'

	# D+- modes
	root -l -b -q '../merge.C("ndpm1","M55_ndpm1.root","M55120*_fsim_all.root","M55124*_fsim_all.root","M55128*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M55_ndpm2.root","M55121*_fsim_all.root","M55125*_fsim_all.root","M55129*_fsim_all.root","M55900*_fsim_all.root")'
#	root -l -b -q '../merge.C("ndpm3","M55_ndpm3.root","M55122*_fsim_all.root","M55126*_fsim_all.root","M55130*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M55_ndpm4.root","M55123*_fsim_all.root","M55127*_fsim_all.root","M55131*_fsim_all.root","M55900*_fsim_all.root")'

	# D+- modes single source
	root -l -b -q '../merge.C("ndpm1","M55120_ndpm1.root","M55120*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M55121_ndpm2.root","M55121*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M55123_ndpm4.root","M55123*_fsim_all.root","M55900*_fsim_all.root")'

	root -l -b -q '../merge.C("ndpm1","M55124_ndpm1.root","M55124*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M55125_ndpm2.root","M55125*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M55127_ndpm4.root","M55127*_fsim_all.root","M55900*_fsim_all.root")'

	root -l -b -q '../merge.C("ndpm1","M55128_ndpm1.root","M55128*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm2","M55129_ndpm2.root","M55129*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("ndpm4","M55131_ndpm4.root","M55131*_fsim_all.root","M55900*_fsim_all.root")'

	# Ds+- modes
	root -l -b -q '../merge.C("nds1","M55_nds1.root","M55140*_fsim_all.root","M55142*_fsim_all.root","M55144*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M55_nds2.root","M55141*_fsim_all.root","M55143*_fsim_all.root","M55155*_fsim_all.root","M55900*_fsim_all.root")'

	# Ds+- modes single source
	root -l -b -q '../merge.C("nds1","M55140_nds1.root","M55140*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M55141_nds2.root","M55141*_fsim_all.root","M55900*_fsim_all.root")'

	root -l -b -q '../merge.C("nds1","M55142_nds1.root","M55142*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M55143_nds2.root","M55143*_fsim_all.root","M55900*_fsim_all.root")'

	root -l -b -q '../merge.C("nds1","M55144_nds1.root","M55144*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nds2","M55145_nds2.root","M55145*_fsim_all.root","M55900*_fsim_all.root")'

	# J/psi modes
	root -l -b -q '../merge.C("njpsi1","M55_njpsi1.root","M55200*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("njpsi2","M55_njpsi2.root","M55201*_fsim_all.root","M55900*_fsim_all.root")'

	# eta_c modes
	root -l -b -q '../merge.C("netac1","M55_netac1.root","M55220*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("netac2","M55_netac2.root","M55221*_fsim_all.root","M55900*_fsim_all.root")'
#	root -l -b -q '../merge.C("netac3","M55_netac3.root","M55222*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("netac4","M55_netac4.root","M55223*_fsim_all.root","M55900*_fsim_all.root")'

	# chi_c0 modes
#	root -l -b -q '../merge.C("nchic01","M55_nchic01.root","M55240*_fsim_all.root","M55900*_fsim_all.root")'
#	root -l -b -q '../merge.C("nchic02","M55_nchic02.root","M55241*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nchic03","M55_nchic03.root","M55242*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("nchic04","M55_nchic04.root","M55243*_fsim_all.root","M55900*_fsim_all.root")'

	# h_c modes
	root -l -b -q '../merge.C("nhc","M55_nhc.root","M55260*_fsim_all.root","M55900*_fsim_all.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M55_n2e.root","M55300*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2mu","M55_n2mu.root","M55320*_fsim_all.root","M55900*_fsim_all.root")'
	root -l -b -q '../merge.C("n2g","M55_n2gam.root","M55340*_fsim_all.root","M55900*_fsim_all.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M55_nlam.root","M55400*_fsim_all.root","M55900*_fsim_all.root")'

	# Lambda_c mode
	root -l -b -q '../merge.C("nlamc","M55_nlamc.root","M55420*_fsim_all.root","M55900*_fsim_all.root")'

	# event tuple
	root -l -b -q '../merge.C("ntpev","M55_ntp.root","M55*_fsim_all.root")'
	;;


*)
	"Unknown energy!"
esac
