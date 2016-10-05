/*
	For handling of group related commands
*/

// function to create a group
int create_grp(char *, char *);
// function to join a group
int join_grp(char *, char *);
// function to perform sanity checks before sending message to the group
// for ex: the person should be a member of the group to which he is sending the message
int msg_group_sanity_checks(char *, char *);