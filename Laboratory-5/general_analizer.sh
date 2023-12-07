#Formato de Impresión del resultado de los tests
OK='[102m[1m'"------> OK <------"'[0m'
NOOK='[101m[1m'"------> NO OK <------"'[0m'
Prueba="Ejecutando Prueba nº"
Inicio='[7m[1m'
Fin='[0m'
limite=6
programas=3
Nombre="Image analysis"
Prefijo="image_"
Rotation='[102m[30m'"------> Rotation <------"'[0m'
Flip='[101m[1m'"------> Flip <------"'[0m'
Histogram='[103m[30m'"------> Histogram <------"'[0m'

	echo $Inicio"Compilando todo"$Fin
	echo

	echo "g++ Rot_Img_env.cpp -o $Prefijo"rot" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
	g++ Rot_Img_env.cpp -o $Prefijo"rot" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
	echo

	echo "g++ Flip_Img_env.cpp -o $Prefijo"flip" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
	g++ Flip_Img_env.cpp -o $Prefijo"flip" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
	echo

	echo "g++ Hist_Img_env.cpp -o $Prefijo"hist" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL"
	g++ Hist_Img_env.cpp -o $Prefijo"hist" -I ./ -lm -lpthread -lX11 -ljpeg -lOpenCL
	echo

	echo $Inicio$Nombre$Fin
	
	# ------------------ Image rotation ------------------
	echo $Rotation
	
    start=`date +%s`
	# Testing image rotation
	for i in $(seq 1 $limite); do
		#echo $Prueba $i
		./$Prefijo"rot" "false" >> img_rot_metrics.csv
	done
    end=`date +%s`

	rotation_time = $end - $start
	throughput_rot = $limite/$rotation_time

	echo
	echo "The rotation test has finished"
	echo "Analized: $limite images"
    echo "Execution time was $rotation_time seconds."
	echo "The throughput of the image rotation is $throughput_rot images per second."
	echo
	echo



    # ------------------ Image flip ------------------
	echo $Flip
	
	start=`date +%s`
	# Testing image rotation
	for i in $(seq 1 $limite); do
		#echo $Prueba $i
		./$Prefijo"flip" "false" >> img_flip_metrics.csv
	done
    end=`date +%s`

	flip_time = $end - $start
	throughput_flip = $limite/$flip_time

	echo
	echo "The flip test has finished"
	echo "Analized: $limite images"
    echo "Execution time was $flip_time seconds."
	echo "The throughput of the image flip is $throughput_flip images per second."
	echo
	echo


    # ------------------ Image Histogram ------------------
	echo $Histogram
	
	start=`date +%s`
	# Testing image rotation
	for i in $(seq 1 $limite); do
		#echo $Prueba $i
		./$Prefijo"hist" "false" >> img_hist_metrics.csv
	done
	end=`date +%s`

	hist_time = $end - $start
	throughput_hist = $limite/$hist_time

	echo
	echo "The histogram test has finished"
	echo "Analized: $limite images"
    echo "Execution time was $hist_time seconds."
	echo "The throughput of the image histogram is $throughput_hist images per second."
	echo
	echo
	

	echo
	echo