CXX=g++-10
CXXFLAGS=`pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17 -O3
CPPFLAGS=-I ../include/

OBJ=../obj/
BIN=../bin/

$(BIN)main_threads.out: $(OBJ)main_threads.o $(OBJ)parallel_funcs.o $(OBJ)shared_queue.o $(OBJ)timer.o
	$(CXX) $(OBJ)main_threads.o $(OBJ)parallel_funcs.o $(OBJ)shared_queue.o $(OBJ)timer.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)main_threads.out

$(BIN)main_sequential.out: $(OBJ)main_sequential.o $(OBJ)sequential_funcs.o $(OBJ)timer.o
	$(CXX) $(OBJ)main_sequential.o $(OBJ)sequential_funcs.o $(OBJ)timer.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)main_sequential.out

$(BIN)seq_funcs_perf_eval.out: $(OBJ)timer.o $(OBJ)sequential_funcs.o $(OBJ)seq_funcs_perf_eval.o
	$(CXX) $(OBJ)timer.o $(OBJ)sequential_funcs.o $(OBJ)seq_funcs_perf_eval.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)seq_funcs_perf_eval.out

all: $(BIN)main_sequential.out $(BIN)seq_funcs_perf_eval.out $(BIN)main_threads.out

$(OBJ)main_threads.o: parallel/main_threads.cpp
	$(CXX) -c parallel/main_threads.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_threads.o

$(OBJ)parallel_funcs.o: parallel/parallel_funcs.cpp
	$(CXX) -c parallel/parallel_funcs.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)parallel_funcs.o

$(OBJ)shared_queue.o: parallel/shared_queue.cpp
	$(CXX) -c parallel/shared_queue.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)shared_queue.o

$(OBJ)main_sequential.o: sequential/main_sequential.cpp
	$(CXX) -c sequential/main_sequential.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_sequential.o

$(OBJ)timer.o: auxiliary/timer.cpp
	$(CXX) -c auxiliary/timer.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)timer.o

$(OBJ)sequential_funcs.o: sequential/sequential_funcs.cpp
	$(CXX) -c sequential/sequential_funcs.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)sequential_funcs.o
	
$(OBJ)seq_funcs_perf_eval.o: sequential/seq_funcs_perf_eval.cpp
	$(CXX) -c sequential/seq_funcs_perf_eval.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)seq_funcs_perf_eval.o

clean:
	rm $(OBJ)*.o $(BIN)*.out