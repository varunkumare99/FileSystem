#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "superblock.h"
#include "inode.h"
#include "disk_block.h"
#include "fileSystem.h"


static struct superblock sb;
static struct inode *inodes;
static struct disk_block *dbs;

// initialize the file system
void create_fileSystem(){

    // init superblock
    sb.num_inodes = NUM_OF_INODES;
    sb.num_blocks = NUM_OF_BLOCKS;
    sb.size_blocks = sizeof (struct disk_block);

    // init inodes
    if ((inodes = (struct inode *) malloc (sizeof (struct inode) * sb.num_inodes)) == NULL){
        fprintf(stderr, "malloc failure");
        exit (1);
    }
    for (int i=0; i < sb.num_inodes; ++i) {
        inodes[i].size = EMPTY_BLOCK;
        inodes[i].first_block = EMPTY_BLOCK;
        strncpy(inodes[i].name, DEFAULT_NAME, NAME_SIZE);
    }

    // init diskblocks
    if ((dbs = (struct disk_block *) malloc (sizeof (struct disk_block) * sb.num_blocks)) == NULL) {
        fprintf (stderr, "malloc failure");
        exit (1);
    }
    for (int i=0; i < sb.num_blocks; ++i) {
        dbs[i].next_block_num = EMPTY_BLOCK;
    }
}

// load the file system
void mount_fileSystem(){
    FILE *fileSystem;
    if ((fileSystem = fopen("fileSystem_data", "r")) == NULL) {
        fprintf(stderr, "error opening file\n");
        exit(1);
    }

    // read superblock
    fread(&sb, sizeof (struct superblock) , 1, fileSystem);
    if (ferror (fileSystem)) {
        fprintf (stderr, "error reading superblock\n");
        exit (1);
    }


    // read inodes
    if ((inodes = (struct inode *) malloc (sizeof (struct inode) * sb.num_inodes)) == NULL){
        fprintf(stderr, "malloc failure");
        exit (1);
    }
    fread (inodes, sizeof (struct inode) , sb.num_inodes, fileSystem);
    if (ferror (fileSystem)) {
        fprintf (stderr, "error reading superblock\n");
        exit (1);
    }


    // read diskblocks
    if ((dbs = (struct disk_block *) malloc (sizeof (struct disk_block) * sb.num_blocks)) == NULL) {
        fprintf (stderr, "malloc failure");
        exit (1);
    }
    fread (dbs, sizeof (struct disk_block) , sb.num_blocks, fileSystem);
    if (ferror (fileSystem)) {
        fprintf (stderr, "error reading superblock\n");
        exit (1);
    }


    fclose (fileSystem);
}

// write the file system
void sync_fileSystem() {
    FILE *fileSystem;
    if ((fileSystem = fopen("fileSystem_data", "w+")) == NULL) {
        fprintf (stderr, "error opening file\n");
        exit (1);
    }

    // write superblock
    fwrite(&sb, sizeof (struct superblock) , 1, fileSystem);
    if (ferror (fileSystem)) {
        fprintf (stderr, "error writing superblock\n");
        exit (1);
    }

    // write inodes
    fwrite (inodes, sizeof (struct inode) , sb.num_inodes, fileSystem);
    if (ferror (fileSystem)) {
        fprintf (stderr, "error writing inodes\n");
        exit (1);
    }
    // write diskblocks
    fwrite (dbs, sizeof (struct disk_block) , sb.num_blocks, fileSystem);
    if (ferror (fileSystem)) {
        fprintf (stderr, "error writing disk_blocks\n");
        exit (1);
    }

    fclose (fileSystem);
}

