import os
import numpy as np
import matplotlib.pyplot as plt

if __name__ == '__main__':
	with open('key_value_pairs.txt', 'r') as f:
		x1 = np.array(list(map(float, f.readline().split())))
		y1 = np.array(list(map(float, f.readline().split())))

		x2 = np.array(list(map(float, f.readline().split())))
		y2 = np.array(list(map(float, f.readline().split())))

		print(y2)

		plt.scatter(x1, y1)
		plt.plot(x2, y2, linewidth=2.0, color='red')

		plt.show()
