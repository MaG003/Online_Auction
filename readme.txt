client:
g++ -o client auction_room.cpp user.cpp client.cpp main.cpp

server:
g++ -o server auction_room.cpp user.cpp connect_sql.cpp server.cpp -lodbc