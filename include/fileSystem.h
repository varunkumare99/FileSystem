void create_fileSystem(); // initialize the file system

void mount_fileSystem(); // load the file system

void sync_fileSystem(); // write the file system

void print_fileSystem(); // print info about fileSystem

int allocate_file(const char *fileName); // allocate a file on the file system

void delete_file();

void set_fileSize (int filenum, int size); // add / delete blocks allocated to a file

void write_byte (int filenum, int pos, char *data);

void read_byte(int filenum, int pos, char *data);

#define NUM_OF_INODES 10
#define NUM_OF_BLOCKS 100
#define EMPTY_BLOCK -1
#define END_OF_FILE -2
