import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y = [], []
data2_x, data2_y = [], []
data3_x, data3_y = [], []

# Read data from the first CSV file
with open('matrices_lv_clock_1.csv', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y = map(float, row)
        data1_x.append(x)
        data1_y.append(y)

# Read data from the second CSV file
with open('matrices_lv_clock.csv', 'r') as file2:
    csv_reader = csv.reader(file2)
    for row in csv_reader:
        x, y = map(float, row)
        data2_x.append(x)
        data2_y.append(y)
        
# Read data from the second CSV file
with open('matrices_lv_clock_2.csv', 'r') as file3:
    csv_reader = csv.reader(file3)
    for row in csv_reader:
        x, y = map(float, row)
        data3_x.append(x)
        data3_y.append(y)

        

# Create a single plot for all data
plt.plot(data1_x, data1_y, label='matrices_lv_1')
plt.plot(data2_x, data2_y, label='matrices_lv_2')
plt.plot(data3_x, data3_y, label='matrices_lv_3')

# Customize the plot as needed (e.g., labels, titles, legends)
plt.xlabel('Size of the matrix')
plt.ylabel('Time spent (s)')
plt.title('Time spent by each implementation')
plt.legend()

# Show or save the plot
plt.show()
# If you want to save the plot as an image file (e.g., PNG), you can use plt.savefig('output.png')

for i in range(0,len(data1_y)):
	mean = (data1_y[i] + data2_y[i] + data3_y[i])/3
	numerator = (data1_y[i] - mean)**2 + (data2_y[i] - mean)**2 + (data3_y[i] - mean)**2
	deviation =  math.sqrt(numerator/3)
	#print(data1_x[i], ",", mean)
	print(data1_x[i], ",", deviation)

