////////////////////////////////////////////////////////////////////
//File: Flip_Img_env.cpp
//
//Description: file for the image flip with openCL
//
// 
////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

// Image libraries
#define cimg_use_jpeg
#include <iostream>
#include "CImg/CImg.h"
using namespace cimg_library;

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
  
// check error, in such a case, it exits

void cl_error(cl_int code, const char *string){
	if (code != CL_SUCCESS){
		printf("%d - %s\n", code, string);
	    exit(-1);
	}
}

// Function to initialize an array with sequential values
void initializeArray(float *array, size_t size) {
  for (size_t i = 0; i < size; ++i) {
      array[i] = (float)i;  // Initialization to floating points
  }
}

void fillImg(cl_uchar3 *filledImg, CImg<unsigned char> originImg){
    // Get image dimensions
    const int width = originImg.width();
    const int height = originImg.height();

    // Iterate through image pixels and fill with specified color
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Set pixel value to filledImg
            filledImg[y * width + x] = {originImg(x, y, 0), originImg(x, y, 1), originImg(x, y, 2)};
        }
    }
}

void fillImg(CImg<unsigned char> &destImg, cl_uchar3 *fillerImg){
    // Get image dimensions
    const int width = destImg.width();
    const int height = destImg.height();

    // Iterate through image pixels and fill with specified color
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // Set pixel value to filledImg
            destImg(x,y,0) = fillerImg[y * width + x].s[0];
            destImg(x,y,1) = fillerImg[y * width + x].s[1];
            destImg(x,y,2) = fillerImg[y * width + x].s[2];
        }
    }
}

void plotHistogram(const unsigned int* data, size_t arraySize) {
    // Find the maximum value in the data
    unsigned int maxVal = *std::max_element(data, data + arraySize);

    // Set the number of bins for the histogram
    const int numBins = 255;

    // Calculate the width of each bin
    double binWidth = static_cast<double>(maxVal) / numBins;

    // Initialize a histogram vector
    std::vector<unsigned int> histogram(numBins, 0);

    // Fill the histogram
    for (size_t i = 0; i < arraySize; ++i) {
        int binIndex = static_cast<int>(data[i] / binWidth);
        if (binIndex == numBins) {
            binIndex--; // Adjust for the last bin
        }
        histogram[binIndex]++;
    }

    // Normalize the histogram for better visualization
    unsigned int maxBinHeight = *std::max_element(histogram.begin(), histogram.end());
    for (unsigned int& binHeight : histogram) {
        binHeight = static_cast<unsigned int>((static_cast<double>(binHeight) / maxBinHeight) * 50); // Normalize to a maximum height of 50
    }

    // Plot the histogram
    for (int i = 0; i < numBins; ++i) {
        std::cout << i * binWidth << " - " << (i + 1) * binWidth << " | ";
        for (unsigned int j = 0; j < histogram[i]; ++j) {
            std::cout << "*";
        }
        std::cout << std::endl;
    }
}

