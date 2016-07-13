import os

os.system("echo initAmount=10000, renewalRate=1.75%, 3000 days, RES_CONSIDERED_DEPLETED 1.1, RES_CONSIDERED_RENEWED 1.0, MIN_RES_AMOUNT 1000, dial TOOL_PROBABILITY_FACTOR >> testInventingSpeed/runTime.txt")
tool = [70, 65, 60, 55, 45, 35]
loc = "testInventingSpeed/"
for i in tool:
    os.system("echo tool inventing spped = 0.0000" + str(i) + " >> " + loc + "runTime.txt")
    for j in range(100):
        os.system("echo RUN " + str(j) + " >> " + loc + "runTime.txt")
        os.system("./societies -p " + loc + "tool" + str(i) + ".conf -z " + loc + "agentcsv.csv -t toolInventing" + str(i) + " -s " + loc + "/tool" + str(i) + " -v 0 >> " + loc + "runTime.txt")
    os.system("echo ------------------------------------------------------>> " + loc + "runTime.txt")

