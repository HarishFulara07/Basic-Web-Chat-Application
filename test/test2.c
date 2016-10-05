/*
* Test program to check that atmost 20 users can simultaneously login at a time
*/

#include "../header/common_header.h"
#include "../header/client_header.h"

char username[21][41] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
							"N", "O", "P", "Q", "R", "S", "T"};
char password[21][41] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
							"N", "O", "P", "Q", "R", "S", "T"};

pthread_t p[21];
pthread_mutex_t lock;

int ind = 0;

void * login_user(void * arg) {
	pthread_mutex_lock(&lock);

	printf("Client%d -> ", ind+1);

	initialize_client_for_login();

	bzero(read_buffer, 1025);
	read_msg = read(login_cli_sockfd, read_buffer, 1024);

	if(read_msg > 0) {
		if(strcmp(read_buffer, "-1") == 0) {
			fprintf(stdout, "ERROR: server too busy. Please try again after sometime.\n\n");
		}
		else {
			fprintf(stdout, "Message from server: %s\n\n", read_buffer);
		}
	}
	else {
		fprintf(stderr, "ERROR in receiving login ACK from the server.\n\n");
		exit(1);
	}

	++ind;

	pthread_mutex_unlock(&lock);
}

int main() {
	int i;

	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("\n mutex init failed\n");
		return 1;
	}

	// trying to make 21 registration connections to the server simultaneously
	// the 21st connection will be declined by the server because the server can simultaneously handle only 20 registration connections
	for (i = 0; i < 21; ++i) {
		pthread_create(&p[i], NULL, login_user, &i);
	}

	pthread_join(p[0], NULL);
	pthread_join(p[1], NULL);
	pthread_join(p[2], NULL);
	pthread_join(p[3], NULL);
	pthread_join(p[4], NULL);
	pthread_join(p[5], NULL);
	pthread_join(p[6], NULL);
	pthread_join(p[7], NULL);
	pthread_join(p[8], NULL);
	pthread_join(p[9], NULL);
	pthread_join(p[10], NULL);
	pthread_join(p[11], NULL);
	pthread_join(p[12], NULL);
	pthread_join(p[13], NULL);
	pthread_join(p[14], NULL);
	pthread_join(p[15], NULL);
	pthread_join(p[16], NULL);
	pthread_join(p[17], NULL);
	pthread_join(p[18], NULL);
	pthread_join(p[19], NULL);
	pthread_join(p[20], NULL);

	printf("The 21st client is unable to login because the server can only handle 20 login connections at a time.\n");
	return 0;
}