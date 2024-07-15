import numpy as np
import matplotlib.pyplot as plt
import sys

file = sys.argv[1]

# import data as csv

data = np.genfromtxt(file, delimiter=',', skip_header=1)

# plot data
plt.subplot(2,1,1)
plt.plot(data[:,0], data[:,1])
plt.title("Lift vs. Time")

plt.subplot(2,1,2)
plt.plot(data[:,0], data[:,2])
plt.title("Drag vs. Time")

plt.show()