all: test_cloudfile cloudfs

clean:
	rm -f *.o test_cloudfile cloudfs

test_cloudfile: test_cloudfile.o cloudfile.o http.o network.o network-posix.o rc.o
	$(CXX) -W -Wall -o $@ $^

cloudfs: fuse.o cloudfile.o http.o network.o network-posix.o rc.o
	$(CXX) -W -Wall -o $@ $^ -lfuse

%.o: %.cpp
	$(CXX) -W -Wall -c -o $@ $^

.PHONY:
	all clean
