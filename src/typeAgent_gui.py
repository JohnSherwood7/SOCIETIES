from Tkinter import *
from tkFileDialog import asksaveasfilename, askopenfilename
#import tktable as tktable

class App:

    def __init__(self, master):

        self.NUM_TYPES = 2      # TODO: read from somewhere?

        """Set the name, location of each box"""
        frame = Frame(master)
        frame.grid(column=0, row=0)
        
        #spacing1 = Label(frame, text="           ")
        #spacing1.grid(column=2, row=0)

        frameTitle = Label(frame, text="Types of Agents", font='bold')
        frameTitle.grid(column=6, row=0)
        
        # TODO: the two more attributes' names
        self.attributes = ["steepness", "scaling", "minResEff", "maxResEff", 
                      "minDevEff", "maxDevEff", "maxDevExp", "lifetime", "tradePower", "patent"]

        """ Create a table for each agent """
        '''attributes header: horizontal'''
        for i in range(len(self.attributes)):
            exec(self.attributes[i] + "Label = Label(frame, text='" + self.attributes[i] + "'); "
                + self.attributes[i] + "Label.grid(column=" + str(i+1) + ", row=1" + ", sticky='w')")

        '''resource id header: vertical'''
        for i in range(self.NUM_RESOURCES):
            exec("res" + str(i) + "Label = Label(frame, text='res " + str(i) + "'); res"
                + str(i) + "Label.grid(column=0, row=" + str(i+2) + ", sticky='w')")

        '''boxes for each resource and each attribute value'''
        for i in range(len(self.attributes)):
            for j in range(self.NUM_RESOURCES):
                exec("self." + self.attributes[i] + str(j) + "Var = DoubleVar(master);"
                    + self.attributes[i] + str(j) + "Box = Entry(frame, textvariable=self." + self.attributes[i] + str(j) + "Var, width=10); "
                    + self.attributes[i] + str(j) + "Box.grid(column=" + str(i+1) + ", row=" + str(j+2) + ", sticky=W);")

        self.resetVar()
    
    

    def resetVar(self):
        self.aconfFile = open('../../conf/guiTest.aconf', 'r')
        agNum = 0
        lineNum = 0
        lineList = self.aconfFile.readlines()
        count = 0
        for lineNum in range(len(lineList)):
            if ('#' in lineList[lineNum]):
                agNum += 1
                count = 0
            if ('#' not in lineList[lineNum]) and (lineNum != len(lineList)-1):
                lineSplit = [x for x in lineList[lineNum].split(', ')]
                for i in range(len(self.attributes)):
                    exec("self." + self.attributes[i] + str(count) + "Var.set(float(" + lineSplit[i] + ")); ")
                    
                count += 1
        

if __name__ == '__main__':
    GUI = Tk()
    GUI.title('Agents')
    app = App(GUI)
    GUI.mainloop()
