
// Authors: 
// Ashutosh Yadav 18249094 
// Emma Hurley 17212723
// Conall McAteer 18173586

// Initial Program adapted from ADDENDUM 3 in project requirements inclusive of Phase 1 and Phase 2 work

// https://tldp.org/HOWTO/Partition-Mass-Storage-Definitions-Naming-HOWTO/x190.html used for partition types with complete list. 

// Phase 1 updated to add linux partition types as per Phase 1 feedback  


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


struct Partition{ int type; int start_sect; int size;} part_entry[4]; // 4 x partition table entry
struct fatPartition{ int sect_per_cluster; int fat_area; int root_dir; int clust2Add; } fat_Info;


int main(int argc, char *argv[]){
    if (argv[1] != NULL){

        // Define some variables
        int i, offset = 16, root_offset = 32, not_exist = 0;
        FILE *fp;
        char buf_part_table[64], vol_type[12], buf_fat_part[512], buf_root_part[512], buf_del_part[512];
        int *tmp_ptr, fs, fr, fileseek_FAT, fileread_FAT, fileseek_root, fileread_root, fsDEL,frDEL, first_partition_Saddress, sizeSectA, sizeSectB, noFatCopies, maxDirectories, rootDirSize, rootDirAddr, sizeReservedArea, sectorAddr, sizeReservedArea2;

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

                case 00 : strcpy ( vol_type, "NOT-VALID"); break;
                case 0x0B: strcpy ( vol_type,"FAT-32"); break;
                case 0x0C : strcpy( vol_type, "FAT-32 (LBA)"); break;
                case 0x0E : strcpy( vol_type, "FAT-16 (LBA)"); break;       
                case 0x01 : strcpy ( vol_type, "FAT-12"); break;
                case 0x02 : strcpy ( vol_type, "XENIX root"); break;               
                case 0x03 : strcpy ( vol_type, "XENIX usr"); break;            
                case 0x05 : strcpy ( vol_type, "MS-DOS"); break;
                case 0x06 : strcpy ( vol_type, "FAT-16"); break;
                case 0x07 : strcpy ( vol_type, "NTFS"); break;
                case 0x08 : strcpy ( vol_type, "AIX"); break;
                case 0x09 : strcpy ( vol_type, "AIX Bootable"); break;
                case 0x16 : strcpy ( vol_type, "Hidden FAT16"); break;
                case 0x17 : strcpy (vol_type, "Hidden NTFS Partition"); break;
                case 0x82 : strcpy ( vol_type, "Linux Swap"); break;
                case 0x83 : strcpy ( vol_type, "Linux FS"); break;
                case 0x1B : strcpy (vol_type, "Hidden FAT-32"); break;                          
                
                default: strcpy ( vol_type, "NOT-DECODED"); break;
            }
            
            // Print out partition content
            printf ("Partition %d: Type: %-12s Start: %-12d Size: %-12d\n", i, vol_type, part_entry[i].start_sect,
            part_entry[i].size);
            }
            printf("\n\nThe total number of valid partitions is: %d\n\n", (4 - not_exist));
            printf("################################################################## \n");



      
/* 

    PHASE 2 

    Write the full tool to display all the required features as stated above:
    a) Partition information,
    b) FAT Volume formation
    c) NTFS Volume information (see Addendum-1)
    
*/

/* ################################################################## */

