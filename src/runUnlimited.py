import os

res = ['6', '8']

for n in res:
    loc = 'testUnlimited/res' + n
    for i in range(100):
        os.system("./societies -p " + loc + "/res" + n + ".conf -z testUnlimited/agentcsv.csv -t res" + n + " -s " + loc + " -v 0 >> " + loc + "/runTime.txt")
