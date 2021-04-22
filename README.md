#How to run the code
1. Compile both the codes using "gcc server.c -o server" for server.c and "gcc client.c -o client" for client.c .
1. It will be better if you separate server.c and client.c in two different directories, let the client directory be called client and server's directory be called server . Now compile both the codes as told above.
1. Furthur create the test files in the server folder.
1. Run them in their respective folders using ./client in the client directory and ./server in the server directory.
1. Commands implemented for the client are get and exit.
1. get can be used to fetch files from the server directory to client directory.
1. exit is meant to exit the client and dimiss the connection with the server.
1. When client exits, the server instead of keep on running it will ask whether you want to keep it running or not.
1. Any other command will not be recognised by the client.
1. The code as suggested by the assignment pdf is designed to just handle the filenames so do not provide absolute or relative paths for file.

##Brief Implementation Details
1. The server first creates a socket and binds it with a port and an IP address. Then the client tries to connect with the server. In result the server accepts the request and starts communication. Firstly the client sends either the number of files that has been given as an argument to get or sends exit if the command entered was exit. Then after delivering the number of files both the codes run a loop for a number of iterations that is equal to this number of files. Corresponding to each iteration the client sends the file name to the server. Then the server checks wehther the requested file is present or not. If it is present then it sends the number of characters in it. But if it is not present then it sends -1 to the client indicating that the file was not present. Now after sending the number of bytes the server will extract the charachters of the file in chunks of 1000 bytes and send them to the client side where these will be written to new file being downloaded there. Once all the files are transfered the user can again enter get or either exit. The server will keep on running but after the end of each connection the server will ask the user whther to terminate the server or to keep it running
