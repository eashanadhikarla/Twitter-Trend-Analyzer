
g++ --std=c++11 server.cpp -Wl -o Server \
    -I/root/build/libmongocxx-install/include/mongocxx/v_noabi \
    -I/root/build/libmongocxx-install/include/bsoncxx/v_noabi \
    -L/root/build/libmongocxx-install/lib \
    -L/root/build/libmongocxx-install/lib/libmongocxx.so.v_noabi \
    -L/root/build/libmongocxx-install/lib/libmongocxx.so \
    -L/root/build/libmongocxx-install/lib/libmongocxx.so.3.3.1 \
    -lmongocxx -lbsoncxx -lcpprest -lboost_system -lcrypto -lssl

    # -I/root/Project5/Restweb-server/include \