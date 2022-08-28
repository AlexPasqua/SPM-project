CXX=g++-10
CXXFLAGS=`pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -fopenmp -std=c++17 -O3
CPPFLAGS=-I include/

OBJ=obj/
BIN=bin/
PAR_SRC=src/parallel/
SEQ_SRC=src/sequential/

$(BIN)main_ff.out: $(OBJ)main_ff.o $(OBJ)sequential_funcs.o
	$(CXX) $(OBJ)main_ff.o $(OBJ)sequential_funcs.o $(CXXFLAGS) $(CPPFLAGS)  -o $(BIN)main_ff.out

$(BIN)main_threads.out: $(OBJ)main_threads.o $(OBJ)parallel_funcs.o $(OBJ)sequential_funcs.o
	$(CXX) $(OBJ)main_threads.o $(OBJ)parallel_funcs.o $(OBJ)sequential_funcs.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)main_threads.out

$(BIN)main_sequential.out: $(OBJ)main_sequential.o $(OBJ)sequential_funcs.o
	$(CXX) $(OBJ)main_sequential.o $(OBJ)sequential_funcs.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)main_sequential.out

$(BIN)seq_funcs_perf_eval.out: $(OBJ)sequential_funcs.o $(OBJ)seq_funcs_perf_eval.o
	$(CXX) $(OBJ)sequential_funcs.o $(OBJ)seq_funcs_perf_eval.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)seq_funcs_perf_eval.out

all: $(BIN)main_sequential.out $(BIN)seq_funcs_perf_eval.out $(BIN)main_threads.out $(BIN)main_ff.out

$(OBJ)main_ff.o: $(PAR_SRC)main_ff.cpp
	$(CXX) -c $(PAR_SRC)main_ff.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_ff.o

$(OBJ)main_threads.o: $(PAR_SRC)main_threads.cpp
	$(CXX) -c $(PAR_SRC)main_threads.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_threads.o

$(OBJ)parallel_funcs.o: $(PAR_SRC)parallel_funcs.cpp
	$(CXX) -c $(PAR_SRC)parallel_funcs.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)parallel_funcs.o

$(OBJ)main_sequential.o: $(SEQ_SRC)main_sequential.cpp
	$(CXX) -c $(SEQ_SRC)main_sequential.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_sequential.o

$(OBJ)sequential_funcs.o: $(SEQ_SRC)sequential_funcs.cpp
	$(CXX) -c $(SEQ_SRC)sequential_funcs.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)sequential_funcs.o
	
$(OBJ)seq_funcs_perf_eval.o: $(SEQ_SRC)seq_funcs_perf_eval.cpp
	$(CXX) -c $(SEQ_SRC)seq_funcs_perf_eval.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)seq_funcs_perf_eval.o

clean:
	rm $(OBJ)*.o $(BIN)*.out