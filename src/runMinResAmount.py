import os

minResAmounts = ["100", "300", "500", "1000", "1500", "2000", "2500", "3000"]
for i in minResAmounts:
    loc = "testMinResAmount/minResAmount" + i
    os.system("echo MIN_RES_AMOUNT = " + i + " >> testMinResAmount/runTime.txt")
    for j in range(30):
        os.system("echo RUN " + str(j) + " >> testMinResAmount/runTime.txt")
        os.system("./societies -p " + loc + "/devices_test.conf -z ../../conf/agentcsv.csv -t minResAmount" + i + " -s " + loc + " -v 0 >> testMinResAmount/runTime.txt")
        os.system("echo                          >> testRandomized/runTime.txt")

