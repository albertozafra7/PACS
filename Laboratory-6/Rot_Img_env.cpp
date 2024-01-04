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
  cl_uint n_devices[num_platforms_ids];				// effective number of devices in use for each platform
	
  cl_device_id device_id;             				// compute device id 
  cl_context context;                 				// compute context
  cl_command_queue command_queue[2];     				// compute command queue
  cl_program program;                         // define a program
  cl_kernel kernel;                           // create a kernel
    

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
    err = clGetDeviceIDs(platforms_ids[i], CL_DEVICE_TYPE_ALL, num_devices_ids, devices_ids[i], &(n_devices[i]));
    cl_error(err, "Error: Failed to Scan for Devices IDs");
    if(standard_print)
      printf("\t[%d]-Platform. Number of available devices: %d\n", i, n_devices[i]);

    for(int j = 0; j < n_devices[i]; j++){
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
  context = clCreateContext(properties, 2 /*n_devices*/, devices_ids[0], NULL, NULL, &err);
  cl_error(err, "Failed to create a compute context\n");

  // 4. Create a command queue
  cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
  for(size_t i = 0; i < 2 /*n_devices*/; ++i){
    command_queue[i] = clCreateCommandQueueWithProperties(context, devices_ids[0][i], proprt, &err);
    std::cout << "Creating command queue n" << i << std::endl;
    cl_error(err, "Failed to create a command queue\n");
  }

  // 2. Calculate size of the file
  FILE *fileHandler = fopen("img_rot_kernel.cl", "r"); //Open the kernel file
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
  err = clBuildProgram(program, 2, *devices_ids, NULL, NULL, NULL);
  if (err != CL_SUCCESS){
    size_t len;
    char buffer[2048];

    printf("Error: Some error at building process.\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(-1);
  }

  // Create a compute kernel with the program we want to run
  kernel = clCreateKernel(program, "img_rot", &err);
  cl_error(err, "Failed to create kernel from the program\n");


//+++++++++++++++++++++

  // 5 Set the input and output vars

  // Create and initialize input array in host memory
  CImg<unsigned char> originImg("image.jpg");

  // Set the pivot
  int pivot_x = originImg.width()/2;
  int pivot_y = originImg.height()/2;

  // Set the angle in degrees
  float angle = 3.14;

  // Set the width and the height
  int img_width = originImg.width();
  int img_height = originImg.height();

  cl_uchar3 inputImg[img_width*img_height];
  fillImg(inputImg, originImg);

  cl_uchar3 outputImg[img_width*img_height];

  size_t n_images = 5000;

  // 6 Create OpenCL buffer visible to the OpenCl runtime

  // -------- Memory Transfer time (data interchanged between host and device) --------

  // Create events for measuring memory transfer time
  cl_event writeEvent[2], readEvent[2];

  // Input and output buffers for each device
  cl_mem in_device_object[2];
  cl_mem out_device_object[2];

  for (size_t i = 0; i < 2; ++i) {
      in_device_object[i] = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(cl_uchar3) * (img_width * img_height), NULL, &err);
      cl_error(err, "Failed to create memory buffer at device\n");

      out_device_object[i] = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_uchar3) * (img_width * img_height), NULL, &err);
      cl_error(err, "Failed to create memory buffer at device\n");
  }

  // -------- Global WRITE bandwithd --------
  cl_ulong writeStart[2];
  cl_ulong writeEnd[2];

  cl_ulong writeTime_acc[2] = {0,0};

  // 7 Replicate input images across devices
  for (size_t i = 0; i < n_images; ++i) {
      for (size_t dev = 0; dev < 2; ++dev) {
          // Copy inputImg to in_device_object[dev]
          err = clEnqueueWriteBuffer(command_queue[dev], in_device_object[dev], CL_TRUE, 0, sizeof(cl_uchar3) * (img_width * img_height), inputImg, 0, NULL, &writeEvent[dev]);
          cl_error(err, "Failed to enqueue a write command on device\n");
      }

      // Waits for the 2 events to be finished to get the kernel execution time in each device
      clWaitForEvents(2, writeEvent);

      for (size_t dev = 0; dev < 2; ++dev){
        clGetEventProfilingInfo(writeEvent[dev], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &writeStart[dev], NULL);
        clGetEventProfilingInfo(writeEvent[dev], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &writeEnd[dev], NULL);
        writeTime_acc[dev] += (writeEnd[dev] - writeStart[dev]);
      }

  }

  // -------- Kernel execution time --------
  cl_event kernel_exectime_event_device[2];

  size_t global_size_device[2] = {static_cast<size_t>(img_width), static_cast<size_t>(img_height)}; // Each device does 1 full image

  cl_ulong kernel_time_start[2];
  cl_ulong kernel_time_end[2];

  cl_ulong kernel_time_acc[2] = {0,0};
  size_t acc = 0;


  // 8 Set the arguments to the kernel
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_device_object);
  cl_error(err, "Failed to set argument 0 --> Input buffer (image)\n");
  err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &out_device_object);
  cl_error(err, "Failed to set argument 1 --> Output buffer (image)\n");
  err = clSetKernelArg(kernel, 2, sizeof(pivot_x), &pivot_x);
  cl_error(err, "Failed to set argument 2 --> Pivot in X\n");
  err = clSetKernelArg(kernel, 3, sizeof(pivot_y), &pivot_y);
  cl_error(err, "Failed to set argument 3 --> Pivot in Y\n");
  err = clSetKernelArg(kernel, 4, sizeof(angle), &angle);
  cl_error(err, "Failed to set argument 4 --> Rotation angle\n");
  err = clSetKernelArg(kernel, 5, sizeof(img_width), &img_width);
  cl_error(err, "Failed to set argument 5 --> IMG width\n");
  err = clSetKernelArg(kernel, 6, sizeof(img_height), &img_height);
  cl_error(err, "Failed to set argument 6 --> IMG height\n");
  

  // Launch Kernel for both devices
  for (size_t i = 0; i < n_images; ++i) {
      for (size_t dev = 0; dev < 2; ++dev) {

          // 9 Enqueue kernel for the devices
          err = clEnqueueNDRangeKernel(command_queue[dev], kernel, 2, NULL, global_size_device, NULL /*local_size*/, 0, NULL, &kernel_exectime_event_device[dev]);
          cl_error(err, "Failed to launch kernel to the device\n");
      } 
      // Waits for the 2 events to be finished to get the kernel execution time in each device
      clWaitForEvents(2, kernel_exectime_event_device);

      for (size_t dev = 0; dev < 2; ++dev){
      // Get starting and ending time of the event
        clGetEventProfilingInfo(kernel_exectime_event_device[dev], CL_PROFILING_COMMAND_START, sizeof(kernel_time_start), &kernel_time_start[dev], NULL);
        clGetEventProfilingInfo(kernel_exectime_event_device[dev], CL_PROFILING_COMMAND_END, sizeof(kernel_time_end), &kernel_time_end[dev], NULL);
        kernel_time_acc[dev] += kernel_time_end[dev]-kernel_time_start[dev];
        acc++;
      }
  }

  if(standard_print){
    std::cout << std::endl;
    std::cout << "Device 0 accumulated time = " << kernel_time_acc[0]/ 1.0e+9 << " s" << std::endl;
    std::cout << "Device 1 accumulated time = " << kernel_time_acc[1]/ 1.0e+9 << " s" << std::endl;
    std::cout << "Images processed = " << acc << std::endl << std::endl;
  }


  
  clFinish(command_queue[0]);
  clFinish(command_queue[1]);

  // -------- Global READ bandwithd --------

  // 10 Read data from device memory back to host memory
  for (size_t i = 0; i < n_images; ++i) {
      for (size_t dev = 0; dev < 2; ++dev) {
          err = clEnqueueReadBuffer(command_queue[dev], out_device_object[dev], CL_TRUE, 0, sizeof(cl_uchar3) * (img_width * img_height), outputImg, 0, NULL, &readEvent[dev]);
          cl_error(err, "Failed to enqueue a read command\n");
      }
  }

  // -------- Global WRITE bandwithd --------
  cl_ulong readStart[2];
  cl_ulong readEnd[2];

  cl_ulong readTime_acc[2] = {0,0};

  // 7 Replicate input images across devices
  for (size_t i = 0; i < n_images; ++i) {
      for (size_t dev = 0; dev < 2; ++dev) {
          // Copy inputImg to in_device_object[dev]
          err = clEnqueueReadBuffer(command_queue[dev], out_device_object[dev], CL_TRUE, 0, sizeof(cl_uchar3) * (img_width * img_height), outputImg, 0, NULL, &readEvent[dev]);
          cl_error(err, "Failed to enqueue a read command\n");
      }

      // Waits for the 2 events to be finished to get the kernel execution time in each device
      clWaitForEvents(2, readEvent);

      for (size_t dev = 0; dev < 2; ++dev){
        clGetEventProfilingInfo(readEvent[dev], CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &readStart[dev], NULL);
        clGetEventProfilingInfo(readEvent[dev], CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &readEnd[dev], NULL);
        readTime_acc[dev] += (readEnd[dev] - readStart[dev]);
      }

  }

  // Wait for the commands to finish --> bandwidth
  clFinish(command_queue[0]);
  clFinish(command_queue[1]);

  // 11 Write code to check correctness of execution
 
  CImg<unsigned char> finalImg(originImg);
  fillImg(finalImg, outputImg);

  // Save the image to a file (e.g., in PNG format)
  const char* filename = "output.png";
  finalImg.save(filename);

  // Display the saved image filename
  std::cout << "Image saved to: " << filename << std::endl;

  // 12 Release all the OpenCL memory objects allocated along the program
  clReleaseMemObject(in_device_object[0]);
  clReleaseMemObject(out_device_object[0]);
  clReleaseMemObject(in_device_object[1]);
  clReleaseMemObject(out_device_object[1]);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(command_queue[0]);
  clReleaseCommandQueue(command_queue[1]);
  clReleaseContext(context);

