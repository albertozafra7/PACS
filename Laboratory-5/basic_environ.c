////////////////////////////////////////////////////////////////////
//File: basic_environ.c
//
//Description: base file for environment exercises with openCL
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

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  int err;                            	// error code returned from api calls
  size_t t_buf = 50;			// size of str_buffer
  char str_buffer[t_buf];		// auxiliary buffer	
  size_t e_buf;				// effective size of str_buffer in use
	    
  size_t global_size;                      	// global domain size for our calculation
  size_t local_size;                       	// local domain size for our calculation

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

  // ***Task***: print on the screen the name, host_timer_resolution, vendor, version, ...

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VENDOR, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Vendor: %s\n", i, str_buffer);

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_VERSION, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Version: %s\n", i, str_buffer);

    /*err = clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_HOST_TIMER_RESOLUTION, t_buf, str_buffer, &e_buf);
    cl_error(err, "Error: Failed to get host timer resolution\n");
    printf("\t[%d]-Host Timer Resolution: %s\n", i, str_buffer);*/ //Not defined??
    
    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_PROFILE, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Profile: %s\n", i, str_buffer);

    err= clGetPlatformInfo(platforms_ids[i], CL_PLATFORM_EXTENSIONS, t_buf, str_buffer, &e_buf);
    cl_error (err, "Error: Failed to get info of the platform\n");
    printf( "\t[%d]-Platform Extensions: %s\n", i, str_buffer);
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
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_COMPUTE_UNITS: %d\n\n", i, j, max_compute_units_available);

      // ***Task***: print on the screen the cache size, global mem size, local memsize, max work group size, profiling timer resolution and ... of each device
      
      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %s\n", i, j,str_buffer);

      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_GLOBAL_MEM_CACHE_SIZE: %s\n", i, j,str_buffer);

      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_LOCAL_MEM_SIZE: %s\n", i, j,str_buffer);

      err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_MAX_WORK_GROUP_SIZE: %s\n", i, j,str_buffer);

       err = clGetDeviceInfo(devices_ids[i][j], CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(str_buffer), str_buffer, NULL);
      cl_error(err, "clGetDeviceInfo: Getting device name");
      printf("\t\t [%d]-Platform [%d]-Device CL_DEVICE_PROFILING_TIMER_RESOLUTION: %s\n", i, j,str_buffer);
    }
  }	
  
  // 3. Create a context, with a device
  //TODO double chech number of platforms, if there are more than 1, idk which one to choose
  cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platforms_ids[0], 0}; //0 if we start from the 1st platform (if it has more than one)
  context = clCreateContext(properties, n_devices[0], devices_ids[0], NULL, NULL, &err); //assuming we're taking 1st platform
  cl_error(err, "Failed to create a compute context\n");

  // 4. Create a command queue
  cl_command_queue_properties proprt[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
  command_queue = clCreateCommandQueueWithProperties(context, devices_ids[0][0], proprt, &err);//assuming we're taking 1st platform...
  cl_error(err, "Failed to create a command queue\n");

  // 5. Calculate size of the file
  FILE *fileHandler = fopen("kernel.cl", "r"); //Open the kernel file
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
  kernel = clCreateKernel(program, (const char**)&sourceCode, &err); //Is this pointer right?
  cl_error(err, "Failed to create kernel from the program\n");



  // Set the size of the arrays
  const size_t arraySize = 100;  // Which arraySize?

  // Create and initialize input array in host memory
  float *inputArray = (float *)malloc(arraySize * sizeof(float));
  initializeArray(inputArray, arraySize);

  // Create output array in host memory
  float *outputArray = (float *)malloc(arraySize * sizeof(float));
  initializeArray(outputArray, arraySize);

  // Create OpenCL buffer visible to the OpenCl runtime
  cl_mem in_device_object  = clCreateBuffer(context, CL_MEM_READ_ONLY, /***???***/, NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");
  cl_mem out_device_object = clCreateBuffer(context, CL_MEM_WRITE_ONLY, /***???***/, NULL, &err);
  cl_error(err, "Failed to create memory buffer at device\n");

  return 0;
}

