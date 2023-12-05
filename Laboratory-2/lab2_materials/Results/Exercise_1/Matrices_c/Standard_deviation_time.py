import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y, data1_z, data1_w = [], [], [], []
data2_x, data2_y, data2_z, data2_w = [], [], [], []
data3_x, data3_y = [], []

# Read data from the first CSV file
with open('times_c_clock_filtered.txt', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data1_x.append(x)
        data1_y.append(y)
        data1_z.append(z)
        data1_w.append(w)

# Read data from the second CSV file
with open('times_c_clock_1_filtered.txt', 'r') as file2:
    csv_reader = csv.reader(file2)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data2_x.append(x)
        data2_y.append(y)
        data2_z.append(z)
        data2_w.append(w)
        
# Read data from the second CSV file
with open('matrices_c_clock_1.csv', 'r') as file3:
    csv_reader = csv.reader(file3)
    for row in csv_reader:
        x, y = map(float, row)
        data3_x.append(x)
        data3_y.append(y)
        

for i in range(0,len(data1_x)):
	mean_x = (data1_x[i] + data2_x[i])/2
	mean_y = (data1_y[i] + data2_y[i])/2
	mean_z = (data1_z[i] + data2_z[i])/2
	numerator_x = (data1_x[i] - mean_x)**2 + (data2_x[i] - mean_x)**2
	deviation_x =  math.sqrt(numerator_x/2)
	numerator_y = (data1_y[i] - mean_y)**2 + (data2_y[i] - mean_y)**2
	deviation_y =  math.sqrt(numerator_y/2)
	numerator_z = (data1_z[i] - mean_z)**2 + (data2_z[i] - mean_z)**2
	deviation_z =  math.sqrt(numerator_z/2)
	print(data3_x[i], ",", mean_x, ",", mean_y, ",", mean_z)
	#print(data3_x[i], ",", deviation_x, ",", deviation_y, ",", deviation_z)

for i in range(len(data1_x)+1,len(data2_x)):
	print(data3_x[i], ",", data2_x[i],",",data2_y[i], ",", data2_z[i])
	#print(data1_x[i],",",deviation)
	#print(deviation)

