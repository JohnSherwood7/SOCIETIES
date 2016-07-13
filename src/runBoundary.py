import os

init = [2712000, 1356000, 678000, 339000]

for i in init:
    location = "testBoundary/" + str(i)
    for j in range(10):
        os.system("echo initialAmount = " + str(i) + "    RUN " + str(j) + " >> testBoundary/runTime.txt")
        os.system("./societies -p " + location + "/devices_test.conf -z " + location + "/agentcsv.csv -t " + str(i) + " -s " + location + " -v 0 >> testBoundary/runTime.txt")
    os.system("echo ------------------------------ >> testBoundary/runTime.txt")

