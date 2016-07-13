import os

os.system("echo Test all the added code for senior project. >> finalTest/runTime.txt")
os.system("echo Unlimited Resources, Homogeneous agents. However, the extraction is not randomized. >> finalTest/runTime.txt")

for i in range(15):
	os.system("echo RUN " + str(i) + " >> finalTest/runTime.txt")
	os.system("./societies -p finalTest/confVals.conf -z finalTest/agentcsv.csv -t unlimited_nonrandomized -s finalTest -v 0 >> finalTest/runTime.txt")
	os.system("echo                  >> finalTest/runTime.txt")
	os.system("echo ----------------------------------------- >> finalTest/runTime.txt")
