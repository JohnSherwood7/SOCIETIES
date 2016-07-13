import os

hetero = ["HM", "HM2", "TP", "GS", "MS", "IS"]
for h in hetero:
	path = "researchReport/step2/6_6/" + h
	os.system("./societies -p " + path + "/devices_test.conf -z " + path + "/AgentTypes.csv -t " + h + " -s " + path + " -v 0 >> " + path + "/output.txt")
