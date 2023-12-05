__kernel void img_rot(__global const uchar *input,
                           __global uchar *output,
                           const int pivot_x,
                           const int pivot_y,
                           const float angle,
                           const int width,
                           const int height) {
    const float pi = 3.14159265358979323846;
    const float cosA = cos(angle * pi / 180.0);
    const float sinA = sin(angle * pi / 180.0);

    const int x = get_global_id(0);
    const int y = get_global_id(1);


    // Calculate the rotated coordinates
    const int newX = (x - pivot_x) * cosA - (y - pivot_y) * sinA + pivot_x;
    const int newY = (x - pivot_x) * sinA + (y - pivot_y) * cosA + pivot_y;

    if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
        // Map the 2D coordinates to the 1D array index
        const int index = newY * width + newX;

        // Copy pixel from input to output
        output[y * width + x] = input[index];
    }
    else {
        // If the new coordinates are out of bounds, set the pixel to black
        output[y * width + x] = 0;
    }
}
