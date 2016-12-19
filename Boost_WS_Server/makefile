CC = g++ -std=c++11
CC_FLAGS = -w
LIBS = -lodbc -lboost_system -lboost_serialization -lboost_thread -lprotobuf -L/home/ubuntu/cproj/AsyncServerParent/AsyncServerParent/ -lasync_server -L/home/ubuntu/cproj/HTMLHandle/HTMLHandle/ -lhtml_handle
INCLUDES = -I/home/ubuntu/cproj/AsyncServerParent/AsyncServerParent/ -I/home/ubuntu/cproj/HTMLHandle/HTMLHandle/

EXEC = run
SOURCES = $(wildcard *.cpp)
PROTO_SOURCES = $(shell find . -name *.pb.cc)
OBJECTS = $(SOURCES:.cpp=.o)
PROTO_OBJECTS = $(PROTO_SOURCES:.pb.cc=.o)

# Main target                                                                                                                                                                                               
$(EXEC): $(OBJECTS) $(PROTO_OBJECTS)
	$(CC) -c $(OBJECTS) $(PROTO_OBJECTS) -o $(EXEC) $(LIBS)
	ar rcs libws_server.a $(OBJECTS) $(PROTO_OBJECTS)

# To obtain object files                                                                                                                                                                                    
%.o: %.cpp
	$(CC) -c $(INCLUDES) $(CC_FLAGS) $< -o $@

%.o: %.pb.cc
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files                                                                                                                                                                                 
clean:
	rm -f $(EXEC) $(OBJECTS) $(PROTO_OBJECTS)