/* Fat Volume Information */

			first_partition_Saddress = part_entry[0].start_sect * 512;
      fileseek_FAT = fseek(fp, first_partition_Saddress, SEEK_SET); //Seek to the start of the part_entry list
      fileread_FAT = fread(buf_fat_part, 1, 512, fp); // Read the 512-byte block to memory buffer
      

      printf("\n");
      
      
      //the number of sectors per cluster
      fat_Info.sect_per_cluster = *(char*)(buf_fat_part + 0xd ); // Char can store 512 thats why we use it
      printf("The sectors per cluster is: %-12d", fat_Info.sect_per_cluster);  //Printing the sectors per cluster
      
      
      //the size of the FAT area,
      //Fat area size = (Size of FAT in sectors) * (Number of FAT copies)
      noFatCopies = *(char*)(buf_fat_part + 0x10);
      sizeSectA = *(unsigned char*)(buf_fat_part + 0x16);
      sizeSectB = *(char*)(buf_fat_part + 0x17);
      sizeSectB = (sizeSectB<<8) ;
      sizeSectA = sizeSectA + sizeSectB;      
      fat_Info.fat_area = sizeSectA * noFatCopies;
      printf("\nThe fat area is: %-12d", fat_Info.fat_area);  //Printing the Fat area
      
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
        rootDirAddr = (part_entry[0].start_sect) + (sizeReservedArea) + fat_Info.fat_area;
        sectorAddr = rootDirAddr + rootDirSize;
        printf("\nThe sector address for #2 is: %d", sectorAddr);


        printf("\nThe root dir addr: %d \n ", rootDirAddr);


        





/* ################################################################## */

/* NTFS Volume Information */

/* 
    Program requirements
    You program will examine the NTFS file partition volume and report the following
    information to the screen:
    - How many bytes per sector for this NTFS volume
    - How many sectors per cluster for this NTFS volume
    - What is the sector address for the $MFT file record
    - What is the type and length of the first attribute in the $MFT record
     - What ids the type and length of the second attribute in the $MFT record
*/

