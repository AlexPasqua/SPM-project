CXX=g++-10
CXXFLAGS=`pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17 -O3
CPPFLAGS=-I ../include/

OBJ=../obj/
BIN=../bin/

$(BIN)main_sequential.out: $(OBJ)main_sequential.o $(OBJ)sequential_funcs.o $(OBJ)utimer.o
	$(CXX) $(OBJ)main_sequential.o $(OBJ)sequential_funcs.o $(OBJ)utimer.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)main_sequential.out

$(BIN)seq_funcs_perf_eval.out: $(OBJ)utimer.o $(OBJ)sequential_funcs.o $(OBJ)seq_funcs_perf_eval.o
	$(CXX) $(OBJ)utimer.o $(OBJ)sequential_funcs.o $(OBJ)seq_funcs_perf_eval.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)seq_funcs_perf_eval.out

all: $(BIN)main_sequential.out $(BIN)seq_funcs_perf_eval.out

$(OBJ)main_sequential.o: sequential/main_sequential.cpp
	$(CXX) -c sequential/main_sequential.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_sequential.o

$(OBJ)utimer.o: auxiliary/utimer.cpp
	$(CXX) -c auxiliary/utimer.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)utimer.o

$(OBJ)sequential_funcs.o: sequential/sequential_funcs.cpp
	$(CXX) -c sequential/sequential_funcs.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)sequential_funcs.o
	
$(OBJ)seq_funcs_perf_eval.o: sequential/seq_funcs_perf_eval.cpp
	$(CXX) -c sequential/seq_funcs_perf_eval.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)seq_funcs_perf_eval.o

clean:
	rm $(OBJ)*.o $(BIN)*.out