import os

os.system("echo initAmount=10000, renewalRate=1.75%, 3.0%, 3000 days, no social control >> 02272014/runTime_church.txt")

renewalRate = ["3"]
for t in renewalRate:
	os.system("echo ----- RENEWAL_RATE = " + t + "----- >> 02272014/runTime_church.txt")
	for i in range(100):
		os.system("echo RUN " + str(i) + " >> 02272014/runTime_church.txt")
		os.system("./societies -p 02272014/confVals.conf -z 02272014/renewal" + t + "/agentcsv.csv -t renewal" + t + " -s 02272014/church/renewal" + t + " -v 0 >> 02272014/runTime_church.txt")
		os.system("echo                  >> 02272014/runTime_church.txt")
	os.system("echo ----------------------------------------- >> 02272014/runTime_church.txt")
