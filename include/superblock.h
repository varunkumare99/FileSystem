// meta information about file system
// number of inodes
// number of disk blocks
// size of disk blocks
struct superblock {
    int num_inodes;
    int num_blocks;
    int size_blocks;
};
