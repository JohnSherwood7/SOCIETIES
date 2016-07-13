import os

rounds = 10
dir = "timeTesting/5/"
for round in range(rounds):
	os.system("./societies -v 0 -s " + dir + " -p " + dir + "/devices_test.conf -z " + dir + "/agentcsv.csv >> " + dir + "/output.txt")
        os.system("echo    >> " + dir + "output.txt")
