from optparse import OptionParser
import numpy as np
import csv, os, sys



# To receive a list of numbers seperated by comma
def foo_callback(option, opt, value, parser):
  setattr(parser.values, option.dest, value.split(','))

parser = OptionParser()
parser.add_option("-l", "--location", dest="directory", help="the name of the folder where the csv files are stored")
parser.add_option("-d", "--days", dest="dayNumber", help="the last number of days to be calculated")
parser.add_option("-r", "--run", dest="runNumber", type='string',
                  action='callback', callback=foo_callback,
                  help="the specific runs to be calculated, use ',' to separate each run. Calculate all the runs when not specified")
parser.add_option("-t", "--title", dest="title", help="the title of the output file without extension. The output file will be automatically in the same directory as specified in -l. If not specified, will be set as summary")
(options, args) = parser.parse_args()



# If -l or -d is not specified correctly, print error message and terminate program.
if options.directory == None:
    print "No location specified. Use -l or --location followed by directory."
    sys.exit()
if options.dayNumber == None:
    print "No numnber of days specified. Use -d or --dayNumber followed by a positive inteeger."
    sys.exit()
elif options.dayNumber <= 0:
    print "Please enter a positive integer for day number."
    sys.exit()
if options.dayNumber.isdigit() == False:
    print "Please enter a positive integer for day number."
    sys.exit()
if options.title == None:
    options.title = "summary"

"""
Open a csv file and a txt file to save the summaries.
The csv file save the average of the last -d days results of each run.
The txt file outputs the average of all the runs.
"""
resultFileName = options.directory + "/" + options.title + ".csv"
os.system("touch " + resultFileName)
resultFile = open(resultFileName, 'wb')
csvwriter = csv.writer(resultFile)
summaryTxt = options.directory + "/" + options.title + ".txt"


# convert the runNumbers user entered into a list of integers
runNum = []
if options.runNumber == None:
    os.system("echo Summarize the last " +  options.dayNumber + " days of all runs in " + options.directory + " > " + summaryTxt)
else:
    for i in options.runNumber:
        runNum.append(int(i))
    # print the run number in the first line into csv file
    temp = runNum
    temp.insert(0, "Run")
    csvwriter.writerow(temp)
    os.system("echo Summarize the last" + options.dayNumber + "days of run #" + runNum + " in " + options.directory + " > " + summaryTxt)



"""
If no -r is specified, return all the rows except for the first one in the csvreader file.
If -r is specified, return a list of the rows that are selected.
"""
def selectRows(csvreader):
    data = []
    count = 0
    for row in csvreader:
        if options.runNumber == None:
            if count != 0:
                del row[len(row)-1]
                del row[0]
                for i in range(len(row)):
                    row[i] = float(row[i])
                data.append(row)
        else:
            if count in runNum:
                del row[len(row)-1]
                del row[0]
                for i in range(len(row)):
                    row[i] = float(row[i])
                data.append(row)
        count = count + 1
    return data



"""
Calculate Income:
average units gathered per agent over the last -d days;
and the SD
"""
csvfile = open(options.directory + "/unitsGathered.csv")
csvreader = csv.reader(csvfile)
data = selectRows(csvreader)
avg = []     #to store the average unitsHeld of the last days for each day'''
sd = []      #to store the standard deviation of the last days for each day'''

# put run number at first line
temp = []
temp.append("Run")
for i in range(len(data)):
    temp.append(i+1)
csvwriter.writerow(temp)

#Calculate average and sd
for row in data:
    avg.append(np.mean(row[len(row)-int(options.dayNumber):len(row)]))
    sd.append(np.std(row[len(row)-int(options.dayNumber):len(row)]))
os.system("echo Income: average = " + str(np.mean(avg)) + ", SD = " + str(np.mean(sd)) + " >> " + summaryTxt)

# print avg and sd in summary.csv file
avg.insert(0, "Income(avg)")
sd.insert(0, "Income(sd)")
csvwriter.writerow(avg)
csvwriter.writerow(sd)
csvfile.close()



"""
Calculate Wealth:
average units gathered per agent over the last -d days;
and the SD
"""
csvfile = open(options.directory + "/unitsHeld.csv")
csvreader = csv.reader(csvfile)
data = selectRows(csvreader)
avg = []
for row in data:
    avg.append(np.mean(row[len(row)-int(options.dayNumber):len(row)]))
os.system("echo Wealth: average = " + str(np.mean(avg)) + " >> " + summaryTxt)
avg.insert(0, "Wealth(avg)")
csvwriter.writerow(avg)
csvfile.close()



"""
Calculate Technology Index:
percent of the time to extract using each level of technology;
= %industry*4 + %factory*3 + %machine*2 + %tool*1
"""
csvfile = open(options.directory + "/technology.csv")
csvreader = csv.reader(csvfile)
data = selectRows(csvreader)
avg = []
for row in data:
    avg.append(np.mean(row[len(row)-int(options.dayNumber):len(row)]))
os.system("echo Technology Index: average = " + str(np.mean(avg)) + " >> " + summaryTxt)
avg.insert(0, "Technology Index")
csvwriter.writerow(avg)
csvfile.close()



"""
Calculate Depleted Res:
the number of depleted resources
"""
csvfile = open(options.directory + "/depletedRes.csv")
csvreader = csv.reader(csvfile)
data = selectRows(csvreader)
avg = []
for row in data:
    avg.append(np.mean(row[len(row)-int(options.dayNumber):len(row)]))
os.system("echo Depleted Res: average = " + str(np.mean(avg)) + " >> " + summaryTxt)
avg.insert(0, "Depleted Res")
csvwriter.writerow(avg)
csvfile.close()



"""
Calculate Environment Index:
the ratio of current total resources' amount to the total initial amount
"""
csvfile = open(options.directory + "/environment.csv")
csvreader = csv.reader(csvfile)
data = selectRows(csvreader)
avg = []
for row in data:
    avg.append(np.mean(row[len(row)-int(options.dayNumber):len(row)]))
os.system("echo Environment Index: average = " + str(np.mean(avg)) + " >> " + summaryTxt)
avg.insert(0, "Environment Index")
csvwriter.writerow(avg)
csvfile.close()
resultFile.close()
