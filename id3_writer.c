/**
 * @file id3_writer.c
 * @brief Implementation of functions for writing and editing ID3 tags in MP3 files.
 */

#include <stdio.h>
#include <string.h>
#include "id3_writer.h"
#include "id3_reader.h"
#include "id3_utils.h"
#include <stdlib.h>

/**
 * @brief Writes the ID3 tags to an MP3 file.
 *
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @return 0 on success, non-zero on failure.
 */

int check_id3_tag_presence(FILE *fptr)
{
    if (fptr == NULL)
    {
        printf("Error : File pointer is null.\n");
        return 0;
    }

    char ID[3];
    if (fread(ID, 1, 3, fptr) != 3)
    {
        printf("Error : Could not read the header.\n");
        return 0;
    }
    if (strcmp(ID, "ID3") == 0)
    {
        return 1; // ID3 tag present
    }

    return 0;
}

int little_to_big_endian(const unsigned int *bytes)
{
    int result = 0;

    for (int i = 3; i >= 0; i--) // Start from the last byte and move to the first
    {
        result = (result << 8) + bytes[i];
    }

    return result;
}

int write_id3_tags(const char *filename, FILE *fptr, TagData *data)
{
    // Implementation for writing ID3 tags
    // Open the file
    FILE *fp_temp = fopen("temp.mp3", "wb");
    if (fp_temp == NULL)
    {
        printf("Error: Could not open file.\n");
        return 1;
    }

    char header[10];
    fread(header, 1, 10, fptr);
    fwrite(header, 1, 10, fp_temp);

    const char *tags[] = {"TPE1", "TIT2", "TALB", "TYER", "TCON", "COMM"};
    char **tagValues[] = {&data->artist, &data->title, &data->album, &data->year, &data->genre, &data->comment};

    char tag[5] = {0}; 
    unsigned char size_bytes[4];
    int content_size;
    int count_tags = 0;
    while (count_tags != 6)
    {
        
        fread(tag, 1, 4, fptr);

        fwrite(tag, 1, 4, fp_temp);

        tag[4] = '\0';

        if (fread(size_bytes, 1, 4, fptr) != 4)
        {
            break;
        }
        // else
        // {
        //     fwrite(size_bytes, 1, 4, fp_temp);
        // }
        content_size = big_to_little_endian(size_bytes);

        // Skip flags
        // fseek(fptr, 3, SEEK_CUR);
        char flag[3];
        fread(flag, 1, 3, fptr);

        for (int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++)
        {
            if (strcmp(tag, tags[i]) == 0)
            {
                count_tags++;
                char *content = malloc(content_size);
                if (content == NULL)
                {
                    printf("Error: Memory allocation failed for tag %s\n", tags[i]);
                    fclose(fp_temp);
                    free_tag_data(data);
                    return 1;
                }

  
                fread(content, 1, (content_size - 1), fptr);
                content[content_size] = '\0'; // Null-terminate the string

                if (strcmp(content, *tagValues[i]) == 0)
                {
                    // int content_sizeb = little_to_big_endian(size_bytes);
                    fwrite(size_bytes, 1, 4, fp_temp);
                    fwrite(flag, 1, 3, fp_temp);
                    fwrite(content, 1, content_size - 1, fp_temp);

                }
                else
                {
                    // int *size;
                    // *size = strlen(*tagValues[i]);
                    // int *big_size;
                    // *big_size = little_to_big_endian(size);
                    // fwrite(big_size, 1, 4, fp_temp);
                    // fwrite(flag, 1, 3, fp_temp);
                    // fwrite(*tagValues[i], 1, *big_size, fp_temp);

                    // Calculate the new size (content size without null terminator)
                    int new_size = strlen(*tagValues[i]) + 1;

                    // Convert size to 4 bytes in big-endian
                    unsigned char big_endian_size[4];
                    for (int j = 0; j < 4; j++)
                    {
                        big_endian_size[3 - j] = (new_size >> (j * 8)) & 0xFF; // Extract each byte
                    }

                    // Write the new size in big-endian format
                    fwrite(big_endian_size, 1, 4, fp_temp);

                    // Write the flags (3 bytes)
                    fwrite(flag, 1, 3, fp_temp);

                    // Write the tag value (the actual content)
                    fwrite(*tagValues[i], 1, new_size - 1, fp_temp);
                }
                free(content);
                break;
            }
        }
    }

    //  printf("______%ld\n",ftell(fptr));
    //  printf("______%ld\n",ftell(fp_temp));

    char byte[1024];
    int count;
    long start = ftell(fptr);
    fseek(fptr, 0L, SEEK_END);
    long end = ftell(fptr);
    fseek (fptr, start, SEEK_SET);

    while((ftell (fptr)) != end)
    {
        count = fread (byte, 1, 1024, fptr);
        if(count < 0)
        {
            printf("Error: Unable to read.");
            return 1;
        }
        fwrite(byte, 1, count, fp_temp);
    }

    fclose(fptr);
    fclose(fp_temp);

    return 0;
}

