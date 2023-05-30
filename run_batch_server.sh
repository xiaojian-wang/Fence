#!/bin/bash
Thread_num=10
Tmp_fifo=/tmp/$$.fifo
mkfifo $Tmp_fifo
exec 6<> $Tmp_fifo
rm -f $Tmp_fifo

for i in `seq $Thread_num`
do
        echo >&6
done


START=0
batch_num=431999 # no space no space no spacce....
run_command='./OnlineFeeBase_NewLessOutput_centralized'
config='Iteration_test'
ini='OnlinceFee_poisson_tx.ini'
# log='&> /mnt/data/xwang244/Omnetpp_results/iteration_6.12/sendFeeFlagOn_6-12.log'

for run_num in $(eval echo "{$START..$batch_num}")
do  
read -u 6
{
echo $run_command -m -r $run_num -u Cmdenv -c $config -n . $ini $log
./OnlineFeeBase_NewLessOutput_centralized -m -r $run_num -u Cmdenv -c Iteration_test -n . OnlinceFee_poisson_tx.ini $ini $log
echo >&6
} &

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
