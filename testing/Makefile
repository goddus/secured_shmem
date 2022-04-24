all: init_list.o free_list.o cleanup_list.o secure_shmem.o leader_test.o follower_test.o init_list cleanup_list leader_test follower_test

init_list.o: ../src/free_list/init_list.c
	gcc -g -c ../src/free_list/init_list.c

free_list.o: ../src/free_list/free_list.c
	gcc -g -c ../src/free_list/free_list.c

cleanup_list.o: ../src/free_list/cleanup_list.c
	gcc -g -c ../src/free_list/cleanup_list.c

secure_shmem.o: ../src/secure_shmem.c
	gcc -g -c ../src/secure_shmem.c

leader_test.o: leader_test.c
	gcc -g -c leader_test.c

follower_test.o: follower_test.c
	gcc -g -c follower_test.c

init_list: init_list.o free_list.o
	gcc -lrt -lm init_list.o free_list.o -o init_list

cleanup_list: cleanup_list.o free_list.o
	gcc -lrt -lm cleanup_list.o free_list.o -o cleanup_list

leader_test: secure_shmem.o free_list.o leader_test.o
	gcc -lrt -lm secure_shmem.o free_list.o leader_test.o -o leader_test

follower_test: secure_shmem.o free_list.o follower_test.o
	gcc -lrt -lm secure_shmem.o free_list.o follower_test.o -o follower_test
