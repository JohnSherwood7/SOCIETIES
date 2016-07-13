import os

os.system("echo initAmount=10000, renewalRate=1.5%, MIN_RES_AMOUNT=100, 5000 days >> testMinResAmount/runTime.txt")
for j in range(100):
    os.system("echo RUN " + str(j) + " >> testMinResAmount/runTime.txt")
    os.system("./societies -p testMinResAmount/minResAmount100/devices_test.conf -z ../../conf/agentcsv.csv -t minResAmount100 -s testMinResAmount/minResAmount100/ -v 0 >> testMinResAmount/runTime.txt")
    os.system("echo                          >> testRandomized/runTime.txt")

