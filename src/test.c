#include <stdio.h>
#include "fileSystem.h"

void init_file_system() {
    create_fileSystem();
    sync_fileSystem();
    print_fileSystem(); 
}

void create_two_files() {
    mount_fileSystem();
    int filenumOne = allocate_file ("first");
    set_fileSize (filenumOne, 7000);
    int filenumTwo = allocate_file ("second");
    set_fileSize (filenumTwo, 3000);

    // write data to file one
    char data = 'x';
    for (int i=0; i < 49; ++i) {
        write_byte (filenumOne, i*100, &data);
    }
    sync_fileSystem();
    print_fileSystem(); 
}

void create_delete_file() {
    mount_fileSystem();
    int filenumOne = allocate_file ("third");
    set_fileSize (filenumOne, 3000);
    int filenumTwo = allocate_file ("fourth");
    set_fileSize (filenumTwo, 7000);

    print_fileSystem();
    delete_file(filenumOne);
    delete_file(filenumTwo);
    sync_fileSystem();
    print_fileSystem(); 
}


int main() {
    //init_file_system();
    //create_two_files();
    create_delete_file();
    return 0;
}


