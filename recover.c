/*
*
*   CS50x 2019
*   Problem Set 3 -> Recover.c
*
*   Recover JPEG images from a file
*
*   Adam DeCaria
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Please include a file name to search JPG");
    }

    // capture file name from command line
    char *infile = argv[1];

    // open the file in read mode
    FILE *inptr = fopen(infile, "r");

    // check if file exists/can be opened
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 1;
    }

    // size of buffer -> 512 as spec says to read 512 bytes at a time
    int buffer_size = 512;

    // buffer for what is being read from the file
    BYTE buffer[512] = {0x00};

    // flag to check end of file - if 1, end of file is reached
    int end_of_file = 0;

    // output file for recovered images - remember to initialize to NULL or segmentation fault
    FILE *img = NULL;

    // filename of jpg file to be written; follows pattern of 001, 002, 003, etc... corresponding to jpg_counter
    char filename[8] = {'0', '0', 'x', '.', 'j', 'p', 'g', '\0'};

    // used to keep track of the # of jpgs found thus far; also used to name jpg files
    int jpg_counter = 0;

    int first_jpg_found = 0;

    // while not at the end of file, execute program logic
    while (end_of_file != 1)
    {
        // check for end of file
        if (fread(buffer, buffer_size, 1, inptr) == 0)
        {
            end_of_file = 1;
            continue;
        }
        // first, check if beginning of new JPEG
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
        {
            // check if this is the first JPEG found
            if (first_jpg_found == 0)
            {
                sprintf(filename, "%03i.jpg", jpg_counter);
                img = fopen(filename, "w");

                if (img == NULL)
                {
                    fclose(inptr);
                    fprintf(stderr, "Could not create %s.\n", filename);
                    return 1;
                }

                fwrite(buffer, buffer_size, 1, img);

                first_jpg_found = 1;
                jpg_counter++;
            }
            // otherwise, close the last image file before moving on
            else
            {
                fclose(img);

                sprintf(filename, "%03i.jpg", jpg_counter);
                img = fopen(filename, "w");

                if (img == NULL)
                {
                    fclose(inptr);
                    fprintf(stderr, "Could not create %s.\n", filename);
                    return 1;
                }

                fwrite(buffer, buffer_size, 1, img);
                jpg_counter++;
            }
        }
        // if not the beginning of a new JPEG, continue writing to open file
        else
        {
            if (img != NULL)
            {
                fwrite(buffer, buffer_size, 1, img);
                continue;
            }
        }
    }

    fclose(inptr);
    fclose(img);

    return 0;
} // EOF
