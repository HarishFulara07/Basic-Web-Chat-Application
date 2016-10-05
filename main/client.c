/*
	main client program
*/
#include "../header/common_header.h"
#include "../header/client_header.h"

int main(void) {
	int choice;
	/*
		Chat Portal Home Page
	*/
	do {
		fprintf(stdout, "\n----------Welcome to the Chat Portal----------\n\n");
		fprintf(stdout, "Press 1 and hit Enter - to Register\n");
		fprintf(stdout, "Press 2 and hit Enter - to Login\n");
		fprintf(stdout, "Press 3 and hit Enter - to exit\n\n");
		
		fprintf(stdout, "Enter your choice: ");
		fscanf(stdin, "%d", &choice);

		if(choice == 1) {
			initialize_client_for_registration();

			bzero(read_buffer, 1025);
			read_msg = read(reg_cli_sockfd, read_buffer, 1024);

			if(read_msg > 0) {
				if(strcmp(read_buffer, "-1") == 0) {
					fprintf(stdout, "\nERROR: server too busy. Please try again after sometime.\n");
					continue;
				}
				else {
					fprintf(stdout, "\nMessage from server: %s\n", read_buffer);
				}
			}
			else {
				fprintf(stderr, "\nERROR in receiving registration ACK from the server.\n");
				exit(1);
			}

			char username[41];
			char password[41];
			
			fprintf(stdout, "\n-----------Sign Up for Chat Portal-----------\n\n");
			fprintf(stdout, "Enter your desired username: ");
			fscanf(stdin, "%s", username);
			fprintf(stdout, "Enter your desired password: ");
			fscanf(stdin, "%s", password);

			// sending registration credentials to the server in 1:username:password:end format
			bzero(write_buffer, 1025);
			strcpy(write_buffer, "1:");
			strcat(write_buffer, username);
			strcat(write_buffer, ":");
			strcat(write_buffer, password);
			strcat(write_buffer, ":end\0");

			write_msg = write(reg_cli_sockfd, write_buffer, strlen(write_buffer));

			if(write_msg < 0) {
				fprintf(stderr, "\nERROR in sending registration credentials to the server.\n");
				exit(1);
			}
			else {
				fprintf(stdout, "\nRegistration credentials sent to the server.\n");
			}

			//waiting for ACK from the server for registration
			bzero(read_buffer, 1025);
			read_msg = read(reg_cli_sockfd, read_buffer, 1024);
			
			if(read_msg > 0) {
				fprintf(stdout, "\nMessage from server: %s\n", read_buffer);
			}
			else {
				fprintf(stderr, "\nERROR in receiving registration ACK from the server.\n");
				exit(1);
			}
		}
		else if (choice == 2) {
			initialize_client_for_login();

			bzero(read_buffer, 1025);
			read_msg = read(login_cli_sockfd, read_buffer, 1024);

			if(read_msg > 0) {
				if(strcmp(read_buffer, "-1") == 0) {
					fprintf(stdout, "\nERROR: server too busy. Please try again after sometime.\n");
					continue;
				}
				else {
					fprintf(stdout, "\nMessage from server: %s\n", read_buffer);
				}
			}
			else {
				fprintf(stderr, "\nERROR in receiving login ACK from the server.\n");
				exit(1);
			}

			char username[41];
			char password[41];
			
			fprintf(stdout, "\n--------------Log into Chat Portal--------------\n\n");
			fprintf(stdout, "Enter your username: ");
			fscanf(stdin, "%s", username);
			fprintf(stdout, "Enter your password: ");
			fscanf(stdin, "%s", password);

			// sending login credentials to the server in 2:username:password:end format
			bzero(write_buffer, 1025);
			strcpy(write_buffer, "2:");
			strcat(write_buffer, username);
			strcat(write_buffer, ":");
			strcat(write_buffer, password);
			strcat(write_buffer, ":end\0");

			write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

			if(write_msg < 0) {
				fprintf(stderr, "\nERROR in sending login credentials to the server.\n");
				exit(1);
			}
			else {
				fprintf(stdout, "\nLogin credentials sent to the server.\n");
			}

			//waiting for ACK from the server for login
			bzero(read_buffer, 1025);
			read_msg = read(login_cli_sockfd, read_buffer, 1024);

			if(read_msg > 0) {
				// successfully logged in
				if(strcmp(read_buffer, "1") == 0) {
					fprintf(stdout, "\n----------Welcome to the Chat Portal----------\n\n");
					fprintf(stdout, "Hello!!! %s\n\n", username);	

					logout = 0;

					while(!logout) {
						char command[21];

						fprintf(stdout, ">> ");
						fflush(stdout);

						// clear the socket set
						FD_ZERO(&readfd);
						// add the registration and login socket to the set
						FD_SET(login_cli_sockfd, &readfd);
						FD_SET(STDIN_FILENO, &readfd);

						max_sd = MAX(login_cli_sockfd, STDIN_FILENO);
						
						select(max_sd+1, &readfd, NULL, NULL, NULL);
						
						// there is a message to be read
						if(FD_ISSET(login_cli_sockfd, &readfd)) {
							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg > 0) {
								char * id;
								char delim[2] = ":";

								id = strtok(read_buffer, delim);

								// id = 1 means the message is a personal chat message
								if(strcmp(id, "1") == 0) {
									char * from = strtok(NULL, delim);
									char * msg = strtok(NULL, delim);

									fprintf(stdout, "\n\nReceived a message from: %s.\n", from);
									fprintf(stdout, "Message: %s\n\n", msg);
								}
								// id = 2 means message is a group chat message
								else if(strcmp(id, "2") == 0) {
									char * from = strtok(NULL, delim);
									char * by = strtok(NULL, delim);
									char * msg = strtok(NULL, delim);

									fprintf(stdout, "\n\nReceived a message from the group '%s' by %s.\n", from, by);
									fprintf(stdout, "Message: %s\n\n", msg);
								}
								continue;
							}
						}
						// there is an input command to be read
						if(FD_ISSET(STDIN_FILENO, &readfd)) {
							bzero(command, 21);
							read(STDIN_FILENO, command, 20);
							command[strlen(command)-1] = '\0';
						}
						
						bzero(write_buffer, 1025);

						// send the command to the server for processing
						// send the 'logout' command
						if(strcmp(command, "logout") == 0 || strcmp(command, "/logout") == 0) {
							logout = 1;
						}
						// send the 'who' command
						else if(strcmp(command, "who") == 0 || strcmp(command, "/who") == 0) {
							// 'who' command has id = 4
							strcpy(write_buffer, "4:end");
							strcat(write_buffer, "\0");

							write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "\nERROR in sending 'who' command to the server.\n");
								exit(1);
							}

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'who' command by the server.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "\n%s\n", read_buffer);
							}
						}
						// send the 'msg' command
						else if(strcmp(command, "msg") == 0 || strcmp(command, "/msg") == 0) {
							char to[41], msg[921];
							int ind = 0;

							fprintf(stdout, "\nWhom do you want to message: ");
							scanf("%s", to);
							fprintf(stdout, "What is your message: ");
							fflush(stdout);

							int flag = 0;

							// getting message from user (can take spaces also)
							// going via this approach because scanf doesn't take spaces
							while(1) {
								char ch;
								scanf("%c", &ch);

								if(ch == '\n' && !flag) {
									flag = 1;
								}
								else if(ch == '\n' && flag) {
									msg[ind] = '\0';
									break;
								}
								else {
									msg[ind] = ch;
									ind++;
								}
							}

							// 'msg' command has id = 5
							// 'msg' is sent in the format 5:from:to:msg:end
							strcpy(write_buffer, "5:");
							strcat(write_buffer, username);
							strcat(write_buffer, ":");
							strcat(write_buffer, to);
							strcat(write_buffer, ":");
							strcat(write_buffer, msg);
							strcat(write_buffer, ":end\0");

							write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "\nERROR in sending 'msg' command to the server.\n");
								exit(1);
							}

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'msg' command by the server.\n");
								exit(1);
							}
							else {
								fprintf(stdout, "\n%s\n\n", read_buffer);
							}
						}
						// send the 'create_grp' command
						else if(strcmp(command, "create_grp") == 0 || strcmp(command, "/create_grp") == 0) {
							char grp_name[41];

							fprintf(stdout, "\nEnter your desired group name: ");
							fscanf(stdin, "%s", grp_name);
							// 'create_grp' command has id = 6
							strcpy(write_buffer, "6:");
							strcat(write_buffer, grp_name);
							strcat(write_buffer, ":");
							strcat(write_buffer, username);
							strcat(write_buffer, ":end\0");

							write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "\nERROR in sending 'create_grp' command to the server.\n");
								exit(1);
							}

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'create_grp' command by the server.\n");
								exit(1);
							}
							else {
								if(strcmp(read_buffer, "1") == 0) {
									fprintf(stdout, "\nGroup created successfully\n\n");
								}
								else {
									fprintf(stdout, "\nAnother group with same name already exists. Please enter a different name.\n\n");
								}
							}
						}
						// send the 'join_grp' command
						else if(strcmp(command, "join_grp") == 0 || strcmp(command, "/join_grp") == 0) {
							char grp_name[41];

							fprintf(stdout, "\nEnter the name of the group you want to join: ");
							fscanf(stdin, "%s", grp_name);
							// 'join_grp' command has id = 7
							strcpy(write_buffer, "7:");
							strcat(write_buffer, grp_name);
							strcat(write_buffer, ":");
							strcat(write_buffer, username);
							strcat(write_buffer, ":end\0");

							write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "\nERROR in sending 'join_grp' command to the server.\n");
								exit(1);
							}

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'join_grp' command by the server.\n");
								exit(1);
							}
							else {
								if(strcmp(read_buffer, "1") == 0) {
									fprintf(stdout, "\nGroup joined successfully.\n\n");
								}
								else if(strcmp(read_buffer, "0") == 0) {
									fprintf(stdout, "\nYou are already a member of the group.\n\n");
								}
								else {
									fprintf(stdout, "\nNo group with the desired name exists. Please enter a valid group name.\n\n");
								}
							}
						}
						// send the 'msg_group' command
						else if(strcmp(command, "msg_group") == 0 || strcmp(command, "/msg_group") == 0) {
							char grp_name[41];
							char msg[921];
							int ind = 0;
							int flag = 0;

							fprintf(stdout, "\nEnter the name of the group: ");
							fscanf(stdin, "%s", grp_name);
							fprintf(stdout, "\nEnter the message: ");

							// getting message from user (can take spaces also)
							// going via this approach because scanf doesn't take spaces
							while(1) {
								char ch;
								scanf("%c", &ch);

								if(ch == '\n' && !flag) {
									flag = 1;
								}
								else if(ch == '\n' && flag) {
									msg[ind] = '\0';
									break;
								}
								else {
									msg[ind] = ch;
									ind++;
								}
							}

							// 'msg_group' command has id = 8
							strcpy(write_buffer, "8:");
							strcat(write_buffer, grp_name);
							strcat(write_buffer, ":");
							strcat(write_buffer, username);
							strcat(write_buffer, ":");
							strcat(write_buffer, msg);
							strcat(write_buffer, ":end\0");

							write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "\nERROR in sending 'msg_group' command to the server.\n");
								exit(1);
							}

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'msg_group' command by the server.\n");
								exit(1);
							}
							else {
								if(strcmp(read_buffer, "1") == 0) {
									fprintf(stdout, "\nMessage broadcasted to the group successfully.\n\n");
								}
								else if(strcmp(read_buffer, "0") == 0) {
									fprintf(stdout, "\nYou cannot send messages to the group because you are not a member of the group.\n\n");
								}
								else {
									fprintf(stdout, "\nNo group with the given name exists. Please enter a valid group name.\n\n");
								}
							}
						}
						// send the 'send' command
						else if(strcmp(command, "send") == 0 || strcmp(command, "/send") == 0) {
							// to store the complete path of the file
							char complete_path[2049];

							bzero(send_file_path, 1025);
							bzero(send_file_name, 1025);
							char recv_username[41];

							fprintf(stdout, "\nEnter the receiver's username: ");
							fscanf(stdin, "%s", recv_username);
							fprintf(stdout, "\nEnter the name of the file to be sent: ");
							fscanf(stdin, "%s", send_file_name);
							fprintf(stdout, "\nEnter absolute path of the directory in which the file is present: ");
							fscanf(stdin, "%s", send_file_path);

							// compute the complete path of the file
							strcpy(complete_path, send_file_path);
							strcat(complete_path, "/");
							strcat(complete_path, send_file_name);

							send_file_desc = open(complete_path, O_RDONLY);
							
							if(send_file_desc == -1) {
								fprintf(stderr, "\nInvalid file path %s. Please enter a valid file path.\n\n", complete_path);
								continue;
							}

							// get statistics of the file
							if(fstat(send_file_desc, &file_stat) < 0) {
								fprintf(stderr, "\nError: Unable to get the file statistics.\n");
								close(send_file_desc);
								exit(1);
							}

							// discard file sending if file size is greater than 1MB (1000000 bytes)
							if(file_stat.st_size > 1000000) {
								fprintf(stderr, "\nFile is too large. Please select a file of size less than 1 MB.\n\n");
								close(send_file_desc);
								continue;
							}

							// 'send' command has id = 9
							strcpy(write_buffer, "9:");
							strcat(write_buffer, recv_username);
							strcat(write_buffer, ":");
							strcat(write_buffer, username);
							strcat(write_buffer, ":");
							strcat(write_buffer, int_to_str(file_stat.st_size));
							strcat(write_buffer, ":");
							strcat(write_buffer, send_file_name);
							strcat(write_buffer, ":end\0");

							// first send sender's and receiver's credentials to the server
							write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

							if(write_msg < 0) {
								fprintf(stderr, "\nERROR in sending 'send' command to the server.\n");
								close(send_file_desc);
								exit(1);
							}

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'send' command by the server.\n");
								close(send_file_desc);
								exit(1);
							}
							else {
								if(strcmp(read_buffer, "-1") == 0) {
									fprintf(stderr, "\nError: The receiving user %s is not online. Pleae try again after sometime.\n\n", recv_username);
									close(send_file_desc);
									continue;
								}
								else if(strcmp(read_buffer, "-2") == 0) {
									fprintf(stderr, "\nError: Receiver already has a file pending to be received. Please send after sometime.\n\n");
									close(send_file_desc);
									continue;
								}
							}

							send_offset = 0;
        					send_remain_data = file_stat.st_size;
        					
							// sending file data to the server
							while (((bytes_sent = sendfile(login_cli_sockfd, send_file_desc, &send_offset, 1024)) > 0) && (send_remain_data > 0)) {
								send_remain_data -= bytes_sent;
							}

							// closing the file descriptor
							close(send_file_desc);

							bzero(read_buffer, 1025);
							read_msg = read(login_cli_sockfd, read_buffer, 1024);

							if(read_msg < 0) {
								fprintf(stderr, "\nERROR in processing the 'send' command by the server.\n");
								exit(1);
							}
							else {
								fprintf(stderr, "\nMessage from server: %s.\n\n", read_buffer);
							}
						}
						// send the 'recv' command
						else if(strcmp(command, "recv") == 0 || strcmp(command, "/recv") == 0) {
							char file_path[50], sender_uname[41], file_name[1025], receive[2], ch;	

							strcpy(file_path, "file/");
							strcat(file_path, username);
							strcat(file_path, "\0");

							// check if there is a file to be received
							FILE * rfp = fopen(file_path, "r");

							// there is no file to receive
							if(rfp == NULL) {
								fprintf(stderr, "\nThere is no file to receive.\n\n");
								continue;
							}

							// there is a file to be received
							fscanf(rfp, "%s\n", sender_uname);
							fscanf(rfp, "%s\n", file_name);

							fprintf(stdout, "\n%s sent you the file '%s'\n\n", sender_uname, file_name);
							fprintf(stdout, "Do you want to receive it (Y/N)? ");
							fscanf(stdin, "%s", receive);
							fprintf(stdout, "\n");

							if(strcmp(receive, "n") == 0 || strcmp(receive, "N") == 0) {
								fclose(rfp);
								// delete the file because receiver doesn't want to receive the file
								remove(file_path);
								continue;
							}

							fprintf(stdout, "Enter the absolute path of the directory where you want to save the file: ");
							fscanf(stdin, "%s", recv_file_path);

							strcat(recv_file_path, "/");
							strcat(recv_file_path, file_name);
							strcat(recv_file_path, "\0");

							// file descriptor for saving the received file
							FILE * wfp = fopen(recv_file_path, "w");

							if(wfp == NULL) {
								fprintf(stderr, "\nInvalid file path %s. Please enter a valid file path.\n\n", recv_file_path);
								continue;
							}

							while(1) {
								ch = fgetc(rfp);
								
								if(ch == EOF) {
									break;
								}

								fputc(ch, wfp);
							}

							fclose(rfp);
							fclose(wfp);

							remove(file_path);

							fprintf(stdout, "\nFile received successfully.\n\n");
						}
						else {
							fprintf(stderr, "\nPlease enter a valid command\n\n");
						}
					}

					if(logout) {
						// send message to server to close client's connection
						// logout command has id = 3
						bzero(write_buffer, 1025);
						strcpy(write_buffer, "3:");
						strcat(write_buffer, username);
						strcat(write_buffer, ":");
						strcat(write_buffer, "end\0");

						write_msg = write(login_cli_sockfd, write_buffer, strlen(write_buffer));

						if(write_msg < 0) {
							fprintf(stderr, "\nERROR in logging out user.\n");
							exit(1);
						}

						bzero(read_buffer, 1025);
						read_msg = read(login_cli_sockfd, read_buffer, 1024);

						if(read_msg > 0) {
							fprintf(stdout, "\nMessage from server: %s\n", read_buffer);
						}
						else {
							fprintf(stderr, "\nERROR in receiving logout ACK from the server.\n");
							exit(1);
						}
			
						break;
					}
				}
				// error in logging in
				else {
					fprintf(stderr, "\nInvalid login credentials. Please provide correct credentials. Please register first if you are not already registered.\n");
				}
			}
			else {
				fprintf(stderr, "\nERROR in receiving login ACK from the server.\n");
				exit(1);
			}
		}
		else if (choice == 3) {
			break;
		}
		else {
			fprintf(stdout, "\nInvalid choice. Please try again.\n");
		}

	} while(choice != 3);

	return(0);
}