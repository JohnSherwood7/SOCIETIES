import os

minRes = [5000]
for m in minRes:
    for i in range(15):
        os.system("./societies -p testRecipe/min" + str(m) + ".conf -z testRecipe/agentcsv.csv -s testRecipe/min" + str(m) + " -t min" + str(m) + " -v 0")

