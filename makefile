
main: master.o slave.o main_master.o main_slave.o
	g++ -o main_master main_master.o master.o  \
	 -std=c++11 -O0 -g -lcaf_core -lcaf_io -pthread -lboost_serialization  
	g++ -o main_slave main_slave.o slave.o \
	 -std=c++11 -O0 -g -lcaf_core -lcaf_io -pthread -lboost_serialization 
master.o: master.cpp   
	g++ master.cpp -std=c++11 -O0 -g -c 

slave.o: slave.cpp  
	g++ slave.cpp -std=c++11 -O0 -g -c  

main_master.o: main_master.cpp  
	g++ main_master.cpp -std=c++11 -O0 -g -c      

main_slave.o: main_slave.cpp  
	g++ main_slave.cpp -std=c++11 -O0 -g -c   
clean:
	rm   master.o slave.o main_master.o main_slave.o 
