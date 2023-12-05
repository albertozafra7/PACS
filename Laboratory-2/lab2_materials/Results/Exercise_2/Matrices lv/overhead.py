import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y, data1_z = [], [], []


# Read data from the first CSV file
with open('average_matrices_lv_getime.csv', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y, z = map(float, row)
        data1_x.append(x)
        data1_y.append(y)
        data1_z.append(z)
        


        

for i in range(0,len(data1_y)):
	total_time = (data1_y[i] + data1_z[i])
	overhead = data1_z[i]/total_time
	overhead = overhead*100
	print(overhead)

