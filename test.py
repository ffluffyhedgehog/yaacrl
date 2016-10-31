#!/usr/bin/env python2

# Draws clear specgram orspecgram with peaks
# Change draw_peaks for right value

import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
spec = []
draw_peaks = False

with open('spec.test.txt', 'r') as f:
    for line in f:
        spec.append([float(x) for x in line.strip().split()])

if draw_peaks:
    peaks_times = []
    peaks_freqs = []
    with open('peaks.test.txt', 'r') as f:
        for line in f:
            freq, time = line.strip().split()
            peaks_times.append(int(time))
            peaks_freqs.append(int(freq))

spec = np.array(spec)
fig, ax = plt.subplots()
ax.imshow(spec)
if draw_peaks:
    ax.scatter(peaks_times, peaks_freqs)
ax.set_xlabel('time')
ax.set_ylabel('frequency')
plt.gca().invert_yaxis()
plt.show()

