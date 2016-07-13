import os

os.system("echo initAmount=10000, renewalRate=1.5%, 5000 days, MIN_RES_AMOUNT=1000, dial TAX_RATE >> 12072013/runTime2.txt")

tax = ["0", "1", "5", "10"]

for t in tax:
	os.system("echo ----- TAX_RATE = " + t + "----- >> 12072013/runTime2.txt")
	for i in range(100):
		os.system("echo RUN " + str(i) + " >> 12072013/runTime2.txt")
		os.system("./societies -p 12072013/config/min1000_tax" + t + ".conf -z 12072013/config/agentcsv.csv -t min1000_tax" + t + " -s 12072013/min1000_tax" + t + " -v 0 >> 12072013/runTime2.txt")
		os.system("echo                  >> 12072013/runTime2.txt")
	os.system("echo ----------------------------------------- >> 12072013/runTime2.txt")
