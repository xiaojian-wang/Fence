#!/bin/bash
Thread_num=40 # the server has 48 cores
Tmp_fifo=/tmp/$$.fifo
mkfifo $Tmp_fifo
exec 6<> $Tmp_fifo
rm -f $Tmp_fifo

for i in `seq $Thread_num`
do
        echo >&6
done


START=0

batch_num=47 # 
run_command='./OnlineFeeBase_reconstruct'
# config='Iteration_test'
config_list="test_01"
ini='workload_differentEpochtest.ini'

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
