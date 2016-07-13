import shlex, os

files = ["unitsTraded", "unitsGathered", "unitsHeld", "complexity", "devDeviceUse", "deviceDiscovered",
	"deviceMade", "devicePercents", "gini", "HHI", "meanUtility", "totalTimeUsage", "totalUtility", "availableResources"]
minResAmounts = [100, 500, 1000, 1500, 2000, 2500, 3000, 5000, 7000, 9000, 10000]
for file in files:
	os.system("> total/" + file + ".csv")
	writefile = open('total/' + file + '.csv', 'w')
	for i in minResAmounts:
		to_readfile = open("minResAmount" + str(i) + '/' + file + '.csv', 'r')
		reading_file = to_readfile.read()
		writefile.write(reading_file)
		to_readfile.close()
	writefile.close()	
