import os

num_agents = [16, 12, 9, 6, 3, 24]
for i in num_agents:
	location = "researchReport/step1b/" + str(i) + "_24"
	os.system("./societies -p " + location + "/devices_test.conf -z " + location + "/AgentTypes.csv -t " + str(i) + "_24 -s " + location + " -v 0 >> " + location + "/output.txt")
