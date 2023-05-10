#include "centre.h"
/*Responsible for printing/displaying information based on the given flags*/
void display_centre(pid_t pid, int perprocess, int sysWide, int vnode, int composite, int threshold, int no_arg, int binary, int txt){
    Process_Node* root = fetch_info();
    Process_Node* head = root;
    int selected = 0;
    if(pid != 0){
        head = navigate_to_process(root, pid);
        selected = 1;
        if(head == NULL) {
            printf("The Process: %d does not exist or does not owned by you!\n", pid);
            clearAll(root);
            return;
        }
    }
    if(selected) printf(">>> Target PID: %d\n", head -> pid);
    if(perprocess) print_fd_table(head, selected);
    if(sysWide) print_sys_wide(head, selected);
    if(vnode) print_vnode(head, selected);
    if(composite || no_arg) print_composite(head, selected);
    if(threshold > 0) print_threshold_flag(root, threshold);
    if(binary) write_binary_composite(head, selected);
    if(txt) write_composite(head, selected);
    clearAll(root);
}