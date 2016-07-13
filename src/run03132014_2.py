import os

os.system("echo initAmount=10000, renewalRate=1.75%, min = 100-900, 3000 days >> 03132014/runTime_2.txt")

min_res = ["700", "800", "900"]
for t in min_res:
	os.system("echo MIN_RES_AMOUNT = " + t + " ----- >> 03132014/runTime_2.txt")
	for i in range(100):
		os.system("echo RUN " + str(i) + " >> 03132014/runTime_2.txt")
		os.system("./societies -p 03132014/min" + t + "/confVals.conf -z 03132014/agentcsv.csv -t min" + t + " -s 03132014/min" + t + " -v 0 >> 03132014/runTime_2.txt")
		os.system("echo                  >> 03132014/runTime_2.txt")
	os.system("echo ----------------------------------------- >> 03132014/runTime_2.txt")
