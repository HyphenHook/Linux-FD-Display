#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_SIZE 2048
/*
process_node for storing information about fd and process pair
pid = process id
fd = the fd we are examining
inode = the inode this fd is pointing to
link_name = the link name of the fd is pointing to
next = the next node of the linked list
tail = the tail node of the linked list
*/
typedef struct process_node{
    pid_t pid;
    int fd;
    ino_t inode;
    char link_name[MAX_SIZE];
    struct process_node* next;
    struct process_node* tail;
} Process_Node;

Process_Node* fetch_info();
Process_Node* navigate_to_process(Process_Node* root, pid_t pid);
void print_order(Process_Node* root);
Process_Node* clearAll(Process_Node* root);