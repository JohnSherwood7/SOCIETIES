import os, datetime

#ratios = [3, 10, 30, 100, 300, 1000]
ratios = [100, 300, 1000]
tests = ["devices_test"]
rounds = 3

for ratio in ratios:
	os.system("mkdir devTradePower/1_" + str(ratio))
	for test in tests:
	        dir = "devTradePower/1_" + str(ratio) + "/" + test
        	os.system("mkdir " + dir)
		for round in range(rounds):
			os.system("./societies -v 0 -s " + dir + " -p ../../conf/" + test + ".conf" + " -z devTradePower/1_" + str(ratio) + "/agentcsv.csv >> " + dir + "/output.txt")
                        os.system("echo      >> " + dir + "/output.txt")
