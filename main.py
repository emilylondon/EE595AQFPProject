import sys
import subprocess 
import os 
import math
import matplotlib
matplotlib.use('QtAgg')
import matplotlib.pyplot as pl
from matplotlib import gridspec
import numpy as np
import csv

from PyQt6.QtGui import QGuiApplication
from PyQt6.QtQml import QQmlApplicationEngine
from PyQt6.QtWidgets import QApplication, QFileDialog
from PyQt6.QtCore import QObject, pyqtSlot, PYQT_VERSION_STR

print(PYQT_VERSION_STR)

app = QGuiApplication(sys.argv)

def plotResults(output, scatter):
    temp = []
    bit_error = []
    labels = set()

    temp1 = []
    bit_error1 = []
    labels1 = set()

    #reads the temperature plot csv
    with open(output, 'r') as csvFile1:
        reader1 = csv.DictReader(csvFile1)
        labels1 = reader1.fieldnames
        for row in reader1:
            try:
                temp1.append(float(row[labels1[0]]))
                bit_error1.append(float(row[labels1[1]]))
            except: 
                print("First row")
    csvFile1.close()
    
    #reads the scatter plot csv
    with open(scatter, 'r') as csvFile:
        reader = csv.DictReader(csvFile)
        labels = reader.fieldnames
        for row in reader:
            try:
                temp.append(float(row[labels[1]]))
                bit_error.append(float(row[labels[2]]))
            except: 
                print("First row")
    csvFile.close()

    cols = 1
    rows = 2

    gs = gridspec.GridSpec(rows, cols)
    fig = pl.figure()

    ax = fig.add_subplot(gs[0])
    ax.scatter(temp, bit_error)
    ax.set_xlabel(labels[1])
    ax.set_ylabel(labels[2])

    print(temp1)

    ax2 = fig.add_subplot(gs[1])
    ax2.plot(temp1, bit_error1)
    ax2.set_xlabel(labels1[0])
    ax2.set_xlabel(labels1[1])
    
    fig.set_tight_layout(True)
    fig.show()
    input()
  

def runMinimizer(netlistFile):
    #os.system("cd results")
    nSize = len(netlistFile)
    outputFileName = netlistFile[:nSize-4]
    outputTempGraphFileName = outputFileName + "_temp_ber.csv"
    outputScatterFileName = outputFileName + "_scatter.csv"
    os.system("cd $")
    cmd = "./montecarlo " + netlistFile + " " + str(10)
    subprocess.call(cmd, shell = True)
    plotResults(outputTempGraphFileName, outputScatterFileName)


class FileExplorer(QObject):
    @pyqtSlot()
    def newButtonClicked(self):
        napp = QApplication([])
        fileFinder = QFileDialog()
        fileFinder.setNameFilter("Netlist files and descriptions (*.cir)")
        fileFinder.setFileMode(QFileDialog.FileMode.ExistingFiles)

        if fileFinder.exec():
            files = fileFinder.selectedFiles()
            netlist = files[0]
            netlist = os.path.relpath(netlist, "Users/emilylondon/College/EE595/EE595SFQProject")
            netlist = netlist.replace('../', '')
            #netlist = '/' + netlist

            """
            if ".txt" in files[0]:
                logic = files[0]
                netlist = files[1]
            else:
                netlist = files[0]
                logic = files[1]
            """
            print("Files selected")
            runMinimizer(netlist)
        else:
            print("No files selected")
    @pyqtSlot()
    def loadButtonClicked(self):
        napp = QApplication([])
        fileFinder = QFileDialog()
        fileFinder.setNameFilter("Output csv files and descriptions (*.csv)")
        fileFinder.setFileMode(QFileDialog.FileMode.ExistingFiles)
        if fileFinder.exec():
            files = fileFinder.selectedFiles()
            
            if "scatter" in files[0]:
                scatter = files[0]
                output = files[1]
            else:
                output = files[0]
                scatter = files[1]
    
            print("Files selected")
            plotResults(scatter, output)
        else:
            print("No files selected")



#QQuickWindow.setSceneGraphBackend('software')

engine = QQmlApplicationEngine()
#engine.quit.connect(app.quit)

actions = FileExplorer()

engine.rootContext().setContextProperty("actions", actions)
engine.load('main.qml')

if not engine.rootObjects():
    sys.exit(-1)

sys.exit(app.exec())

