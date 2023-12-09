import numpy as np

# Function to parse the output file and extract RSS values
def parse_top_output(file_path):
    rss_values = []
    
    with open(file_path, 'r') as file:
        for line in file:
            columns = line.split()
            if len(columns) >= 6 and columns[0] != "USER":  # Skip header line
                rss_values.append(int(columns[5]))
    
    return rss_values

# Function to calculate average and standard deviation
def calculate_stats(rss_values):
    average_rss = np.mean(rss_values)
    std_dev_rss = np.std(rss_values)
    return average_rss, std_dev_rss

# Main function
def main():
    file_path = 'host_mem_hist.txt'  # Replace with the actual file path
    rss_values = parse_top_output(file_path)

    if rss_values:
        average_rss, std_dev_rss = calculate_stats(rss_values)
        print(f"Average RSS: {average_rss} KB")
        print(f"Standard Deviation RSS: {std_dev_rss} KB")
    else:
        print("No valid RSS values found in the file.")

if __name__ == "__main__":
    main()

