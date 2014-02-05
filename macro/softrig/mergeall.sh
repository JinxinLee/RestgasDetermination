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
	root -l -b -q '../merge.C("nphi","M24_nphi.root","M24000*sof.root","M24900*sof.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M24_n2e.root","M24300*sof.root","M24900*sof.root")'
	root -l -b -q '../merge.C("n2mu","M24_n2mu.root","M24320*sof.root","M24900*sof.root")'
	root -l -b -q '../merge.C("n2gam","M24_n2gam.root","M24340*sof.root","M24900*sof.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M24_nlam.root","M24400*sof.root","M24900*sof.root")'

	# event tuple
	root -l -b -q '../merge.C("ntp","M24_ntp.root","M24*sof.root")'
	;;

38)
	# phi mode
	root -l -b -q '../merge.C("nphi","M38_nphi.root","M38000*sof.root","M38900*sof.root")'

	# D0 modes
	root -l -b -q '../merge.C("nd01","M38_nd01.root","M38100*sof.root","M38103*sof.root","M38106*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("nd02","M38_nd02.root","M38101*sof.root","M38104*sof.root","M38107*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("nd03","M38_nd03.root","M38102*sof.root","M38105*sof.root","M38108*sof.root","M38900*sof.root")'

	# D+- modes
	root -l -b -q '../merge.C("ndpm1","M38_ndpm1.root","M38120*sof.root","M38124*sof.root","M38128*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("ndpm2","M38_ndpm2.root","M38121*sof.root","M38125*sof.root","M38129*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("ndpm3","M38_ndpm3.root","M38122*sof.root","M38126*sof.root","M38130*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("ndpm4","M38_ndpm4.root","M38123*sof.root","M38127*sof.root","M38131*sof.root","M38900*sof.root")'

	# J/psi modes
	root -l -b -q '../merge.C("njpsi1","M38_njpsi1.root","M38200*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("njpsi2","M38_njpsi2.root","M38201*sof.root","M38900*sof.root")'

	# eta_c modes
	root -l -b -q '../merge.C("netac1","M38_netac1.root","M38220*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("netac2","M38_netac2.root","M38221*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("netac3","M38_netac3.root","M38222*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("netac4","M38_netac4.root","M38223*sof.root","M38900*sof.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M38_n2e.root","M38300*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("n2mu","M38_n2mu.root","M38320*sof.root","M38900*sof.root")'
	root -l -b -q '../merge.C("n2gam","M38_n2gam.root","M38340*sof.root","M38900*sof.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M38_nlam.root","M38400*sof.root","M38900*sof.root")'

	# event tuple
	root -l -b -q '../merge.C("ntp","M38_ntp.root","M38*sof.root")'
	;;

45)
	# phi mode
	root -l -b -q '../merge.C("nphi","M45_nphi.root","M45000*sof.root","M45900*sof.root")'

	# D0 modes
	root -l -b -q '../merge.C("nd01","M45_nd01.root","M45100*sof.root","M45103*sof.root","M45106*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("nd02","M45_nd02.root","M45101*sof.root","M45104*sof.root","M45107*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("nd03","M45_nd03.root","M45102*sof.root","M45105*sof.root","M45108*sof.root","M45900*sof.root")'

	# D+- modes
	root -l -b -q '../merge.C("ndpm1","M45_ndpm1.root","M45120*sof.root","M45124*sof.root","M45128*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("ndpm2","M45_ndpm2.root","M45121*sof.root","M45125*sof.root","M45129*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("ndpm3","M45_ndpm3.root","M45122*sof.root","M45126*sof.root","M45130*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("ndpm4","M45_ndpm4.root","M45123*sof.root","M45127*sof.root","M45131*sof.root","M45900*sof.root")'

	# Ds+- modes
	root -l -b -q '../merge.C("nds1","M45_nds1.root","M45140*sof.root","M45142*sof.root","M45144*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("nds2","M45_nds2.root","M45141*sof.root","M45143*sof.root","M45145*sof.root","M45900*sof.root")'

	# J/psi modes
	root -l -b -q '../merge.C("njpsi1","M45_njpsi1.root","M45200*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("njpsi2","M45_njpsi2.root","M45201*sof.root","M45900*sof.root")'

	# eta_c modes
	root -l -b -q '../merge.C("netac1","M45_netac1.root","M45220*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("netac2","M45_netac2.root","M45221*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("netac3","M45_netac3.root","M45222*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("netac4","M45_netac4.root","M45223*sof.root","M45900*sof.root")'

	# chi_c0 modes
	root -l -b -q '../merge.C("nchic01","M45_nchic01.root","M45240*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("nchic02","M45_nchic02.root","M45241*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("nchic03","M45_nchic03.root","M45242*sof.root","M45900*sof.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M45_n2e.root","M45300*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("n2mu","M45_n2mu.root","M45320*sof.root","M45900*sof.root")'
	root -l -b -q '../merge.C("n2gam","M45_n2gam.root","M45340*sof.root","M45900*sof.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M45_nlam.root","M45400*sof.root","M45900*sof.root")'

	# event tuple
	root -l -b -q '../merge.C("ntp","M45_ntp.root","M45*sof.root")'
	;;

