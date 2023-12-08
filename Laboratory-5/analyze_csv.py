import pandas as pd
import numpy as np

def analyze_csv(file_path):
    # Read the CSV file into a pandas DataFrame with default headers
    df = pd.read_csv(file_path, header=None, names=[
        'Total Execution Time (s)',
        'Kernel Execution Time (s)',
        'Host to Device (Write) Bandwidth (MB/s)',
        'Host to Device (Read) Bandwidth (MB/s)',
        'Kernel Bandwidth (MB/ns)'
    ])

    # Initialize dictionaries to store mean and standard deviation for each column
    means = {}
    std_devs = {}

    # Iterate through columns
    for column in df.columns:
        # Convert column values to numeric, ignoring 'inf' values
        numeric_values = pd.to_numeric(df[column], errors='coerce')
        numeric_values = numeric_values[~np.isinf(numeric_values)]

        # Calculate mean and standard deviation
        column_mean = numeric_values.mean()
        column_std_dev = numeric_values.std()

        # Store results in dictionaries
        means[column] = column_mean
        std_devs[column] = column_std_dev

    return means, std_devs

# Usage
file_path = ['img_rot_metrics.csv','img_flip_metrics.csv','img_hist_metrics.csv'] # Measurement files
for f in file_path:
    means, std_devs = analyze_csv(f)

    # Print results
    print(f + " Results:")
    print("\tMean values:")
    print(means)
    print("\n\tStandard deviation values:")
    print(std_devs)
