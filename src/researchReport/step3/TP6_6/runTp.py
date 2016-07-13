import os

ratios = [3, 10, 100, 300, 1000]
for r in ratios:
	path = "researchReport/step3/TP6_6/" + str(r)
	os.system("./societies -p researchReport/step3/TP6_6/devices_test.conf -z " + path + "/AgentTypes.csv -t 6_6_TP" + str(r) + " -v 0 -s " + path + " >> " + path + "/output.txt")
