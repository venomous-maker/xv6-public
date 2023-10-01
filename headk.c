#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_LINE_LENGTH 1024
char * error = "";
void print_usage() {
    error = "Usage: head [-n num] || [-<num>] [file...]";
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

    int line_count = 14; // Default number of lines to display
    int linecount = 0;
    int file_offset = 1; // Start of file arguments (skip program name)
    char **lines = NULL;

    // Check for the -n option to specify the number of lines
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'n') {
        if (argc < 3) {
            print_usage();
        }
        line_count = atoi(argv[2]);
        file_offset = 3;
    } else if ((argc < 3 && argv[1][0] == '-') || argc < 2){
        print_usage();
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
                error = "head: cannot open file";
            }
        }

       lines = read_lines(fd, &linecount);
        if(linecount < line_count) line_count = linecount;
        int res = head(line_count, lines, error);
        if (file_name != 0) {
            close(fd);
        }
    }

    exit();
}
