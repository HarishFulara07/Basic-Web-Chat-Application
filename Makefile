all:
	gcc helper/server_helper.c helper/register_helper.c helper/login_helper.c helper/group_helper.c main/server.c -o server
	gcc helper/client_helper.c main/client.c -o client
	gcc helper/client_helper.c test/test1.c -lpthread -o reg_limit_test
	gcc helper/client_helper.c test/test2.c -lpthread -o login_limit_test
clean:
	rm -f ./server
	rm -f ./client
	rm -f reg_limit_test
	rm -f login_limit_test