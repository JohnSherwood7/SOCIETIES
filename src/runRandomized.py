import os

os.system("echo RANDOMIZED Extraction -- unlimited resources >> testRandomized/runTime.txt")
for i in range(7):
    os.system("echo RUN " + str(i) + "     >> testRandomized/runTime.txt")
    os.system("./societies -p ../../conf/devices_test.conf -z ../../conf/agentcsv.csv -t randomized -s testRandomized -v 0 >> testRandomized/runTime.txt")
    os.system("echo                          >> testRandomized/runTime.txt")

