#include "../include/display.h"
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * Lucas Laviolette
 * A01362479
 * Assignment 1
 */

#define BUFFER_SIZE 4096
#define DEFAULT_FILE_PERMISSIONS 0644

void print_usage(const char *prog_name);
void to_uppercase(char *str);
void to_lowercase(char *str);

// Function pointer type definition for filter functions
typedef void (*filter_func_t)(char *);

int main(const int argc, char *argv[])
{
    int           opt;
    const char   *input_file      = NULL;
    const char   *output_file     = NULL;
    const char   *filter_function = NULL;
    int           input_fd;
    int           output_fd;
    ssize_t       bytes_read;
    char          buffer[BUFFER_SIZE];
    filter_func_t filter_func = NULL;

    // Parse command-line options
    while((opt = getopt(argc, argv, "i:o:f:")) != -1)
    {
        switch(opt)
        {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'f':
                filter_function = optarg;
                break;
            default:
                print_usage(argv[0]);
        }
    }

    if(input_file == NULL || output_file == NULL || filter_function == NULL)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Select filter function based on filter_function argument
    if(strcmp(filter_function, "upper") == 0)
    {
        filter_func = to_uppercase;
    }
    else if(strcmp(filter_function, "lower") == 0)
    {
        filter_func = to_lowercase;
    }
    else if(strcmp(filter_function, "null") == 0)
    {
        filter_func = NULL;
    }
    else
    {
        fprintf(stderr, "Unknown filter function: %s\n", filter_function);
        return EXIT_FAILURE;
    }

    // Open the input file
    input_fd = open(input_file, O_CLOEXEC);
    if(input_fd == -1)
    {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Open/create the output file with named constant for permissions
    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, DEFAULT_FILE_PERMISSIONS);
    if(output_fd == -1)
    {
        perror("Error opening output file");
        close(input_fd);
        return EXIT_FAILURE;
    }

    // Copy contents from input_file to output_file
    while((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0)
    {
        ssize_t bytes_written;

        // Apply filter function if defined
        if(filter_func != NULL)
        {
            filter_func(buffer);
        }

        bytes_written = write(output_fd, buffer, (size_t)bytes_read);
        if(bytes_written != bytes_read)
        {
            perror("Error writing to output file");
            close(input_fd);
            close(output_fd);
            return EXIT_FAILURE;
        }
    }

    if(bytes_read == -1)
    {
        perror("Error reading from input file");
    }

    close(input_fd);
    close(output_fd);

    // Display information
    display("File copy successful.");
    display(filter_function);

    return 0;
}

_Noreturn void print_usage(const char *prog_name)
{
    fprintf(stderr, "Usage: %s -i input_file -o output_file -f filter_function\n", prog_name);
    exit(EXIT_FAILURE);
}

// Function to convert a string to uppercase
void to_uppercase(char *str)
{
    for(int i = 0; str[i] != '\0'; i++)
    {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

// Function to convert a string to lowercase
void to_lowercase(char *str)
{
    for(int i = 0; str[i] != '\0'; i++)
    {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}