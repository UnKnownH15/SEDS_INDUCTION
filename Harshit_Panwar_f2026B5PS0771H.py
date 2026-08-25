import pandas as pnd
import numpy as ny
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# data extraction step
data_1 = pnd.read_csv('Depth Data.csv')
data_1['Depth (m)'] = pnd.to_numeric(data_1['Depth (m)'], errors='coerce')
data_1['Depth (m)'] = data_1['Depth (m)'].interpolate(method='linear')

# ignoring sudden data change like too sudden drop or spike
mediannn = data_1['Depth (m)'].rolling(window=3, center=True).median()
difference = ny.abs(data_1['Depth (m)'] - mediannn)

data_1['Depth_Despiked'] = ny.where(difference > 10, mediannn, data_1['Depth (m)'])
data_1['Depth_Despiked'] = data_1['Depth_Despiked'].fillna(data_1['Depth (m)'])

# smooth random noise
data_1['Depth_Clean'] = data_1['Depth_Despiked'].rolling(window=10, center=True).mean().fillna(data_1['Depth_Despiked'])

# animation
fig, ax = plt.subplots(figsize=(10, 6))

ax.set_xlim(0, len(data_1))
ax.set_ylim(data_1['Depth_Clean'].min() - 50, 0)
ax.set_title('Live Sea Floor Depth Monitoring')
ax.set_xlabel('Time (s)')
ax.set_ylabel('Depth (m)')
ax.grid(True)


line, = ax.plot([], [], color='red', linewidth=2, label='cleaned depth')
ax.legend()

def animate(i):
    x = data_1['Point'].iloc[:i]
    y = data_1['Depth_Clean'].iloc[:i]
    line.set_data(x, y)
    return line,

ani = animation.FuncAnimation(fig, animate, frames=len(data_1), interval=10, blit=True)

plt.show()