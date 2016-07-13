"""
Graph all the data in a csv file line by line.
The first line is used as x-axis. 
Best to graph unitsHeld, unitsGathered, meanUtility, totalUtility.
To run: python singleLineGraph.py -f filename.csv -t title
"""
import sys, csv
import matplotlib.pyplot as plt
from optparse import OptionParser

data = []

parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                    help="read data from FILE", metavar="FILE")
parser.add_option("-t", "--title", dest="title",
                  help="the title of the graph")
(options, args) = parser.parse_args()

'''read the filename from command line'''
csvfile = open(options.filename)
csvreader = csv.reader(csvfile)

'''
read the data from the csvfile line by line.
Deleted the first and last index in each line.
The first index is the title, the last is '\n'
'''
for row in csvreader:
    del row[0]
    del row[len(row)-1]
    for i in range(len(row)):
        row[i] = float(row[i])
    data.append(row)

'''plot the data line by line.'''
for i in range(len(data)-1):
    plt.plot(data[0], data[i+1])
if options.title != None:
    plt.title(options.title)
plt.show() 
