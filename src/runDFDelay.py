import os, datetime

ratios = [1, 2, 5, 10]
rounds = 3

for ratio in ratios:
	os.system("mkdir diffusionDelay/" + str(ratio))
	os.system("> diffusionDelay/" + str(ratio) + "/timeRunning.txt")
	file = open("diffusionDelay/" + str(ratio) + "/timeRunning.txt",'w')
	for round in range(rounds):
		startTime = datetime.datetime.now()
		os.system("./societies -v 0 -s diffusionDelay/" + str(ratio) + " -p ../../conf/devices_test_" + str(ratio) + ".conf" + " -z ../../conf/agentcsv.csv")
		endTime = datetime.datetime.now()
		st = "*****  RUN " + str(round) + " DURATION: " + str(endTime - startTime) + " secs ***** "
		file.write(st)
	file.close()
