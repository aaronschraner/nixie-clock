CC=g++
CPPFLAGS=
CPPFILES=nixie_driver.cpp
LINKFLAGS=-lwiringPi -lpthread
EXE=nixiedriver

$(EXE):$(CPPFILES)
	g++ -o $(EXE) $(CPPFILES) $(CPPFLAGS) $(LINKFLAGS)

run:$(EXE)
	sudo ./$(EXE)

clean:
	rm -fv $(EXE) *~


