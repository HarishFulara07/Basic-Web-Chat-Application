#include "../header/common_header.h"
#include "../header/register_header.h"

// function to register client into the chat portal
int register_client(char * username, char * password) {
	FILE * fp = fopen("database/registration_database.txt", "r");

	// if the file doesn't exist
	if(fp == NULL) {
		// create a new file
		fp = fopen("database/registration_database.txt", "w");
		fclose(fp);
		// open the newly created file in read mode
		fp = fopen("database/registration_database.txt", "r");
	}

	//check if the username already exists
	while(!feof(fp)) {
		char uname[41];
		char upass[41];

		fscanf(fp, "%s %s", uname, upass);

		//check if the username already exists
		if(strcmp(username, uname) == 0) {
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);

	// username doesn't exist in the database, hence, add it in the database

	fp = fopen("database/registration_database.txt", "a");

	fprintf(fp, "%s %s\n", username, password);

	fclose(fp);

	// successfully appended
	return(1);
}

