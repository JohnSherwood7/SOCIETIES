import os

os.system("echo initAmount=10000, renewalRate=1.75%, MIN_RES_AMOUNT=0, 1500 days >> testRecipe/runTime.txt")
resNum = [5,6,7,8,10,12]
for i in resNum:
    os.system("echo 8 agents " + str(i) + " resources >> testRecipe/runTime.txt")
    for j in range(3):
        os.system("echo RUN " + str(j) + " >> testRecipe/runTime.txt")
        os.system("./societies -p testRecipe/conf" + str(i) + ".conf -z testRecipe/agentcsv.csv -t " + str(i) + "res -s testRecipe -v 0 >> testRecipe/runTime.txt")
    os.system("echo                          >> testRecipe/runTime.txt")

