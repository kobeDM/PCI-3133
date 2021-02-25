
# for linux
CXX     = g++
#CXXFLAGS= -O2 -Wall -I. -I/usr/local/universe_vme
#CXXFLAGS= -O2 -Wall -I. -I/usr/local/vmedrv
CXXFLAGS= -O2 -Wall -I. 
LD      = g++
LDFLAGS = -O2 -Wall
EXTRACFLAGS=-lgpg3100

# create parameters
#CXXFLAGS += $(ROOTCFLAGS)
#LIBS      = $(ROOTLIBS) $(SYSLIBS)
#GLIBS     = $(ROOTGLIBS) $(SYSLIBS)
LIBS      = $(SYSLIBS)
GLIBS     = $(SYSLIBS)

#MIuOBJ2 = v785.o rpv130.o rpv160.o memory.o

#MIuOBJ = rpv130.o 

all: runPCI-3133_read

runPCI-3133_read	: runPCI-3133_read.o 
	$(LD) $(LDFLAGS) $(EXTRACFLAGS)  $(LIBS) -o $@  runPCI-3133_read.o 

runPCI-3133_read.o	: runPCI-3133_read.c 
	$(CXX) $(CXXFLAGS)  $(EXTRACFLAGS)  -c -o $@  runPCI-3133_read.c


clean:
	rm -f *.o
	rm -f *~
