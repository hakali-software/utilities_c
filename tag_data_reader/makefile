TagDataReader: main.o dbios.o
	g++ -std=c++17 main.o dbios.o -o TagDataReader

main.o: main.cpp
	g++ -std=c++17 -c main.cpp

dbios.o: dbios.cpp
	g++ -std=c++17 -c dbios.cpp

clean:
	rm *.o TagDataReader
