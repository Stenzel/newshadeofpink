# makefile for new shade of pink testsuite
# (c) 2014 Stefan Stenzel 
# stefan at ioptigan.com

SRC		= pink.cpp pink-int.cpp pink-low.cpp pink52.cpp testpink.cpp 
INC		= pink.h pink-int.h pink-low.h pink52.h floathack.h

#c++20 for std::bit_cast, else uses a union which works but is undefined behaviour.
OPT		= -std=c++20 -Wno-narrowing

pink.wav:	testpink
			./testpink

testpink:	$(SRC) $(INC)
			g++ $(OPT) $(SRC) -o $@

clean:		
			rm -f testpink *.wav

