#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include "centre.h"

/*Responsible for parsing and checking flags and sending the flags to centre*/
int main(int argc, char** argv){
    int opt;
    pid_t processid = 0;
    int valid = 1;
    int perprocess = 0, systemWide = 0, vNodes = 0, composite = 0, threshold = 0, no_arg = 1, binary = 0, txt = 0;
    while(opt != -1){
        static struct option lng_opt[] = {
            {"per-process",     no_argument,        0, 'p'},
            {"systemWide",      no_argument,        0, 's'},
            {"Vnodes",          no_argument,        0, 'v'},
            {"composite",       no_argument,        0, 'c'},
            {"threshold",       required_argument,  0, 't'},
            {"output_TXT",      no_argument,        0, 'a'},
            {"output_binary",   no_argument,        0, 'b'},
            {0,                 0,                  0,  0}
        };
        if(optind >= argc) break;
        int num = 1;
        for(int i = 0; argv[optind][i] != '\0'; i++){
            if(argv[optind][i] < '0' || argv[optind][i] > '9')
            {
                num = 0;
                break;
            }
        }
        if(num){
            processid = strtol(argv[optind], NULL, 10);
            if(processid <= 0){
                printf("%s: Invalid process ID: %d provided!\n", argv[0], processid);
                valid = 0;
            }
            optind++;
        }
        else if(argv[optind][0] != '\0' && (argv[optind][0] != '-' || argv[optind][1] != '-')){
            valid = 0;
            printf("%s: Unrecognized option: %s\n", argv[0], argv[optind]);
            optind++;
        }
        else{
            opt = getopt_long(argc, argv, "psvct:ab", lng_opt, NULL);
            switch(opt){
                case 'p':{
                    no_arg = 0;
                    perprocess = 1;
                    break;
                }
                case 's':{
                    no_arg = 0;
                    systemWide = 1;
                    break;
                }
                case 'v':{
                    no_arg = 0;
                    vNodes = 1;
                    break;
                }
                case 'c':{
                    no_arg = 0;
                    composite = 1;
                    break;
                }
                case 't':{
                    threshold = strtol(optarg, NULL, 10);
                    if(threshold <= 0){
                        printf("%s: Invalid Threshold: %d provided!\n", argv[0], threshold);
                        valid = 0;
                    }
                    break;
                }
                case 'a':{
                    txt = 1;
                    break;
                }
                case 'b':{
                    binary = 1;
                    break;
                }
                case '?':{
                    valid = 0;
                    break;
                }
            }
        }
    }
    if(valid) display_centre(processid, perprocess, systemWide, vNodes, composite, threshold, no_arg, binary, txt);
    return 0;
}
