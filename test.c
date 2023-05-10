#include <stdio.h>
#include <unistd.h>
#include "display.h"
void read_binary(){
    FILE *fp = fopen("compositeTable.bin", "rb");
    char hi;
    while(fread(&hi, sizeof(char), 1, fp)){
        printf("%c", hi);
    }
    fclose(fp);
}
int main(){
    Process_Node* result = fetch_info();
    write_composite(result, 0);
    write_binary_composite(result, 0);
    read_binary();
    return 0;
}
