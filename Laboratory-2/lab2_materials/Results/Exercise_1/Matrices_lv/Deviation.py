import matplotlib.pyplot as plt
import math
import csv

# Initialize empty lists to store data from each CSV file
data1_x, data1_y, data1_z, data1_w = [], [], [], []
data2_x, data2_y = [], []

# Read data from the first CSV file
with open('mean_times_matrices_lv_clock.txt', 'r') as file1:
    csv_reader = csv.reader(file1)
    for row in csv_reader:
        x, y, z, w = map(float, row)
        data1_x.append(x)
        data1_y.append(y)
        data1_z.append(z)
        data1_w.append(w)
        
# Read data from the fourth CSV file
with open('means_matrices_lv_clock.csv', 'r') as file4:
    csv_reader = csv.reader(file4)
    for row in csv_reader:
        x, y = map(float, row)
        data2_x.append(x)
        data2_y.append(y)
        
deviation = []
deviation_y = []
deviation_x = []
for i in range(0,len(data1_x)):
	mean = (data1_y[i] + data2_y[i] + data1_w[i])/3
	mean_x = (data1_y[i] + data2_y[i])/2
	mean_y = (data1_w[i] + data2_y[i])/2
	numerator = (data1_y[i] - mean)**2 + (data2_y[i] - mean)**2 + (data1_w[i] - mean)**2
	deviation.append(math.sqrt(numerator/3))
	numerator_y = (data1_w[i] - mean_y)**2 + (data2_y[i] - mean_y)**2
	deviation_y.append(math.sqrt(numerator_y/2))
	numerator_x = (data1_y[i] - mean_x)**2 + (data2_y[i] - mean_x)**2
	deviation_x.append(math.sqrt(numerator_x/2))
	
sumatory = 0
sumatory_x = 0
sumatory_y = 0
for i in range(0,len(deviation)):
	sumatory += deviation[i]
	sumatory_y += deviation_y[i]
	sumatory_x += deviation_x[i]


#print(data4_x[i], ",", mean_x, ",", mean_y, ",", mean_z)
print(len(deviation))
print()
print("total standard average deviation = ",  sumatory/len(deviation))
print("standard average deviation elapsed = ", sumatory_y/len(deviation))
print("standard average deviation user = ", sumatory_x/len(deviation))

