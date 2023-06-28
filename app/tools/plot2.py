import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.animation import FuncAnimation


j = 0

def init():
    #ax.set_xlim(left=2, right=None)
    #ax.set_ylim(bottom=None, top=7)
    #ax.set_xlim(0, 1000)
    #ax.set_ylim(0, 300000)
    #ln.set_data([], [])
    return ln,

def update(i, f, xdata, ydata, ln):
    global j
    pullData = f.read()
    dataArray = pullData.split('\n')
    for eachLine in dataArray:
        if len(eachLine)>1:
            g = eachLine.split()
            val_avg = int(g[2].split(':')[1])
            val_min = int(g[3].split(':')[1])
            val_max = int(g[4].split(':')[1])
            #print(j, val_avg)
            j += 1
            xdata.append(j)
            ydata[0].append(val_avg)
            ydata[1].append(val_min)
            ydata[2].append(val_max)
            #ln.set_data(xdata, ydata)
    #xdata.append(frame)
    #ydata.append(np.sin(frame))
    ln.set_data(xdata, ydata[0])
    #p = plt.fill_between(xdata, ydata1, ydata2, facecolor = 'C1', alpha = 0.2)
    return ln,




fig, ax = plt.subplots()
ax.set(xlim=(None, 1e5), ylim=(None, 1e6), title='ADC', xlabel='Time (s)', ylabel='Voltage (µV)')
#ax.set(xlim=(None, 1e5), ylim=(None, 1e6), title='ADC', xlabel='Time (s)', ylabel='Ampere (pA)')
xdata = []
ydata = [[],[],[]] # Avg, Min, Max
ln, = plt.plot([], [], lw=2)
#f = open("C:/Users/JohanSA/Documents/YAT/YAT-Log-20230626-171333.log","r")
#f = open("YAT-Log-20230626-171849.log","r")
f = open("YAT-Log-20230622-130141.log","r")
#plt.xticks(rotation=45, ha='right')
#plt.subplots_adjust(bottom=0.30)
#plt.title('TMP102 Temperature over Time')
#plt.ylabel('Temperature (deg C)')
#plt.xlim(left=0)

ani = FuncAnimation(fig, update, init_func=init, blit=True, fargs=(f, xdata, ydata, ln), interval=100)
plt.show()