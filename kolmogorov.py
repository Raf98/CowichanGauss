from scipy import stats
import numpy as np
from scipy.stats import kstest
from matplotlib import pyplot as plt
import math
import os


class SimulationData:
    def __init__(self, results, numRC, label):
        self.results = results
        self.numRC = numRC
        self.label = label
        self.mean = 0
        self.median = 0
        self.variance = 0
        self.stdDeviation = 0
        self.KSStatisticD = 0
        self.pvalue = 0

    def calcMean(self):
        sumAll = 0
        for i in self.results:
            sumAll += i
        self.mean = sumAll/self.results.__len__()

    def calcMedian(self):
        sortedResults = sorted(self.results)
        pos1 = int((self.results.__len__())/2)
        pos2 = int((self.results.__len__())/2) + 1
        #print(str(pos1) + " " + str(pos2))
        self.median = (sortedResults[pos1] + sortedResults[pos2])/2

    def calcVariance(self):
        numeratorVar = 0
        for i in self.results:
            numeratorVar += math.pow(i - self.mean, 2)
            #print(str(numeratorVar) + " " + str(self.mean))
        self.variance = numeratorVar/(self.results.__len__())
        #print(str(self.results.__len__()))

    def calcStdDeviation(self):
        self.stdDeviation = math.sqrt(self.variance)

files = []

#files.append(open("output_Seq_3_test1", "r"))
#files.append(open("output_Seq_5_test2", "r"))
#files.append(open("output_Seq_10_test3", "r"))

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

    results = [float(i) for i in strArray]
    # print(results)

    figure = plt.figure()

    plt.plot(results)
    plt.xlabel("Sample")
    plt.ylabel("Execution time(ns)")

    samplesList = [i+1 for i in range(results.__len__())]
    # print(samplesList)

    fileName = os.path.basename(file.name)
    plotTitle = fileName.split("_")[1] + "-" + \
        fileName.split("_")[2] + " rows and columns"
    plt.title(plotTitle)
    plt.scatter(samplesList, results)

    figuresDir = "Figures"
    if not os.path.exists(figuresDir):
        os.makedirs(figuresDir)
    figure.savefig(figuresDir + "/" + fileName + "_Fig")
    plt.show()

    print(plotTitle)
    x = np.array(results)

    if("OpenMP" in fileName):
        sd = SimulationData(results, int(fileName.split("_")[2]), "OpenMP")
        openmp.append(sd)
    if("Pthreads" in fileName):
        sd = SimulationData(results, int(fileName.split("_")[2]), "Pthreads")
        pthreads.append(sd)

count = 0

# plt.cla()
# plt.clf()

benchmarkData = []

while(count < 3):
    benchmarkData.append(openmp[count])
    benchmarkData.append(pthreads[count])

    figure = plt.figure()
    plt.plot(openmp[count].results, label=openmp[count].label)
    plt.plot(pthreads[count].results, label=pthreads[count].label)
    plt.legend()
    plt.xlabel("Sample")
    plt.ylabel("Execution time(ns)")

    plotTitle = "OpenMP and Pthreads Comparison - " + \
        str(openmp[count].numRC) + " rows and columns"
    plt.title(plotTitle)

    figuresDir = "Figures"
    if not os.path.exists(figuresDir):
        os.makedirs(figuresDir)
    figure.savefig(figuresDir + "/" + "OpenMP_Pthreads_" +
                   str(openmp[count].numRC) + "_Fig")
    plt.show()

    count += 1

figure = plt.figure()
figuresDir = "Figures"

boxPlotData = [i.results for i in benchmarkData]
boxPlotLabel = [(i.label + "-" + str(i.numRC)) for i in benchmarkData]
plt.boxplot(boxPlotData, patch_artist=True, labels=boxPlotLabel)

figure.savefig(figuresDir + "/" + "OpenMP_Pthreads_BoxPlot")
plt.show()


detailsFile = open("benchmarkDetailsFile", "w")

for bd in benchmarkData:
    bd.calcMean()
    bd.calcMedian()
    bd.calcVariance()
    bd.calcStdDeviation()

    x = np.array(bd.results)
    KSValues = kstest(x, 'norm', (bd.mean, bd.stdDeviation))
    bd.KSStatisticD = KSValues[0]
    bd.pvalue = KSValues[1]
    print('KS-statistic D = %f pvalue = %f', bd.KSStatisticD, bd.pvalue)

    fileStr = bd.label + "-" + str(bd.numRC).replace('.',',') + ": Mean: " + str(bd.mean).replace('.',',') + "; Median: " + str(bd.median).replace('.',',') + \
            "; Variance: " + str(bd.variance).replace('.',',') + "; Standard Deviation: " + str(bd.stdDeviation).replace('.',',') + \
                "; KS-Statistic D: " + str(bd.KSStatisticD).replace('.',',') + "; pvalue: " + str(bd.pvalue).replace('.',',') +"\n"
    detailsFile.write(fileStr)

detailsFile.close()