void writeHistogramValuesToFile(const unsigned int* histogramRed, const unsigned int* histogramGreen, const unsigned int* histogramBlue, size_t arraySize, const std::string& fileName) {
    // Open the file for writing
    std::ofstream outFile(fileName);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << fileName << std::endl;
        return;
    }

    // Write histogram values to the file
    for (size_t i = 0; i < arraySize; ++i) {
        outFile << i << "," << histogramRed[i] << "," << histogramGreen[i] << "," << histogramBlue[i] << std::endl;
    }

    // Close the file
    outFile.close();

    std::cout << "Histogram values written to: " << fileName << std::endl;
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{

  bool standard_print = true;
  if(argc == 2){
    if((strchr(argv[1], 'F') != NULL) || (strchr(argv[1], 'f') != NULL))
      standard_print = false;
  }

  // -------- Global Execution time --------
	clock_t global_start_time;

  global_start_time = clock();

  // --------------------------------------

  // Start of the program

  int err;                            	// error code returned from api calls
  size_t t_buf = 100;			// size of str_buffer
  char str_buffer[t_buf];		// auxiliary buffer	
  size_t e_buf;				// effective size of str_buffer in use
	    
  size_t global_size[2];                      	// global domain size for our calculation
  size_t local_size[2];                       	// local domain size for our calculation

  const cl_uint num_platforms_ids = 10;				// max of allocatable platforms
  cl_platform_id platforms_ids[num_platforms_ids];		// array of platforms
  cl_uint n_platforms;						// effective number of platforms in use
  const cl_uint num_devices_ids = 10;				// max of allocatable devices
  cl_device_id devices_ids[num_platforms_ids][num_devices_ids];	// array of devices
  cl_uint cl_n_devices[num_platforms_ids];				// effective number of devices in use for each platform
	
  cl_device_id device_id;             				// compute device id 
  cl_context context;                 				// compute context
  cl_command_queue command_queue[2];     				// compute command queue
  cl_program program;                         // define a program
  cl_kernel kernel;                           // create a kernel

  // Specific program variables
  size_t n_images = 2500;
  size_t n_devices = 2;
    

  // 1. Scan the available platforms:
  err = clGetPlatformIDs (num_platforms_ids, platforms_ids, &n_platforms);
  cl_error(err, "Error: Failed to Scan for Platforms IDs");
  if(standard_print)
    printf("Number of available platforms: %d\n\n", n_platforms);

  for (int i = 0; i < n_platforms; i++ ){
    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_NAME, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    if(standard_print)
      printf( "\t[%d]-Platform Name: %s\n", i, str_buffer);

    // ***Task***: print on the screen the name, vendor, version, ...

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VENDOR, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    if(standard_print)
      printf( "\t[%d]-Platform Vendor: %s\n", i, str_buffer);

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VERSION, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    if(standard_print)
      printf( "\t[%d]-Platform Version: %s\n", i, str_buffer);

    
    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_PROFILE, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    if(standard_print)
      printf( "\t[%d]-Platform Profile: %s\n", i, str_buffer);

  }
  if(standard_print)
    printf("\n");
	
  // 2. Scan for devices in each platform
  for (int i = 0; i < n_platforms; i++ ){
    err = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, num_devices_ids, devices_ids[i], &(cl_n_devices[i]));
    cl_error(err, "Error: Failed to Scan for Devices IDs");
    if(standard_print)
      printf("\t[%d]-Platform. Number of available devices: %d\n", i, cl_n_devices[i]);

    for(int j = 0; j < cl_n_devices[i]; j++){
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_NAME, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      if(standard_print)
        printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_NAME: %s\n", i, j,str_buffer);

      cl_uint max_compute_units_available;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units_available), &max_compute_units_available, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device max compute units available");
      if(standard_print)
        printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", i, j, max_compute_units_available);

      // ***Task***: print on the screen the cache size, global mem size, local memsize, max work group size, profiling timer resolution and ... of each device   
      cl_ulong DEVICE_GLOBAL_MEM_CACHE_SIZE;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(DEVICE_GLOBAL_MEM_CACHE_SIZE), &DEVICE_GLOBAL_MEM_CACHE_SIZE, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device global memory cache size");
      if(standard_print)
        printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %ld\n", i, j,DEVICE_GLOBAL_MEM_CACHE_SIZE);

	    cl_ulong DEVICE_LOCAL_MEM_SIZE;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(DEVICE_LOCAL_MEM_SIZE), &DEVICE_LOCAL_MEM_SIZE, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device local mem size");
      if(standard_print)
        printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_LOCAL_MEM_SIZE: %ld\n", i, j,DEVICE_LOCAL_MEM_SIZE);
	  
	  
	    size_t DEVICE_MAX_WORK_GROUP_SIZE;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(DEVICE_MAX_WORK_GROUP_SIZE), &DEVICE_MAX_WORK_GROUP_SIZE, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device maximum work group size");
      if(standard_print)
        printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_GROUP_SIZE: %ld\n", i, j,DEVICE_MAX_WORK_GROUP_SIZE);

	    size_t DEVICE_PROFILING_TIMER_RESOLUTION;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(DEVICE_PROFILING_TIMER_RESOLUTION), &DEVICE_PROFILING_TIMER_RESOLUTION, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device profiling timer resolution");
      if(standard_print)
        printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_PROFILING_TIMER_RESOLUTION: %ld\n", i, j,DEVICE_PROFILING_TIMER_RESOLUTION);
    }
  }	
  
  // 3. Create a context, with a device
  cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_ids[0], 0}; 
  context = clCreateContext(properties, n_devices, devices_ids[0], NULL, NULL, &err);
  cl_error(err, "Failed to create a compute context\n");

  // 4. Create a command queue
  cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
  for(size_t i = 0; i < n_devices; ++i){
    command_queue[i] = clCreateCommandQueueWithProperties(context, devices_ids[0][i], proprt, &err);
    std::cout << "Creating command queue n" << i << std::endl;
    cl_error(err, "Failed to create a command queue\n");
  }

  // 2. Calculate size of the file
  FILE *fileHandler = fopen("img_hist_kernel.cl", "r"); //Open the kernel file
  fseek(fileHandler, 0, SEEK_END); // Move the file pointer to the end of the file
  size_t fileSize = ftell(fileHandler);
  rewind(fileHandler);

  // read kernel source into buffer
  char * sourceCode = (char*) malloc(fileSize + 1); // Allocate memory for the buffer to store the kernel source code
  sourceCode[fileSize] = '\0';
  fread(sourceCode, sizeof(char), fileSize, fileHandler); // Read the contents of the file into the buffer
  fclose(fileHandler);

  // create program from buffer
  program = clCreateProgramWithSource(context, 1, (const char**)&sourceCode, NULL, &err);
  cl_error(err, "Failed to create program with source\n");
  free(sourceCode);

  // Build the executable and check errors
  err = clBuildProgram(program, n_devices, *devices_ids, NULL, NULL, NULL);
  if (err != CL_SUCCESS){
    size_t len;
    char buffer[2048];

    printf("Error: Some error at building process.\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(-1);
  }

  // Create a compute kernel with the program we want to run
  kernel = clCreateKernel(program, "img_histogram", &err);
  cl_error(err, "Failed to create kernel from the program\n");


//+++++++++++++++++++++

  // 5 Set the input and output vars

  // Create and initialize input array in host memory
  CImg<unsigned char> originImg("image.jpg");


  // Set the width and the height
  int img_width = originImg.width();
  int img_height = originImg.height();

  cl_uchar3 inputImg[img_width*img_height];
  fillImg(inputImg, originImg);

  int arraySize = 255;

  // Create and initialize output array in host memory
  unsigned int* histogramRed = (unsigned int *)malloc(arraySize * sizeof(unsigned int));
  unsigned int* histogramGreen = (unsigned int *)malloc(arraySize * sizeof(unsigned int));
  unsigned int* histogramBlue = (unsigned int *)malloc(arraySize * sizeof(unsigned int));


  // 6 Create OpenCL buffer visible to the OpenCl runtime

  // -------- Memory Transfer time (data interchanged between host and device) --------

  // Create events for measuring memory transfer time
  cl_event writeEvent[n_devices][n_images], readEvent_hred[n_devices][n_images], readEvent_hblue[n_devices][n_images], readEvent_hgreen[n_devices][n_images];

  // Input and output buffers for each device
  cl_mem in_device_object[n_devices][n_images];
  cl_mem out_device_object_hRed[n_devices][n_images];
  cl_mem out_device_object_hGreen[n_devices][n_images];
  cl_mem out_device_object_hBlue[n_devices][n_images];
  for (size_t dev = 0; dev < n_devices; ++dev) {
    for (size_t i = 0; i < n_images; ++i) {
      in_device_object[dev][i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar3) * (img_width * img_height), NULL, &err);
      cl_error(err, "Failed to create memory buffer at device\n");

      out_device_object_hRed[dev][i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint)*arraySize, NULL, &err);
      cl_error(err, "Failed to create memory buffer at device\n");
      out_device_object_hGreen[dev][i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint)*arraySize, NULL, &err);
      cl_error(err, "Failed to create memory buffer at device\n");
      out_device_object_hBlue[dev][i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint)*arraySize, NULL, &err);
      cl_error(err, "Failed to create memory buffer at device\n");
      // 7 Replicate input images across devices
      // Copy inputImg to in_device_object[dev]
      err = clEnqueueWriteBuffer(command_queue[dev], in_device_object[dev][i], CL_FALSE, 0, sizeof(cl_uchar3) * (img_width * img_height), inputImg, 0, NULL, &writeEvent[dev][i]);
      cl_error(err, "Failed to enqueue a write command on device\n");
    }
  }


  // -------- Global WRITE bandwithd --------

  // -------- Kernel execution time --------
  cl_event kernel_exectime_event_device[n_devices][n_images];

  size_t global_size_device[2] = {static_cast<size_t>(img_width), static_cast<size_t>(img_height)}; // Each device does 1 full image

  // Launch Kernel for both devices
  for (size_t dev = 0; dev < n_devices; ++dev) {
    for (size_t i = 0; i < n_images; ++i) {
          // 8 Set the arguments to the kernel
          err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_device_object[dev][i]);
          cl_error(err, "Failed to set argument 0 --> Input buffer (image)\n");
          err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &out_device_object_hRed[dev][i]);
          cl_error(err, "Failed to set argument 1 --> Output buffer (image)\n");
          err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &out_device_object_hGreen[dev][i]);
          cl_error(err, "Failed to set argument 2 --> Input buffer (image)\n");
          err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &out_device_object_hBlue[dev][i]);
          cl_error(err, "Failed to set argument 3 --> Input buffer (image)\n");
          err = clSetKernelArg(kernel, 4, sizeof(img_width), &img_width);
          cl_error(err, "Failed to set argument 4 --> IMG width\n");
          err = clSetKernelArg(kernel, 5, sizeof(img_height), &img_height);
          cl_error(err, "Failed to set argument 5 --> IMG height\n");
          

          // 9 Enqueue kernel for the devices
          err = clEnqueueNDRangeKernel(command_queue[dev], kernel, 2, NULL, global_size_device, NULL /*local_size*/, 0, NULL, &kernel_exectime_event_device[dev][i]);
          cl_error(err, "Failed to launch kernel to the device\n");
      } 
  }


  // clFinish(command_queue[0]);
  // clFinish(command_queue[1]);

  // -------- Global READ bandwithd --------

  // 10 Read data from device memory back to host memory
  for (size_t dev = 0; dev < n_devices; ++dev) {
    for (size_t i = 0; i < n_images; ++i) {
      err = clEnqueueReadBuffer(command_queue[dev], out_device_object_hRed[dev][i], CL_FALSE, 0, sizeof(cl_uint)*arraySize, histogramRed, 0, NULL, &readEvent_hred[dev][i]);
      cl_error(err, "Failed to enqueue a read command\n");
      err = clEnqueueReadBuffer(command_queue[dev], out_device_object_hBlue[dev][i], CL_FALSE, 0, sizeof(cl_uint)*arraySize, histogramBlue, 0, NULL, &readEvent_hblue[dev][i]);
      cl_error(err, "Failed to enqueue a read command\n");
      err = clEnqueueReadBuffer(command_queue[dev], out_device_object_hGreen[dev][i], CL_FALSE, 0, sizeof(cl_uint)*arraySize, histogramGreen, 0, NULL, &readEvent_hgreen[dev][i]);
      cl_error(err, "Failed to enqueue a read command\n");
    }
  }


  // Wait for the commands to finish --> bandwidth
  // clFinish(command_queue[0]);
  // clFinish(command_queue[1]);

  // 11 Write code to check correctness of execution
 if(standard_print){
    originImg.display("My first CImg code");

    printf("Plotting Histogram Red\n");
    plotHistogram(histogramRed, arraySize);

    printf("Plotting Histogram Green\n");
    plotHistogram(histogramGreen, arraySize);

    printf("Plotting Histogram Blue\n");
    plotHistogram(histogramBlue, arraySize);

    writeHistogramValuesToFile(histogramRed, histogramGreen, histogramBlue, arraySize, "histogram_values.txt");
  }

  // 12 Release all the OpenCL memory objects allocated along the program
  for(size_t i = 0; i < n_images; ++i){
    clReleaseMemObject(in_device_object[0][i]);
    clReleaseMemObject(out_device_object_hRed[0][i]);
    clReleaseMemObject(out_device_object_hGreen[0][i]);
    clReleaseMemObject(out_device_object_hBlue[0][i]);
    clReleaseMemObject(in_device_object[1][i]);
    clReleaseMemObject(out_device_object_hRed[1][i]);
    clReleaseMemObject(out_device_object_hGreen[1][i]);
    clReleaseMemObject(out_device_object_hBlue[1][i]);
  }
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(command_queue[0]);
  clReleaseCommandQueue(command_queue[1]);
  clReleaseContext(context);

  free(histogramRed);
  free(histogramBlue);
  free(histogramGreen);

