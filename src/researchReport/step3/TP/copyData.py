import shlex, os

files = ["unitsTraded", "unitsGathered", "unitsHeld", "complexity", "devDeviceUse", "deviceDiscovered", "deviceMade", "devicePercents", "gini", "HHI", "meanUtility", "totalTimeUsage", "totalUtility"]
ratio = ['3', '10', '100', '300', '1000']

for file in files:
	os.system("> total/" + file + ".csv")
	writefile = open('total/' + file + '.csv', 'w')
	for i in ratio:
		writefile.write("ratio = 1:" + i + '\n')
		to_readfile = open(i + '/' + file + '.csv', 'r')
		reading_file = to_readfile.read()
		writefile.write(reading_file)
		to_readfile.close()
	writefile.close()	
