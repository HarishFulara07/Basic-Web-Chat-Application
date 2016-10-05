/*
	main server program
*/

#include "../header/common_header.h"
#include "../header/server_header.h"
#include "../header/register_header.h"
#include "../header/login_header.h"
#include "../header/group_header.h"

int main(void) {
	int i;

	// initiallizes the server to listen on two ports: registration port and login port
	initialize_server();

	while(1) {
		// clear the socket set
		FD_ZERO(&fds);

		// add the registration and login socket to the set
		FD_SET(reg_ser_sockfd, &fds);
		FD_SET(login_ser_sockfd, &fds);
		
		max_sd = MAX(reg_ser_sockfd, login_ser_sockfd);

		// add registration sockets to fd_set
		for (i = 0 ; i < max_sockets_num ; i++) {
			sock_desc = reg_sockets[i];

			// if valid socket descriptor then add to fd_set
			if(sock_desc > 0) {
				FD_SET(sock_desc, &fds);
			}

			// maximum value socket descriptor. It is used in select function
			if(sock_desc > max_sd) {
				max_sd = sock_desc;
			}
		}

		// add login sockets to fd_set
		for (i = 0 ; i < max_sockets_num ; i++) {
			sock_desc = login_sockets[i];

			// if valid socket descriptor then add to fd_set
			if(sock_desc > 0) {
				FD_SET(sock_desc, &fds);
			}

			// maximum value socket descriptor. It is used in select function
			if(sock_desc > max_sd) {
				max_sd = sock_desc;
			}
		}

		// wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
		// when a socket is ready to be read, select will return and fds will have those sockets
		// which are ready to be read
		select(max_sd + 1, &fds, NULL, NULL, NULL);

		// if something happened on the registration socket
		if(FD_ISSET(reg_ser_sockfd, &fds)) {
			/*
				Accept the connection from the client.
			*/
			reg_cli_sock_len = sizeof(reg_cli_sock);
			reg_cli_sockfd = accept(reg_ser_sockfd, (struct sockaddr *)&reg_cli_sock, &reg_cli_sock_len);

			if(reg_cli_sockfd < 0) {
				fprintf(stderr, "ERROR in accepting the connection.\n");
				exit(1);
			}
			// server cannot process the incoming registration request
			else if(cur_reg_sockets_num >= max_sockets_num) {
				bzero(write_buffer, 1025);
				strcpy(write_buffer, "-1\0");
				
				write_msg = write(reg_cli_sockfd, write_buffer, strlen(write_buffer));

				if(write_msg < 0) {
					fprintf(stderr, "ERROR in sending 'server too busy' ACK to the client.\n");
					exit(1);
				}
				else {
					fprintf(stdout, "'server too busy' ACK sent to the client.\n\n");
				}
			}
			else {
				bzero(write_buffer, 1025);
				strcpy(write_buffer, "Connection Accepted. Client OK to register.\0");
				fprintf(stdout, "%s\n", write_buffer);
				write_msg = write(reg_cli_sockfd, write_buffer, strlen(write_buffer));

				if(write_msg < 0) {
					fprintf(stderr, "ERROR in sending registration ACK to the client.\n");
					exit(1);
				}
				else {
					fprintf(stdout, "Registration ACK sent to the client.\n\n");
				}

				// add new socket to array of sockets
				for (i = 0; i < max_sockets_num; i++) {
					// if position is empty
					if(reg_sockets[i] == 0) {
						reg_sockets[i] = reg_cli_sockfd;
						cur_reg_sockets_num++;
						break;
					}
				}
			}
		}
		// if something happened on the login socket
		else if(FD_ISSET(login_ser_sockfd, &fds)) {
			/*
				Accept the connection from the client.
			*/
			login_cli_sock_len = sizeof(login_cli_sock);
			login_cli_sockfd = accept(login_ser_sockfd, (struct sockaddr *)&login_cli_sock, &login_cli_sock_len);

			if(login_cli_sockfd < 0) {
				fprintf(stderr, "ERROR in accepting the connection.\n");
				exit(1);
			}
			// server cannot process the incoming login request
			else if(cur_login_sockets_num >= max_sockets_num) {
				bzero(write_buffer, 1025);
				strcpy(write_buffer, "-1\0");
				
				write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

				if(write_msg < 0) {
					fprintf(stderr, "ERROR in sending 'server too busy' ACK to the client.\n");
					exit(1);
				}
				else {
					fprintf(stdout, "'server too busy' ACK sent to the client.\n\n");
				}
			}
			else {
				bzero(write_buffer, 1025);
				strcpy(write_buffer, "Connection Accepted. Client OK to login.\0");
				fprintf(stdout, "%s\n", write_buffer);
				write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

				if(write_msg < 0) {
					fprintf(stderr, "ERROR in sending login ACK to the client.\n");
					exit(1);
				}
				else {
					fprintf(stdout, "Login ACK sent to the client.\n\n");
				}

				// add new socket to array of sockets
				for (i = 0; i < max_sockets_num; i++) {
					// if position is empty
					if(login_sockets[i] == 0) {
						login_sockets[i] = login_cli_sockfd;
						cur_login_sockets_num++;
						break;
					}
				}
			}
		}

		// else its some IO operation on some other socket
		for (i = 0; i < 2*max_sockets_num; i++) {
			if(i < max_sockets_num) {
				sock_desc = reg_sockets[i];	
			}
			else if(i >= max_sockets_num) {
				sock_desc = login_sockets[i % max_sockets_num];
			}

			if(FD_ISSET(sock_desc, &fds)) {
				bzero(read_buffer, 1025);
				read_msg = read(sock_desc, read_buffer, 1024);
				
				// check if it was for closing, if user presses ctrl+c or closes the terminal
				if(read_msg == 0) {
					close(sock_desc);
					if(i < max_sockets_num) {
						reg_sockets[i] = 0;
					}
					else {
						login_sockets[i % max_sockets_num] = 0;
					}
				}				
				else if(read_msg > 0) {
					// read message sent from the client
					// splitting the string on delimiter :
					const char delim[2] = ":";
					char * id;

					id = strtok(read_buffer, delim);

					// id = 1 means user wants to register
					if(strcmp(id, "1") == 0) {
						char * username = strtok(NULL, delim);
						char * password = strtok(NULL, delim);

						// register_client() function is defined in register_main.c
						int response_code = register_client(username, password);

						bzero(write_buffer, 1025);
						
						// if response code is 1 then it means client is registered successfully
						if(response_code == 1) {
							strcpy(write_buffer, "You are registered successfully.\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending registration complete ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Registration complete ACK sent to the client.\n\n");
							}
						}
						// else it means username already exists in the database					
						else {
							strcpy(write_buffer, "Username already exists. Please try again with a different username.\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending registration fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Registration fail ACK sent to the client.\n\n");
							}
						}

						// close the socket after handling client registration
						// mark as 0 in sockets array for reuse 
						close(sock_desc);
						reg_sockets[i] = 0;
						cur_reg_sockets_num--;
					}
					// id = 2 means user wants to login
					else if(strcmp(id, "2") == 0) {
						char * username = strtok(NULL, delim);
						char * password = strtok(NULL, delim);

						// login_client() function is defined in login_main.c
						int response_code = login_client(username, password);

						bzero(write_buffer, 1025);
						
						// if response code is 1 then it means client is logged in successfully
						if(response_code == 1) {
							strcpy(write_buffer, "1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending login complete ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Login complete ACK sent to the client.\n\n");
							}

							int j;
							// log user as logged in in loggedin_users array
							for (j = 0; j < max_sockets_num; j++) {
								// if position is empty
								if(strcmp(loggedin_users[j], "\0") == 0) {
									strcpy(loggedin_users[j], username);
									strcat(loggedin_users[j], "\0");
									loggedin_users_sfd[j] = sock_desc;
									break;
								}
							}
						}
						// else it means login credentials are invalid					
						else {
							strcpy(write_buffer, "-1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending login fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Login fail ACK sent to the client.\n\n");
							}

							// close the socket if user login credentials are invalid
							// mark as 0 in sockets array for reuse 
							close(sock_desc);
							login_sockets[i % max_sockets_num] = 0;
							cur_login_sockets_num--;
						}
					}
					// id = 3 means user wants to logout
					else if(strcmp(id, "3") == 0) {	
						char * username = strtok(NULL, delim);

						int j;
						// remove user from loggedin_users array
						for (j = 0; j < max_sockets_num; j++) {
							// if username is found
							if(strcmp(loggedin_users[j], username) == 0) {
								strcpy(loggedin_users[j], "\0");
								loggedin_users_sfd[j] = 0;
								break;
							}
						}

						bzero(write_buffer, 1025);
						strcpy(write_buffer, "Successfully logged out.\0");

						write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

						if(write_msg < 0) {
							fprintf(stderr, "ERROR in sending logout ACK to the client.\n");
							exit(1);
						}
						else {
							fprintf(stdout, "Logout ACK sent to the client.\n\n");
						}

						// close the socket if user wants to logout
						// mark as 0 in sockets array for reuse 
						close(sock_desc);
						login_sockets[i % max_sockets_num] = 0;
						cur_login_sockets_num--;
					}
					// id = 4 means user has sent "who" command
					else if(strcmp(id, "4") == 0) {
						int j;

						bzero(write_buffer, 1025);

						strcpy(write_buffer, "");
						// log user as logged in in loggedin_users array
						for (j = 0; j < max_sockets_num; j++) {
							// if position is empty
							if(strcmp(loggedin_users[j], "\0") != 0) {
								strcat(write_buffer, loggedin_users[j]);
								strcat(write_buffer, "\n");
							}
						}
						strcat(write_buffer, "\0");

						write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

						if(write_msg < 0) {
							fprintf(stderr, "ERROR in sending 'who' command ACK to the client.\n");
							exit(1);
						}
						else {
							fprintf(stdout, "'who' command ACK sent to the client.\n\n");
						}
					}
					// id = 5 means user has sent "msg" command
					else if(strcmp(id, "5") == 0) {
						char * from = strtok(NULL, delim);
						char * to = strtok(NULL, delim);
						char * msg = strtok(NULL, delim);
						int j, is_online = 0, sfd;

						// search whether the receiver is online or not
						for (j = 0; j < max_sockets_num; j++) {
							// if user is online
							if(strcmp(loggedin_users[j], to) == 0) {
								is_online = 1;
								sfd = loggedin_users_sfd[j];
								break;
							}
						}

						if(!is_online) {
							// tell sender that receiver is not online
							bzero(write_buffer, 1025);
							strcpy(write_buffer, "Unable to send message. Receiver is not online or receiver's username is invalid.");
							strcat(write_buffer, "\0");

							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending message fail ACK to the sender %s.\n", from);
								exit(1);
							}
							else {
								fprintf(stdout, "Message fail sent ACK to the sender %s\n\n", from);
							}

							continue;
						}

						// message being forwarded from server to the receiver
						bzero(write_buffer, 1025);
						strcpy(write_buffer, "1:");
						strcat(write_buffer, from);
						strcat(write_buffer, ":");
						strcat(write_buffer, msg);
						strcat(write_buffer, ":end");
						strcat(write_buffer, "\0");

						write_msg = write(sfd, write_buffer, strlen(write_buffer));

						if(write_msg < 0) {
							fprintf(stderr, "ERROR in sending message to the receiver %s.\n", to);
							exit(1);
						}
						else {
							fprintf(stdout, "Message sent to the receiver %s.\n\n", to);
						}

						// sending ACK to the sender
						bzero(write_buffer, 1025);
						strcpy(write_buffer, "Mesasge sent successfully.");
						strcat(write_buffer, "\0");

						write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

						if(write_msg < 0) {
							fprintf(stderr, "ERROR in sending message ACK to the sender %s.\n", from);
							exit(1);
						}
						else {
							fprintf(stdout, "Message sent ACK to the sender %s\n\n", from);
						}
					}
					// id = 6 means user has sent "create_grp" command
					else if(strcmp(id, "6") == 0) {
						char * grp_name = strtok(NULL, delim);
						char * username = strtok(NULL, delim);

						// create_grp() function is defined in group_helper.c
						int response_code = create_grp(grp_name, username);

						bzero(write_buffer, 1025);

						// if response code is 1 then it means that group is created successfully
						if(response_code == 1) {
							strcpy(write_buffer, "1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group created ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group created ACK sent to the client.\n\n");
							}
						}
						// else it means group is not created				
						else {
							strcpy(write_buffer, "-1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group creation fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group creation fail ACK sent to the client.\n\n");
							}
						}
					}
					// id = 7 means user has sent "join_grp" command
					else if(strcmp(id, "7") == 0) {
						char * grp_name = strtok(NULL, delim);
						char * username = strtok(NULL, delim);

						// join_grp() function is defined in group_helper.c
						int response_code = join_grp(grp_name, username);

						bzero(write_buffer, 1025);

						// if response code is 1 then it means that group is created successfully
						if(response_code == 1) {
							strcpy(write_buffer, "1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group joined ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group joined ACK sent to the client.\n\n");
							}
						}
						// if response code is 0 then it means that user is already a member of the group
						else if(response_code == 0) {
							strcpy(write_buffer, "0\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group join fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group join fail ACK sent to the client.\n\n");
							}
						}
						// else it means group is not created				
						else {
							strcpy(write_buffer, "-1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group join fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group join fail ACK sent to the client.\n\n");
							}
						}
					}
					// id = 8 means user has sent "msg_group" command
					else if(strcmp(id, "8") == 0) {
						char * grp_name = strtok(NULL, delim);
						char * username = strtok(NULL, delim);

						// msg_group_sanity_checks() function is defined in group_helper.c
						int response_code = msg_group_sanity_checks(grp_name, username);

						bzero(write_buffer, 1025);

						// if response code is 1 then it means that group exists
						// and user is a member of the group
						if(response_code == 1) {
							// lets send message to all the members of the group
							char * msg = strtok(NULL, delim);

							// open the group file
							char file_name[61];
							strcpy(file_name, "database/");
							strcat(file_name, grp_name);
							strcat(file_name, ".txt");

							FILE * fp = fopen(file_name, "r");

							while(!feof(fp)) {
								int j, is_online = 0, sfd;
								char uname[41];

								fscanf(fp, "%s", uname);

								// search whether the receiver is online or not
								for (j = 0; j < max_sockets_num; j++) {
									// if user is online
									if(strcmp(loggedin_users[j], uname) == 0 && strcmp(uname, username) != 0) {
										is_online = 1;
										sfd = loggedin_users_sfd[j];
										break;
									}
								}

								// if user is not online, continue
								if(!is_online) {
									continue;
								}

								// message being forwarded from server to the receiver
								bzero(write_buffer, 1025);
								strcpy(write_buffer, "2:");
								strcat(write_buffer, grp_name);
								strcat(write_buffer, ":");
								strcat(write_buffer, username);
								strcat(write_buffer, ":");
								strcat(write_buffer, msg);
								strcat(write_buffer, ":end");
								strcat(write_buffer, "\0");

								write(sfd, write_buffer, strlen(write_buffer));
							}

							// close the file
							fclose(fp);

							bzero(write_buffer, 1025);
							strcpy(write_buffer, "1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group message ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group message ACK sent to the client.\n\n");
							}
						}
						// if response code is 0 then it means that user is not a member of the group
						else if(response_code == 0) {
							strcpy(write_buffer, "0\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group message fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group message fail ACK sent to the client.\n\n");
							}
						}
						// else it means group does not exists				
						else {
							strcpy(write_buffer, "-1\0");
							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending group message fail ACK to the client.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "Group message fail ACK sent to the client.\n\n");
							}
						}
					}
					// id = 9 means user has sent "send" command
					else if(strcmp(id, "9") == 0) {
						char * to = strtok(NULL, delim);
						char * from = strtok(NULL, delim);
						int file_size = atoi(strtok(NULL, delim));
						char * file_name = strtok(NULL, delim);
						char file_path[2049];
						int j, is_online = 0, sfd, file_rem;
						ssize_t len;
						
						strcpy(file_path, "file/");
						strcat(file_path, to);
						strcat(file_path, "\0");

						// search whether the receiver is online or not
						for (j = 0; j < max_sockets_num; j++) {
							// if user is online
							if(strcmp(loggedin_users[j], to) == 0) {
								is_online = 1;
								sfd = loggedin_users_sfd[j];
								break;
							}
						}

						if(!is_online) {
							// tell sender that receiver is not online
							bzero(write_buffer, 1025);
							strcpy(write_buffer, "-1");
							strcat(write_buffer, "\0");

							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending file send fail ACK to the sender %s.\n", from);
								exit(1);
							}
							else {
								fprintf(stdout, "Sending file send fail ACK to the sender %s\n\n", from);
							}
							continue;
						}
						else {
							// to check if receiver already has a file pending to be read
							FILE * check_fp = fopen(file_path, "r");

							// receiver has a file pending to be read
							if(check_fp != NULL) {
								bzero(write_buffer, 1025);
								strcpy(write_buffer, "-2\0");
								write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

								if(write_msg < 0) {
									fprintf(stderr, "ERROR in sending receiver busy ACK to the client.\n");
									exit(1);
								}
								else {
									fprintf(stdout, "Receiver busy ACK sent to the client.\n\n");
								}

								fclose(check_fp);
								continue;
							}

							// else tell sender that server is ready to receive the file
							bzero(write_buffer, 1025);
							strcpy(write_buffer, "1");
							strcat(write_buffer, "\0");

							write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "ERROR in sending file send OK ACK to the sender %s.\n", from);
								exit(1);
							}
							else {
								fprintf(stdout, "Sending file send OK ACK to the sender %s\n\n", from);
							}
						}

						// else the receiver has no file pending to be read, so we can continue
						FILE * fp = fopen(file_path, "w");

						if(fp == NULL) {
							fprintf(stderr, "Failed to open file %s.\n\n", file_path);
							exit(1);
						}

						file_rem = file_size;

						bzero(write_buffer, 1025);

						fprintf(fp, "%s\n", from);
						fprintf(fp, "%s\n", file_name);
						// reading file data sent by the client
						while ((file_rem > 0) && ((len = read(sock_desc, write_buffer, 1024)) > 0)) {
							fprintf(fp, "%s", write_buffer);
							file_rem -= len;
							bzero(write_buffer, 1025);
						}

						fclose(fp);

						bzero(write_buffer, 1025);
						strcpy(write_buffer, "File sent successfully\0");
						write_msg = write(sock_desc, write_buffer, strlen(write_buffer));

						if(write_msg < 0) {
							fprintf(stderr, "ERROR in sending file send ACK to the client.\n");
							exit(1);
						}
						else {
							fprintf(stdout, "File send ACK sent to the client.\n\n");
						}
					}
				}

				else {
					fprintf(stderr, "ERROR in reading from socket.");
				}
			}
		}
	}

	return(0);
}