void transfer_data(const char *filename, const char *temp)
{
    FILE *fptr = fopen(filename, "wb");
    FILE *fp_temp = fopen(temp, "rb");
    // char ch;
    // // Read data from source image
    // while (fread(&ch, 1, 1, fp_temp))
    // {
    //     // Write data to the destination image
    //     fwrite(&ch, 1, 1, fptr);
    // }

    int count;
    char ch [1024];
    long start = ftell (fp_temp);

    fseek (fp_temp, 0L, SEEK_END);
    long end = ftell (fp_temp);
    fseek (fp_temp, 0L, SEEK_SET);

    while ((ftell (fp_temp)) != end)
    {
        count = fread (ch, 1, 1024, fp_temp);
        if (count < 0)
        {
            printf ("ERROR: Unable to read from the MP3 file.\n");
        }
        else
        {
            fwrite (ch, 1, count, fptr);
        }
    }

    fclose(fptr);
    fclose(fp_temp);
}

/** TODO: Add documentation as sample given above */
int edit_tag(const char *filename, const char *tag, const char *value)
{

    FILE *fptr = fopen(filename, "r+b");
    if (!fptr)
    {
        perror("Error: File could not open!");
        return 1;
    }

    if (check_id3_tag_presence(fptr))
    {
        printf("ID3 tag is present in the file.\n");
    }
    else
    {
        printf("ID3 tag is not present in the file.\n");
        return 1;
    }
    rewind(fptr);

    TagData *data = read_id3_tags(filename);
    if (!data)
    {
        return 1;
    }
    // Implementation for writing ID3 tags to file

    // fseek(fptr, 10, SEEK_SET);
    const char *flags[] = {"-a", "-T", "-A", "-y", "-g", "-c"};
    char **tagValues[] = {&data->artist, &data->title, &data->album, &data->year, &data->genre, &data->comment};

    int flag = 0;

    for (int i = 0; i < sizeof(flags) / sizeof(flags[0]); i++)
    {
        if (strcmp(tag, flags[i]) == 0)
        {
            flag = 1;
            *tagValues[i] = realloc(*tagValues[i], strlen(value) + 1);
            if (*tagValues[i] == NULL)
            {
                printf("Error: Memory reallocation failed!\n");
                fclose(fptr);
                free_tag_data(data);
                return 1; // Handle reallocation failure
            }

            strcpy(*tagValues[i], value);

            break;
        }
    }

    if(flag == 0)
    {
        printf("Enter valid flag!\n");
        return -1;
    }

    if (write_id3_tags(filename, fptr, data) == 0)
    {
        printf("Data Written Successfully!\n");
    }
    else
    {
        return 1;
    }

    transfer_data(filename, "temp.mp3");

    free_tag_data(data);

    return 0;
}