// **************** Measurement calculations ****************

  // +++++ Total execution time in seconds +++++

  clock_t exec_time = clock() - global_start_time;
  if(standard_print)
    std::cout << "Total execution time = " << ((float)exec_time)/CLOCKS_PER_SEC  << " seconds" << std::endl;


  // +++++ Kernel Execution Time +++++
  double kernel_exec_time_ns[2];

  if(standard_print)
    std::cout << std::endl;


  for (size_t dev = 0; dev < 2; ++dev){
    kernel_exec_time_ns[dev] = kernel_time_acc[dev]/n_images;
    if(standard_print)
      printf("The device %d has a Kernel Execution time of %0.10f seconds \n", dev, kernel_exec_time_ns[dev] / 1.0e+9);  // Print the execution time in seconds
  }

  // +++++ Bandwidth --> HOST TO DEVICE +++++

  // Calculate the time taken for write and read operations
  double writeBandwidth[2], readBandwidth[2];
  size_t dataSize = sizeof(cl_uchar3) * (img_width*img_height) * n_images;

  for(size_t dev = 0; dev < 2; ++dev){
    // Calculate bandwidth
    double writeTime[2], readTime[2];
    writeTime[dev] = writeTime_acc[dev] * 1.0e-9; // Convert nanoseconds to seconds
    readTime[dev] = readTime_acc[dev] * 1.0e-9;

    writeBandwidth[dev] = dataSize / writeTime[dev]; // in bytes per second
    readBandwidth[dev] = dataSize / readTime[dev];

    // Print or use the bandwidth values as needed
    if(standard_print){
      printf("General Write Bandwidth (Host to device %d): %.2f MB/s\n", dev, writeBandwidth[dev] / (1024 * 1024));
      printf("General Read Bandwidth (Host to device %d): %.2f MB/s\n", dev, readBandwidth[dev] / (1024 * 1024));
    }
  }


  // +++++ Bandwidth --> DEVICE TO LOCAL MEMORY +++++

  
  // Calculate bandwidth
  size_t dataSize_kernel = sizeof(cl_uchar3) * (img_width*img_height) * n_images; // Adjust data size based on your specific kernel data requirements
  double kernelBandwidth[2];
  for(size_t dev = 0; dev < 2; ++dev)
    kernelBandwidth[dev] = dataSize_kernel / kernel_time_acc[dev]; // in bytes per nanosecond
  

  // Print or use the bandwidth value as needed
  if(standard_print){
    printf("Kernel Bandwidth (Device %d access to local memory): %.4f MB/ns\n", 0, kernelBandwidth[0] / (1024 * 1024));
    printf("Kernel Bandwidth (Device %d access to local memory): %.4f MB/ns\n", 1, kernelBandwidth[1] / (1024 * 1024));
  }


  // +++++ Work unbalance +++++
  // Calculate the workload imbalance ratio
  double unbalance_ratio = 0;
  if(kernel_time_acc[0] < kernel_time_acc[1])
    unbalance_ratio = (kernel_time_acc[1]/ 1.0e+9)/((kernel_time_acc[1]+kernel_time_acc[0])/ 1.0e+9);
  else
    unbalance_ratio = (kernel_time_acc[0]/ 1.0e+9)/((kernel_time_acc[1]+kernel_time_acc[0])/ 1.0e+9);
  
  std::cout << std::endl << "Imbalance ratio = " << kernel_exec_time_ns[0]/kernel_exec_time_ns[1] << std::endl;

  // Print or use the imbalance ratio as needed
  if(standard_print)
    printf("Workload unbalance: %.2f\n", unbalance_ratio);

  // **************** Measurement prints for further analyzing ****************
  // The output will be reduced to the following prints:
  // total execution time, kernel execution time, Host to device (Write) bandwidth, Host to device (Read) Bandwidth, Kernel bandwidth
  if(!standard_print)
    printf("%.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %.10f, %10f\n", 
          ((float)exec_time)/CLOCKS_PER_SEC, 
          (kernel_exec_time_ns[0] / 1.0e+9), 
          (kernel_exec_time_ns[1] / 1.0e+9),
          writeBandwidth[0] / (1024 * 1024), 
          writeBandwidth[1] / (1024 * 1024), 
          readBandwidth[0] / (1024 * 1024), 
          readBandwidth[1] / (1024 * 1024), 
          kernelBandwidth[0] / (1024 * 1024),
          kernelBandwidth[1] / (1024 * 1024));
  
  return 0;
}