55)
	# phi mode
	root -l -b -q '../merge.C("nphi","M55_nphi.root","M55000*sof.root","M55900*sof.root")'

	# D0 modes
	root -l -b -q '../merge.C("nd01","M55_nd01.root","M55100*sof.root","M55103*sof.root","M55106*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("nd02","M55_nd02.root","M55101*sof.root","M55104*sof.root","M55107*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("nd03","M55_nd03.root","M55102*sof.root","M55105*sof.root","M55108*sof.root","M55900*sof.root")'

	# D+- modes
	root -l -b -q '../merge.C("ndpm1","M55_ndpm1.root","M55120*sof.root","M55124*sof.root","M55128*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("ndpm2","M55_ndpm2.root","M55121*sof.root","M55125*sof.root","M55129*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("ndpm3","M55_ndpm3.root","M55122*sof.root","M55126*sof.root","M55130*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("ndpm4","M55_ndpm4.root","M55123*sof.root","M55127*sof.root","M55131*sof.root","M55900*sof.root")'

	# Ds+- modes
	root -l -b -q '../merge.C("nds1","M55_nds1.root","M55140*sof.root","M55142*sof.root","M55144*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("nds2","M55_nds2.root","M55141*sof.root","M55143*sof.root","M55155*sof.root","M55900*sof.root")'

	# J/psi modes
	root -l -b -q '../merge.C("njpsi1","M55_njpsi1.root","M55200*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("njpsi2","M55_njpsi2.root","M55201*sof.root","M55900*sof.root")'

	# eta_c modes
	root -l -b -q '../merge.C("netac1","M55_netac1.root","M55220*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("netac2","M55_netac2.root","M55221*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("netac3","M55_netac3.root","M55222*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("netac4","M55_netac4.root","M55223*sof.root","M55900*sof.root")'

	# chi_c0 modes
	root -l -b -q '../merge.C("nchic01","M55_nchic01.root","M55240*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("nchic02","M55_nchic02.root","M55241*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("nchic03","M55_nchic03.root","M55242*sof.root","M55900*sof.root")'

	# electro-magnetic modes
	root -l -b -q '../merge.C("n2e","M55_n2e.root","M55300*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("n2mu","M55_n2mu.root","M55320*sof.root","M55900*sof.root")'
	root -l -b -q '../merge.C("n2gam","M55_n2gam.root","M55340*sof.root","M55900*sof.root")'

	# Lambda mode
	root -l -b -q '../merge.C("nlam","M55_nlam.root","M55400*sof.root","M55900*sof.root")'

	# Lambda_c mode
	root -l -b -q '../merge.C("nlamc","M55_nlamc.root","M55420*sof.root","M55900*sof.root")'

	# event tuple
	root -l -b -q '../merge.C("ntp","M55_ntp.root","M55*sof.root")'
	;;


*)
	"Unknown energy!"
esac
