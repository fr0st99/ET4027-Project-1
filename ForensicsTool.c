
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

struct Partition
{
  int type;
  int start_sect;
  int size;
} part_entry[4]; // 4 x partition table entry
int main(int argc, char *argv[])
{
  if (argv[1] != NULL)
  { /* Check if file is present or not */

    // Define some variables
    int i, offset = 16, offset_root = 32, not_exist = 0;
    FILE *fp;
    char buf_part_table[64], buf_delete_part[512], vol_type[12], buf_fat_part[512], buf_root_part[512];
    int *tmp_ptr, fileseek_FAT, fileread_FAT, fileseek_root, fileread_root, first_partition_Saddress, sect_size_A, sect_size_B, null_FAT_CP, max_Dirs, root_dir_size, root_dir_address, reserved_area_size1, sector_Address, reserved_area_size2;
    int fileseek_delete, fileread_delete;
    struct fatPartition
    {
      int sect_per_cluster;
      int fat_area;
      int root_dir;
    } fat_Info;
    fp = fopen(argv[1], "rb");        // Open file for reading - binary mode. Should use error check!
    fseek(fp, 0x1BE, SEEK_SET);       // Seek to the start of the part_entry list
    fread(buf_part_table, 1, 64, fp); // Read the 64-byte block to memory buffer

    printf("\n");
    printf("\n");
    printf("############################## FORENSIC_TOOL_COMPLETE #################################### \n");
    printf("############################## Partitions on %s disk image ############################## \n", argv[1]);
    printf("\n");

    for (i = 0; i < 4; i++)
    {
      part_entry[i].type = *(char *)(buf_part_table + 0x04 + (i * offset));
      if (part_entry[i].type == 0)
        not_exist++;
      part_entry[i].start_sect = *(int *)(buf_part_table + 0x08 + (i * offset));
      part_entry[i].size = *(int *)(buf_part_table + 0x0C + (i * offset));

      switch (part_entry[i].type)
      {

      case 00:
        strcpy(vol_type, "NOT-VALID");
        break;
      case 0x0B:
        strcpy(vol_type, "FAT-32");
        break;
      case 0x0C:
        strcpy(vol_type, "FAT-32 (LBA)");
        break;
      case 0x0E:
        strcpy(vol_type, "FAT-16 (LBA)");
        break;
      case 0x01:
        strcpy(vol_type, "FAT-12");
        break;
      case 0x02:
        strcpy(vol_type, "XENIX root");
        break;
      case 0x03:
        strcpy(vol_type, "XENIX usr");
        break;
      case 0x05:
        strcpy(vol_type, "MS-DOS");
        break;
      case 0x06:
        strcpy(vol_type, "FAT-16");
        break;
      case 0x07:
        strcpy(vol_type, "NTFS");
        break;
      case 0x08:
        strcpy(vol_type, "AIX");
        break;
      case 0x09:
        strcpy(vol_type, "AIX Bootable");
        break;
      case 0x16:
        strcpy(vol_type, "Hidden FAT16");
        break;
      case 0x17:
        strcpy(vol_type, "Hidden NTFS Partition");
        break;
      case 0x82:
        strcpy(vol_type, "Linux Swap");
        break;
      case 0x83:
        strcpy(vol_type, "Linux FS");
        break;
      case 0x1B:
        strcpy(vol_type, "Hidden FAT-32");
        break;
      default:
        strcpy(vol_type, "NOT-DECODED");
        break;
      }

      // Print out partition content
      printf("Partition %d: Type: %-12s Start: %-12d Size: %-12d\n", i, vol_type, part_entry[i].start_sect, part_entry[i].size);
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
    fileseek_FAT = fseek(fp, first_partition_Saddress, SEEK_SET); // Seek to the start of the part_entry list
    fileread_FAT = fread(buf_fat_part, 1, 512, fp);

    printf("\n");
    printf("**********************FAT VOLUME INFORMATION**********************");
    printf("\n");
    printf("\n");

    fat_Info.sect_per_cluster = *(char *)(buf_fat_part + 0xd);
    printf("Sectors per cluster: %-12d", fat_Info.sect_per_cluster);

    null_FAT_CP = *(char *)(buf_fat_part + 0x10);
    sect_size_A = *(unsigned char *)(buf_fat_part + 0x16);
    sect_size_B = *(char *)(buf_fat_part + 0x17);
    sect_size_B = (sect_size_B << 8);
    sect_size_A = sect_size_A + sect_size_B;
    fat_Info.fat_area = sect_size_A * null_FAT_CP;
    printf("\nFAT Area: %-12d", fat_Info.fat_area);
    max_Dirs = *(int *)(buf_fat_part + 0x11);
    root_dir_size = (max_Dirs * 32) / 512;
    printf("\nRoot directory size: %d", root_dir_size);

    /*

     Data area = 1st volume + reserved area size + fat area size

     Sector address of Cluster 2 = 1st sector data area + size of root directory

     */

    reserved_area_size1 = (*(char *)(buf_fat_part + 0x0E));
    reserved_area_size2 = (*(char *)(buf_fat_part + 0x0F));
    reserved_area_size2 = reserved_area_size2 << 8;
    reserved_area_size1 = reserved_area_size2 + reserved_area_size1;
    printf("\nSize of reserved area: %-12d", reserved_area_size1);
    printf("\nStart sector: %-12d", part_entry[0].start_sect);
    root_dir_address = (part_entry[0].start_sect) + (reserved_area_size1) + fat_Info.fat_area;
    sector_Address = root_dir_address + root_dir_size;
    printf("\nSector address for Cluster #2: %d", sector_Address);
    printf("\n");
    printf("\n##################################################################");
    printf("\n");

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
        - What is the type and length of the second attribute in the $MFT record
    */

    /* ################################################################## */

    printf("\n");
    printf("**********************NTFS VOLUME INFORMATION**********************");
    printf("\n");
    printf("\n");

    /* Variable Declaration */

    char sector_Address_MFT[8];
    char buff_NTFS_table[64];
    char buff_offset_table[512];

    /* Attribute declarations */

    /* Attribute 1 type and length */

    char attribute1_type[4];
    char attribute1_length[4];
    char type_attribute1[50];

    /* Attribute 2 type and length */

    char attribute2_length[4];
    char attribute2_type[4];
    char type_attribute2[50];

    int NTFS_sectorSize;
    int fileseek_NTFS;
    int fileread_NTFS;
    int bytes_per_sector_0C;
    int bytes_per_sector_0B;
    int sector_MFT;
    int fileseek_MFT;
    int fileread_MFT;

    struct NTFS_Partition
    {

      int sp_Cluster;
      int bp_Sector;
      int s_addressMFT;
      int attrib_1, attrib_2, attrib_len_1, attrib_len_2;

    } info_NTFS;

    for (int r = 0; r < 4; r++)
    {

      /* Check if NTFS (0x07) */

      if (part_entry[r].type == 0x07)
      {

        NTFS_sectorSize = part_entry[r].start_sect;
        fileseek_NTFS = fseek(fp, (NTFS_sectorSize * 512), SEEK_SET);
        fileread_NTFS = fread(buff_NTFS_table, 1, 64, fp);

        /* - How many byes per sector for this NTFS volume */

        bytes_per_sector_0B = *(unsigned char *)(buff_NTFS_table + 0x0B);
        bytes_per_sector_0C = *(unsigned char *)(buff_NTFS_table + 0x0C);
        bytes_per_sector_0C = (bytes_per_sector_0C << 8);
        info_NTFS.bp_Sector = bytes_per_sector_0C + bytes_per_sector_0B;

        printf("\nBytes per sector for this NTFS volume: %d", info_NTFS.bp_Sector);

        /* How many sectors per cluster for this NTFS volume */

        info_NTFS.sp_Cluster = *(unsigned char *)(buff_NTFS_table + 0x0D);
        printf("\nSectors Per Cluster for this NTFS volume: %d", info_NTFS.sp_Cluster);

        /* - What is the sector address for the $MFT file record */

        sector_Address_MFT[0] = *(unsigned char *)(buff_NTFS_table + 0x30);
        sector_Address_MFT[1] = *(unsigned char *)(buff_NTFS_table + 0x31);
        sector_Address_MFT[2] = *(unsigned char *)(buff_NTFS_table + 0x32);
        sector_Address_MFT[3] = *(unsigned char *)(buff_NTFS_table + 0x33);
        sector_Address_MFT[4] = *(unsigned char *)(buff_NTFS_table + 0x34);
        sector_Address_MFT[5] = *(unsigned char *)(buff_NTFS_table + 0x35);
        sector_Address_MFT[6] = *(unsigned char *)(buff_NTFS_table + 0x36);
        sector_Address_MFT[7] = *(unsigned char *)(buff_NTFS_table + 0x37);

        sector_MFT = (sector_Address_MFT[7] << 56);

        sector_MFT = sector_MFT + (sector_Address_MFT[6] << 48);
        sector_MFT = sector_MFT + (sector_Address_MFT[5] << 40);
        sector_MFT = sector_MFT + (sector_Address_MFT[4] << 32);
        sector_MFT = sector_MFT + (sector_Address_MFT[3] << 24);
        sector_MFT = sector_MFT + (sector_Address_MFT[2] << 16);
        sector_MFT = sector_MFT + (sector_Address_MFT[1] << 8);
        sector_MFT = sector_MFT + (sector_Address_MFT[0]);
        info_NTFS.s_addressMFT = sector_MFT * info_NTFS.sp_Cluster;
        printf("\nSector address for the $MFT file record: %d", info_NTFS.s_addressMFT);

        /*
        - What is the sector address for the $MFT file record
        - What is the type and length of the first attribute in the $MFT record

         */

        /* Type of first attribute */

        fileseek_MFT = fseek(fp, ((NTFS_sectorSize + info_NTFS.s_addressMFT) * 512), SEEK_SET);
        fileread_MFT = fread(buff_offset_table, 1, 512, fp);

        int attribute_1 = *(unsigned char *)(buff_offset_table + 0x14);
        int attribute_1_B = *(unsigned char *)(buff_offset_table + 0x15);
        attribute_1 = (attribute_1_B << 8) + attribute_1;

        printf("\n");

        /* Start of first attribute for $MFT */

        attribute1_type[0] = *(unsigned char *)(buff_offset_table + 0x00 + attribute_1);
        attribute1_type[1] = *(unsigned char *)(buff_offset_table + 0x01 + attribute_1);
        attribute1_type[2] = *(unsigned char *)(buff_offset_table + 0x02 + attribute_1);
        attribute1_type[3] = *(unsigned char *)(buff_offset_table + 0x03 + attribute_1);

        info_NTFS.attrib_1 = attribute1_type[3] << 24;
        info_NTFS.attrib_1 = info_NTFS.attrib_1 + (attribute1_type[2] << 16);
        info_NTFS.attrib_1 = info_NTFS.attrib_1 + (attribute1_type[1] << 8);
        info_NTFS.attrib_1 = info_NTFS.attrib_1 + attribute1_type[0];

        switch (info_NTFS.attrib_1)
        {

        default:
          strcpy(type_attribute1, "No type found");
          break;
        case 16:
          strcpy(type_attribute1, "$Standard_Information");
          break;
        case 32:
          strcpy(type_attribute1, "$Attribute_List");
          break;
        case 48:
          strcpy(type_attribute1, "$File_Name");
          break;
        case 64:
          strcpy(type_attribute1, "$Object_ID");
          break;
        case 80:
          strcpy(type_attribute1, "$Security_Descriptor");
          break;
        case 96:
          strcpy(type_attribute1, "$Volume_Name");
          break;
        case 122:
          strcpy(type_attribute1, "$Volume_Info");
          break;
        case 128:
          strcpy(type_attribute1, "$Data");
          break;
        case 144:
          strcpy(type_attribute1, "$Index_Root");
          break;
        case 160:
          strcpy(type_attribute1, "$Index_Allocation");
          break; // check this
        case 176:
          strcpy(type_attribute1, "$BitMap");
          break;
        case 192:
          strcpy(type_attribute1, "$Reparse_Point");
          break;
        case 256:
          strcpy(type_attribute1, "$Logged_Utility_Stream");
          break;
        }
        printf("\nType of Attribute #1: %s", type_attribute1);

        /* Length of first attribute */

        attribute1_length[0] = *(unsigned char *)(buff_offset_table + 0x04 + attribute_1);
        attribute1_length[1] = *(unsigned char *)(buff_offset_table + 0x05 + attribute_1);
        attribute1_length[2] = *(unsigned char *)(buff_offset_table + 0x06 + attribute_1);
        attribute1_length[3] = *(unsigned char *)(buff_offset_table + 0x07 + attribute_1);

        info_NTFS.attrib_len_1 = attribute1_length[3] << 24;
        info_NTFS.attrib_len_1 = info_NTFS.attrib_len_1 + (attribute1_length[2] << 16);
        info_NTFS.attrib_len_1 = info_NTFS.attrib_len_1 + (attribute1_length[1] << 8);
        info_NTFS.attrib_len_1 = info_NTFS.attrib_len_1 + attribute1_length[0];
        printf("\nLength of Attribute #1: %d", info_NTFS.attrib_len_1);

        /* Start of second attribute */
        int attribute_2 = attribute_1 + info_NTFS.attrib_len_1;

        attribute2_type[0] = *(unsigned char *)(buff_offset_table + 0x00 + attribute_2);
        attribute2_type[1] = *(unsigned char *)(buff_offset_table + 0x01 + attribute_2);
        attribute2_type[2] = *(unsigned char *)(buff_offset_table + 0x02 + attribute_2);
        attribute2_type[3] = *(unsigned char *)(buff_offset_table + 0x03 + attribute_2);

        info_NTFS.attrib_2 = attribute2_type[3] << 24;
        info_NTFS.attrib_2 = info_NTFS.attrib_2 + (attribute2_type[2] << 16);
        info_NTFS.attrib_2 = info_NTFS.attrib_2 + (attribute2_type[1] << 8);
        info_NTFS.attrib_2 = info_NTFS.attrib_2 + attribute2_type[0];

        switch (info_NTFS.attrib_2)
        {
        default:
          strcpy(type_attribute2, "No type found");
          break;
        case 16:
          strcpy(type_attribute2, "$Standard Information");
          break;
        case 32:
          strcpy(type_attribute2, "$Attribute List");
          break;
        case 48:
          strcpy(type_attribute2, "$File Name");
          break;
        case 64:
          strcpy(type_attribute2, "$Object ID");
          break;
        case 80:
          strcpy(type_attribute2, "$Security Descriptor");
          break;
        case 96:
          strcpy(type_attribute2, "$Volume Name");
          break;
        case 122:
          strcpy(type_attribute2, "$Volume Info");
          break;
        case 128:
          strcpy(type_attribute2, "$Data");
          break;
        case 144:
          strcpy(type_attribute2, "$Index Root");
          break;
        case 160:
          strcpy(type_attribute2, "$Index Allocation");
          break; // check this
        case 176:
          strcpy(type_attribute2, "$BitMap");
          break;
        case 192:
          strcpy(type_attribute2, "$Reparse Point");
          break;
        case 256:
          strcpy(type_attribute2, "$Logged Utility Stream");
          break;
        }

        printf("\nType of Attribute #2: %s", type_attribute2);

        /* Length of second Attribute */

        attribute2_length[0] = *(unsigned char *)(buff_offset_table + 0x04 + attribute_2);
        attribute2_length[1] = *(unsigned char *)(buff_offset_table + 0x05 + attribute_2);
        attribute2_length[2] = *(unsigned char *)(buff_offset_table + 0x06 + attribute_2);
        attribute2_length[3] = *(unsigned char *)(buff_offset_table + 0x07 + attribute_2);

        info_NTFS.attrib_len_2 = attribute2_length[3] << 24;
        info_NTFS.attrib_len_2 = info_NTFS.attrib_len_2 + (attribute2_length[2] << 16);
        info_NTFS.attrib_len_2 = info_NTFS.attrib_len_2 + (attribute2_length[1] << 8);
        info_NTFS.attrib_len_2 = info_NTFS.attrib_len_2 + attribute2_length[0];

        printf("\nLength of Attribute #2: %d", info_NTFS.attrib_len_2);

        printf("\n");

        printf("\n");
        printf("\n##################################################################");
        printf("\n");

        /* ################################################################## */

        /* Deleted file details (FAT) */

        /* 
        
           For the first deleted file on the volume’s root directory, display the name and size of
           that file, and the number of the first cluster. Display the first 16 characters of the
           content of that file (assume it is a simple text file). 
           
        */

        printf("\n");
        printf("**********************DELETED FILE DETAILS**********************");
        printf("\n");
        printf("\n");

        int del_startCluster;
        int clusterDelArray[2];
        fileseek_root = fseek(fp, root_dir_address * 512, SEEK_SET);
        fileread_root = fread(buf_root_part, 1, 512, fp);

        for (int j = 0; j < fileread_root; j++)
        {
          int fbyte = *(char *)(buf_part_table + (j * offset_root));
          fbyte = fbyte & 0x000000ff;
          char firstFT[15]; //first 16 
          firstFT[0] = fbyte;

          /* Name of deleted file operation */

          /* References https://www.ntfs.com/disk-scan.htm#:~:text=For%20example%2C%20on%20FAT%20any,has%20been%20deleted%20or%20not. */

          if (fbyte == 0xe5)
          { /* on FAT any deleted entry, file or folder are marked with ASCII symbol 229 (0xE5) that becomes the first symbol of the entry */
            printf("\nName of 1st Deleted File (FAT): %c", fbyte);
            for (i; i <= 15; i++)
            {
              firstFT[i] = *(char *)(buf_part_table + (j * offset_root) + i);
              printf("%c", firstFT[i]);
            }

            /* Contents of deleted file */

            clusterDelArray[0] = *(unsigned char *)(buf_part_table + (j * offset_root) + 0x1A);
            clusterDelArray[1] = *(unsigned char *)(buf_part_table + (j * offset_root) + 0x1B);
            del_startCluster = (clusterDelArray[1] << 8);
            del_startCluster = del_startCluster + clusterDelArray[0];
            int clusterStartAddress = sector_Address + ((del_startCluster - 2) * 8);
            int del_fr = clusterStartAddress * 512;
            fileseek_delete = fseek(fp, del_fr, SEEK_SET);
            fileread_delete = fread(buf_delete_part, 1, 512, fp);
            printf("\nContents of deleted file: ");
            for (int tmp = 0; tmp <= 16; tmp++)
            {
              printf("%c", buf_delete_part[tmp]);
            }

            printf("\nStart cluster of 1st deleted file: %-12d", del_startCluster);

            printf("\n");
            printf("\n");
            printf("\n##################################################################");
            printf("\n");
          }
        }
      }
    }

    fclose(fp);
  }
  else
  {
    /* No image file passed to program */
    printf("Please pass an image file (.dd) when using this program\n");
  }

  return (0);
}

/* END of File */
/* ################################################################## */
