CXX=g++-10
CXXFLAGS=`pkg-config --cflags opencv4` `pkg-config --libs opencv4` -pthread -std=c++17 -O3
CPPFLAGS=-I ../include/

OBJ=../obj/
BIN=../bin/

$(BIN)main_sequential.out: $(OBJ)main_sequential.o $(OBJ)rgb2gray.o $(OBJ)smoothing.o
	$(CXX) $(OBJ)main_sequential.o $(OBJ)rgb2gray.o $(OBJ)smoothing.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)main_sequential.out

$(BIN)seq_smoothing_perf_eval.out: $(OBJ)utimer.o $(OBJ)rgb2gray.o $(OBJ)smoothing.o $(OBJ)seq_smoothing_perf_eval.o
	$(CXX) $(OBJ)utimer.o $(OBJ)rgb2gray.o $(OBJ)smoothing.o $(OBJ)seq_smoothing_perf_eval.o $(CXXFLAGS) $(CPPFLAGS) -o $(BIN)seq_smoothing_perf_eval.out

$(OBJ)main_sequential.o: sequential/main_sequential.cpp
	$(CXX) -c sequential/main_sequential.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)main_sequential.o

$(OBJ)utimer.o: auxiliary/utimer.cpp
	$(CXX) -c auxiliary/utimer.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)utimer.o

$(OBJ)rgb2gray.o: sequential/rgb2gray.cpp
	$(CXX) -c sequential/rgb2gray.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)rgb2gray.o
	
$(OBJ)smoothing.o: sequential/smoothing.cpp
	$(CXX) -c sequential/smoothing.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)smoothing.o

$(OBJ)seq_smoothing_perf_eval.o: sequential/seq_smoothing_perf_eval.cpp
	$(CXX) -c sequential/seq_smoothing_perf_eval.cpp $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ)seq_smoothing_perf_eval.o

clean:
	rm $(OBJ)*.o $(BIN)*.out