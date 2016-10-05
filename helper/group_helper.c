#include "../header/common_header.h"
#include "../header/group_header.h"

// function to create a group
int create_grp(char * grp_name, char * username) {
	FILE * fp = fopen("database/grp_database.txt", "r");

	// if the file doesn't exist
	if(fp == NULL) {
		// create a new file
		fp = fopen("database/grp_database.txt", "w");
		fclose(fp);
		// open the newly created file in read mode
		fp = fopen("database/grp_database.txt", "r");
	}

	// check if there is a group with the same name
	while(!feof(fp)) {
		char read_grp_name[41];

		fscanf(fp, "%s", read_grp_name);

		if(strcmp(read_grp_name, grp_name) == 0) {
			fclose(fp);
			return -1;
		}
	}

	// else append the group name
	fp = fopen("database/grp_database.txt", "a");	

	fprintf(fp, "%s\n", grp_name);

	fclose(fp);

	// create a separate file for the group
	// the name of the file is same as the group name
	char file_name[61];
	strcpy(file_name, "database/");
	strcat(file_name, grp_name);
	strcat(file_name, ".txt");

	fp = fopen(file_name, "a");

	fprintf(fp, "%s\n", username);

	fclose(fp);

	return 1;
}

// function to join a group
int join_grp(char * grp_name, char * username) {
	char file_name[61];
	strcpy(file_name, "database/");
	strcat(file_name, grp_name);
	strcat(file_name, ".txt");

	FILE * fp = fopen(file_name, "r");

	// if the file doesn't exist, it means the group doesn't exists
	if(fp == NULL) {
		return -1;
	}
	
	// check if the user is already a member of the group
	while(!feof(fp)) {
		char uname[41];

		fscanf(fp, "%s", uname);

		if(strcmp(uname, username) == 0) {
			return 0;
		}
	}

	// else add user to the group file
	fclose(fp);

	fp = fopen(file_name, "a");

	fprintf(fp, "%s\n", username);

	fclose(fp);

	return 1;
}

// function to perform sanity checks before sending message to the group
int msg_group_sanity_checks(char * grp_name, char * username) {
	char file_name[61];
	strcpy(file_name, "database/");
	strcat(file_name, grp_name);
	strcat(file_name, ".txt");

	FILE * fp = fopen(file_name, "r");

	// if the file doesn't exist, it means the group doesn't exists
	if(fp == NULL) {
		return -1;
	}
	
	// check if the user is a member of the group
	while(!feof(fp)) {
		char uname[41];

		fscanf(fp, "%s", uname);

		if(strcmp(uname, username) == 0) {
			fclose(fp);
			return 1;
		}
	}

	// else it means that group exists but user is not a member of the group
	fclose(fp);

	return 0;
}