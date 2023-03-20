import sys
import subprocess 
import os 

from PyQt6.QtGui import QGuiApplication
from PyQt6.QtQml import QQmlApplicationEngine
from PyQt6.QtWidgets import QApplication, QFileDialog
from PyQt6.QtCore import QObject, pyqtSlot, PYQT_VERSION_STR

print(PYQT_VERSION_STR)

app = QGuiApplication(sys.argv)

def runTimex(netlistFile, txtFile):
    #os.system("cd results")
    cmd = "./TimeEx " + netlistFile + "-d " + txtFile + "-x"
    subprocess.call(cmd, shell = True)


class FileExplorer(QObject):
    @pyqtSlot()
    def newButtonClicked(self):
        napp = QApplication([])
        fileFinder = QFileDialog()
        fileFinder.setNameFilter("Netlist files and descriptions (*.js *.txt)")
        fileFinder.setFileMode(QFileDialog.FileMode.ExistingFiles)

        if fileFinder.exec():
            files = fileFinder.selectedFiles()
            if ".txt" in files[0]:
                logic = files[0]
                netlist = files[1]
            else:
                netlist = files[0]
                logic = files[1]
            print("Files selected")
            runTimex(netlist, logic)
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


