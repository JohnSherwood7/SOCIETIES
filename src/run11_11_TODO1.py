import os

initAmounts = ["50", "100", "1000", "10000", "50000", "100000"]
renewalRates = ["0.1", "0.5", "1.5", "3", "9"]

for i in initAmounts:
	for j in renewalRates:
		for k in range(3):
	                loc = "11_11_TODO1/initAmount" + i + "_renewalRate" + j
        		os.system("echo  --------------- >> " + loc + "/runTime.txt")
			os.system("./societies -p ../../conf/devices_test.conf -z " + loc + "/agentcsv.csv -t " + loc[12:] + "_updated -s " + loc + " -v 0 >> " + loc + "/runTime.txt")
			os.system("echo                    >> " + loc + "/runTime.txt")
                        
