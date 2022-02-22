
// Authors: 
// Ashutosh Yadav 18249094 
// Emma Hurley 17212723
// Conall McAteer 18173586

// Initial Program adapted from ADDENDUM 3 in project requirements


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct Partition{ char type; int start_sect; int size;} part_entry[4]; // 4 x partition table entry
int main(int argc, char *argv[])
{
// Define some variables
int i, offset = 16, not_exist = 0;int *tmp_ptr;
FILE *fp;
char buf_part_table[64], vol_type[12];

fp = fopen(argv[1], "rb"); // Open file for reading - binary mode. Should use error check!
fseek(fp, 0x1BE, SEEK_SET); // Seek to the start of the part_entry list
fread(buf_part_table, 1, 64, fp); // Read the 64-byte block to memory buffer

for (i=0; i <4; i++) {
part_entry[i].type = *(char*)(buf_part_table + 0x04 +(i * offset) );
if (part_entry[i].type == 0) not_exist++;
part_entry[i].start_sect = *(int*)(buf_part_table + 0x08 +(i * offset));
part_entry[i].size = *(int*)(buf_part_table + 0x0C + (i * offset)) ;
 switch (part_entry[i].type) {
case 00 : strcpy ( vol_type, "NOT-VALID"); break;
case 06 : strcpy ( vol_type, "FAT-16"); break;
case 07 : strcpy ( vol_type, "NTFS"); break;
case 0x0B: strcpy ( vol_type, "FAT-32"); break;
default: strcpy ( vol_type, "NOT-DECODED"); break;
 }
// Print out partition content
printf ("Partition %d: Type: %-12s Start: %-12d Size: %-12d\n", i, vol_type, part_entry[i].start_sect,
part_entry[i].size);
 }
printf("\n\nThe total number of valid partitions is: %d\n\n", (4 - not_exist));
fclose(fp);
 return(0);
}
