import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

data = pd.read_csv("Depth Data.csv")

raw_depth = pd.to_numeric(data["Depth (m)"], errors="coerce")

clean_depth = raw_depth.copy()
mean_depth = clean_depth.mean()
std_depth = clean_depth.std()
clean_depth[(clean_depth - mean_depth).abs() > 3 * std_depth] = np.nan
clean_depth = clean_depth.interpolate()
clean_depth = clean_depth.bfill()

smooth_depth = clean_depth.rolling(window=7, min_periods=1, center=True).mean()

time = np.arange(1, len(smooth_depth) + 1)

fig, ax = plt.subplots(figsize=(10, 6))
raw_line, = ax.plot([], [], color="red", linewidth=1, label="Original Data")
smooth_line, = ax.plot([], [], color="blue", linewidth=2, label="Filtered Data")

ax.set_xlim(0, len(time))
ax.set_ylim(min(raw_depth.min(), smooth_depth.min()) - 20, max(raw_depth.max(), smooth_depth.max()) + 20)
ax.set_title("Ship Depth vs Time")
ax.set_xlabel("Time (s)")
ax.set_ylabel("Depth (m)")
ax.grid(True)
ax.legend()

def update(frame):
    raw_line.set_data(time[:frame], raw_depth[:frame])
    smooth_line.set_data(time[:frame], smooth_depth[:frame])
    return raw_line, smooth_line

ani = animation.FuncAnimation(fig, update, frames=len(time), interval=20, blit=True, repeat=False)

plt.show()