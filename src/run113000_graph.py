import os

location = "testBoundary/113000_resAmountGraph"

os.system("echo initialAmount = 113000, renewal rate = 1.5% >> " + location + "/runTime.txt")
for i in range(40):
    os.system("./societies -p " + location + "/devices_test.conf -z " + location + "/agentcsv.csv -s " + location + " -v 0 >> " + location + "/runTime.txt")
    os.system("echo ------------------------------ >> " + location + "/runTime.txt")

