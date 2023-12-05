import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y, data1_z, data1_w = [], [], [], []
data2_x, data2_y, data2_z, data2_w = [], [], [], []
data3_x, data3_y, data3_z, data3_w = [], [], [], []
data4_x, data4_y, data4_z = [], [], []

# Read data from the first CSV file
with open('filtered_times_lv_getime.txt', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data1_x.append(x)
        data1_y.append(y)
        data1_z.append(z)
        data1_w.append(w)

# Read data from the second CSV file
with open('filtered_times_lv_getime_1.txt', 'r') as file2:
    csv_reader = csv.reader(file2)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data2_x.append(x)
        data2_y.append(y)
        data2_z.append(z)
        data2_w.append(w)

# Read data from the third CSV file
with open('filtered_times_lv_getime_2.txt', 'r') as file3:
    csv_reader = csv.reader(file3)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data3_x.append(x)
        data3_y.append(y)
        data3_z.append(z)
        data3_w.append(w)
        
# Read data from the fourth CSV file
with open('average_matrices_lv_getime.csv', 'r') as file4:
    csv_reader = csv.reader(file4)
    for row in csv_reader:
        x, y, z = map(float, row)
        data4_x.append(x)
        data4_y.append(y)
        data4_z.append(z)
        

for i in range(0,len(data1_x)):
	mean_x = (data1_x[i] + data2_x[i] + data3_x[i])/3
	mean_y = (data1_y[i] + data2_y[i] + data3_y[i])/3
	mean_z = (data1_z[i] + data2_z[i] + data3_z[i])/3
	numerator_x = (data1_x[i] - mean_x)**2 + (data2_x[i] - mean_x)**2 + (data3_x[i] - mean_x)**2
	deviation_x =  math.sqrt(numerator_x/3)
	numerator_y = (data1_y[i] - mean_y)**2 + (data2_y[i] - mean_y)**2 + (data3_y[i] - mean_y)**2
	deviation_y =  math.sqrt(numerator_y/3)
	numerator_z = (data1_z[i] - mean_z)**2 + (data2_z[i] - mean_z)**2 + (data3_z[i] - mean_z)**2
	deviation_z =  math.sqrt(numerator_z/3)
	print(data4_x[i], ",", mean_x, ",", mean_y, ",", mean_z)
	#print(data3_x[i], ",", deviation_x, ",", deviation_y, ",", deviation_z)

