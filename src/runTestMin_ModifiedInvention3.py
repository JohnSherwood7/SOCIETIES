import os

os.system("echo initAmount=10000, renewalRate=1.75%, 3000 days, RES_CONSIDERED_DEPLETED 1.1, RES_CONSIDERED_RENEWED 1.0, dial MIN_RES_AMOUNT >> testRecipe/runTime.txt")
min_res = [400, 1000, 2000]
loc = "testMin_ModifiedInvention2/renewal21/"
for i in min_res:
    os.system("echo MIN_RES_AMOUNT = " + str(i) + " >> " + loc + "runTime.txt")
    for j in range(50):
        os.system("echo RUN " + str(j) + " >> " + loc + "runTime.txt")
        os.system("./societies -p " + loc + "min" + str(i) + ".conf -z " + loc + "agentcsv.csv -t min_" + str(i) + " -s " + loc + "min" + str(i) + " -v 0 >> " + loc + "runTime.txt")
    os.system("echo ------------------------------------------------------>> " + loc + "runTime.txt")

