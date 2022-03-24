
// Authors: 
// Ashutosh Yadav 18249094 
// Emma Hurley 17212723
// Conall McAteer 18173586

// Initial Program adapted from ADDENDUM 3 in project requirements inclusive of Phase 1 and Phase 2 work


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct Partition{ int type; int start_sect; int size;} part_entry[4]; // 4 x partition table entry
struct FATpart{ int sectPerCluster; int FATarea; int rootDir; int clust2Add; } FATinfo;


int main(int argc, char *argv[])
{
    if (argv[1] != NULL){

        // Define some variables
        int i, offset = 16, rootOffset = 32, not_exist = 0;
        FILE *fp;
        char buf_part_table[64], vol_type[12], buf_fat_part[512], buf_root_part[512], buf_del_part[512];

        int *tmp_ptr, fs, fr, fsFAT, frFAT, fsRoot, frRoot, fsDEL,frDEL, firstPartADD, sizeSectA, sizeSectB, noFatCopies, maxDirectories, rootDirSize, rootDirAddr, sizeReservedArea, sectorAddr, sizeReservedArea2;
        
        
        
   
        fp = fopen(argv[1], "rb"); // Open file for reading - binary mode. Should use error check!
        fs = fseek(fp, 0x1BE, SEEK_SET); // Seek to the start of the part_entry list
        fr = fread(buf_part_table, 1, 64, fp); // Read the 64-byte block to memory buffer

        printf("############################## FORENSIC_TOOL_PHASE 1 #################################### \n");
        printf("############################## Partitions on %s disk image ############################## \n", argv[1]);


        for (i=0; i <4; i++) {
            part_entry[i].type = *(char*)(buf_part_table + 0x04 +(i * offset) );
            if (part_entry[i].type == 0) not_exist++;
            part_entry[i].start_sect = *(int*)(buf_part_table + 0x08 +(i * offset));
            part_entry[i].size = *(int*)(buf_part_table + 0x0C + (i * offset)) ;
            
            switch (part_entry[i].type) {
                case 0x00 : strcpy ( vol_type, "NOT-VALID"); break;
                case 0x01 : strcpy ( vol_type, "FAT-12"); break;
                case 0x02 : strcpy ( vol_type, "XENIX root"); break;
                case 0x05 : strcpy ( vol_type, "MS-DOS"); break;
                case 0x06 : strcpy ( vol_type, "FAT-16"); break;
                case 0x07 : strcpy ( vol_type, "NTFS"); break;
                
                case 0x0B: strcpy ( vol_type,"FAT-32"); break;
                case 0x0C: strcpy (vol_type, "FAT-32 (INT 13 extension)"); break;
                case 0x0E: strcpy (vol_type, "FAT-16 (>= 32MB, INT 13 extension)"); break;
                case 0x0F: strcpy (vol_type, "E FAT-16"); break; //check this
                /* Hidden file systems */
                case 0x14 : strcpy (vol_type, "Hidden FAT-16 (<32mb)"); break;
                case 0x16 : strcpy ( vol_type, "Hidden FAT16"); break;
                case 0x17 : strcpy (vol_type, "Hidden NTFS Partition"); break;
                case 0x1B : strcpy (vol_type, "Hidden FAT-32"); break;
                case 0x1C : strcpy (vol_type, "Hidden FAT-32 (Ext. INT 13)"); break;
                case 0x1E : strcpy (vol_type, "Hidden FAT-16 (>32 MB, Ext. INT 13)"); break;
                //end of the hidden systems

                case 0x42 : strcpy (vol_type, "Secure File System"); break;
                case 0x82 : strcpy (vol_type, "Linux Swap partition"); break;
                case 0x83 : strcpy (vol_type, "Linux Native File System (ext2/3/4)"); break;
                
                default: strcpy ( vol_type, "NOT-DECODED"); break;
            }
            
            // Print out partition content
            printf ("Partition %d: Type: %-12s Start: %-12d Size: %-12d\n", i, vol_type, part_entry[i].start_sect,
            part_entry[i].size);
            }
            printf("\n\nThe total number of valid partitions is: %d\n\n", (4 - not_exist));
            printf("################################################################## \n");



      
//Do Phase 2 here
    //FAT Volume Information
    //For the first partition only (Fat16 only) display:
      //First partition SECTOR address, Jumps to offset of FAT 16
			firstPartADD = part_entry[0].start_sect * 512;
      
      fsFAT = fseek(fp, firstPartADD, SEEK_SET); //Seek to the start of the part_entry list
      
      frFAT = fread(buf_fat_part, 1, 512, fp); // Read the 512-byte block to memory buffer

      printf("\n");
    //the number of sectors per cluster
      FATinfo.sectPerCluster = *(char*)(buf_fat_part + 0xd ); // Char can store 512 thats why we use it
      printf("The sectors per cluster is: %-12d", FATinfo.sectPerCluster);  //Printing the sectors per cluster
      
    //the size of the FAT area,
      //Fat area size = (Size of FAT in sectors) * (Number of FAT copies)
      noFatCopies = *(char*)(buf_fat_part + 0x10);
      sizeSectA = *(unsigned char*)(buf_fat_part + 0x16);
      sizeSectB = *(char*)(buf_fat_part + 0x17);
      sizeSectB = (sizeSectB<<8) ;
      sizeSectA = sizeSectA + sizeSectB;      
      FATinfo.FATarea = sizeSectA * noFatCopies;
      printf("\nThe fat area is: %-12d", FATinfo.FATarea);  //Printing the Fat area
      
                //the size of the root direcotry
                    //Root dir size = ((Max no. of directories)*(dir entry size in bytes)/sector size
        maxDirectories =  *(int*)(buf_fat_part + 0x11);
        rootDirSize = (maxDirectories * 32)/512;
        printf("\nThe root directory size: %d", rootDirSize);


    //the sector address of Cluster #2,
    //First Sector of data area = first sector of volume + size of reserved area + size of fat area
    //Sector Address of #2 = (First sector of data area ) + (Size of root dir)

        sizeReservedArea = (*(char*)(buf_fat_part + 0x0E));
        sizeReservedArea2 = (*(char*)(buf_fat_part + 0x0F));

        sizeReservedArea2 = sizeReservedArea2<<8;
        sizeReservedArea = sizeReservedArea2 + sizeReservedArea;
        printf("\nThe SIZE OF RESERVED AREA is: %-12d", sizeReservedArea);
        printf("\nThe start sec is: %-12d", part_entry[0].start_sect);
        rootDirAddr = (part_entry[0].start_sect) + (sizeReservedArea) + FATinfo.FATarea;
        sectorAddr = rootDirAddr + rootDirSize;
        printf("\nThe sector address for #2 is: %d", sectorAddr);


        //For the first deleted file on the volume's root directory display:
        //Notes: 0XE5 code tells us that its a deleted file which is 229 decimal
        printf("\nThe root dir addr: %d \n ", rootDirAddr);

        //Multiplied by 512 because that is how many directory entries per sector. (FAT16 info)
        fsRoot = fseek(fp, rootDirAddr * 512 , SEEK_SET); //Seek to the start of the part_entry list
        frRoot = fread(buf_root_part, 1, 512, fp); // Read the 512-byte block to memory buffer
        
        for(int j = 0; j < frRoot; j++){
          	int firstByte = *(char*)(buf_part_table +(j * rootOffset));
            int k = 1;
            firstByte = firstByte & 0x000000ff;
            char firstTEN[11];
            firstTEN[0] = firstByte;

            //the name of that file
            if(firstByte == 0xe5){
            	printf("\nThe name of the first deleted file is: %c", firstByte);
              for(k; k <= 11; k++){
                firstTEN[k] = *(char*)(buf_part_table +(j * rootOffset) + k);
                printf("%c",firstTEN[k]);
              }

                //the size of that file
                int sizeDelFile;
                int sizeDelFileArr[4];
                sizeDelFileArr[0] = *(unsigned char*)(buf_part_table +(j * rootOffset) + 0x1C);
                sizeDelFileArr[1] = *(unsigned char*)(buf_part_table +(j * rootOffset) + 0x1D);
                sizeDelFileArr[2] = *(unsigned char*)(buf_part_table +(j * rootOffset) + 0x1E);
                sizeDelFileArr[3] = *(unsigned char*)(buf_part_table +(j * rootOffset) + 0x1F);
                sizeDelFile = (sizeDelFileArr[3] << 24);
                sizeDelFile = sizeDelFile + (sizeDelFileArr[2] << 16);
                sizeDelFile = sizeDelFile + (sizeDelFileArr[1] << 8);
                sizeDelFile = sizeDelFile + (sizeDelFileArr[0]);
                //printf("\n%X%X%X%X", sizeDelFileArr[3],sizeDelFileArr[2],sizeDelFileArr[1],sizeDelFileArr[0]);
                printf("\nDeleted File Size in bytes: %-12d", sizeDelFile);
                
                
                
                //the number of the first cluster
                int startingCluster;
                int SCArr[2];
                SCArr[0] = *(unsigned char*)(buf_part_table +(j * rootOffset) + 0x1A);
                SCArr[1] = *(unsigned char*)(buf_part_table +(j * rootOffset) + 0x1B);
                startingCluster = (SCArr[1] << 8);
                startingCluster = startingCluster + SCArr[0];
                printf("\nDeleted File Starting Cluster: %-12d", startingCluster);


                //The first 16 characters of the content of that file (Assume simple text file)
                int CSA = sectorAddr +  ((startingCluster - 2) * 8);
                printf("\nCSA is: %-12d\n", CSA);

                int fileRead = CSA * 512;
                fsDEL = fseek(fp, fileRead, SEEK_SET); //Seek to the start of the part_entry list
                frDEL = fread(buf_del_part, 1, 512, fp); // Read the 512-byte block to memory buffer
                printf("The first 16 Characters are: ");
                for (int w = 0; w <= 16; w++)
                {
                    printf("%c", buf_del_part[w]);
                }
                break;
            }
        }
        printf("\n\n\n");

        //NTFS Vars here
        int ntfsSS, fsNTFS, frNTFS, bpsC, bpsB, MFTSect, fsMFT, frMFT;
        char MFTSectAddr[8];
        char buf_ntfs_table[64], buff_mtf_table[512] , buff_OTW_table[512];

        struct NTFSpart{ int bytesPerSect; int SectorsPerClus; int SectorAddrMFT; int type1, type2, length1, length2; } NTFSinfo;
        //NTFS Volume Information
        for(int r = 0; r < 4; r++){
          if(part_entry[r].type == 0x07){
            ntfsSS = part_entry[r].start_sect;
						
            printf("\nNTFS Starting Sector: %d", ntfsSS);
            fsNTFS = fseek(fp, (ntfsSS*512), SEEK_SET); //Seek to the start of the part_entry list
            frNTFS = fread(buf_ntfs_table, 1, 64, fp); // Read the 512-byte block to memory buffer

           //How many byes per sector for this NTFS volume
            bpsB = *(unsigned char*)(buf_ntfs_table + 0x0B);
            bpsC = *(unsigned char*)(buf_ntfs_table + 0x0C);
            bpsC = (bpsC << 8);
            NTFSinfo.bytesPerSect = bpsC + bpsB;
            printf("\nBytes Per Sector: %d",NTFSinfo.bytesPerSect);


          //How many sectors per cluster for this NTFS volume
            NTFSinfo.SectorsPerClus = *(unsigned char*)(buf_ntfs_table + 0x0D);
            printf("\nSectors Per Cluster: %d",NTFSinfo.SectorsPerClus);

          //What is the sector address for the $MFT file record
            MFTSectAddr[0]= *(unsigned char*)(buf_ntfs_table + 0x30);
            MFTSectAddr[1]= *(unsigned char*)(buf_ntfs_table + 0x31);
            MFTSectAddr[2]= *(unsigned char*)(buf_ntfs_table + 0x32);
            MFTSectAddr[3]= *(unsigned char*)(buf_ntfs_table + 0x33);
            MFTSectAddr[4]= *(unsigned char*)(buf_ntfs_table + 0x34);
            MFTSectAddr[5]= *(unsigned char*)(buf_ntfs_table + 0x35);
            MFTSectAddr[6]= *(unsigned char*)(buf_ntfs_table + 0x36);
            MFTSectAddr[7]= *(unsigned char*)(buf_ntfs_table + 0x37);
            MFTSect = (MFTSectAddr[7] << 56);
            MFTSect = MFTSect + (MFTSectAddr[6] << 48);
            MFTSect = MFTSect + (MFTSectAddr[5] << 40);
            MFTSect = MFTSect + (MFTSectAddr[4] << 32);
            MFTSect = MFTSect + (MFTSectAddr[3] << 24);
            MFTSect = MFTSect + (MFTSectAddr[2] << 16);
            MFTSect = MFTSect + (MFTSectAddr[1] << 8);
            MFTSect = MFTSect + (MFTSectAddr[0]);
            NTFSinfo.SectorAddrMFT = MFTSect * NTFSinfo.SectorsPerClus;

            printf("\nSector Address for $MFT: %d", NTFSinfo.SectorAddrMFT);

            
            //What is the type and length of the first two attributes in the $MFT record

            //TYPE OF FIRST ATTRIB
            fsMFT = fseek(fp, ((ntfsSS + NTFSinfo.SectorAddrMFT)*512), SEEK_SET); //Seek to the start of the part_entry list
            frMFT = fread(buff_OTW_table, 1, 512, fp); // Read the 512-byte block to memory buffer
            
            int Attrib1 = *(unsigned char*)(buff_OTW_table + 0x14);
            int Attrib1b = *(unsigned char*)(buff_OTW_table + 0x15);
            Attrib1 =  (Attrib1b << 8) + Attrib1; //GIVES US 56



              //Start of first attribute for MFT 
            char AttribType[4];
            AttribType[0] = *(unsigned char*)(buff_OTW_table + 0x00 + Attrib1);
            AttribType[1] = *(unsigned char*)(buff_OTW_table + 0x01 + Attrib1);
            AttribType[2] = *(unsigned char*)(buff_OTW_table + 0x02 + Attrib1);
            AttribType[3] = *(unsigned char*)(buff_OTW_table + 0x03 + Attrib1);

            NTFSinfo.type1 = AttribType[3] << 24;
            NTFSinfo.type1 =NTFSinfo.type1 + (AttribType[2] << 16);
            NTFSinfo.type1 =NTFSinfo.type1 + (AttribType[1] << 8);
            NTFSinfo.type1 =NTFSinfo.type1 + AttribType[0];
            
            char typeName[50];

            switch (NTFSinfo.type1) {
                case 16 : strcpy (typeName, "Standard Information"); break;
                case 32 : strcpy (typeName, "Attribute List"); break;
                case 48 : strcpy (typeName, "File Name"); break;
                case 64 : strcpy (typeName, "Object ID"); break;
                case 80 : strcpy (typeName, "Security Descriptor"); break;
                case 96 : strcpy (typeName, "Volume Name"); break;
                case 122: strcpy (typeName,"Volume Info"); break;
                case 128: strcpy (typeName, "Data"); break;
                case 144: strcpy (typeName, "Index Root"); break;
                case 160: strcpy (typeName, "Index Allocation"); break; //check this
                case 176 : strcpy (typeName, "BitMap"); break;
                case 192 : strcpy (typeName, "Reparse Point"); break;
                case 256 : strcpy (typeName, "Logged Utility Stream"); break;
                default: strcpy (typeName, "Type not found!"); break;
            }
            printf("\nAttribute 1 Type: %s",typeName);


            //LENGTH OF FIRST ATTRIB
            char AttribLen[4];
            AttribLen[0] = *(unsigned char*)(buff_OTW_table + 0x04 + Attrib1);
            AttribLen[1] = *(unsigned char*)(buff_OTW_table + 0x05 + Attrib1);
            AttribLen[2] = *(unsigned char*)(buff_OTW_table + 0x06 + Attrib1);
            AttribLen[3] = *(unsigned char*)(buff_OTW_table + 0x07 + Attrib1);

            NTFSinfo.length1 = AttribLen[3] << 24;
            NTFSinfo.length1 =NTFSinfo.length1 + (AttribLen[2] << 16);
            NTFSinfo.length1 =NTFSinfo.length1 + (AttribLen[1] << 8);
            NTFSinfo.length1 =NTFSinfo.length1 + AttribLen[0];
            printf("\nAttribute 1 Length: %d", NTFSinfo.length1);

            //Start of the Second Attribute
            int Attrib2 = Attrib1 +  NTFSinfo.length1;

            char Attrib2Type[4];
            Attrib2Type[0] = *(unsigned char*)(buff_OTW_table + 0x00 + Attrib2);
            Attrib2Type[1] = *(unsigned char*)(buff_OTW_table + 0x01 + Attrib2);
            Attrib2Type[2] = *(unsigned char*)(buff_OTW_table + 0x02 + Attrib2);
            Attrib2Type[3] = *(unsigned char*)(buff_OTW_table + 0x03 + Attrib2);

            NTFSinfo.type2 = Attrib2Type[3] << 24;
            NTFSinfo.type2 =NTFSinfo.type2 + (Attrib2Type[2] << 16);
            NTFSinfo.type2 =NTFSinfo.type2 + (Attrib2Type[1] << 8);
            NTFSinfo.type2 =NTFSinfo.type2 + Attrib2Type[0];

            char typeName2[50];
              switch (NTFSinfo.type2) {
                case 16 : strcpy (typeName2, "Standard Information"); break;
                case 32 : strcpy (typeName2, "Attribute List"); break;
                case 48 : strcpy (typeName2, "File Name"); break;
                case 64 : strcpy (typeName2, "Object ID"); break;
                case 80 : strcpy (typeName2, "Security Descriptor"); break;
                case 96 : strcpy (typeName2, "Volume Name"); break;
                case 122: strcpy (typeName2,"Volume Info"); break;
                case 128: strcpy (typeName2, "Data"); break;
                case 144: strcpy (typeName2, "Index Root"); break;
                case 160: strcpy (typeName2, "Index Allocation"); break; //check this
                case 176 : strcpy (typeName2, "BitMap"); break;
                case 192 : strcpy (typeName2, "Reparse Point"); break;
                case 256 : strcpy (typeName2, "Logged Utility Stream"); break;
                default: strcpy (typeName2, "Type not found!"); break;
            }

            printf("\nAttribute 2 Type: %s", typeName2);


            //LENGTH OF FIRST ATTRIB
            char Attrib2Len[4];
            Attrib2Len[0] = *(unsigned char*)(buff_OTW_table + 0x04 + Attrib2);
            Attrib2Len[1] = *(unsigned char*)(buff_OTW_table + 0x05 + Attrib2);
            Attrib2Len[2] = *(unsigned char*)(buff_OTW_table + 0x06 + Attrib2);
            Attrib2Len[3] = *(unsigned char*)(buff_OTW_table + 0x07 + Attrib2);

            NTFSinfo.length2 = Attrib2Len[3] << 24;
            NTFSinfo.length2 =NTFSinfo.length2 + (Attrib2Len[2] << 16);
            NTFSinfo.length2 =NTFSinfo.length2 + (Attrib2Len[1] << 8);
            NTFSinfo.length2 =NTFSinfo.length2 + Attrib2Len[0];

            printf("\nAttribute 2 Length: %d", NTFSinfo.length2);
          }
        }

        fclose(fp);

    }else{
        printf("Please pass an image file (.dd) when using this program\n");
    }

    return(0);
}









	
