import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y, data1_z = [], [], []
data2_x, data2_y, data2_z = [], [], []
data3_x, data3_y, data3_z = [], [], []

# Read data from the first CSV file
with open('matrices_lv_getime.csv', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y, z = map(float, row)
        data1_x.append(x)
        data1_y.append(y)
        data1_z.append(z)
        

# Read data from the second CSV file
with open('matrices_lv_getime_1.csv', 'r') as file2:
    csv_reader = csv.reader(file2)
    for row in csv_reader:
        x, y, z = map(float, row)
        data2_x.append(x)
        data2_y.append(y)
        data2_z.append(z)
        
# Read data from the second CSV file
with open('matrices_lv_getime_2.csv', 'r') as file3:
    csv_reader = csv.reader(file3)
    for row in csv_reader:
        x, y, z = map(float, row)
        data3_x.append(x)
        data3_y.append(y)
        data3_z.append(z)

        

# Create a single plot for all data
plt.plot(data1_x, data1_y, label='matrices_lv_1 (multiplication)')
plt.plot(data2_x, data2_y, label='matrices_lv_2 (multiplication)')
plt.plot(data3_x, data3_y, label='matrices_lv_3 (multiplication)')
plt.plot(data1_x, data1_z, label='matrices_lv_1 (various)')
plt.plot(data2_x, data2_z, label='matrices_lv_2 (various)')
plt.plot(data3_x, data3_z, label='matrices_lv_3 (various)')
# Customize the plot as needed (e.g., labels, titles, legends)
plt.xlabel('Size of the matrix')
plt.ylabel('Time spent (ms)')
plt.title('Time spent by each implementation')
plt.legend()

# Show or save the plot
plt.show()
# If you want to save the plot as an image file (e.g., PNG), you can use plt.savefig('output.png')

for i in range(0,len(data1_y)):
	mean_y = (data1_y[i] + data2_y[i] + data3_y[i])/3
	mean_z = (data1_z[i] + data2_z[i] + data3_z[i])/3
	numerator_y = (data1_y[i] - mean_y)**2 + (data2_y[i] - mean_y)**2 + (data3_y[i] - mean_y)**2
	deviation_y =  math.sqrt(numerator_y/3)
	numerator_z = (data1_z[i] - mean_z)**2 + (data2_z[i] - mean_z)**2 + (data3_z[i] - mean_z)**2
	deviation_z =  math.sqrt(numerator_z/3)
	print(data1_x[i], ",", mean_y, ",", mean_z)
	#print(data1_x[i], ",", deviation_y, ",", deviation_z)

