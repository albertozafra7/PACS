__kernel void img_histogram(__global const uchar *input,
                             __global uint *histogramRed,
                             __global uint *histogramGreen,
                             __global uint *histogramBlue,
                             const int width,
                             const int height) {
    const int i = get_global_id(0);
    const int j = get_global_id(1);

    if (i < width && j < height) {
        // Calculate the 1D array index for the pixel
        const int index = (j * width + i);

        // Extract color channels
        uchar red = input[index];
        uchar green = input[index + height * width];
        uchar blue = input[index + 2 * height * width];

        // Update histograms
        atomic_inc(&histogramRed[red]);
        atomic_inc(&histogramGreen[green]);
        atomic_inc(&histogramBlue[blue]);
    }
}
