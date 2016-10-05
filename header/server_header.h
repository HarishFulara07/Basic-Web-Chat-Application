/*
	header file for server
*/

// socket descriptors returned by the socket function
int reg_ser_sockfd, login_ser_sockfd;
/*
	* cli_sockfd is the client's socket descriptor.
	* it is returned when the server accepts a connection from the client.
	* all read-write operations will be done on this descriptor to communicate with the client
*/
int reg_cli_sockfd, login_cli_sockfd;
// structure to hold the server's socket information for registration service
struct sockaddr_in reg_ser_sock;
// structure to hold the server's socket information for login service
struct sockaddr_in login_ser_sock;
// structure to hold the client's socket information for registration service
struct sockaddr_in reg_cli_sock;
// structure to hold the client's socket information for login service
struct sockaddr_in login_cli_sock;
// to get size of the structure to hold the client's socket information for registration service
int reg_cli_sock_len;
// to get size of the structure to hold the client's socket information for login service
int login_cli_sock_len;

// set of socket descriptors. It will be used to handle multiple connections at a time
fd_set fds;
// array to hold socket fd of connected sockets for registration
int reg_sockets[20];
// array to hold socket fd of connected sockets for login
int login_sockets[20];
// max number of connnections allowed by the server
int max_sockets_num;
// current number of registration connections
int cur_reg_sockets_num;
// current number of login connections
int cur_login_sockets_num;
// max value of socket descriptor in fd_set. It is used in select function.
int max_sd;
// socket descriptor
int sock_desc;

// buffer for reading message sent from client
char read_buffer[1025];
// buffer for sending message sent to client
char write_buffer[1025];
// to hold the value returned by read() function
int read_msg;
// to hold the value returned by write() function
int write_msg;

// logging all those users who are curently logged in
char loggedin_users[20][40];
// sockets fd of the users who are logged in
// it will be used while processing 'msg' command
int loggedin_users_sfd[20];

// initiallizes the server to listen on two ports: registration port and login port
void initialize_server();
// function to return max of two numbers
int MAX(int, int);