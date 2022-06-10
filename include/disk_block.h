#define BLOCK_SIZE 512
struct disk_block {
    int next_block_num;
    char data[BLOCK_SIZE];
};