/* ################################################################## */

     
        int ntfsSS, fsNTFS, frNTFS, bpsC, bpsB, MFTSect, fsMFT, frMFT;
        char MFTSectAddr[8];
        char buf_ntfs_table[64], buff_mtf_table[512] , buff_OTW_table[512];

        struct NTFSpart{ int bytesPerSect; int SectorsPerClus; int SectorAddrMFT; int type1, type2, length1, length2; } NTFSinfo;
        //NTFS Volume Information
        for(int r = 0; r < 4; r++){
          if(part_entry[r].type == 0x07){ // checks for NTFS
            ntfsSS = part_entry[r].start_sect;
						
            printf("\nStarting Sector (NTFS): %d", ntfsSS);
            fsNTFS = fseek(fp, (ntfsSS*512), SEEK_SET); //Seek to the start of the part_entry list
            frNTFS = fread(buf_ntfs_table, 1, 64, fp); // Read the 512-byte block to memory buffer

           //How many byes per sector for this NTFS volume
            bpsB = *(unsigned char*)(buf_ntfs_table + 0x0B);
            bpsC = *(unsigned char*)(buf_ntfs_table + 0x0C);
            bpsC = (bpsC << 8);
            NTFSinfo.bytesPerSect = bpsC + bpsB;
            printf("\nBytes per sector for this NTFS volume: %d",NTFSinfo.bytesPerSect);


          //How many sectors per cluster for this NTFS volume
            NTFSinfo.SectorsPerClus = *(unsigned char*)(buf_ntfs_table + 0x0D);
            printf("\nSectors Per Cluster for this NTFS volume: %d",NTFSinfo.SectorsPerClus);

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

            printf("\nSector address for the $MFT file record: %d", NTFSinfo.SectorAddrMFT);


            /*     
            - What is the sector address for the $MFT file record
            - What is the type and length of the first attribute in the $MFT record

             */

            

            /* Type of first attribute */

            fsMFT = fseek(fp, ((ntfsSS + NTFSinfo.SectorAddrMFT)*512), SEEK_SET); //Seek to the start of the part_entry list
            frMFT = fread(buff_OTW_table, 1, 512, fp); // Read the 512-byte block to memory buffer
            
            int Attrib1 = *(unsigned char*)(buff_OTW_table + 0x14);
            int Attrib1b = *(unsigned char*)(buff_OTW_table + 0x15);
            Attrib1 =  (Attrib1b << 8) + Attrib1; //GIVES US 56

            printf("\n");



            /* Start of first attribute for $MFT */
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

                default: strcpy (typeName, "Type not found!"); break;
                case 16 : strcpy (typeName, "$Standard_Information"); break;
                case 32 : strcpy (typeName, "$Attribute_List"); break;
                case 48 : strcpy (typeName, "$File_Name"); break;
                case 64 : strcpy (typeName, "$Object_ID"); break;
                case 80 : strcpy (typeName, "$Security_Descriptor"); break;
                case 96 : strcpy (typeName, "$Volume_Name"); break;
                case 122: strcpy (typeName,"$Volume_Info"); break;
                case 128: strcpy (typeName, "$Data"); break;
                case 144: strcpy (typeName, "$Index_Root"); break;
                case 160: strcpy (typeName, "$Index_Allocation"); break; //check this
                case 176 : strcpy (typeName, "$BitMap"); break;
                case 192 : strcpy (typeName, "$Reparse_Point"); break;
                case 256 : strcpy (typeName, "$Logged_Utility_Stream"); break;
                
            }
            printf("\nType of Attribute #1: %s",typeName);


            /* Length of first attribute */

            char AttribLen[4];
            AttribLen[0] = *(unsigned char*)(buff_OTW_table + 0x04 + Attrib1);
            AttribLen[1] = *(unsigned char*)(buff_OTW_table + 0x05 + Attrib1);
            AttribLen[2] = *(unsigned char*)(buff_OTW_table + 0x06 + Attrib1);
            AttribLen[3] = *(unsigned char*)(buff_OTW_table + 0x07 + Attrib1);

            NTFSinfo.length1 = AttribLen[3] << 24;
            NTFSinfo.length1 =NTFSinfo.length1 + (AttribLen[2] << 16);
            NTFSinfo.length1 =NTFSinfo.length1 + (AttribLen[1] << 8);
            NTFSinfo.length1 =NTFSinfo.length1 + AttribLen[0];
            printf("\nLength of Attribute #1: %d", NTFSinfo.length1);

            
            
            
            /* Start of second attribute */
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

            printf("\nType of Attribute #2: %s", typeName2);


            /* Length of second Attribute */

            char Attrib2Len[4];
            Attrib2Len[0] = *(unsigned char*)(buff_OTW_table + 0x04 + Attrib2);
            Attrib2Len[1] = *(unsigned char*)(buff_OTW_table + 0x05 + Attrib2);
            Attrib2Len[2] = *(unsigned char*)(buff_OTW_table + 0x06 + Attrib2);
            Attrib2Len[3] = *(unsigned char*)(buff_OTW_table + 0x07 + Attrib2);

            NTFSinfo.length2 = Attrib2Len[3] << 24;
            NTFSinfo.length2 =NTFSinfo.length2 + (Attrib2Len[2] << 16);
            NTFSinfo.length2 =NTFSinfo.length2 + (Attrib2Len[1] << 8);
            NTFSinfo.length2 =NTFSinfo.length2 + Attrib2Len[0];

            printf("\nLength of Attribute #2: %d", NTFSinfo.length2);

            printf("\n");


            /* Deleted file details (FAT) */

            fileseek_root = fseek(fp, rootDirAddr * 512 , SEEK_SET); //Seek to the start of the part_entry list
            fileread_root = fread(buf_root_part, 1, 512, fp); // Read the 512-byte block to memory buffer

            for(int j = 0; j < fileread_root; j++){
          	int fbyte = *(char*)(buf_part_table +(j * root_offset));
            int k = 1;
            fbyte = fbyte & 0x000000ff;
            char f_ten[11];
            f_ten[0] = fbyte;

            /* Name of deleted file operation */

            /* https://www.ntfs.com/disk-scan.htm#:~:text=For%20example%2C%20on%20FAT%20any,has%20been%20deleted%20or%20not. */

            /* on FAT any deleted entry, file or folder are marked with ASCII symbol 229 (0xE5) that becomes the first symbol of the entry */

            if(fbyte == 0xe5){
            	printf("\nName of 1st Deleted File (FAT): %c", fbyte);
              for(k; k <= 11; k++){
                f_ten[k] = *(char*)(buf_part_table +(j * root_offset) + k);
                printf("%c",f_ten[k]);
              }             
          }

          
        }

        printf("\n");

          }
        }

        fclose(fp);

    }else{

        /* No image file passed to program */
        printf("Please pass an image file (.dd) when using this program\n");
    }

    

    return(0);
}

/* END of File */









	
