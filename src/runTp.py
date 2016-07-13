import os

ratios = [3, 10, 100, 300, 1000]
for r in ratios:
	path = "researchReport/step3/TP/" + str(r)
	os.system("./societies -p researchReport/step3/TP/devices_test.conf -z " + path + "/AgentTypes.csv -t TP" + str(r) + " -v 0 -s " + path + " >> " + path + "/output.txt")
