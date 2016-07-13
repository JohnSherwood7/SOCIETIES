import os, datetime

ratios = ['5', '1', '05', '01']
rounds = 1

for ratio in ratios:
	for round in range(rounds):
		os.system("./societies -v 0 -s inventPowerGatherSpeed/" + ratio + " -p inventPowerGatherSpeed/devices_test.conf -z inventPowerGatherSpeed/" + ratio + "/agentcsv.csv -t IP" + ratio + " >> inventPowerGatherSpeed/" + ratio + "/output.txt")
