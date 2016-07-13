import csv

attributes = ["steepness", "scaling", "minResEff", "maxResEff", "maxResExp",
		"minDevEff", "maxDevEff", "maxDevExp", "lifetime", "tradePower", "patent",
		"num_agents"]
NUM_TYPES = 2

f = open('agentcsv.csv', 'w')
f.write(', ')
f.write(', '.join(attributes))

types = []
type0 = [2.5, 600, 3.0, 9.0, 600.0, 9.0, 27.0, 40.0, 50, 0, 0, 1]
type1 = [2.5, 600, 3.0, 9.0, 600.0, 9.0, 27.0, 40.0, 50, 0, 0, 1]
for i in range(NUM_TYPES):
	exec("types.append(type" + str(i) + ")")
	f.write('\ntype ' + str(i))
	for j in range(len(types[i])):
		f.write(', ' + str(types[i][j]))
