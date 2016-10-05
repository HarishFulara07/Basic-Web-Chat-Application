/*
	Header file for client
*/

// socket descriptors returned by the socket function
int reg_cli_sockfd, login_cli_sockfd;
// structure to hold the client's socket information for registration service
struct sockaddr_in reg_cli_sock;
// structure to hold the client's socket information for login service
struct sockaddr_in login_cli_sock;

// buffer for reading message sent from server
char read_buffer[1025];
// buffer for sending message sent to the server
char write_buffer[1025];
// to hold the value returned by read() function
int read_msg;
// to hold the value returned by write() function
int write_msg;

//to logout client from chat portal
int logout;

// to check if there is a message to be read before entering a command
fd_set readfd;
// max value of socket descriptor in fd_set. It is used in select function.
int max_sd;

// complete absolute path of the file which client wants to send to another user
char send_file_path[1025];
// complete name of the file which client wants to send to another user
char send_file_name[1025];
// file descriptor of the file to be sent
int send_file_desc;
// structure to hold file statistics
struct stat file_stat;
// number of bytes of file sent from client to server
int bytes_sent;
// number of offset bytes of file sent from client to server
long int send_offset;
// number of bytes remaining to be sent from client to server
int send_remain_data;

// path where the received file is to be saved
char recv_file_path[2049];

// initiallizes the client to connect to server for registration
void initialize_client_for_registration();
// initiallizes the client to connect to server for login
void initialize_client_for_login();
// function to return max of two numbers
int MAX(int, int);
// convert int to string
char * int_to_str(int);