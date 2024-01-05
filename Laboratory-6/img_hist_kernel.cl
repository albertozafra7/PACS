__kernel void img_histogram(__global const uchar3 *input,
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

        // Extract color channels as uchar3
        uchar3 pixel = input[index];

        // Update histograms
        atomic_inc(&histogramRed[pixel.s0]);
        atomic_inc(&histogramGreen[pixel.s1]);
        atomic_inc(&histogramBlue[pixel.s2]);
    }
}