/** 
 * ET4027 Laboratory Assignment #0
 * Author: Josh Guyer, William Lavelle
 * Due Date: 27/3/2020
 * Summary: Displays the number of partitions on the disk and for each
 * partition displays the start sector, size of partition, and file type.
 * For the FAT16 partition, displays the number of sectors per cluster, the size
 * of the FAT area, the size of the root directory, and the sector address of
 * cluster 2. Creates and outputs a file for each of the files in the partition.
 * For the first deleted file, displays the name and the size of the file, 
 * and the number of the first cluster. Display information for an NTFS partition such as
 * how many bytes per secotr, how many sectors per cluster, sector address for the $MFT file record,
 * and what is the type and length of the first two attributes in the $MFT record.
 **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BPS 512 //Bytes per Sector
struct Partition
{
    char type;
    int start_sect;
    int size;
} part_entry[4]; // 4 x partition table entry
struct MBRinfo {
    int sectorsPerCluster;
    int bytesPerSector;
    int copies;
    int RA;
    int FATsize;
    int rootDirEntires;
    int cluster2;
} FAT16; //first partition (FAT16)
struct NTFSBootSector {
    int bytesPerSector;
    int sectorsPerCluster;
    int sectorAddress;
    int MFT;
} NTFS;
struct file{
    char fileName[11];
    int startingCluster;
    int fileSize;
} currentFile; //currentFile of the FAT16 partition
void parseFAT16(int startSector, FILE *fp); //Read the boot sector of a FAT16 partition
void peekFAT16(int firstSectorDA, FILE *fp); //Look at the files from the root directory of the FAT16
void parseNTFS(int startSector, FILE *fp); //Read the boot sector of a NTFS partition
void outputFileFromPartition(int startingCluster, char name[], FILE *fp, int size); //output a file from the partition
int main(int argc, char *argv[]) {
    int not_exist = 0;
    int *tmp_ptr;
    FILE *fp;
    char buf_part_table[64], vol_type[12];
    fp = fopen(argv[1], "r");        // Open file for reading - binary mode. Should use error check!
    fseek(fp, 0x1BE, SEEK_SET);       // Seek to the start of the part_entry list
    fread(buf_part_table, 1, 64, fp); // Read the 64-byte block to memory buffer
    printf("----====Partitions on %s disk image====----\n", argv[1]);
    for (int i = 0; i < 4; i++) {
        part_entry[i].type = *(char *)(buf_part_table + 0x04 + (i * 0x10)); //1 byte 
        if (part_entry[i].type == 0)
            not_exist++;
        part_entry[i].start_sect = *(int *)(buf_part_table + 0x08 + (i * 0x10)); //4 bytes 
        part_entry[i].size = *(int *)(buf_part_table + 0x0C + (i * 0x10)); //4 bytes
        switch (part_entry[i].type) //finds the type of the partition
        {
        case 00:
            strcpy(vol_type, "NOT-VALID");
            break;
        case 05:
            strcpy(vol_type, "MS-DOS");
            break;
        case 06:
            strcpy(vol_type, "FAT-16");
            break;
        case 07:
            strcpy(vol_type, "NTFS");
            break;
        case 0x0B:
            strcpy(vol_type, "FAT-32 (CHS)");
            break;
        case 0x0C:
            strcpy(vol_type, "FAT-32 (LBA)");
            break;
        case 0x0E:
            strcpy(vol_type, "FAT-16 (LBA)");
            break;
        default:
            strcpy(vol_type, "NOT-DECODED");
            break;
        }
        // Print out partition content
        printf("Partition %d: Type: %-12s Start: %-12d Size: %-12d\n", i, vol_type, part_entry[i].start_sect,
               part_entry[i].size);
    }
    printf("\nThe total number of valid partitions is: %d\n\n", (4 - not_exist));
    for(int i = 0; i < 4; i++) {
        if(part_entry[i].type == 6){
            parseFAT16(part_entry[0].start_sect, fp);
        }
    }
    fclose(fp);
    return (0);
}
void parseFAT16(int startSector, FILE *fp) { //print the information on a FAT16 partition
    char buf_volume_table[64]; //64 bytes of partition information
    fseek(fp, BPS * startSector, SEEK_SET);
    fread(buf_volume_table, 1, 64, fp);
    FAT16.RA = *(short *)(buf_volume_table + 0x0E); //1 word 2 bytes
    FAT16.copies = *(char *)(buf_volume_table + 0x10); //1 byte
    FAT16.FATsize = *(short *)(buf_volume_table + 0x16); //1 word 2 bytes
    FAT16.rootDirEntires = *(short *)(buf_volume_table + 0x11); //1 word 2 bytes
    FAT16.sectorsPerCluster = *(char *)(buf_volume_table + 0x0D); //1 byte
    FAT16.bytesPerSector = *(short *)(buf_volume_table + 0x0B); //1 word 2 bytes
    int FATarea = FAT16.FATsize * FAT16.copies; // size of FAT table * the number of copies of the FAT table
    int firstSectorDA = startSector + FAT16.RA + FATarea; //start sector + reserved area + FAT area
    int rootDirSize = (FAT16.rootDirEntires * 32) / FAT16.bytesPerSector; //number of entires * directory entry size / bytes per sector
    FAT16.cluster2 = firstSectorDA + rootDirSize;
    printf("----====FAT16 information====----\n");
    printf("Size of Root Directory (Sectors):   %d\n", rootDirSize);
    printf("First sector of FAT Area:           %d\n", startSector + FAT16.RA);
    printf("Sectors per Cluster:                %d\n", FAT16.sectorsPerCluster);
    printf("First sector of Data Area:          %d\n", firstSectorDA);
    printf("Cluster #2 location:                %d\n\n", firstSectorDA + rootDirSize);
    peekFAT16(firstSectorDA, fp);
}
void parseNTFS(int startSector, FILE *fp) {
    fseek(fp, BPS * startSector + 0x0B, SEEK_SET);
}
void peekFAT16(int firstSectorDA, FILE *fp) { //look at the files in the root directory of a FAT16 partition
    char tmpFile[32]; //32 byte directory entry
    int deleted = 0, total = 0;
    printf("----====Files located in FAT16 partition====----\n");
    fseek(fp, BPS * firstSectorDA, SEEK_SET);
    fread(tmpFile, 1, 32, fp);
    int fileCount = 0;
    while(tmpFile[0] != 0) {
        if(tmpFile[0x0B] != 0x0F) {//checks for long file name
            for(int i = 0; i < 11; i++) {
                currentFile.fileName[i] = (char)tmpFile[i]; //11 byte file name
            }
            currentFile.fileSize = *(int *)(tmpFile + 0x1C); //4 byte file size
            currentFile.startingCluster = *(short *)(tmpFile + 0x1A); //2 byte starting cluster
            int startSector = (FAT16.cluster2 + ((currentFile.startingCluster - 2) * FAT16.sectorsPerCluster));//calculates the starting sector of the file
            if(tmpFile[0] < 0) {//checks if the file is deleted
                printf("Deleted File Name: %12s Size: %7d Starting Cluster: %3d Starting Sector: %5d\n", currentFile.fileName, currentFile.fileSize, currentFile.startingCluster, startSector);
                deleted++;
            } else {
                printf("File Name:         %12s Size: %7d Starting Cluster: %3d Starting Sector: %5d\n", currentFile.fileName, currentFile.fileSize, currentFile.startingCluster, startSector);
                total++;
            }
            outputFileFromPartition(currentFile.startingCluster, currentFile.fileName, fp, currentFile.fileSize);
        }   
        fileCount++;
        fseek(fp, BPS * firstSectorDA + 32*fileCount, SEEK_SET);
        fread(tmpFile, 1, 32, fp);
    }
    printf("\nTotal files recovered: %d \nTotal deleted files recovered: %d\n\n", total, deleted);
}
void outputFileFromPartition(int startingCluster, char name[], FILE *fp, int size) {
    char data[size]; //stores every byte of the file in an array
    FILE *out= fopen(name,"w"); //creates output file from the file name in the partition
    fseek(fp, BPS * (FAT16.cluster2 + ((startingCluster - 2) * FAT16.sectorsPerCluster)), SEEK_SET);
    fread(data, 1, size, fp);
    for(int i = 0; i < size; i++) {
        fprintf(out, "%c", data[i]); //prints the data to an output file
    }
}