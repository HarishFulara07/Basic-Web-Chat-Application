#include "../header/common_header.h"
#include "../header/login_header.h"

// function to log client into the chat portal
int login_client(char * username, char * password) {
	FILE * fp = fopen("database/registration_database.txt", "r");

	// if the file doesn't exist, implies, user doesn't exist
	if(fp == NULL) {
		return -1;
	}

	// check if the username and password exist in the database
	while(!feof(fp)) {
		char uname[41];
		char upass[41];

		fscanf(fp, "%s %s", uname, upass);

		// check if the username already exists
		if(strcmp(username, uname) == 0 && strcmp(password, upass) == 0) {
			fclose(fp); 
			return 1;
		}
	}

	fclose(fp);
	// login credentials are invalid or user is not registered
	return -1;
}