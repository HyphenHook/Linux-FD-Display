# Recreating the System-Wide FD Tables 
# An assignment for CSCB09
Quick Information
===================================
This application is developed in C and is targetted only towards Linux systems.
This has only been tested on an Ubuntu machine.
It reports the FD specific information regarding all user owned processes (or can be just one depending on the command-line arguments)

Program Usage
===================================
1) If there is no compiled file (no "main" file in this directory) then please open terminal and navigate to the directory where you found this README file and execute in the terminal
> make  

>> **Use "make clean" if you wish to clean .o files and the main program**  
>> **Use "make r-all" if you wish to clean and also recompile**  

2) To run the program navigate to the directory where you found this README file and execute
> ./main \<OPTIONAL FLAGS\>
>
>  \<OPTIONAL FLAGS\>:
>- --per-process   |prints the process FD table
>- --systemWide    |prints the system-wide FD table
>- --Vnodes        |prints the Vnodes table
>- --composite     |prints the composed table of the above 3 flags
>- --threshold=X   |prints all offending processes owned by the user whos' number of assigned FD is larger than X
>- --output_TXT    |saves the composite table in text form into a file named: compositeTable.txt
>- --output_binary |saves the composite table in binary form into a file name: compositeTable.bin  

>> You can specify a specific user-owned process' by including the process ID in the arguments to display information only about that process. In addition, if multiple process ID are given then the process ID near the end of the line will be used.  
>> Ex. Suppose we only want to look at process with the ID of 1943 then we will do:  
>> ./main 1943
>> Ex. Suppose we perform ./main 1943 1999 then the program will search for 1999 and ignore 1943

>> Any processes that aren't owned by the operating user or it does not exist in the system will cause the program to terminate.

>> Without any optional flags regarding which tables to print (per-process, systemWide, Vnodes, composite), the program will default to printing the composite table of all user owned' processes or process specified

>> All information obtained are only from processes owned by the operating user. As such it will not display processes that are owned by other users.  
>> The program will default to printing the FD tables of all processes owned by the operating user unless the operating user specifies a user owned process identifier

Function Overview
===================================

## pid_node.h  
-----------------------------------
This is a header file for defining a linked list implementation for storing information related to user process.  
**struct process_node**
- This node contains the following attribute:
    - pid = the pid ID of the process
    - fd = the current FD number of the process we are at
    - inode = the inode the this FD is pointing to
    - link_name[MAX_SIZE] = the FD file's symoblic link name with the MAX_SIZE = 2048
    - next = the next process/fd pair on the list
    - tail = the tail pointer for easy linked list insert. This element is not NULL for only the head of the linked list.

Although this is named process_node, it is more like a FD and Process pair since each node consists of one entry of the fd info

## pid_node.c
-----------------------------------
This is the implementation file that creates the process_nodes.  

**int check_valid_process(char\* fname)**
- This function checks whether a given file name (from "/proc" folder) is a process/fd file by checking if the file name consist of only numbers.
- It checks by using a for loop to check whether fname consist of only numbers.
- Parameters:
    - fname = the name of the file we are checking
- Returns 1 if the file/folder is a valid fd or process and 0 otherwise.  

**int is_owner(pid_t pid)**
- This function checks whether a given process id is owned by the current operating user.
- It checks this information by using \<pwd.h\> for its passwd struct to get the current operating user's UID and also using \<sys/stat.h\> for its stat struct to get the "/proc/{pid}/" owner UID and compares the two to see if they match
- Parameters:
    - pid = the process ID
- Returns 1 if the process is owned by the operating user and 0 otherwise.

**Process_Node\* new_process_node()**
- This function allocates memory (by calling malloc) for a new process_node.
- Returns the pointer for the newly allocated process_node

**Process_Node\* delete_head(Process_Node\* root)**
- This function deletes the head of the given linked list root
- Parameters:
    - root = the head of the linked list
- Returns the pointer for the new head after deletion

**Process_Node\* insert(Process_Node\* root, Process_Node\* connective)**
- This function inserts the process_node connective at the end of the linked list root
- It simply appends the connective node to the linked list
- Parameters:
    - root = the head of the linked list
    - connective = the node to be inserted at the back of the linked list
