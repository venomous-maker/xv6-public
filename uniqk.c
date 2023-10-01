#include "types.h"
#include "stat.h"
#include "user.h"
char * error = "";
void print_usage() {
    error = "Usage: uniq [-c] [-i] [-d] input_file";
}

// Function to read and return lines of a file
char** read_lines(int fd, int* line_count) {
    // Initialize variables
    int max_lines = MAX_LINES; // Initial size for the array of lines
    char** lines = (char**)malloc(max_lines * sizeof(char*));
    if (lines == NULL) {
        return NULL;
    }

    char* line = NULL;
    int line_len = 0;
    *line_count = 0;

    // Read lines from the file
    while (1) {
        char ch;
        int n = read(fd, &ch, sizeof(ch));

        if (n < 0) {
            free(line);
            free(lines);
            return NULL;
        } else if (n == 0) {
            // End of file, break the loop
            break;
        }

        // Allocate memory for the current line
        if (line == NULL) {
            line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
            if (line == NULL) {
                free(lines);
                return NULL;
            }
            line_len = 0;
        }

        if (ch != '\n') {
            line[line_len] = ch;
            line_len++;
        } else {
            // Remove the trailing newline character
            line[line_len] = '\0';

            // Check if the line is empty, and skip it if it is
            if (line_len > 0) {
                lines[*line_count] = line;
                (*line_count)++;

                // Check if we need to resize the array
                if (*line_count >= max_lines) {
                    max_lines *= 2;
                    char** new_lines = (char**)malloc(max_lines * sizeof(char*));
                    if (new_lines == NULL) {
                        free(lines);
                        return lines; // Return what we have so far
                    }
                    memmove(new_lines, lines, *line_count * sizeof(char*));
                    free(lines);
                    lines = new_lines;
                }
            }

            line = NULL;
            line_len = 0;
        }
    }

    if (line != NULL) {
        lines[*line_count] = line;
        (*line_count)++;
    }

    return lines;
}

int main(int argc, char *argv[]) {
    int count_flag = 0;
    int ignore_case_flag = 0;
    int duplicate_flag = 0;
    int piped = 0;
    int line_count = 0;
    int fd = 0;
    char ** lines = NULL;
    // Parse command-line arguments
    int opt;
    char* input_file = 0;
    if(argc > 1){
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                switch (argv[i][1]) {
                    case 'c':
                        count_flag = 1;
                        break;
                    case 'i':
                        ignore_case_flag = 1;
                        break;
                    case 'd':
                        duplicate_flag = 1;
                        break;
                    default:
                        print_usage();
                }
            } else {
                if (input_file == 0) {
                    input_file = argv[i];
                } else {
                    print_usage();
                }
            }
        }
    }
    else{
        char buf[MAX_LINE_LENGTH];
        // Read input from stdin (pipe)
        int k = 1;
        while (1) {
            int n = read(0, buf, sizeof(buf));

            if (n <= 0) {
                break; // End of input or error
            }else{
                line_count++;
                if (lines == NULL){
                    lines = (char**)malloc( MAX_LINES * sizeof(char*));
                }
                buf[n] = '\0'; // Null-terminate the line
                lines[k-1] = buf;
                piped = 1;
            }
        }
    }

    if (input_file == 0 && !piped) {
        print_usage();
    }

    if(!piped ){
        line_count = 0;
        if((fd = open(input_file, 0)) < 0){
            error = "uniq: cannot input file";
        }
        lines = read_lines(fd, &line_count);
    }
    if (lines == NULL){
        error = "Could not read lines from file";
    }
    int res = uniq(ignore_case_flag, duplicate_flag, count_flag, lines, line_count, error);
    close(fd);
    return res;
}