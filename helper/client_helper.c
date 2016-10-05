/*
	helper client program which is used by main client program to initialize the client, i.e,
	make the client ready for connecting with the server 
*/

#include "../header/common_header.h"
#include "../header/client_header.h"

// initiallizes the client to connect to server for registration
void initialize_client_for_registration() {
	// reg_port_no will be used for client registration on chat portal
	reg_port_no = 6060;

	/*
		Create the socket
	*/
	reg_cli_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*
		Check whether the socket is created successfully
	*/
	if(reg_cli_sockfd < 0) {
		fprintf(stderr, "ERROR creating socket. Please try again.\n");
		exit(1);
	}

	/*
		Initialize the socket information
	*/
	bzero((char *)&reg_cli_sock, sizeof(reg_cli_sock));

	reg_cli_sock.sin_family = AF_INET;
	reg_cli_sock.sin_port = htons(reg_port_no);

	/*
		Convert the localhost address (127.0.0.1) to a network address in IPv4 family
	*/
	if(inet_pton(AF_INET, "127.0.0.1", &(reg_cli_sock.sin_addr)) <= 0) {
        fprintf(stderr,"ERROR: Invalid address.\n");
        exit(1);
    }

    /*
    	Connect to the server
    */
    if(connect(reg_cli_sockfd, (struct sockaddr *)&reg_cli_sock, sizeof(reg_cli_sock)) < 0) {
    	fprintf(stderr, "ERROR connecting. Please try again.\n");
    	exit(1);
    }
}

// initiallizes the client to connect to server for login
void initialize_client_for_login() {
	// login_port_no will be used for client login on chat portal
	login_port_no = 7070;

	/*
		Create the socket
	*/
	login_cli_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	/*
		Check whether the socket is created successfully
	*/
	if(login_cli_sockfd < 0) {
		fprintf(stderr, "ERROR creating socket. Please try again.\n");
		exit(1);
	}

	/*
		Initialize the socket information
	*/
	bzero((char *)&login_cli_sock, sizeof(login_cli_sock));

	login_cli_sock.sin_family = AF_INET;
	login_cli_sock.sin_port = htons(login_port_no);

	/*
		Convert the localhost address (127.0.0.1) to a network address in IPv4 family
	*/
	if(inet_pton(AF_INET, "127.0.0.1", &(login_cli_sock.sin_addr)) <= 0) {
        fprintf(stderr,"ERROR: Invalid address.\n");
        exit(1);
    }

    /*
    	Connect to the server
    */
    if(connect(login_cli_sockfd, (struct sockaddr *)&login_cli_sock, sizeof(login_cli_sock)) < 0) {
    	fprintf(stderr, "ERROR connecting. Please try again.\n");
    	exit(1);
    }
}

// return the max value
int MAX(int x, int y) {
	return x > y ? x : y;
}

char * int_to_str(int n) {
	int len = 0;
	int tmp = n;
	int pos = 0;

	while(tmp > 0) {
		len++;
		tmp = tmp / 10;
	}

	tmp = n;

	int num[len];
	char * str = (char *)malloc((len+1) * sizeof(char)); 

	while(tmp > 0) {
		int rem = tmp % 10;
		num[pos] = rem;
		pos++;
		tmp = tmp / 10;
	}

	while(pos > 0) {
		pos--;
		str[len-pos-1] = num[pos] + '0';
	}

	str[len] = '\0';

	return str;
}