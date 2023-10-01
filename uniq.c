#include "types.h"
#include "stat.h"
#include "user.h"

void print_usage() {
    printf(2, "Usage: uniq [-c] [-i] [-d] input_file\n");
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

// Function to check if a sentence is available in an array of strings
int* isStringAvailable(const char* string, char* strings[], int numStrings) {
    int *result = (int*) malloc(2 * sizeof(int));
    result[0] = 0;
    result[1] = 0;
    if (string == NULL || strings == NULL || numStrings <= 0) {
        return result; // Handle invalid input
    }

    for (int i = 0; i < numStrings; i++) {
        if (strings[i] != NULL && strcmp(strings[i], string) == 0) {
            result[0] = 1;
            result[1] = i;
            return result; // Sentence found in one of the strings
        }
    }

    return result; // Sentence not found in any of the strings
}

int main(int argc, char *argv[]) {
    printf(1, "Uniq command is getting executed in user mode.\n\n");
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
                        exit();
                }
            } else {
                if (input_file == 0) {
                    input_file = argv[i];
                } else {
                    print_usage();
                    exit();
                }
            }
        }
    }
    else{
        char buf[MAX_LINE_LENGTH];
        char prev_line[MAX_LINE_LENGTH] = ""; // Store the previous line
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
        exit();
    }
    
    if(!piped ){
        line_count = 0;
        if((fd = open(input_file, 0)) < 0){
        printf(1, "uniq: cannot open %s\n", input_file);
        exit();
        }
        lines = read_lines(fd, &line_count);
    }
    
    char** original_lines;
    if (ignore_case_flag) {
        original_lines = lines;
        lines = (char**)malloc(line_count * sizeof(char*));  // Create a new array for lowercase lines
        if (lines == 0) {
            printf(2, "Memory allocation failed\n");
            exit();
        }
        
        for(int i = 0; i < line_count; i++) {
            lines[i] = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));  // Allocate memory for each line
            if (lines[i] == 0) {
                printf(2, "Memory allocation failed\n");
                exit();
            }
            strcpy(lines[i], original_lines[i]);  // Copy the original line
            strlower(lines[i]);  // Convert the copied line to lowercase
        }
    }

    int *uniq_counter;
    char **uniq;
    char** uniq_print;
    int uniqs = (line_count > 0) ? 1 : 0;
    if (uniqs) {
        uniq = (char **)malloc(1 * sizeof(char *));
        uniq_counter = (int *)malloc(1 * sizeof(int));
        uniq[0] = lines[0];
        uniq_counter[0] = 1;
        if (ignore_case_flag) {
            uniq_print = (char **)malloc(1 * sizeof(char *));
            uniq_print[0] = original_lines[0];
        }
    }

    for (int i = 1; i < line_count; i++) {
        int *result = isStringAvailable(lines[i], uniq, uniqs);
        if (result[0]) {
            uniq_counter[result[1]] = uniq_counter[result[1]] + 1;
        } else {
            // Add the new line to the uniq list
            uniqs++;

            // Allocate new memory for uniq and copy old contents
            char** new_uniq = (char**)malloc(uniqs * sizeof(char*));
            int* new_uniq_counter = (int*)malloc(uniqs * sizeof(int));

            if (new_uniq == NULL || new_uniq_counter == NULL) {
                printf(2, "Memory allocation failed");
                return 1; // Handle memory allocation error
            }

            for (int j = 0; j < uniqs - 1; j++) {
                new_uniq[j] = uniq[j];
                new_uniq_counter[j] = uniq_counter[j];
            }

            // Add the new line and set its count
            new_uniq[uniqs - 1] = lines[i];
            new_uniq_counter[uniqs - 1] = 1;

            // Free old memory and update pointers
            free(uniq);
            free(uniq_counter);
            uniq = new_uniq;
            uniq_counter = new_uniq_counter;

            if (ignore_case_flag) {
                // Allocate new memory for uniq_print and copy old contents
                char** new_uniq_print = (char**)malloc(uniqs * sizeof(char*));

                if (new_uniq_print == NULL) {
                    printf(2, "Memory allocation failed");
                    return 1; // Handle memory allocation error
                }

                for (int j = 0; j < uniqs - 1; j++) {
                    new_uniq_print[j] = uniq_print[j];
                }

                // Add the new line to uniq_print
                new_uniq_print[uniqs - 1] = original_lines[i];

                // Free old memory and update uniq_print pointer
                free(uniq_print);
                uniq_print = new_uniq_print;
            }
        }
    }

    if (ignore_case_flag) {
        uniq = uniq_print;
    }
    // Print the unique lines with counts if -c flag is set
    for (int i = 0; i < uniqs; i++) {
        if (duplicate_flag) {
            if (count_flag) {
                if (uniq_counter[i] > 1) {
                    printf(1, "%d %s\n", uniq_counter[i], uniq[i]);
                }
            } else {
                if (uniq_counter[i] > 1) {
                    printf(1, "%s\n", uniq[i]);
                }
            }
        } else if (count_flag) {
            printf(1, "%d %s\n", uniq_counter[i], uniq[i]);
        } else {
            printf(1, "%s\n", uniq[i]);
        }
    }

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    
    if (ignore_case_flag) {
        for (int i = 0; i < line_count; i++) {
            free(original_lines[i]);
        }
        free(original_lines);
    }

    close(fd);
    exit();
}
