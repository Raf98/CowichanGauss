from scipy import stats
import numpy as np
from scipy.stats import kstest
from matplotlib import pyplot as plt
import math
import os

# plt.plot([0,1,2,3,4])
# plt.show()

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
    #print('KS-statistic D = %6.3f pvalue = %6.4f' % kstest(x, 't'))


