from scipy import stats
import numpy as np
from scipy.stats import kstest
from matplotlib import pyplot as plt
import math
import os

class SimulationData:
  def __init__(self, results, numRC):
    self.results = results
    self.numRC = numRC

files = []

files.append(open("output_Seq_3_test1", "r"))
files.append(open("output_Seq_5_test2", "r"))
files.append(open("output_Seq_10_test3", "r"))

files.append(open("output_Pthreads_3_test1", "r"))
files.append(open("output_Pthreads_5_test2", "r"))
files.append(open("output_Pthreads_10_test3", "r"))

files.append(open("output_OpenMP_3_test1", "r"))
files.append(open("output_OpenMP_5_test2", "r"))
files.append(open("output_OpenMP_10_test3", "r"))

openmp = []
pthreads = []

for file in files:
    strArray = []
    strArray = file.read().split(",")
    strArray.pop()

    results = [int(i) for i in strArray]
    #print(results)

    figure = plt.figure()

    plt.plot(results)
    plt.xlabel("Sample")
    plt.ylabel("Execution time(ns)")

    samplesList = [i+1 for i in range(results.__len__())]
    #print(samplesList)

    fileName = os.path.basename(file.name)
    plotTitle = fileName.split("_")[1] + "-" + fileName.split("_")[2] + " rows and columns"
    plt.title(plotTitle) 
    plt.scatter(samplesList, results)

    figuresDir = "Figures"
    if not os.path.exists(figuresDir):
        os.makedirs(figuresDir)
    figure.savefig(figuresDir + "/" + fileName + "_Fig")
    plt.show()

    print(plotTitle)
    x = np.array(results)
    print('KS-statistic D = %6.3f pvalue = %6.4f' % kstest(x, 'norm'))

    if("OpenMP" in fileName):
        sd = SimulationData(results, int(fileName.split("_")[2]))
        openmp.append(sd)
    if("Pthreads" in fileName):
        sd = SimulationData(results, int(fileName.split("_")[2]))
        pthreads.append(sd)
    #print('KS-statistic D = %6.3f pvalue = %6.4f' % kstest(x, 't'))

count = 0

#plt.cla()
#plt.clf()

while(count < 3):
    figure = plt.figure()
    plt.plot(openmp[count].results, label = "OpenMP")
    plt.plot(pthreads[count].results, label = "Pthreads")
    plt.legend()
    plt.xlabel("Sample")
    plt.ylabel("Execution time(ns)")

    plotTitle = "OpenMP and Pthreads Comparison - " + str(openmp[count].numRC) + " rows and columns"
    plt.title(plotTitle) 
    #plt.hist(openmp[count].results)
    #plt.hist(pthreads[count].results)

    figuresDir = "Figures"
    if not os.path.exists(figuresDir):
        os.makedirs(figuresDir)
    figure.savefig(figuresDir + "/" + "OpenMP_Pthreads_" + str(openmp[count].numRC) + "_Fig")
    plt.show()

    count += 1

