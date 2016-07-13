import os, datetime

ratios = [5, 10, 20, 40, 60, 80, 100]
rounds = 3

for ratio in ratios:
	os.system("mkdir devTradePower/1_" + str(ratio))
	os.system("> devTradePower/1_" + str(ratio) + "/timeRunning.txt")
	file = open("devTradePower/1_" + str(ratio) + "/timeRunning.txt",'w')
	for round in range(rounds):
		startTime = datetime.datetime.now()
		os.system("./societies -v 0 -s devTradePower/1_" + str(ratio) + " -p ../../conf/devices_test.conf" + " -z ../../conf/agentcsv2_1_" + str(ratio) + ".csv")
		endTime = datetime.datetime.now()
		st = "*****  RUN " + str(round) + " DURATION: " + str(endTime - startTime) + " secs ***** "
		file.write(st)
	file.close()
