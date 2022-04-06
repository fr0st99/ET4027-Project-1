#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int i, offset = 16, root_offset = 32, not_exist = 0;
FILE *fp;
char buf_part_table[64], vol_type[12], buf_fat_part[512], buf_root_part[512], buf_del_part[512];
int *tmp_ptr, fs, fr, fileseek_FAT, fileread_FAT, fileseek_root, fileread_root, fsDEL,frDEL, first_partition_Saddress, sizeSectA, sizeSectB, noFatCopies, maxDirectories, rootDirSize, rootDirAddr, sizeReservedArea, sectorAddr, sizeReservedArea2;
fp = fopen(argv[1], "rb"); // Open file for reading - binary mode. Should use error check!
fs = fseek(fp, 0x1BE, SEEK_SET); // Seek to the start of the part_entry list
fr = fread(buf_part_table, 1, 64, fp); // Read the 64-byte block to memory buffer

first_partition_Saddress = part_entry[0].start_sect * 512;
      fileseek_FAT = fseek(fp, first_partition_Saddress, SEEK_SET); //Seek to the start of the part_entry list
      fileread_FAT = fread(buf_fat_part, 1, 512, fp); // Read the 512-byte block to memory buffer
      

      printf("\n");
      printf("Fat Volume Information:");
      printf("\n");
      printf("\n");
      
      
      //the number of sectors per cluster
      fat_Info.sect_per_cluster = *(char*)(buf_fat_part + 0xd ); // Char can store 512 thats why we use it
      printf("Sectors per cluster: %-12d", fat_Info.sect_per_cluster);  //Printing the sectors per cluster
      
      
      //the size of the FAT area,
      //Fat area size = (Size of FAT in sectors) * (Number of FAT copies)
      noFatCopies = *(char*)(buf_fat_part + 0x10);
      sizeSectA = *(unsigned char*)(buf_fat_part + 0x16);
      sizeSectB = *(char*)(buf_fat_part + 0x17);
      sizeSectB = (sizeSectB<<8) ;
      sizeSectA = sizeSectA + sizeSectB;      
      fat_Info.fat_area = sizeSectA * noFatCopies;
      printf("\nFAT Area: %-12d", fat_Info.fat_area);  //Printing the Fat area
      
        //the size of the root direcotry
        //Root dir size = ((Max no. of directories)*(dir entry size in bytes)/sector size
        maxDirectories =  *(int*)(buf_fat_part + 0x11);
        rootDirSize = (maxDirectories * 32)/512;
        printf("\nRoot directory size: %d", rootDirSize);


    //the sector address of Cluster #2,
    //First Sector of data area = first sector of volume + size of reserved area + size of fat area
    //Sector Address of #2 = (First sector of data area ) + (Size of root dir)

        sizeReservedArea = (*(char*)(buf_fat_part + 0x0E));
        sizeReservedArea2 = (*(char*)(buf_fat_part + 0x0F));

        sizeReservedArea2 = sizeReservedArea2<<8;
        sizeReservedArea = sizeReservedArea2 + sizeReservedArea;
        printf("\nSize of reserved area: %-12d", sizeReservedArea);
        printf("\nStart sector: %-12d", part_entry[0].start_sect);
        rootDirAddr = (part_entry[0].start_sect) + (sizeReservedArea) + fat_Info.fat_area;
        sectorAddr = rootDirAddr + rootDirSize;
        printf("\nSector address for Cluster #2: %d", sectorAddr);


        printf("\nRoot directory address: %d \n ", rootDirAddr);

        printf("\n");
        printf("\n##################################################################");
        printf("\n");