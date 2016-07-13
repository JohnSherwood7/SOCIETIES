import os, datetime

ratios = [3, 10, 30, 300, 1000]
rounds = 1

for ratio in ratios:
	for round in range(rounds):
		os.system("./societies -v 0 -s ResDevTP/" + str(ratio) + " -p ResDevTP/devices_test.conf -z ResDevTP/" + str(ratio) + "/agentcsv.csv -t TP" + str(ratio) + " >> ResDevTP/" + str(ratio) + "/output.txt")
