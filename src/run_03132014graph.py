import os

os.system("echo initAmount=10000, renewalRate=1.75%, min = 0,300,600,1000, 3000 days >> 03132014/runTime.txt")

min_res = ["0", "300", "600", "1000"]
for t in min_res:
	location = "03132014/min" + t + "_graph"
	os.system("echo MIN_RES_AMOUNT = " + t + " ----- >> 03132014/runTime_graph.txt")
	for i in range(40):
		os.system("echo RUN " + str(i) + " >> 03132014/runTime_graph.txt")
		os.system("./societies -p " + location + "/confVals.conf -z 03132014/agentcsv.csv -t min" + t + " -s" + location + " -v 0 >> 03132014/runTime_graph.txt")
		os.system("echo                  >> 03132014/runTime_graph.txt")
	os.system("echo ----------------------------------------- >> 03132014/runTime_graph.txt")
