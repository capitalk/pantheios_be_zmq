CC=g++


COMMON=../../../common
UTILS=$(COMMON)/src/c++
UTILS_SRC=$(UTILS)/utils
PROTO=./proto
PANTHEIOS_ROOT=/opt/src/pantheios
SRC=..

INCLUDE= -I $(PANTHEIOS_ROOT)/include \
		-I $(STLSOFT)/include \
		-I $(UTILS)\
		-I $(COMMON) \
    -I ..

LIBS= -lboost_date_time \
		-lboost_program_options \
		-lboost_filesystem \
		-lzmq \
		-lprotobuf \
		-lrt \
		-lboost_thread \
		-lgtest

# N.B. MAKE SURE YOU LINK WITH THE RIGHT LIBS (check the compiler gccXX)
PANTHEIOS_LIBS= -L $(PANTHEIOS_ROOT)/lib \
        -l pantheios.1.core.gcc44.file64bit \
        -l pantheios.1.fe.N.gcc44.file64bit.debug \
        -l pantheios.1.util.gcc44.file64bit \
	    	-l pantheios.1.be.N.gcc44.file64bit \
        -l pantheios.1.bec.file.gcc44.file64bit \
        -l pantheios.1.bec.fprintf.gcc44.file64bit \
        -l pantheios.1.util.gcc44.file64bit \
        -l pantheios.1.core.gcc44.file64bit 

CFLAGS = -Wall -std=c++0x -pthread -DDEBUG -ggdb -O0  -DLOG -pg -g
LDFLAGS =  -pg

all: test_pantheios_be_zmq 


test_pantheios_be_zmq : test_pantheios_be_zmq.o pantheios_be_zmq.o $(PROTOBUF_OBJS)
	$(CC) $(LDFLAGS) test_pantheios_be_zmq.o pantheios_be_zmq.o $(LIBS) $(PROTOBUF_OBJS) $(PANTHEIOS_LIBS) -o $@ 

test_pantheios_be_zmq.o: test_pantheios_be_zmq.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

pantheios_be_zmq.o: $(SRC)/pantheios_be_zmq.cpp
	$(CC) -c $(INCLUDE) $(CFLAGS) $<

clean :
	rm -f *.o test_pantheios_be_zmq  ../*.o


.PHONY : pantheios_be_zmq 


