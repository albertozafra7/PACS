echo "Compiling"
echo "g++ Flip_Img_env.cpp -o "flip" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Flip_Img_env.cpp -o flip -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo

echo "g++ Rot_Img_env.cpp -o "rot" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Rot_Img_env.cpp -o rot -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo

echo "g++ Flip_Img_env_1device.cpp -o "flip1" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Flip_Img_env_1device.cpp -o flip1 -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo