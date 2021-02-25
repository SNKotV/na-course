import os
import numpy as np
import matplotlib.pyplot as plt

if __name__ == '__main__':
	with open('key_value_pairs.txt', 'r') as f:
		x1 = np.array(list(map(float, f.readline().split())))
		y1 = np.array(list(map(float, f.readline().split())))

		x2 = np.array(list(map(float, f.readline().split())))
		y2 = np.array(list(map(float, f.readline().split())))

		fig, axs = plt.subplots(2, sharex=True)
		axs[0].plot(x1, y1)
		axs[0].set_title('Function')
		axs[1].plot(x2, y2)
		axs[1].set_title('Spline')

		plt.show()
