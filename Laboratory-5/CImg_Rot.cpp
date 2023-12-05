#define cimg_use_jpeg
#include <iostream>
#include <vector>
#include "CImg/CImg.h"
using namespace cimg_library;

std::vector<double> computeRot(const std::vector<int> origin, const std::vector<int> pivot, const float angle){
  double x, y;
  x = cos(angle) * (origin[0] - pivot[0]) - sin(angle) * (origin[1] - pivot[1]) + pivot[0];
  y = sin(angle) * (origin[0] - pivot[0]) + cos(angle) * (origin[1] - pivot[1]) + pivot[1];
  return {x,y};
}

CImg<unsigned char> RotateImg(const std::vector<int> pivot, const float angle, CImg<unsigned char> img){
  CImg<unsigned char> dest_img(img);

  cimg_forXY(dest_img, i, j){
      // std::cout << "i = " << i << " j = " << j << std::endl;
      //std::vector<int> color = {img(i,j,0), img(i,j,1), img(i,j,2)};
      // std::cout << "Color obtained" << std::endl;
      // std::cout << "cos = " << cos(angle) << std::endl;
      /*std::vector<double> dest_pix = computeRot({i,j}, pivot, angle);

      for(int k = 0; k < 3; k++){
        const unsigned char interpolated_value = img.linear_atXYZ(dest_pix[0], dest_pix[1], 0, k);
        dest_img(i,j,k) = interpolated_value;
      }*/

          // Calculate the rotated coordinates
    const int newX = (i - pivot[0]) * cos(angle) - (j - pivot[1]) * sin(angle) + pivot[0];
    const int newY = (i - pivot[0]) * sin(angle) + (j - pivot[1]) * cos(angle) + pivot[1];

    if (newX >= 0 && newX < dest_img.width() && newY >= 0 && newY < dest_img.height()) {
        // Map the 2D coordinates to the 1D array index
        const int src_index = (newY * dest_img.width() + newX);
        const int dest_index = (j * dest_img.width() + i);

        // Copy pixel from input to output
        for(int channel = 0; channel < 3; ++channel)
          dest_img[src_index+channel*dest_img.height()] = img[dest_index+channel*dest_img.height()];
    }
  }

  return dest_img;
}

CImg<unsigned char> CenterImgRotation(const float angle, CImg<unsigned char> img){
   return RotateImg({img.width()/2,img.height()/2},angle,img);
}


int main(){
  CImg<unsigned char> img("image.jpg");  // Load image file "image.jpg" at object img

  std::cout << "Image width: " << img.width() << "Image height: " << img.height() << "Number of slices: " << img.depth() << "Number of channels: " << img.spectrum() << std::endl;  //dump some characteristics of the loaded image

  int i = 500;
  int j = 500;
  std::cout << std::hex << (int) img(i, j, 0, 0) << std::endl;  //print pixel value for channel 0 (red) 
  std::cout << std::hex << (int) img(i, j, 0, 1) << std::endl;  //print pixel value for channel 1 (green) 
  std::cout << std::hex << (int) img(i, j, 0, 2) << std::endl;  //print pixel value for channel 2 (blue) 
  
  for(int x = 0; x < img.height(); x++){
    img(img.width()/2,x,0) = 255;
    img(img.width()/2,x,1) = 0;
    img(img.width()/2,x,2) = 0;
  }
  
  img.display("My first CImg code");             // Display the image in a display window

  CImg<unsigned char> rotatedImg = CenterImgRotation(3.14, img);
  
  rotatedImg.display("Rotated IMG");

  return 0;

}