// **************** Measurement calculations ****************

  // +++++ Total execution time in seconds +++++

  clock_t exec_time = clock() - global_start_time;
  if(standard_print)
    std::cout << "Total execution time = " << ((float)exec_time)/CLOCKS_PER_SEC  << " seconds" << std::endl;


  // +++++ Kernel Execution Time +++++

  cl_ulong kernel_time_start[n_devices];
  cl_ulong kernel_time_end[n_devices];
  double kernel_exec_time_acc_ns[2] = {0,0};
  double kernel_exec_time_avg_ns[n_devices];



  for(size_t i = 0; i < n_images; ++i){
    for (size_t dev = 0; dev < n_devices; ++dev){
      // Get starting and ending time of the event
      clGetEventProfilingInfo(kernel_exectime_event_device[dev][i], CL_PROFILING_COMMAND_START, sizeof(kernel_time_start), &kernel_time_start[dev], NULL);
      clGetEventProfilingInfo(kernel_exectime_event_device[dev][i], CL_PROFILING_COMMAND_END, sizeof(kernel_time_end), &kernel_time_end[dev], NULL);
      
      //std::cout << "Kernel starting time = " << kernel_time_start[dev] << std::endl;
      //std::cout << "Kernel ending time = " << kernel_time_end[dev] << std::endl;

      kernel_exec_time_acc_ns[dev] += kernel_time_end[dev]-kernel_time_start[dev];   // Get the execution time of the kernel in nanoseconds
    }
  }

  //printf("Kernel Execution time: %0.3f milliseconds \n",kernel_exec_time_ns / 1000000.0);
  for(size_t dev = 0; dev < n_devices; ++dev){
    kernel_exec_time_avg_ns[dev] = kernel_exec_time_acc_ns[dev]/n_images;
    if(standard_print){
      printf("\nThe device %d has an accumulated Kernel Execution time of %0.10f seconds \n", dev, kernel_exec_time_acc_ns[dev] / 1.0e+9);  // Print the execution time in seconds
      printf("The device %d has an average Kernel Execution time of %0.10f seconds \n", dev, kernel_exec_time_avg_ns[dev] / 1.0e+9);  // Print the execution time in seconds
    }
  }
  // +++++ Bandwidth --> HOST TO DEVICE +++++

  double writeTime[2] = {0,0};
  double readTime_hred[2] = {0,0};
  double readTime_hgreen[2] = {0,0};
  double readTime_hblue[2] = {0,0};
  cl_ulong writeStart, writeEnd, readStart_hred, readEnd_hred, readStart_hgreen, readEnd_hgreen, readStart_hblue, readEnd_hblue;
  for(size_t i = 0; i < n_images; ++i){
    for(size_t dev = 0; dev < n_devices; ++dev){
      // Calculate the time taken for write and read operations
      clGetEventProfilingInfo(writeEvent[dev][i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &writeStart, NULL);
      clGetEventProfilingInfo(writeEvent[dev][i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &writeEnd, NULL);
      clGetEventProfilingInfo(readEvent_hred[dev][i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &readStart_hred, NULL);
      clGetEventProfilingInfo(readEvent_hred[dev][i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &readEnd_hred, NULL);
      clGetEventProfilingInfo(readEvent_hgreen[dev][i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &readStart_hgreen, NULL);
      clGetEventProfilingInfo(readEvent_hgreen[dev][i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &readEnd_hgreen, NULL);
      clGetEventProfilingInfo(readEvent_hblue[dev][i], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &readStart_hblue, NULL);
      clGetEventProfilingInfo(readEvent_hblue[dev][i], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &readEnd_hblue, NULL);

      writeTime[dev] += (writeEnd - writeStart) * 1.0e-9;
      readTime_hred[dev] += (readEnd_hred - readStart_hred) * 1.0e-9;
      readTime_hgreen[dev] += (readEnd_hgreen - readStart_hgreen) * 1.0e-9;
      readTime_hblue[dev] += (readEnd_hblue - readStart_hblue) * 1.0e-9;
    }
  }

  // Calculate bandwidth
  size_t dataSizeRead = sizeof(unsigned int) * arraySize * 3 * n_images; // *3 because we have 3 arrays
  size_t dataSizeWrite = sizeof(cl_uchar3) * (img_width*img_height) * n_images;

  double writeBandwidth[n_devices], readBandwidth[n_devices];
  for(size_t dev = 0; dev < n_devices; ++dev){
    writeBandwidth[dev] = dataSizeWrite / writeTime[dev]; // in bytes per second
    readBandwidth[dev] = dataSizeRead / (readTime_hred[dev] + readTime_hgreen[dev] + readTime_hblue[dev]);

    // We print the bandwidths
    if(standard_print){
      printf("\nGeneral Write Bandwidth (Host to device %d): %.2f MB/s\n", dev, writeBandwidth[dev] / (1024 * 1024));
      printf("General Read Bandwidth (Host to device %d): %.2f MB/s\n", dev, readBandwidth[dev] / (1024 * 1024));
    }

  }

  // +++++ Bandwidth --> DEVICE TO LOCAL MEMORY +++++

  // Calculate bandwidth
  size_t dataSize_kernel = (sizeof(cl_uchar3)*img_width*img_height + 3*arraySize*sizeof(uint)) * n_images; // Adjust data size based on specific kernel data requirements
  double kernelBandwidth[n_devices];
  
  for(size_t dev = 0; dev < n_devices; ++dev){
    kernelBandwidth[dev] = dataSize_kernel / kernel_exec_time_acc_ns[dev]; // in bytes per nanosecond
    

    // Print or use the bandwidth value as needed
    if(standard_print)
      printf("\nKernel Bandwidth (Device %d access to local memory): %.10f MB/ns\n", dev, kernelBandwidth[dev] / (1024 * 1024));
  }


  // +++++ Work unbalance +++++
  // Calculate the workload unbalance ratio
  double unbalance_ratio = 0;
  if(kernel_exec_time_acc_ns[0] < kernel_exec_time_acc_ns[1])
    unbalance_ratio = (kernel_exec_time_acc_ns[1]/ 1.0e+9)/((kernel_exec_time_acc_ns[1]+kernel_exec_time_acc_ns[0])/ 1.0e+9);
  else
    unbalance_ratio = (kernel_exec_time_acc_ns[0]/ 1.0e+9)/((kernel_exec_time_acc_ns[1]+kernel_exec_time_acc_ns[0])/ 1.0e+9);
  
  std::cout << std::endl << "Imbalance ratio = " << kernel_exec_time_avg_ns[0]/kernel_exec_time_avg_ns[1] << std::endl;

  // Print or use the imbalance ratio as needed
  if(standard_print)
    printf("Workload unbalance: %.2f\n", unbalance_ratio);

  // **************** Measurement prints for further analyzing ****************
  // The output will be reduced to the following prints:
  // total execution time, kernel execution time, Host to device (Write) bandwidth, Host to device (Read) Bandwidth, Kernel bandwidth
  // if(!standard_print)
  //   printf("%.10f, %.10f, %.10f, %.10f, %.10f, %.10f\n", 
  //         ((float)exec_time)/CLOCKS_PER_SEC, 
  //         (kernel_exec_time_ns[0] / 1.0e+9), 
  //         (kernel_exec_time_ns[1] / 1.0e+9),
  //         writeBandwidth / (1024 * 1024), 
  //         readBandwidth / (1024 * 1024), 
  //         kernelBandwidth / (1024 * 1024));
  
  return 0;
}
