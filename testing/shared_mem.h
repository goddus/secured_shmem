#define shared_mem_name "/region4"
#define shared_mem_size 10

struct shared_data {
	volatile int write_guard;
	volatile int read_guard;
	volatile int delete_guard;
	volatile int data[shared_mem_size];
};
