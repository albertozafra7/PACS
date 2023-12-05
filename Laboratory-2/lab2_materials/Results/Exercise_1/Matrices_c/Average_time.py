import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y = [], []


# Read data from the first CSV file
with open('matrices_c_3.csv', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y = map(float, row)
        data1_x.append(x)
        data1_y.append(y)


mean = 0

for i in range(0,len(data1_y)):
	mean = data1_y[i]/data1_x[i]

print(mean)

