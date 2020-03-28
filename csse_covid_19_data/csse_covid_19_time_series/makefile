
BASEDIR = .

BINARY = crunch

SOURCES = main.cpp

OBJECTS = ${SOURCES:.cpp=.o} 

INCLUDES = -I .   

LOCATIONS = -L /usr/lib 

LIBRARIES =   -lstdc++ 

CXXFLAGS = -ggdb -Wall -ansi --std=c++11
CXX = g++ ${CXXFLAGS} -DREENTRANT -D_REENTRANT 

.SUFFIXES:      .cpp .o

.cpp.o:
		@echo
		@echo Building $@
		${CXX} ${INCLUDES} -c -o $@ $<          

all:            ${OBJECTS} ${BINARY} 

${BINARY}:      ${OBJECTS}
		@echo
		@echo Building ${BINARY} Executable
		${CXX} -o $@ \
		${OBJECTS}  \
		${LIBRARIES} \
		${LOCATIONS}
                         
clean:
		rm -f ${BINARY} *.o



