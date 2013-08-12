#!/bin/sh

# both beam offset and gradient are given in parameter list
if [ $1 ] && [ $2 ] && [ $3 ] && [ $4 ] ; then
  evts=$1
  mom=$2

  dirname_base=$3
  #dirname_base="100k_box_plab_1.5GeV_th_0.0-0.9deg"
  #dirname_base="100k_elastic_only_plab_1.5GeV_thmin_0.025deg"
  filename_base=`echo $dirname_base | sed -e 's/\./o/g'`

  #filename_base="100k_elastic_only_plab_1o5GeV_thmin_0o025deg"
  index=$4

  if [ ! $9 ] && [ ! ${10} ] && [ ! ${11} ] && [ ! ${12} ]; then
    gradx=0.0
    grady=0.0
    gradx_sig=0.0
    grady_sig=0.0
  fi

  if [ $5 ] && [ $6 ] && [ $7 ] && [ $8 ] ; then
    dirname="${dirname_base}_pixel_beam_offsetXYDXDY_$5_$6_$7_$8_gradXYDXDY_${gradx}_${grady}_${gradx_sig}_${grady_sig}"
    dirname_full="${DATA_DIR}/${dirname}"

    qsub -N reconstructMCData_$dirname -l nodes=1:ppn=1,walltime=20:00:00 -j oe -o /home/pflueger/himster_output_logs/$dirname -v var1="$evts",var2="$mom",var3="${GEN_DATA}/$dirname_base/${filename_base}_$index.root",var4="$index",var5="$dirname",var6="${dirname_full}",var7="$5",var8="$6",var9="$7",var10="$8",var11="${gradx}",var12="${grady}",var13="${gradx_sig}",var14="${grady_sig}" -V ./runreconstructMCData.sh
  fi
fi

exit 0;
