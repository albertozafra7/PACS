echo '[102m[1m'"------> Compiling multiple device environments <------"'[0m'

echo '[101m[1m'"------> Flip <------"'[0m'
echo "g++ Flip_Img_env.cpp -o "flip" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Flip_Img_env.cpp -o flip -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo

echo '[102m[30m'"------> Rotation <------"'[0m'
echo "g++ Rot_Img_env.cpp -o "rot" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Rot_Img_env.cpp -o rot -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo

echo '[103m[30m'"------> Histogram <------"'[0m'
echo "g++ Hist_Img_env.cpp -o "hist" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Hist_Img_env.cpp -o hist -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo


echo '[101m[1m'"------> Compiling unique device environment <------"'[0m'

echo '[101m[1m'"------> Flip <------"'[0m'
echo "g++ Flip_Img_env_1device.cpp -o "flip1" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Flip_Img_env_1device.cpp -o flip1 -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo

echo '[102m[30m'"------> Rotation <------"'[0m'
echo "g++ Rot_Img_env_1device.cpp -o "rot1" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Rot_Img_env_1device.cpp -o rot1 -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo

echo '[103m[30m'"------> Histogram <------"'[0m'
echo "g++ Hist_Img_env_1device.cpp -o "hist1" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
g++ Hist_Img_env_1device.cpp -o hist1 -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
echo