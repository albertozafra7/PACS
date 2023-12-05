__kernel void img_rot(__global const uchar *input,
                           __global uchar *output,
                           const int pivot_x,
                           const int pivot_y,
                           const float angle,
                           const int width,
                           const int height) {
   // const float pi = 3.14159265358979323846;
    //const float cosA = cos(angle * pi / 180.0);
    //const float sinA = sin(angle * pi / 180.0);

    const int i = get_global_id(0);
    const int j = get_global_id(1);

    if(i < width && j < height){
        const int newX = (i - pivot_x) * cos(angle) - (j - pivot_y) * sin(angle) + pivot_x;
        const int newY = (i - pivot_x) * sin(angle) + (j - pivot_y) * cos(angle) + pivot_y;

        if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
            // Map the 2D coordinates to the 1D array index
            const int src_index = (newY * width + newX);
            const int dest_index = (j * width + i);

            // Copy pixel from input to output
            for(int channel = 0; channel < 3; ++channel)
                output[src_index+channel*height*width] = input[dest_index+channel*height*width];
        }
    }
}
