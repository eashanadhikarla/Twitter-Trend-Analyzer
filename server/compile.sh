g++ --std=c++14 server.cpp -o Server \
    -I/root/build/libmongocxx-install/include/mongocxx/v_noabi \
    -I/root/build/libmongocxx-install/include/bsoncxx/v_noabi \
    -L/root/build/libmongocxx-install/lib -lmongocxx -lbsoncxx \
    -lcpprest -lboost_system -lcrypto -lssl -ljsoncpp

# Running the server..
# LD_LIBRARY_PATH=/root/build/libmongocxx-install/lib ./Server