import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y = [], []
data2_x, data2_y, data2_z, data2_w = [], [], [], []
data3_x, data3_y = [], []

# Read data from the first CSV file
with open('matrices_c_clock_mean.csv', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y = map(float, row)
        data1_x.append(x)
        data1_y.append(y)

# Read data from the second CSV file
with open('mean_times_matrice_c_clock.txt', 'r') as file2:
    csv_reader = csv.reader(file2)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data2_x.append(x)
        data2_y.append(y)
        data2_z.append(z)
        data2_w.append(w)

        
#print(data1_y)
# Create a single plot for all data
plt.plot(data1_x, data1_y, label='clock')
plt.plot(data2_x, data2_y, label='time user')
#plt.plot(data2_x, data2_z, label='time system')
plt.plot(data2_x, data2_w, label='time elapsed')

# Customize the plot as needed (e.g., labels, titles, legends)
plt.xlabel('Size of the matrix')
plt.ylabel('Time spent (s)')
#plt.title('Time measured by clock and time libraries')
plt.legend()

# Show or save the plot
plt.show()

