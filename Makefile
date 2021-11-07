PLATFORM = X11
HDR = graphics.h easygl_constants.h

FLAGS = -g -Wall -D$(PLATFORM) 

ifeq ($(PLATFORM),X11)
   GRAPHICS_LIBS = -lX11
endif

Part1: Part1.cpp $(HDR)
	g++ -std=c++11 Part1.cpp -c $(FLAGS) 
	g++ -c -g -Wall -D$(PLATFORM) graphics.cpp
	g++ -g -Wall -D$(PLATFORM) graphics.o Part1.o $(GRAPHICS_LIBS) -o Part1_exe.o  

Part2: Part2.cpp $(HDR)
	g++ -std=c++11 Part2.cpp -c $(FLAGS) 
	g++ -c -g -Wall -D$(PLATFORM) graphics.cpp
	g++ -g -Wall -D$(PLATFORM) graphics.o Part2.o $(GRAPHICS_LIBS) -o Part2_exe.o  