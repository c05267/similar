all: genRep genNew
	cd IP; make
	g++ -o genRep genRep.o IP/*.o
	g++ -o genNew genNew.o IP/*.o

genNew:
	g++ -c genNew.cpp

genRep:
	g++ -c genRep.cpp

clean:
	rm -f */*.o *.o */*~ *~ */.*~ .*~ genNew genRep
