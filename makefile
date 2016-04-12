# makefile for new shade of pink testsuite

pink.raw:	testpink
			./testpink

testpink:	pink.h pink.cpp pink-int.h pink-int.cpp pink-low.h pink-low.cpp pink52.h \
			pink52.cpp testpink.cpp
			g++ testpink.cpp pink.cpp pink-int.cpp pink-low.cpp pink52.cpp -o testpink

clean:		
			rm -f testpink *.raw
