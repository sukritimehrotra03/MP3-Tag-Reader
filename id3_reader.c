/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */

#include <stdio.h>
#include <stdlib.h>
#include "id3_reader.h"
#include "error_handling.h"
#include <string.h>


int big_to_little_endian(const unsigned char *bytes) 
{
    int result = 0;

    for (int i = 0; i < 4; i++) 
    {
        result = (result << 8) + bytes[i];
    }

    return result;
}

/**
TODO: Add documention as sample given
 */
TagData* read_id3_tags(const char *filename) 
{
    // Implementation for reading ID3 tags
    
    // Open the file
    FILE *fp_og = fopen(filename, "rb");
    if (fp_og == NULL) {
        printf("Error: Could not open file.\n");
        return NULL;
    }

    // Allocate memory for the TagData structure
    TagData *temp = malloc(sizeof(TagData));
    if (temp == NULL) {
        printf("Error: Memory allocation failed.\n");
        fclose(fp_og);
        return NULL;
    }

    // Array of all the tags
    const char *tags[] = {"TPE1", "TIT2", "TALB", "TYER", "TCON", "COMM"};
    char **tagValues[] = {&temp->artist, &temp->title, &temp->album, &temp->year, &temp->genre, &temp->comment};

    // Set the pointer after the ID3 header
    fseek(fp_og, 10, SEEK_SET);

    char tag[5] = {0};
    unsigned char size_bytes[4];
    int content_size;

    while(fread(tag, 1, 4, fp_og) == 4)
    {
        tag[4] = '\0';

        if(fread(size_bytes, 1, 4, fp_og) != 4)
        {
            break;
        } 
        content_size = big_to_little_endian(size_bytes);

        // Skip flags
        fseek(fp_og, 3, SEEK_CUR);

        for(int i = 0; i < sizeof(tags) / sizeof(tags[0]); i++)
        {
            if(strcmp(tag, tags[i]) == 0)
            {
                *tagValues[i] = malloc(content_size);
                if (*tagValues[i] == NULL) 
                {
                    printf("Error: Memory allocation failed for tag %s\n", tags[i]);
                    fclose(fp_og);
                    free_tag_data(temp);
                    return temp;
                }
                fread(*tagValues[i], 1, (content_size - 1), fp_og);
                (*tagValues[i])[content_size] = '\0'; // Null-terminate the string
                break;
            }
        }
    }
    
    fclose(fp_og);
    return temp;
}
/**
TODO: Add documention as sample given
 */
void display_metadata(const TagData *data) {
    // Implementation for displaying metadata

    printf("----------------------------------------------------------------------------\n");
    printf("MP3 Tag Reader and Editor for ID3v2\n");
    printf("----------------------------------------------------------------------------\n");
    printf("Title     :     %-25s\n", data->title ? data->title : "N/A");
    printf("Artist    :     %-25s\n", data->artist ? data->artist : "N/A");
    printf("Album     :     %-25s\n", data->album ? data->album : "N/A");
    printf("Year      :     %-25s\n", data->year ? data->year : "N/A");
    printf("Music     :     %-25s\n", data->genre ? data->genre : "N/A");
    printf("Comment   :     %-25s\n", data->comment ? data->comment : "N/A");
    printf("----------------------------------------------------------------------------\n");
}

/**
TODO: Add documention as sample given
 */
void view_tags(const char *filename) {
    TagData *data = read_id3_tags(filename);
    if (!data) {
        printf("Failed to read ID3 tags.");
        return;
    }
 
    display_metadata(data);
    free_tag_data(data);
}
