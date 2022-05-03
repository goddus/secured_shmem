all: follower_test.o leader_test.o secure_shmem.o follower_test leader_test

follower_test.o: follower_test.c
	gcc -g -c -lrt follower_test.c

leader_test.o: leader_test.c
	gcc -g -c -lrt leader_test.c

secure_shmem.o: ../src/secure_shmem.c
	gcc -g -c -lrt ../src/secure_shmem.c

follower_test: follower_test.o secure_shmem.o
	gcc -lrt -lm follower_test.o secure_shmem.o -o follower_test

leader_test: leader_test.o secure_shmem.o
	gcc -lrt -lm leader_test.o secure_shmem.o -o leader_test
