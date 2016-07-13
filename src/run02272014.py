import os

os.system("echo initAmount=10000, renewalRate=1.5%, 3000 days, no social control >> 02272014/runTime.txt")

renewalRate = ["150"]
for t in renewalRate:
	os.system("echo ----- RENEWAL_RATE = " + t + "----- >> 02272014/runTime.txt")
	for i in range(100):
		os.system("echo RUN " + str(i) + " >> 02272014/runTime.txt")
		os.system("./societies -p 02272014/confVals.conf -z 02272014/renewal" + t + "/agentcsv.csv -t renewal" + t + " -s 02272014/renewal" + t + " -v 0 >> 02272014/runTime.txt")
		os.system("echo                  >> 02272014/runTime.txt")
	os.system("echo ----------------------------------------- >> 02272014/runTime.txt")
