#define NAME_SIZE 10 
#define DEFAULT_NAME "emptyFILE"
struct inode {
    int size;
    int first_block;
    char name[NAME_SIZE];
};
