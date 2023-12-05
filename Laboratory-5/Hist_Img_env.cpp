////////////////////////////////////////////////////////////////////
//File: Hist_Img_env.cpp
//
//Description: file for the image histogram with openCL
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
  
// check error, in such a case, it exits

#include <vector>
#include <algorithm>

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

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  int err;                            	// error code returned from api calls
  size_t t_buf = 50;			// size of str_buffer
  char str_buffer[t_buf];		// auxiliary buffer	
  size_t e_buf;				// effective size of str_buffer in use
	    
  size_t global_size[2];                      	// global domain size for our calculation
  size_t local_size[2];                       	// local domain size for our calculation

  const cl_uint num_platforms_ids = 10;				// max of allocatable platforms
  cl_platform_id platforms_ids[num_platforms_ids];		// array of platforms
  cl_uint n_platforms;						// effective number of platforms in use
  const cl_uint num_devices_ids = 10;				// max of allocatable devices
  cl_device_id devices_ids[num_platforms_ids][num_devices_ids];	// array of devices
  cl_uint n_devices[num_platforms_ids];				// effective number of devices in use for each platform
	
  cl_device_id device_id;             				// compute device id 
  cl_context context;                 				// compute context
  cl_command_queue command_queue;     				// compute command queue
  cl_program program;                         // define a program
  cl_kernel kernel;                           // create a kernel
    

  // 1. Scan the available platforms:
  err = clGetPlatformIDs (num_platforms_ids, platforms_ids, &n_platforms);
  cl_error(err, "Error: Failed to Scan for Platforms IDs");
  printf("Number of available platforms: %d\n\n", n_platforms);

  for (int i = 0; i < n_platforms; i++ ){
    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_NAME, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Name: %s\n", i, str_buffer);

  // ***Task***: print on the screen the name, vendor, version, ...

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VENDOR, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Vendor: %s\n", i, str_buffer);

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VERSION, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Version: %s\n", i, str_buffer);

    
    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_PROFILE, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Profile: %s\n", i, str_buffer);

  }
  printf("\n");
	
  // 2. Scan for devices in each platform
  for (int i = 0; i < n_platforms; i++ ){
    err = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, num_devices_ids, devices_ids[i], &(n_devices[i]));
    cl_error(err, "Error: Failed to Scan for Devices IDs");
    printf("\t[%d]-Platform. Number of available devices: %d\n", i, n_devices[i]);

    for(int j = 0; j < n_devices[i]; j++){
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_NAME, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_NAME: %s\n", i, j,str_buffer);

      cl_uint max_compute_units_available;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units_available), &max_compute_units_available, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device max compute units available");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", i, j, max_compute_units_available);

      // ***Task***: print on the screen the cache size, global mem size, local memsize, max work group size, profiling timer resolution and ... of each device   
      cl_ulong DEVICE_GLOBAL_MEM_CACHE_SIZE;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(DEVICE_GLOBAL_MEM_CACHE_SIZE), &DEVICE_GLOBAL_MEM_CACHE_SIZE, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device global memory cache size");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %ld\n", i, j,DEVICE_GLOBAL_MEM_CACHE_SIZE);

	  cl_ulong DEVICE_LOCAL_MEM_SIZE;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(DEVICE_LOCAL_MEM_SIZE), &DEVICE_LOCAL_MEM_SIZE, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device local mem size");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_LOCAL_MEM_SIZE: %ld\n", i, j,DEVICE_LOCAL_MEM_SIZE);
	  
	  
	  size_t DEVICE_MAX_WORK_GROUP_SIZE;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(DEVICE_MAX_WORK_GROUP_SIZE), &DEVICE_MAX_WORK_GROUP_SIZE, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device maximum work group size");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_GROUP_SIZE: %ld\n", i, j,DEVICE_MAX_WORK_GROUP_SIZE);

	  size_t DEVICE_PROFILING_TIMER_RESOLUTION;
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(DEVICE_PROFILING_TIMER_RESOLUTION), &DEVICE_PROFILING_TIMER_RESOLUTION, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device profiling timer resolution");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_PROFILING_TIMER_RESOLUTION: %ld\n", i, j,DEVICE_PROFILING_TIMER_RESOLUTION);
    }
  }	
  
  // 3. Create a context, with a device
  cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_ids[0], 0}; 
  context = clCreateContext(properties, n_devices[0], devices_ids[0], NULL, NULL, &err);
  cl_error(err, "Failed to create a compute context\n");

  // 4. Create a command queue
  cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
  command_queue = clCreateCommandQueueWithProperties(context, devices_ids[0][0], proprt, &err);//assuming we're taking 1st platform...
  cl_error(err, "Failed to create a command queue\n");

  // 2. Calculate size of the file
  FILE *fileHandler = fopen("histogram_kernel.cl", "r"); //Open the kernel file
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
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS){
    size_t len;
    char buffer[2048];

    printf("Error: Some error at building process.\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(-1);
  }

  // Create a compute kernel with the program we want to run
  kernel = clCreateKernel(program, "img_histogram", &err); //Is this pointer right?
  cl_error(err, "Failed to create kernel from the program\n");


  // 5 Set the input and output vars

  // Create and initialize input array in host memory
  CImg<unsigned char> inputImg("image.jpg");
  

  // Set the width and the height
  int img_width = inputImg.width();
  int img_height = inputImg.height();

  int arraySize = 255;

   // Create and initialize output array in host memory
  unsigned int* histogramRed = (unsigned int *)malloc(arraySize * sizeof(unsigned int));
  unsigned int* histogramGreen = (unsigned int *)malloc(arraySize * sizeof(unsigned int));
  unsigned int* histogramBlue = (unsigned int *)malloc(arraySize * sizeof(unsigned int));

  // 6 Create OpenCL buffer visible to the OpenCl runtime
  cl_mem in_device_object  = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar)*inputImg.size(), NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");
  cl_mem out_device_object_hRed = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint)*arraySize, NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");
  cl_mem out_device_object_hGreen = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint)*arraySize, NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");
  cl_mem out_device_object_hBlue = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uint)*arraySize, NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");
  
  // 7 Write date into the memory object 
  err = clEnqueueWriteBuffer(command_queue, in_device_object, CL_TRUE, 0, sizeof(cl_uchar) * inputImg.size(), inputImg.data(), 0, NULL, NULL);
  cl_error(err, "Failed to enqueue a write command\n");
  
  // 8 Set the arguments to the kernel
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_device_object);
  cl_error(err, "Failed to set argument 0 --> Input buffer (image)\n");
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &out_device_object_hRed);
  cl_error(err, "Failed to set argument 1 --> Output buffer (image)\n");
  err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &out_device_object_hGreen);
  cl_error(err, "Failed to set argument 2 --> Input buffer (image)\n");
  err = clSetKernelArg(kernel, 3, sizeof(cl_mem), &out_device_object_hBlue);
  cl_error(err, "Failed to set argument 3 --> Input buffer (image)\n");
  err = clSetKernelArg(kernel, 4, sizeof(img_width), &img_width);
  cl_error(err, "Failed to set argument 4 --> IMG width\n");
  err = clSetKernelArg(kernel, 5, sizeof(img_height), &img_height);
  cl_error(err, "Failed to set argument 5 --> IMG height\n");



  // 9 Launch Kernel
  local_size[0] = 128;
  local_size[1] = 128;

  global_size[0] = img_width; // N of work items que quieres
  global_size[1] = img_height; // N of work items que quieres

  err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_size, NULL/*local_size*/, 0, NULL, NULL);
  cl_error(err, "Failed to launch kernel to the device\n");

  // 10 Read data form device memory back to host memory
  err = clEnqueueReadBuffer(command_queue, out_device_object_hRed, CL_TRUE, 0, sizeof(cl_uint)*arraySize, histogramRed, 0, NULL, NULL);
  cl_error(err, "Failed to enqueue a read command\n");
   err = clEnqueueReadBuffer(command_queue, out_device_object_hBlue, CL_TRUE, 0, sizeof(cl_uint)*arraySize, histogramBlue, 0, NULL, NULL);
  cl_error(err, "Failed to enqueue a read command\n");
   err = clEnqueueReadBuffer(command_queue, out_device_object_hGreen, CL_TRUE, 0, sizeof(cl_uint)*arraySize, histogramGreen, 0, NULL, NULL);
  cl_error(err, "Failed to enqueue a read command\n");

  // 11 Write code to check correctness of execution
  inputImg.display("My first CImg code");


  printf("Plotting Histogram Red\n");
  plotHistogram(histogramRed, arraySize);

  printf("Plotting Histogram Green\n");
  plotHistogram(histogramGreen, arraySize);

  printf("Plotting Histogram Blue\n");
  plotHistogram(histogramBlue, arraySize);


  

  // 12 Release all the OpenCL memory objects allocated along the program
  clReleaseMemObject(in_device_object);
  clReleaseMemObject(out_device_object_hRed);
  clReleaseMemObject(out_device_object_hGreen);
  clReleaseMemObject(out_device_object_hBlue);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(command_queue);
  clReleaseContext(context);

  free(histogramRed);
  free(histogramBlue);
  free(histogramGreen);

  return 0;
}

