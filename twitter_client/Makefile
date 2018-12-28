CC=g++

all : Client_manager server

libcurl : 
		cd ../libtwitcurl && make install 
		cd ../Client_manager
	
#/usr/lib/x86_64-linux-gnu/
#/usr/include
Client_manager : libcurl Client_manager.cpp
	$(CC) Client_manager.cpp -I./include/ -I /usr/lib/x86_64-linux-gnu/ -L/usr/inlcude/ -I/usr/local/include/mongocxx/v_noabi/ -I/usr/local/include/bsoncxx/v_noabi/ -L/usr/local/lib/ -ljsoncpp -lcpprest -ltwitcurl -lmongocxx -lbsoncxx -o Client_manager



clean : 
		rm Client_manager
		cd ../libtwitcurl && sudo make clean
		cd ../Client_manager

