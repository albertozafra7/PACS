#Formato de Impresión del resultado de los tests
OK='[102m[1m'"------> OK <------"'[0m'
NOOK='[101m[1m'"------> NO OK <------"'[0m'
Prueba="Ejecutando Prueba nº"
Inicio='[7m[1m'
Fin='[0m'
inferior_limite=10
limite=100
programas=3
Nombre="Multiplicacion de matrices"
Prefijo="matrices_"
Custom='[102m[30m'"------> Custom <------"'[0m'
Novector='[101m[1m'"------> NO VECTOR <------"'[0m'
Library='[103m[30m'"------> Library <------"'[0m'

	echo $Inicio$Nombre$Fin
	
	# Custom matrix multiplication
	echo $Custom
	# testing matrices of dimension 5
	echo $Prueba "1"
	echo >> times_lv_getime.txt
	echo $Prueba "1" >> times_lv_getime.txt
	echo >> times_lv_getime.txt
	sudo \perf stat -a --output times_lv_getime_perf.txt --append -r 5 ./$Prefijo"lv_getime" "5" "false" "false" >> matrices_lv_getime_perf.csv
	# Testing matrices of dimension 10,20,30,..., 100
	for i in $(seq 1 $inferior_limite); do
		let j=($i + 1)
		let k=($i * 10)
		echo $Prueba $j
		echo >> times_lv_getime.txt
		echo $Prueba $j >> times_lv_getime.txt
		echo >> times_lv_getime.txt
		sudo \perf stat -a --output times_lv_getime_perf.txt --append -r 5 ./$Prefijo"lv_getime" $k "false" "false" >> matrices_lv_getime_perf.csv
	
	done
	
	# Testing matrices of dimension 100,200,300,..., 10 000
	for i in $(seq 1 $limite); do
		let j=($i + 11)
		let k=($i * 100)
		echo $Prueba $j
		echo >> times_lv_getime.txt
		echo $Prueba $j >> times_lv_getime.txt
		echo >> times_lv_getime.txt
		sudo \perf stat -a --output times_lv_getime_perf.txt --append -r 5 ./$Prefijo"lv_getime" $k "false" "false" >> matrices_lv_getime_perf.csv
	
	done
	
	

	echo
	echo
