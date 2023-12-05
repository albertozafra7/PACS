# Copyright (C) Programacion-II, DLSI, UA 2019

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


# For using this makefile the abreviations for the different implementations are the following:
# c: for customized matrix multiplication
# l: for matrix multiplication without vectorization
# lv: for matrix multiplication with vectorization

#############
# VARIABLES #                     PUEDES CAMBIARLAS
#############--------------------------------------------------------------

# COMPILE OPTIONS
CXXFLAGS= -Wall -O3 -g --std=c++11
# LINK OPTIONS
LDFLAGS=



######################
# COMPUTED VARIABLES #            NO DEBES MODIFICARLAS
######################-----------------------------------------------------
# SOURCES
SRCS=$(wildcard *.cpp) # $(wildcard .cpp /xxx/xxx/.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
# LIBRARY
LIB= -I ./eigen-3.4.0/
# OBJETCS
#OBJS=$(patsubst %.cpp, %.o, $(SRCS)) # patsubst(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
# COMPILATION
#COMP= g++ $(CXXFLAGS) $(LIB) @.cpp -o $@
# NAMES OF THE PROGRAMS
MAT_C= Matrix_Multiplication_Custom
#MAT_L= Matrix_Multiplication_Library
MAT_LV= Matrix_Multiplication_Vectorization
# PROGRAMS TO COMPILE
PROG= $(MAT_C) $(MAT_L) $(MAT_LV)
# PROGRAM NAMES
PR_MAT_C= matrices_c
#PR_MAT_L= matrices_l
PR_MAT_LV= matrices_lv
GETIME_SURNAME= _getime
CLOCK_SURNAME= _clock
PR_Names= $(PR_MAT_C) $(PR_MAT_L) $(PR_MAT_LV)
# TAR FILE
TARF=$(shell basename "$$(pwd)")
#########------------------------------------------------------------------


#########
# RULES #                         NO DEBES MODIFICARLAS
#########------------------------------------------------------------------
all : $(PROG)

########
# CODE #
########

$(MAT_C):
	g++ $(CXXFLAGS) $(MAT_C)$(CLOCK_SURNAME).cpp -o $(PR_MAT_C)$(CLOCK_SURNAME)
	g++ $(CXXFLAGS) $(MAT_C)$(GETIME_SURNAME).cpp -o $(PR_MAT_C)$(GETIME_SURNAME)
	

#$(MAT_L):
#	g++ $(CXXFLAGS) $(LIB) $@.cpp -o $(PR_MAT_L)

$(MAT_LV):
	g++ $(CXXFLAGS) $(LIB) $(MAT_LV)$(CLOCK_SURNAME).cpp -o $(PR_MAT_LV)$(CLOCK_SURNAME)
	g++ $(CXXFLAGS) $(LIB) $(MAT_LV)$(GETIME_SURNAME).cpp -o $(PR_MAT_LV)$(GETIME_SURNAME)

P : 
	@echo '[7m[1m'Linking the lab practice...'[0m'
	$(CXX) $(LDFLAGS) $(OBJS) -o 

#################
# PHONY TARGETS #
#################
run: $(PROG)
	@echo '[7m[1m'Executing the lab practice...'[0m'
	@echo -e '\n'
	@echo '[7m[1m'Executing the custom matrix multiplication program...'[0m'
	./$(PR_MAT_C)$(CLOCK_SURNAME)
	@echo -e '\n'
#	@echo '[7m[1m'Executing the matrix multiplication program with the eigen library and without vectorization...'[0m'
#	./$(PR_MAT_L)
#	@echo -e '\n'
	@echo '[7m[1m'Executing the matrixultiplication program with the eigen library with vectorization...'[0m'
	./$(PR_MAT_LV)$(CLOCK_SURNAME)
	

# Runing custom
$(PR_MAT_C): $(MAT_C)
	@echo '[7m[1m'Executing the custom matrix multiplication program...'[0m'
	./$(PR_MAT_C)$(CLOCK_SURNAME)
	
run_mat_c: $(MAT_C)
	@echo '[7m[1m'Executing the custom matrix multiplication program...'[0m'
	./$(PR_MAT_C)$(CLOCK_SURNAME)
	

	
# Runing with library without vectorization
#$(PR_MAT_L): $(MAT_L)
#	@echo '[7m[1m'Executing the matrix multiplication program with the eigen library and without vectorization...'[0m'
#	./$(PR_MAT_L)

#run_mat_l: $(MAT_L)
#	@echo '[7m[1m'Executing the matrix multiplication program with the eigen library and without vectorization...'[0m'
#	./$(PR_MAT_L)



# Runing with library with vectorization
$(PR_MAT_LV): $(MAT_LV)
	@echo '[7m[1m'Executing the matrixultiplication program with the eigen library with vectorization...'[0m'
	./$(PR_MAT_LV)

run_mat_lv: $(MAT_LV)
	@echo '[7m[1m'Executing the matrixultiplication program with the eigen library with vectorization...'[0m'
	./$(PR_MAT_LV)
	
# Running all tests for getting the data aquisition into the .csv
run_tests: $(PROG)
	./data_acquisition.sh

# If you want to run the lab practice
runv: p$(PR)
	@echo '[7m[1m'Executing the lab practice from valgrind...'[0m'
	@valgrind --leak-check=full ./p$(PR)

vstats: p$(PR)
	@echo '[7m[1m'Estadisticas de valgrind...'[0m'
	@valgrind --leak-check=full ./p$(PR) 2>&1 | grep "lost\|reachable" ; true

debug: p$(PR)
	@echo '[7m[1m'Depurando practica...'[0m'
	@gdb --tui p$(PR)

tgz: clean
	@echo '[7m[1mCreando archivo: $(TARF).tgz ...[0m'
	@cd ..;tar cfz $(TARF).tgz $(TARF); \
	 mv $(TARF).tgz $(TARF);cd $(TARF)

clean:
	@echo '[7m[1m'Limpiando...'[0m'
	@rm -r -f *~ *.o *.tgz

clean_data:
	@echo '[7m[1m'Eliminando archivos .csv ...'[0m'
	@rm -r -f *.csv

.PHONY: clean tgz run debug all
#########------------------------------------------------------------------
