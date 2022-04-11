//客户端与服务器的编译指令


g++ Server.cpp -lwsock32 -o ./server.exe

.\server.exe

g++ Client.cpp -lwsock32 -o ./client.exe
.\client.exe