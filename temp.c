// void set_syncsafe_size(unsigned char *dest, int size) {
//     dest[0] = (size >> 21) & 0x7F;
//     dest[1] = (size >> 14) & 0x7F;
//     dest[2] = (size >> 7) & 0x7F;
//     dest[3] = size & 0x7F;
// }

// // Function to update a specific frame
// void update_frame(char *tag_data, int tag_size, const char *frame_id, const char *new_value) {
//     if (!new_value) return; // Skip if no new value provided

//     // Search for the frame in tag_data
//     char *frame = tag_data;
//     while (frame < tag_data + tag_size - 10) { // Minimum frame size is 10 bytes
//         if (strncmp(frame, frame_id, 4) == 0) {
//             // Found the frame, update its content
//             int frame_size = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7];
//             int new_size = strlen(new_value) + 1; // +1 for text encoding byte (assume UTF-8)

//             if (new_size <= frame_size) {
//                 // Update in-place
//                 frame[10] = 0x03; // UTF-8 encoding
//                 memcpy(frame + 11, new_value, new_size - 1);
//                 memset(frame + 11 + new_size - 1, 0, frame_size - new_size); // Clear unused bytes
//             } else {
//                 printf("Error: New value too large to fit in existing frame.\n");
//             }
//             return;
//         }
//         // Move to the next frame
//         int frame_size = (frame[4] << 24) | (frame[5] << 16) | (frame[6] << 8) | frame[7];
//         frame += 10 + frame_size;
//     }

//     printf("Frame %s not found.\n", frame_id);
// }

// int write_id3_tags(const char *filename, TagData *data) {
//     FILE *fptr = fopen(filename, "r+b");
//     if (!fptr) {
//         printf("Unable to open file.\n");
//         return 1;
//     }

//     // Read ID3 header (10 bytes)
//     char header[10];
//     if (fread(header, 1, 10, fptr) != 10) {
//         printf("Failed to read ID3 header.\n");
//         fclose(fptr);
//         return 1;
//     }

//     // Extract tag size from the header (sync-safe format)
//     int tag_size = ((header[6] & 0x7F) << 21) | ((header[7] & 0x7F) << 14) |
//                    ((header[8] & 0x7F) << 7) | (header[9] & 0x7F);

//     // Allocate buffer for the tag data
//     char *tag_data = malloc(tag_size);
//     if (!tag_data) {
//         printf("Memory allocation failed.\n");
//         fclose(fptr);
//         return 1;
//     }

//     if (fread(tag_data, 1, tag_size, fptr) != tag_size) {
//         printf("Failed to read ID3 tag data.\n");
//         free(tag_data);
//         fclose(fptr);
//         return 1;
//     }

//     fclose(fptr);

//     // Update the frames
//     update_frame(tag_data, tag_size, "TIT2", data->title);
//     update_frame(tag_data, tag_size, "TPE1", data->artist);
//     update_frame(tag_data, tag_size, "TALB", data->album);
//     update_frame(tag_data, tag_size, "TYER", data->year);
//     update_frame(tag_data, tag_size, "TCON", data->genre);
//     update_frame(tag_data, tag_size, "COMM", data->comment);

//     // Write back updated tag data
//     fptr = fopen(filename, "r+b");
//     if (!fptr) {
//         printf("Unable to open file for writing.\n");
//         free(tag_data);
//         return 1;
//     }

//     fseek(fptr, 10, SEEK_SET);
//     fwrite(tag_data, 1, tag_size, fptr);

//     fclose(fptr);
//     free(tag_data);

//     return 0;
// }