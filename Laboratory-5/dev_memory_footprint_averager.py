import numpy as np

def calculate_statistics(file_path):
    # Lists to store used GPU memory values
    used_gpu_memory_values = []

    # Read the file
    with open(file_path, 'r') as file:
        for line in file:
            # Split the line by spaces
            parts = line.split()

            # Check if the line has at least two parts
            if len(parts) >= 2:
                # Assuming the second column contains used_gpu_memory values
                used_gpu_memory = float(parts[1].replace(',', ''))
                used_gpu_memory_values.append(used_gpu_memory)

    # Calculate average and standard deviation
    if used_gpu_memory_values:
        average_memory = np.mean(used_gpu_memory_values)
        std_deviation_memory = np.std(used_gpu_memory_values)
        return average_memory, std_deviation_memory
    else:
        return None, None

# Replace 'memory_usage_log.txt' with the actual file path
file_path = 'dev_mem_rot.txt'

average, std_deviation = calculate_statistics(file_path)

if average is not None and std_deviation is not None:
    print(f"Average Used GPU Memory: {average} MB")
    print(f"Standard Deviation of Used GPU Memory: {std_deviation} MB")
else:
    print("No data found in the input file.")

