#!/bin/bash

rounds=0
b=3
while [ $rounds -lt $b ]
do
	start_time=$(date +%s)
	./societies -v 0 -s tradePower/1_100/trade_test -p ../../conf/trade_test.conf -z ../../conf/agentcsv_1_100.csv
	finish_time=$(date +%s)
	echo "***** 1.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_100/timeRunning.txt
	(( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_100/tool_test -p ../../conf/tool_test.conf -z ../../conf/agentcsv_1_100.csv
        finish_time=$(date +%s)
        echo "***** 2.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_100/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_100/devices_test -p ../../conf/devices_test.conf -z ../../conf/agentcsv_1_100.csv
        finish_time=$(date +%s)
        echo "***** 3.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_100/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_200/trade_test -p ../../conf/trade_test.conf -z ../../conf/agentcsv_1_200.csv
        finish_time=$(date +%s)
        echo "***** 1.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_200/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_200/tool_test -p ../../conf/tool_test.conf -z ../../conf/agentcsv_1_200.csv
        finish_time=$(date +%s)
        echo "***** 2.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_200/timeRunning.txt
        (( rounds++ ))                           
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_200/devices_test -p ../../conf/devices_test.conf -z ../../conf/agentcsv_1_200.csv
        finish_time=$(date +%s)
        echo "***** 3.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_200/timeRunning.txt
        (( rounds++ ))                           
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_400/trade_test -p ../../conf/trade_test.conf -z ../../conf/agentcsv_1_400.csv
        finish_time=$(date +%s)
        echo "***** 1.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_400/timeRunning.txt
        (( rounds++ ))                           
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_400/tool_test -p ../../conf/tool_test.conf -z ../../conf/agentcsv_1_400.csv
        finish_time=$(date +%s)
        echo "***** 2.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_400/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_400/devices_test -p ../../conf/devices_test.conf -z ../../conf/agentcsv_1_400.csv
        finish_time=$(date +%s)
        echo "***** 3.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_400/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_400/trade_test -p ../../conf/trade_test.conf -z ../../conf/agentcsv_1_40.csv
        finish_time=$(date +%s)
        echo "***** 1.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_40/timeRunning.txt
	(( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_40/tool_test -p ../../conf/tool_test.conf -z ../../conf/agentcsv_1_40.csv
        finish_time=$(date +%s)
        echo "***** 2.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_40/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_40/devices_test -p ../../conf/devices_test.conf -z ../../conf/agentcsv_1_40.csv
        finish_time=$(date +%s)
        echo "***** 3.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_40/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_60/trade_test -p ../../conf/trade_test.conf -z ../../conf/agentcsv_1_60.csv
        finish_time=$(date +%s)
        echo "***** 1.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_60/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_60/tool_test -p ../../conf/tool_test.conf -z ../../conf/agentcsv_1_60.csv
        finish_time=$(date +%s)
        echo "***** 2.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_60/timeRunning.txt
        (( rounds++ ))
done

rounds=0
while [ $rounds -lt $b ]
do
        start_time=$(date +%s)
        ./societies -v 0 -s tradePower/1_60/devices_test -p ../../conf/devices_test.conf -z ../../conf/agentcsv_1_60.csv
        finish_time=$(date +%s)
        echo "***** 3.TEST RUN $rounds DURATION: $((finish_time-start_time)) secs *****">>tradePower/1_60/timeRunning.txt
        (( rounds++ ))
done

