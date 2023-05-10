#include "pid_node.h"

/*Checks if the given file name is a valid process or FD file*/
int check_valid_process(char* fname){
    for(int i = 0; fname[i] != '\0'; i++){
        if(fname[i] < '0' || fname[i] > '9') return 0;
    }
    return 1;
}

/*Checks if the given PID is owned by the current operating user
Returns 1 if the operating user is the owner of the process*/
int is_owner(pid_t pid){
    struct passwd *user = getpwuid(geteuid());
    struct stat result;
    char path[MAX_SIZE];
    sprintf(path, "/proc/%d", pid);
    if ( stat(path, &result) == -1 ) return 0;
    return result.st_uid == user -> pw_uid;
}

/*Creates/allocate memory for a new process node and returns the node created*/
Process_Node* new_process_node(){
    Process_Node* result = (Process_Node*) malloc(sizeof(Process_Node));
    result -> next = NULL;
    result -> tail = NULL;
    return result;
}

/*Deletes the head of the linked list and return the new head*/
Process_Node* delete_head(Process_Node* root){
    if(root -> next == NULL){
        free(root);
        return NULL;
    }
    Process_Node* temp = root -> next;
    temp -> tail = root -> tail;
    free(root);
    return temp;
}

/*Inserts the connective node into the tail of the linked list*/
Process_Node* insert(Process_Node* root, Process_Node* connective){
    if(root -> next == NULL || root -> tail == NULL){
        root -> next = connective;
        root -> tail = connective;
    }
    root -> tail -> next = connective;
    root -> tail = connective;
    return root;
}

/*Sets the inode value for this particular node*/
void set_inode(Process_Node* node){
    char path[MAX_SIZE];
    sprintf(path, "/proc/%d/fd/%d", node -> pid, node -> fd);
    struct stat result;
    if( stat(path, &result) == -1) node -> inode = 0;
    node -> inode = result.st_ino;
}

/*Sets the link name for this particular node*/
void set_name(Process_Node* node){
    char path[MAX_SIZE];
    sprintf(path, "/proc/%d/fd/%d", node -> pid, node -> fd);
    ssize_t len;
    len = readlink(path, node -> link_name, MAX_SIZE - 1);
    node -> link_name[len * sizeof(char)] = '\0';
}

/*Process/add more linked list nodes for this particular process 
and for each FD assigned for this process*/
Process_Node* process_fd(Process_Node* root, pid_t pid){
    char path[MAX_SIZE];
    sprintf(path, "/proc/%d/fd", pid);
    DIR *dp = opendir(path);
    if (dp == NULL) return root;
    struct dirent *rdp;
    while((rdp = readdir(dp))){
        if(check_valid_process(rdp -> d_name)){
            Process_Node* next = new_process_node();
            root = insert(root, next);
            root -> tail -> pid = pid;
            root -> tail -> fd = atoi(rdp -> d_name);
            set_name(root -> tail);
            set_inode(root -> tail);
        }
    }
    closedir(dp);
    return root;
}

/*Creates the entirety of the linekd list by scanning through every process in proc*/
Process_Node* fetch_info(){
    Process_Node* root = new_process_node();
    DIR *dp = opendir("/proc");
    if (dp == NULL) return NULL;
    struct dirent *rdp;
    while((rdp = readdir(dp))){
        if(check_valid_process(rdp -> d_name)){
            pid_t pid = atoi(rdp -> d_name);
            if(is_owner(pid)){
                root = process_fd(root, pid);
            }
        }
    }
    closedir(dp);
    return delete_head(root);
}
/*Prints the linked list from head to tail. Used for debugging*/
void print_order(Process_Node* root){
    Process_Node* temp = root;
    while(temp != NULL){
        printf("PID: %d | FD: %d | iNode: %ld | Name: %s \n", temp -> pid, temp -> fd, temp -> inode, temp -> link_name);
        temp = temp -> next;
    }
}

/*Return the pointer of the process we are attempting to find with PID
returns NULL if not found*/
Process_Node* navigate_to_process(Process_Node* root, pid_t pid){
    Process_Node* head = root;
    while(head != NULL){
        if(head -> pid == pid) return head;
        head = head -> next;
    }
    return NULL;
}

/*Deletes all of the linked list*/
Process_Node* clearAll(Process_Node* root){
    while(root != NULL){
        Process_Node* temp = root -> next;
        free(root);
        root = temp;
    }
    return NULL;
}