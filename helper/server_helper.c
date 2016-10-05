/*
	helper server program which is used by main server program to initialize the server, i.e,
	make the server ready for listening for connections 
*/

#include "../header/common_header.h"
#include "../header/server_header.h"

// initiallizes the server to listen on two ports: registration port and login port
void initialize_server() {
	int i;
	// reg_port_no will be used for client registration on chat portal
	reg_port_no = 6060;
	// login_port_no will be used for client login on chat portal
	login_port_no = 7070;

	// max number of connnections allowed by the server is 20
	max_sockets_num = 20;

	cur_reg_sockets_num = 0;
	cur_login_sockets_num = 0;
	
	// array to hold socket fd of connected sockets, initialized with 0
	for (i = 0; i < max_sockets_num; ++i) {
		reg_sockets[i] = 0;
		login_sockets[i] = 0;
		strcpy(loggedin_users[i], "\0");
		loggedin_users_sfd[i] = 0;
	}
	
	/*
		Create the sockets
	*/
	
	reg_ser_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	login_ser_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*
		Check whether the sockets are created successfully
	*/
	if(reg_ser_sockfd < 0) {
		fprintf(stderr, "ERROR creating registration socket.\n");
		exit(1);
	}
	if(login_ser_sockfd < 0) {
		fprintf(stderr, "ERROR creating login socket.\n");
		exit(1);
	}

	/*
		Initialize the sockets
	*/
	bzero((char *)&reg_ser_sock, sizeof(reg_ser_sock));
	bzero((char *)&login_ser_sock, sizeof(login_ser_sock));

	reg_ser_sock.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(reg_ser_sock.sin_addr));
	reg_ser_sock.sin_port = htons(reg_port_no);

	login_ser_sock.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &(login_ser_sock.sin_addr));
	login_ser_sock.sin_port = htons(login_port_no);

	/*
		Bind the host address using bind() call
	*/
	if(bind(reg_ser_sockfd, (struct sockaddr *)&reg_ser_sock, sizeof(reg_ser_sock)) < 0) {
		fprintf(stderr, "ERROR on binding registration socket.\n");
		exit(1);
	}
	if(bind(login_ser_sockfd, (struct sockaddr *)&login_ser_sock, sizeof(login_ser_sock)) < 0) {
		fprintf(stderr, "ERROR on binding login socket.\n");
		exit(1);
	}

	/* 
		* Now start listening for the clients.
		* Here process will go in sleep mode and will wait for the incoming connection
		* The process will be able to queue upto 1024 connections at a time.
   	*/

	if(listen(reg_ser_sockfd, 1024) < 0) {
		fprintf(stderr, "ERROR in listening to clients for registration requests.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "\nServer listening to clients for registration requests.\n\n");
	}

	if(listen(login_ser_sockfd, 1024) < 0) {
		fprintf(stderr, "ERROR in listening to clients for login requests.\n");
		exit(1);
	}
	else {
		fprintf(stdout, "Server listening to clients for login requests.\n\n");
	}
}

// return the max value
int MAX(int x, int y) {
	return x > y ? x : y;
}