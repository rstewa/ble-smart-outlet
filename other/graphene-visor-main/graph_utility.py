import collections
import datetime as dt
import sys
import matplotlib
# matplotlib.use("TkAgg")
import matplotlib.pyplot as plt

from matplotlib.animation import FuncAnimation

degree_symbol = u"\u00b0"

# https://towardsdatascience.com/plotting-live-data-with-matplotlib-d871fac7500b
# https://learn.sparkfun.com/tutorials/graph-sensor-data-with-python-and-matplotlib/update-a-graph-in-real-time
def update(i):
    # times.popleft()
    times.append(i)
    # times.append(dt.datetime.now().strftime('%H:%M:%S.%f'))
    # temps.popleft()
    temps.append(float(sys.stdin.readline()))

    # times = times[-100:]
    # temps = temps[-100:]

    ax.cla()
    # ax.clear()
    ax.plot(times, temps)
    ax.scatter(len(temps) - 1, temps[-1])
    ax.text(len(temps) - 1, temps[-1] + 1, f'{str(temps[-1])}{degree_symbol}C')
    ax.set_ylim(0, 40)
    ax.set_xlim(0, 500)

    # plt.xticks({0, 100, 200, 300, 400})
    # plt.xticks({0, 100, 200, 300, 400}, rotation=45, ha='right')
    # plt.subplots_adjust(bottom=0.30)


times = collections.deque()
temps = collections.deque()

fig = plt.figure(figsize=(12, 6), facecolor='#DEDEDE')
ax = fig.add_subplot(111)
# ax.set_facecolor('#DEDEDE')

# ani = FuncAnimation(fig, update, frames=500, interval=10, blit=False)
ani = FuncAnimation(fig, update, interval=500)
plt.show()

# fpath = './graphs/new-graph.mp4'
# if os.path.exists(fpath):
#     os.remove(fpath)
# 
# ani.save(fpath, fps=30, dpi=300)
