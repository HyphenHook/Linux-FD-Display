#include "display.h"
/*Prints the per-process table*/
void print_fd_table(Process_Node* head, int selected){
    printf("         PID     FD \n");
    printf("        ============\n");
    Process_Node* temp = head;
    int i = 0;
    while(temp != NULL){
        if(selected && head -> pid != temp -> pid) break;
        if(!selected) printf("%-5d    ", i++);
        else printf("         ");
        printf("%-6d  %-4d\n", temp -> pid, temp -> fd);
        temp = temp -> next;
    }
    printf("        ============\n\n");
}

/*Prints the system wide table*/
void print_sys_wide(Process_Node* head, int selected){
    printf("         PID     FD      Filename \n");
    printf("        ========================================\n");
    Process_Node* temp = head;
    int i = 0;
    while(temp != NULL){
        if(selected && head -> pid != temp -> pid) break;
        if(!selected) printf("%-5d    ", i++);
        else printf("         ");
        printf("%-6d  %-4d    %s\n", temp -> pid, temp -> fd, temp -> link_name);
        temp = temp -> next;
    }
    printf("        ========================================\n\n");
}

/*Prints the vnode table*/
void print_vnode(Process_Node* head, int selected){
    if(selected) printf("           FD            Inode\n");
    else printf("           Inode            \n");
    printf("        ========================================\n");
    int i = 0;
    Process_Node* temp = head;
    while(temp != NULL){
        if(selected && head -> pid != temp -> pid) break;
        if(!selected) printf("%-5d      ", i++);
        if(selected) printf("           %-4d          %ld\n", temp -> fd, (long) temp -> inode);
        else printf("%-4ld\n", (long) temp -> inode);
        temp = temp -> next;
    }
    printf("        ========================================\n\n");
}

/*Prints the composite table*/
void print_composite(Process_Node* head, int selected){
    printf("         PID     FD       Filename                Inode\n");
    printf("        ===============================================\n");
    Process_Node* temp = head;
    int i = 0;
    while(temp != NULL){
        if(selected && head -> pid != temp -> pid) break;
        if(!selected) printf("%-5d    ", i++);
        else printf("         ");
        printf("%-6d  %-4d     %-23s %-10ld\n", temp -> pid, temp -> fd, temp -> link_name, (long) temp -> inode);
        temp = temp -> next;
    }
    printf("        ===============================================\n\n");
}

/*Prints all the offending processes with number of FD assigned higher than threshold*/
void print_threshold_flag(Process_Node* head, int threshold){
    printf("## Offending processes:\n");
    Process_Node* temp = head;
    int count = 1;
    while(temp -> next != NULL){
        if(temp -> next -> pid != temp -> pid && count > threshold) printf("%d(%d), ", temp -> pid, count);
        if(temp -> next -> pid != temp -> pid) count = 1;
        else count++;
        temp = temp -> next;
    }
    if(count > threshold) printf("%d(%d)\n", temp -> pid, count);
    else printf("\n");
}

/*Writes the composite table to compositeTable.txt (text file)*/
void write_composite(Process_Node* head, int selected){
    FILE *fp = fopen("compositeTable.txt", "w");
    if(fp == NULL) {
        printf("Text File Failed to Save! Please check location of this program!\n");
        return;
    }
    if(selected) fprintf(fp, ">>> Target PID: %d\n", head -> pid);
    fprintf(fp, "         PID     FD       Filename                Inode\n");
    fprintf(fp, "        ===============================================\n");
    int i = 0;
    Process_Node* temp = head;
    while(temp != NULL){
        if(selected && head -> pid != temp -> pid) break;
        if(!selected) fprintf(fp, "%-5d    ", i++);
        else fprintf(fp, "         ");
        fprintf(fp, "%-6d  %-4d     %-23s %-10ld\n", temp -> pid, temp -> fd, temp -> link_name, (long) temp -> inode);
        temp = temp -> next;
    }
    fprintf(fp, "        ===============================================\n\n");
    fclose(fp);
    printf("Composite Table Text File Saved: compositeTable.txt\n");
}

/*Writes the composite table to compositeTable.bin (binary file)*/
void write_binary_composite(Process_Node* head, int selected){
    FILE *fp = fopen("compositeTable.bin", "wb");
    if(fp == NULL) {
        printf("Binary File Failed to Save! Please check location of this program!\n");
        return;
    }
    char strOut[2*MAX_SIZE];
    int len;
    if(selected){
        len = sprintf(strOut, ">>> Target PID: ");
        fwrite(strOut, sizeof(char), len, fp);
        fwrite(&head -> pid, sizeof(int), 1, fp);
        len = sprintf(strOut, "\n");
        fwrite(strOut, sizeof(char), 1, fp);
    }

    len = sprintf(strOut, "         PID     FD       Filename                Inode\n");
    fwrite(strOut, sizeof(char), len, fp);

    len = sprintf(strOut, "        ===============================================\n");
    fwrite(strOut, sizeof(char), len, fp);

    Process_Node* temp = head;
    int i = 0;
    while(temp != NULL){
        if(selected && head -> pid != temp -> pid) break;
        if(!selected){
            len = sprintf(strOut, "    ");
            fwrite(&i, sizeof(int), 1, fp);
            fwrite(strOut, sizeof(char), len, fp);
            i++;
        }
        else{
            len = sprintf(strOut, "         ");
            fwrite(strOut, sizeof(char), len, fp);
        }
        // sprintf(strOut, "%-6d  %-4d     %-23s %-10ld\n", temp -> pid, temp -> fd, temp -> link_name, (long) temp -> inode);
        fwrite(&temp -> pid, sizeof(int), 1, fp);
        len = sprintf(strOut, "  ");
        fwrite(strOut, sizeof(char), len, fp);
        fwrite(&temp -> fd, sizeof(int), 1, fp);
        len = sprintf(strOut, "     ");
        fwrite(strOut, sizeof(char), len, fp);
        fwrite(&temp -> link_name, sizeof(char), strlen(temp -> link_name), fp);
        len = sprintf(strOut, " ");
        fwrite(strOut, sizeof(char), len, fp);
        fwrite(&temp -> inode, sizeof(long), 1, fp);
        temp = temp -> next;
    }
    len = sprintf(strOut, "        ===============================================\n\n");
    fwrite(strOut, sizeof(char), len, fp);
    fclose(fp);
    printf("Composite Table Binary File Saved: compositeTable.bin\n");
}
