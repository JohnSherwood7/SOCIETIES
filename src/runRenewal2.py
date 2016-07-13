import os

renewalRates = ["40", "50", "60", "70", "80"]
initAmounts = ["10000"]

for renewalRate in renewalRates:
    for initAmount in initAmounts:
        print "amount = " + initAmount + " renewalRate = " + renewalRate
        os.system("echo Initial Amount = " + initAmount + "     renewal rate = " + renewalRate + " >> unitTestRenewal3/output.txt")
        os.system("./societies -p ../../conf/devices_test.conf -z unitTestRenewal3/agentcsv_" + initAmount + "_" + renewalRate + ".csv -t amount" + initAmount + "_renew" + renewalRate + " -s unitTestRenewal3/amount" + initAmount + "_renew" + renewalRate + " -v 0 >> unitTestRenewal3/output.txt")
        os.system("echo      >> unitTestRenewal3/output.txt")