- Returns the pointer for the linked list's head

**void set_inode(Process_Node\* node)**
- This function uses the process_node's information regarding its PID and FD to set the inode attribute of the given node
- It acquires the inode information by using \<sys/stat.h\> for its stat struct to get information from "/proc/{pid}/fd/{fd_num}" and sets the inode information acquired from the stat struct into the node's inode attribute
- Parameters:
    - node = the given node that already has its process ID and FD number set

**void set_name(Process_Node\* node)**
- This function uses the readlink() function to get the symbolic link of the fd file ("/proc/{pid}/fd/{fd_num}") and sets the link info onto the attribute of the given node
- It acquires the link by using readlink() and sets the symbolic link file in the process_node's link_name field.
- Parameters:
    - node = the given node that already has its process ID and FD number set

**Process_Node\* process_fd(Process_Node\* root, pid_t pid)**
- This function goes through all the FD entry in "/proc/{pid}/fd" and inserts a new process_node for each of the FD it finds in the folder
- This function ignores any process' that denies our entry to see the FD files
- For each FD file it finds in "/proc/{pid}/fd" it does the following
    - create a new node by calling the new_process_node() function
    - insert this new node into the root linked list
    - sets the pid information of this new node by using the parameter
    - sets the fd information of this new node by taking the fd filename
    - calls set_name to set the symbolic link information of this new node by using the fd file's symbolic link
    - calls set_inode to set the inode information of this new node by using the inode the fd file is pointing to
- Parameters:
    - root = the head of the linked list
    - pid = the PID of the process whose FD we are examining
- Returns the pointer for the linked list's head

**Process_Node\* fetch_info()**
- This function goes through the /proc file and goes through every process ID and attempts to turn the information into a node for the linked list
- This function does the following:
    - Creates an empty node for the head
    - Go through every file/folder in "/proc"
    - Check the file/folder name to see if its a valid process by calling check_valid_process()
    - If its valid (if invalid we ignore) then we check if we have ownership of this process by passing the pid to is_owner()
    - Then we call process_fd() function with the process id we are currently at and it will begin inserting each fd value for this process as a node into the linked list
- This function is called only once to create the entire linked list for displaying all the information in the program
- Returns the pointer for the linked list's head (it deletes the head and sets the next node as head before return since we created an empty node for the head)

**void print_order(Process_Node\* root)**
- This function goes through the linked list of root and prints all the information for each of the nodes in the linked list
- This function serves no purpose for the assignment but is for debugging/testing purposes
- Parameters:
    - root = the head of the linked list

**Process_Node\* navigate_to_process(Process_Node\* root, pid_t pid)**
- This function given the PID it will attempt to search through the linked list root and return the found node's pointer
- It basically does a linked list traversal and compares the node's pid with the pid parameter
- Parameters:
    - root = the head of the linked list
    - pid = the PID of the process we are trying to find in the linked list
- Returns the beginning pointer of the process_node that has the pid it returns NULL if the pid is not found in the list

**Process_Node\* clearAll(Process_Node\* root)**
- This function basically deletes the entire linked list given the head
- Parameters:
    - root = the head of the linked list
- Returns NULL cause we deleted it :)

## display.c
-----------------------------------
The main implementation for displaying all the FD table information

**void print_fd_table(Process_Node\* head, int selected)**
- This function prints the per-process fd table given the head of the linked list
- For printing a specific process, simply call and store the resultant pointer from navigate_to_process() function to navigate to that process and pass the pointer and selected = 1 to this function
- Parameters:
    - head = the head of the linked list
    - selected = 0 if a PID was not provided as the program argument and 1 if a PID was specified

**void print_sys_wide(Process_Node\* head, int selected)**
- This function prints the system wide fd table given the head of the linked list
- For printing a specific process, simply call and store the resultant pointer from navigate_to_process() function to navigate to that process and pass the pointer and selected = 1 to this function
- Parameters:
    - head = the head of the linked list
    - selected = 0 if a PID was not provided as the program argument and 1 if a PID was specified

