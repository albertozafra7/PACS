__kernel void img_flip(__global const uchar3 *input,
                        __global uchar3 *output,
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
        output[src_index] = input[dest_index];
    }
}
