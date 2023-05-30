#!/bin/bash
Thread_num=48 # the server has 48 cores //12.29 04 has 16 cores
Tmp_fifo=/tmp/$$.fifo
mkfifo $Tmp_fifo
exec 6<> $Tmp_fifo
rm -f $Tmp_fifo

for i in `seq $Thread_num`
do
        echo >&6
done


START=0

# batch_num=329 # for J7 group version
# batch_num=2700 
# batch_num=11880
# batch_num=10000
batch_num=890
run_command='./OnlineFeeBase_TON_revision_updatefrequency'
# config='Iteration_test'
# config_list="test_01_J10R test_01_F2NS_J10R"
# config_list="varyF2 varyP"
# config_list="varyF2_add_3and4"
# config_list="test_01_F2NS test_01_F2NS_J10R varyF2 varyP varyF2_add varyF2_add_more varyF2_add_3and4 "

## this two is for F2=1
# config_list="test_01 test_01_J10R" 
## this one for add P=0.005
# config_list="P5" 
# ini='workload_5seed_11rate_3group_2epoch.ini'

# new ini for workload_5seed_11rate_3group_2epoch_0.00015.ini
# config_list="test_01 test_01_F2NS test_01_J10R test_01_F2NS_J10R varyF2 varyP varyF2_add varyF2_add_more varyF2_add_3and4 P5"
# config_list="P25"

# Note: test varyZ and varyF1(3025)
#config_list="test_01 test_01_F2NS test_01_J10R test_01_F2NS_J10R"
# config_list="WP WN"
# config_list="WP WU" # there is a typo in previous run...
# the WP not runned, I guess this is because the workload name is too long for periodic than the uniform
# config_list="hh_s2_no hh_ss"
# config_list="vary_C0 vary_Z0 vary_F10 vary_F20"
# config_list="vary_F10_more"
# config_list="vary_corruption"
config_list="vary_corruption_more"
ini='workload_vary_corruption.ini'

for config in $config_list; do
for run_num in $(eval echo "{$START..$batch_num}")
do  
read -u 6
{
echo $run_command -m -r $run_num -u Cmdenv -c $config -n . $ini
$run_command -m -r $run_num -u Cmdenv -c $config -n . $ini
echo >&6
} &

done 
done


# for t3 in {1..10}
# do  
# echo $t3  
# done 

#  python3 main.py --t1 0 --t2 0 --t3 3 --t4 4 --t5 5 --t6 6 --t7 7 --t8 0.2

# for (( COUNTER=0; COUNTER<=10; COUNTER+= 1 )); do
#     echo $COUNTER
# done

# # `seq 0 2 10`

# for i in `seq 0 0.1 10`; 
# do echo $i; 
# done