**void print_vnode(Process_Node\* head, int selected)**
- This function prints the vnode table given the head of the linked list
- For printing a specific process, simply call and store the resultant pointer from navigate_to_process() function to navigate to that process and pass the pointer and selected = 1 to this function
- This function will print the FDs of a process if selected is true (indicating we are looking at one process)
- Parameters:
    - head = the head of the linked list
    - selected = 0 if a PID was not provided as the program argument and 1 if a PID was specified

**void print_composite(Process_Node\* head, int selected)**
- This function prints the composite table given the head of the linked list
- For printing a specific process, simply call and store the resultant pointer from navigate_to_process() function to navigate to that process and pass the pointer and selected = 1 to this function
- Parameters:
    - head = the head of the linked list
    - selected = 0 if a PID was not provided as the program argument and 1 if a PID was specified

**void print_threshold_flag(Process_Node\* head, int threshold)**
- This function prints all offending processes that have a number of fd assigned higher than the threshold in the entire linked list
- Prints the information by {pid} (number of fd assigned)
- Parameters:
    - head = the head of the linked list

**void write_composite(Process_Node\* head, int selected)**
- This function writes the composite table information in text/ASCII form to a file named compositeTable.txt
- For printing a specific process, simply call and store the resultant pointer from navigate_to_process() function to navigate to that process and pass the pointer and selected = 1 to this function
- Parameters:
    - head = the head of the linked list
    - selected = 0 if a PID was not provided as the program argument and 1 if a PID was specified

**void write_binary_composite(Process_Node\* head, int selected)**
- This function writes the composite table information in binary form to a file named compositeTable.bin
- For printing a specific process, simply call and store the resultant pointer from navigate_to_process() function to navigate to that process and pass the pointer and selected = 1 to this function
- Parameters:
    - head = the head of the linked list
    - selected = 0 if a PID was not provided as the program argument and 1 if a PID was specified
- NOTE: This writes every numerical value in binary

## centre.c
-----------------------------------
This is the implementation file that handles what to print.

**void display_centre(pid_t pid, int perprocess, int sysWide, int vnode, int composite, int threshold, int no_arg, int binary, int txt)**
- This function receives the argument flags from a2.c and determines what needs to be printed
- When specified a pid, it will call the navigate_to_process() function to get the pointer to the node with the PID in the linked list. If NULL was returned that means the process doesn't exist or it isn't owned by us
- Parameters:
    - pid = the PID we are trying to get the table for (defaults to 0 if no process was specified)
    - perprocess = whether we want perprocess table or not
    - sysWide = whether we want systemWide table or not
    - vnode = whether we want vnode table or not
    - no_arg = determine whether a table flag was provided when using this program, it uses this to print composite if no flag was provided
    - binary = determine whether we wish to save a binary version of composite table or not
    - txt = determine whether we wish to save a text version of composite table or not

## a2.c
-----------------------------------
This is the file that handles argument parsing + reading argument

**int main(int argc, char\*\* argv)**
- This function receives argument flags via argv and uses the getopt_long() function to parse arguments
    - The arguments are returned as following:
    - "--per-process" returns a "p" from getopt_long() and will assign the variable perprocess as 1 to indicate we want a per-process table
    - "--systemWide" returns a "s" from getopt_long() and will assign the variable sysWide as 1 to indicate we want a system wide table
    - "--Vnodes" returns a "v" from getopt_long() and will assign the variable vnode as 1 to indicate we want a Vnodes table
    - "--composite" returns a "c" from getopt_long() and will assign the variable composite as 1 to indicate we want a composite table
    - "--threshold" returns a "t" from getopt_long() and will assign the value of the threshold from getopt_long() to indicate we want know the offending process
    - "--output_binary" returns a "b" from getopt_long() and will assign the variable binary as 1 to indicate we want a binary composite table saved
    - "--output_TXT" returns a "a" from getopt_long() and will assign the variable txt as 1 to indicate we want a text composite table saved
    - The function will check every argument to verify if its numerical (indicating a PID) before passing the argument to getopt_long()
    - If any invalid argument is provided or PID provided is invalid or threshold value is less than 0 then the function will assign the variable valid as 0 and won't proceed to displaying any information
- Parameters:
    - argc = number of argument/flags were provided when running this program
    - argv = the array of arguments/flags
