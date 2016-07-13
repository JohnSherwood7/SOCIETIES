import os, datetime

ratios = ['5', '1', '05', '01']
rounds = 1

for ratio in ratios:
	for round in range(rounds):
		os.system("./societies -v 0 -s inventPower/3000days/" + ratio + " -p inventPower/3000days/devices_test.conf -z inventPower/3000days/" + ratio + "/agentcsv.csv -t IP" + ratio + " >> inventPower/3000days/" + ratio + "/output.txt")
