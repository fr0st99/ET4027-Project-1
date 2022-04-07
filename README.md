# ET4027-Project-1

In order to run the application install a GCC compiler first

Then run the following commands: 

**gcc .\ForensicsTool.c -o ForensicsTool**

**.\ForensicsTool.exe 'PATH OF FILE'** 

e.g .\ForensicsTool.exe 'C:\Users\Big Chungus\Downloads\Sample_1.dd'


Running the last line will display the following information: 

############################## FORENSIC_TOOL_COMPLETE ####################################
############################## Partitions on C:\Users\Big Chungus\Downloads\Sample_1.dd disk image ##############################

Partition 0: Type: FAT-16       Start: 63           Size: 514017

Partition 1: Type: FAT-32       Start: 578340       Size: 1028160

Partition 2: Type: NTFS         Start: 1606500      Size: 369495

Partition 3: Type: NOT-VALID    Start: 0            Size: 0


The total number of valid partitions is: 3

##################################################################

**********************FAT VOLUME INFORMATION**********************

Sectors per cluster: 8

FAT Area: 502

Root directory size: 32

Size of reserved area: 2

Start sector: 63

Sector address for Cluster #2: 599

##################################################################

**********************NTFS VOLUME INFORMATION**********************


Bytes per sector for this NTFS volume: 512

Sectors Per Cluster for this NTFS volume: 8

Sector address for the $MFT file record: 32

Type of Attribute #1: $Standard_Information

Length of Attribute #1: 96

Type of Attribute #2: $File Name

Length of Attribute #2: 104


##################################################################

**********************DELETED FILE DETAILS**********************


Name of 1st Deleted File (FAT): σ    TXT ↑▓.ü

Contents of deleted file:

Section A:  P

Start cluster of 1st deleted file: 19


##################################################################
