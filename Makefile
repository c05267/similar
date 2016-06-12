all: genRep genNew genNew_v2
	cd IP; make
	g++ -o genRep genRep.o IP/*.o
	g++ -o genNew genNew.o IP/*.o
	g++ -o genNew_v2 genNew_v2.o IP/*.o

genNew:
	g++ -c genNew.cpp

genNew_v2:
	g++ -c genNew_v2.cpp

genRep:
	g++ -c genRep.cpp

clean:
	rm -f */*.o *.o */*~ *~ */.*~ .*~ genNew genNew_v2 genRep 
