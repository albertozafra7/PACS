echo "Compiling"
echo "g++ Flip_Img_env.cpp -o "flip" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Flip_Img_env.cpp -o flip -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo