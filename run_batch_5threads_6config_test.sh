#!/bin/bash
Thread_num=5 # the server has 48 cores
Tmp_fifo=/tmp/$$.fifo
mkfifo $Tmp_fifo
exec 6<> $Tmp_fifo
rm -f $Tmp_fifo

for i in `seq $Thread_num`
do
        echo >&6
done


START=0
batch_num=230 # 231 for Iteration_F1_P_withoutHTLCtime_simulen10e6_test # no space no space no spacce.... 1155-1
run_command='./OnlineFeeBase_NewLessOutput_centralized'
# config='Iteration_test'
config_list="Iteration_F1_P_withoutHTLCtime_simulen10e6_clightning_oct5_2020_2000000_workload_rate_1_poisson_valuation_01_Flag10 Iteration_F1_P_withoutHTLCtime_simulen10e6_clightning_oct5_2020_2000000_workload_rate_1_poisson_valuation_01_Flag01 Iteration_F1_P_withoutHTLCtime_simulen10e6_clightning_oct5_2020_2000000_workload_rate_33_poisson_valuation_01_Flag10 Iteration_F1_P_withoutHTLCtime_simulen10e6_clightning_oct5_2020_2000000_workload_rate_33_poisson_valuation_01_Flag01 Iteration_F1_P_withoutHTLCtime_simulen10e6_clightning_oct5_2020_2000000_workload_rate_33_uniform_valuation_01_Flag10 Iteration_F1_P_withoutHTLCtime_simulen10e6_clightning_oct5_2020_2000000_workload_rate_33_uniform_valuation_01_Flag01"
ini='OnlinceFee_poisson_tx.ini'

for config in $config_list; do
for run_num in $(eval echo "{$START..$batch_num}")
do  
read -u 6
{
echo $run_command -m -r $run_num -u Cmdenv -c $config -n . $ini
./OnlineFeeBase_NewLessOutput_centralized -m -r $run_num -u Cmdenv -c $config -n . OnlinceFee_poisson_tx.ini
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