// print info about fileSystem
void print_fileSystem(){
    printf ("Superblock info\n");
    printf ("\tnum inodes : %d\n", sb.num_inodes);
    printf ("\tnum blocks : %d\n", sb.num_blocks);
    printf ("\tsize blocks %d\n", sb.size_blocks);

    printf ("inodes\n");
    for (int i=0; i < sb.num_inodes; ++i) {
        printf ("\tsize: %d block: %d name: %s\n", inodes[i].size, inodes[i].first_block, inodes[i].name);
    }

    printf ("disk blocks\n");
    for (int i=0; i < sb.num_blocks; ++i) {
        printf ("\tblock num: %d next block %d\n", i, dbs[i].next_block_num);
    }
}

int find_empty_inode() {
    for (int i=0; i < sb.num_inodes; ++i) {
        if (inodes[i].first_block == EMPTY_BLOCK) {
            return i;
        }
    }
    return -1;
}

int find_empty_disk_block() {
    for (int i=0; i < sb.num_blocks; ++i) {
        if (dbs[i].next_block_num == EMPTY_BLOCK) {
            return i;
        }
    }
    return -1;
}


/*
 * to allocate a file on the file system :
 * - find an empty inodes
 * - claim the inode
 * - find and claim a disk block
 * - return the file descriptor
 */
int allocate_file(const char *fileName){
    // find an empty inode
    int currInode = find_empty_inode();

    // find / claim a disk block
    int currBlock = find_empty_disk_block();

    // claim inodes and disk block
    inodes[currInode].first_block = currBlock;
    dbs[currBlock].next_block_num = END_OF_FILE;

    strncpy(inodes[currInode].name, fileName, NAME_SIZE);

    // return the file descriptor
    return currInode;
}

// used when file size is to be reduced
void shorten_file (int block_number) {
    int next_block_num = dbs[block_number].next_block_num;
    if (next_block_num >= 0) {
        shorten_file (next_block_num);
    }
    dbs[next_block_num].next_block_num = EMPTY_BLOCK;
}

void free_blocks (int block_number) {
    int next_block_num = dbs[block_number].next_block_num;
    if (next_block_num >= 0 || next_block_num == END_OF_FILE) {
        free_blocks (next_block_num);
    }
    dbs[block_number].next_block_num = EMPTY_BLOCK;
}

// add / delete blocks allocated to file
void set_fileSize (int filenum, int size) {
    int num = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int curr_block_num = inodes[filenum].first_block;
    num--;

    // add blocks if necessary
    while (num > 0) {
        int next_block_num = dbs[curr_block_num].next_block_num;
        if (next_block_num == END_OF_FILE) {
            int empty_block_num = find_empty_disk_block();
            dbs[curr_block_num].next_block_num = empty_block_num;
            dbs[empty_block_num].next_block_num = END_OF_FILE;
        }
        curr_block_num = dbs[curr_block_num].next_block_num;
        num--;
    }

    // shorten file if necessary
    shorten_file (curr_block_num);
    dbs[curr_block_num].next_block_num = END_OF_FILE;
}

void delete_file (int filenum) {
    int first_block = inodes[filenum].first_block;
    free_blocks(first_block);
    inodes[filenum].first_block = EMPTY_BLOCK;
    strncpy(inodes[filenum].name, "emptyfi", 8);
}

int get_block_num (int file, int offset) {
    int blockOffset = offset;
    int block_num = inodes[file].first_block;

    while (blockOffset > 0) {
        block_num = dbs[block_num].next_block_num;
        blockOffset--;
    }
    return block_num;
}

void write_byte (int filenum, int pos, char *data) {
    // calculate which block
    int relative_block_num = pos / BLOCK_SIZE;

    // find the block number
    int block_num = get_block_num (filenum, relative_block_num);
    // calculate the offset in the block
    int offset = pos % BLOCK_SIZE;

    // write the data
    dbs[block_num].data[offset] = *data;
}

void read_byte(int filenum, int pos, char *data) {
    // calculate which block
    int relative_block_num = pos / BLOCK_SIZE;

    // find the block number
    int block_num = get_block_num (filenum, relative_block_num);
    // calculate the offset in the block
    int offset = pos % BLOCK_SIZE;

    // write the data
    *data = dbs[block_num].data[offset];
}




