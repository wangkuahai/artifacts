#!/bin/bash

TEST_NAME=$1
LOOP=(1 2 3 4 5)

chmod 777 $1 "run_${TEST_NAME}.sh"

rm temp 
for i in ${LOOP[*]}
do  
    echo "running run_${TEST_NAME}.sh $i"
    /usr/bin/time -a -o temp -p ./run_${TEST_NAME}.sh
done

real_res=($(cat temp | grep real | grep -o "[0-9]*\.[0-9]*"))
accumulate=0.0
# echo $real_res
echo "time results: ${#real_res[@]}"
for eachtime in ${real_res[*]}
do
    echo $eachtime
    accumulate=$(echo "$accumulate + ${eachtime}" | bc)
done

echo "$accumulate / ${#real_res[*]}"
avg_time=$(echo "scale=3; $accumulate / ${#real_res[*]}" | bc)
echo "avg time : $avg_time"


# if (RUN_TEST=="tar")
# then 
#     for i in LOOP
#     do
#         time 
#     done
# fi


