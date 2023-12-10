import matplotlib.pyplot as plt
import numpy as np

def plot_histograms(file_path):
    # Read histogram values from the file
    data = np.genfromtxt(file_path, delimiter=',')

    # Extract data for each channel
    bin_indices = data[:, 0]
    histogram_red = data[:, 1]
    histogram_green = data[:, 2]
    histogram_blue = data[:, 3]

    # Plot histograms
    plt.figure(figsize=(10, 6))

    plt.subplot(3, 1, 1)
    plt.bar(bin_indices, histogram_red, color='red', alpha=0.7)
    plt.title('Red Channel Histogram')

    plt.subplot(3, 1, 2)
    plt.bar(bin_indices, histogram_green, color='green', alpha=0.7)
    plt.title('Green Channel Histogram')

    plt.subplot(3, 1, 3)
    plt.bar(bin_indices, histogram_blue, color='blue', alpha=0.7)
    plt.title('Blue Channel Histogram')

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    # Specify the file path
    file_path = "histogram_values.txt"

    # Plot histograms
    plot_histograms(file_path)

