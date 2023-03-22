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

def plotResults(outputFile):
    time = []
    data = []
    labels = set()
    with open(outputFile, 'r') as csvFile:
        reader = csv.DictReader(csvFile)
        labels = reader.fieldnames
        print(reader.fieldnames)
        data.append([])
        for row in reader:
            time.append(float(row[labels[0]]))
            for var in range(1,len(labels)):
                data.append([])
                data[var].append(float(row[labels[var]]))
    csvFile.close()

    N = len(labels) - 1
    cols = int(math.ceil(N / 4))
    rows = int(math.ceil(N / cols))

    gs = gridspec.GridSpec(rows, cols)
    fig = pl.figure()
    for var in range(1,len(labels)):
        ax = fig.add_subplot(gs[var - 1])
        ax.plot(time, data[var])
        ax.set_xlabel(labels[0])
        ax.set_ylabel(labels[var])

    fig.set_tight_layout(True)
    fig.show()
    input()
  

def runMinimizer(netlistFile):
    #os.system("cd results")
    nSize = len(netlistFile)
    outputFileName = netlistFile[:nSize-3]
    outputFileName += "csv"
    os.system("cd $")
    cmd = "josim -o " + outputFileName + " " +  netlistFile + " -V 1"
    subprocess.call(cmd, shell = True)
    plotResults(outputFileName)


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
            output = files[0]
            """
            if ".txt" in files[0]:
                logic = files[0]
                netlist = files[1]
            else:
                netlist = files[0]
                logic = files[1]
            """
            print("Files selected")
            plotResults(output)
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

