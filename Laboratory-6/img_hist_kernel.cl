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

__kernel void img_histogram(__global const uchar3 *input,
                             __global uint *histogramRed,
                             __global uint *histogramGreen,
                             __global uint *histogramBlue,
                             const int width,
                             const int height) {
    const int gid_x = get_global_id(0);
    const int gid_y = get_global_id(1);
    const int lid_x = get_local_id(0);
    const int lid_y = get_local_id(1);
    const int group_size_x = get_local_size(0);
    const int group_size_y = get_local_size(1);

    // Shared memory for local histograms
    local uint localHistRed[256];
    local uint localHistGreen[256];
    local uint localHistBlue[256];

    // Initialize local histograms
    if (lid_x < 256) {
        localHistRed[lid_x] = 0;
        localHistGreen[lid_x] = 0;
        localHistBlue[lid_x] = 0;
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // Process pixels in the local group
    for (int j = lid_y; j < height; j += group_size_y) {
        for (int i = lid_x; i < width; i += group_size_x) {
            const int index = j * width + i;
            uchar3 pixel = input[index];

            atomic_inc(&localHistRed[pixel.s0]);
            atomic_inc(&localHistGreen[pixel.s1]);
            atomic_inc(&localHistBlue[pixel.s2]);
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // Accumulate local histograms into global histograms
    if (lid_x < 256) {
        for (int i = lid_x; i < 256; i += group_size_x) {
            atomic_add(&histogramRed[i], localHistRed[i]);
            atomic_add(&histogramGreen[i], localHistGreen[i]);
            atomic_add(&histogramBlue[i], localHistBlue[i]);
        }
    }
}

