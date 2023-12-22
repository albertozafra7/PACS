__kernel void img_flip(__global const uchar *input,
                        __global uchar *output,
                        const int width,
                        const int height) {
    const int i = get_global_id(0);
    const int j = get_global_id(1);

    if (i < width && j < height) {
        // Calculate the corresponding index for the flipped image
        const int newI = width - 1 - i;

        // Map the 2D coordinates to the 1D array index
        const int src_index = (j * width + newI);
        const int dest_index = (j * width + i);

        // Copy pixel from input to output
        for (int channel = 0; channel < 3; ++channel)
            output[src_index + channel * height * width] = input[dest_index + channel * height * width];
    }
}
