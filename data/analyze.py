import numpy as np
import matplotlib.pyplot as plt
import sys

if(len(sys.argv) < 2):
    print("Usage: python analyze.py <data file 1> <data file 2> <data file 3> ...")
    sys.exit(1)

files = sys.argv[1:]
datas = []

for file in files:

    # import data as csv
    data = np.genfromtxt(file, delimiter=',', skip_header=1)

    # plot data
    plt.subplot(2,1,1)
    plt.plot(data[:,0], data[:,1])
    plt.title("Lift vs. Time")
    plt.ylabel("Lift")
    plt.xlabel("Time [s]")

    plt.subplots_adjust(hspace=0.5)

    plt.subplot(2,1,2)
    plt.plot(data[:,0], data[:,2])
    plt.title("Drag vs. Time")
    plt.ylabel("Drag")
    plt.xlabel("Time [s]")

plt.show()