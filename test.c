
#include "types.h"
#include "stat.h"
#include "user.h"

int main(int args, char** argv) {
    int pid_uniq, pid_head;
    struct pstat pinfo_uniq;
    struct pstat pinfo_head;

    // Fork and execute the 'uniq' command with different arguments
    pid_uniq = fork();
    if (pid_uniq == 0) {
        char *uniq_args[] = {"uniq", "input_file.txt", NULL};
        exec("uniq", uniq_args);
        exit();
    }
    // Wait for child processes to exit
    wait();
    // Call the custom 'getpinfo' system call to retrieve process information for 'uniq'
    if (pid_uniq > 0 && getpinfo(pid_uniq, &pinfo_uniq) < 0) {
        printf(2, "Error: getpinfo for uniq failed\n");
        exit();
    }
    
    // Print process information for 'uniq'
    printf(1, "uniq Process Information:\n");
    printf(1, "Creation Time: %d\n", pinfo_uniq.creation_time);
    printf(1, "End Time: %d\n", pinfo_uniq.end_time);
    printf(1, "Total Time: %d\n", pinfo_uniq.total_time);
    
    // Fork and execute the 'head' command with different arguments
    pid_head = fork();
    if (pid_head == 0) {
        char *head_args[] = {"head", "-n", "10", "input_file.txt", NULL};
        exec("head", head_args);
        exit();
    }

    wait();

    // Call the custom 'getpinfo' system call to retrieve process information for 'head'
    if (pid_head > 0 && getpinfo(pid_head, &pinfo_head) < 0) {
        printf(2, "Error: getpinfo for head failed\n");
        exit();
    }
    
    // Print process information for 'head'
    printf(1, "head Process Information:\n");
    printf(1, "Creation Time: %d\n", pinfo_head.creation_time);
    printf(1, "End Time: %d\n", pinfo_head.end_time);
    printf(1, "Total Time: %d\n", pinfo_head.total_time);
    exit();
}


