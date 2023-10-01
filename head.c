#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_LINE_LENGTH 1024

void print_usage() {
    printf(2, "Usage: head [-n num] || [-<num>] [file...]\n");
}

int read_and_print_lines(int fd, int line_count) {
    char buf[MAX_LINE_LENGTH];
    int line_number = 0;

    while (read(fd, buf, sizeof(buf)) > 0) {
        for (int j = 0; j < sizeof(buf) && buf[j] != '\0'; j++) {
            printf(1, "%c", buf[j]);

            if (buf[j] == '\n') {
                line_number++;
                if (line_number >= line_count) {
                    // Reached the specified number of lines, exit loop
                    return 1;
                }
            }
        }

        if (line_number >= line_count) {
            // Reached the specified number of lines, exit loop
            return 1;
        }
    }

    return 0;
}


int main(int argc, char *argv[]) {
    printf(1, "Head command is getting executed in user mode.\n");

    int line_count = 14; // Default number of lines to display
    int file_offset = 1; // Start of file arguments (skip program name)

    // Check for the -n option to specify the number of lines
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'n') {
        if (argc < 3) {
            print_usage();
            exit();
        }
        line_count = atoi(argv[2]);
        file_offset = 3;
    } else if ((argc < 3 && argv[1][0] == '-') || argc < 2){
        print_usage();
        exit();
    }

    // Process files or stdin if no files provided
    for (int i = file_offset; i < argc || i == file_offset; i++) {
        char *file_name = (i < argc) ? argv[i] : 0;
        int fd;

        if (file_name == 0) {
            // If no file specified, read from stdin
            fd = 0;
        } else {
            fd = open(file_name, 0);
            if (fd < 0) {
                printf(2, "head: cannot open %s\n", file_name);
                exit();
            }
        }

        if (read_and_print_lines(fd, line_count)) {
            break; // Exit the loop if the specified lines are printed
        }

        if (file_name != 0) {
            close(fd);
        }
    }

    exit();
}
