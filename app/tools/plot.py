import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import time



fig = plt.figure()
ax1 = fig.add_subplot(1,1,1)
j = 0
f = open("C:/Users/JohanSA/Documents/YAT/YAT-Log-20230620-174545.log","r")
def animate(i):
    global j
    pullData = f.read()
    dataArray = pullData.split('\n')
    xar = []
    yar = []
    for eachLine in dataArray:
        if len(eachLine)>1:
            g = eachLine.split()
            val_avg = g[2].split(':')[1]
            val_min = g[3].split(':')[1]
            val_max = g[4].split(':')[1]
            #print(j, val_avg)
            j += 1
            xar.append(j)
            yar.append(int(val_avg))
    #ax1.clear()
    ax1.plot(xar,yar)
    
    
ani = animation.FuncAnimation(fig, animate, interval=1)
plt.show()

ax1.set_xlabel('time (sec)', fontsize=20)
ax1.set_ylabel('µV', fontsize=20)