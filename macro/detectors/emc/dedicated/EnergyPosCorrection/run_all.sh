# run data production for emc energy correction at GSI batch cluster
#!/bin/bash
for ((i=1000;i<=1050;i++))
do
qsub -o /lustre/panda/dmelnych/emc_correction/sim_emc_"$i".log /lustre/panda/dmelnych/emc_correction/run_data_production_"$i".sh
done

for ((i=2000;i<=2050;i++))
do
qsub -o /lustre/panda/dmelnych/emc_correction/sim_emc_"$i".log /lustre/panda/dmelnych/emc_correction/run_data_production_"$i".sh
done

for ((i=3000;i<=3050;i++))
do
qsub -o /lustre/panda/dmelnych/emc_correction/sim_emc_"$i".log /lustre/panda/dmelnych/emc_correction/run_data_production_"$i".sh
done

for ((i=4000;i<=4050;i++))
do
qsub -o /lustre/panda/dmelnych/emc_correction/sim_emc_"$i".log /lustre/panda/dmelnych/emc_correction/run_data_production_"$i".sh
done
