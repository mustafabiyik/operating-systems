#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


//input format
/**./myfs disk -format
./myfs disk -write source_file destination_file
./myfs disk -read source_file destination_file
./myfs disk -delete file
./myfs disk -list
./myfs disk -defragment
 **/

//Structs
typedef struct FileList{
    char FileName[248]; //248 bytes for the file name
    unsigned int FirstBofFileInFAT; //first block of file in the fat
    unsigned int FileSize; //file size in bytes

} FileList;

//FAT and file list 
typedef struct FSHeader{
    unsigned int FAT[4096]; //4096*4
    FileList FileListObj[128];//128*256
} FSHeader;

//file system struct
typedef struct FileSystem{
    FSHeader HeaderObj;
    char DataPart[4096][512];
} FileSystem;

char *disk;
char buffer[512];
FileSystem FS_Obj;

//Format function
void Format(){
    int i;
      //Format FileList
    for(i=0; i<128; i++){
        FS_Obj.HeaderObj.FileListObj[i].FileName[0] = 0x0;
        FS_Obj.HeaderObj.FileListObj[i].FirstBofFileInFAT = 0;
        FS_Obj.HeaderObj.FileListObj[i].FileSize = 0;
    }

    //Format FAT
    for(i=1; i<4096; i++){
        FS_Obj.HeaderObj.FAT[i] = 0;
    }

    //update FSHeader_info.dat file
    FILE *tempFile;
    tempFile = fopen("./disk/FSHeader_info.dat", "w+");
    if(tempFile != NULL){
        fwrite(&FS_Obj.HeaderObj, sizeof(FS_Obj.HeaderObj), 1, tempFile);
    }
}

//write function
void Write(char *srcPath, char *destFileName){
    //srcPath openning 
    FILE *in_File;
    in_File = fopen(srcPath, "r+");

    fseek(in_File, 0, SEEK_SET);
    if(in_File != NULL){
        FILE *FileDest;
        char *destFileNameKeeper; //holds the direction of the destination file
        destFileNameKeeper= malloc(sizeof(char) * (strlen(destFileName) + 10));
        strcat(destFileNameKeeper,"./disk/");
        strcat(destFileNameKeeper, destFileName);
        //Open destination file
        FileDest = fopen(destFileNameKeeper, "w+");
        fseek(FileDest, 0, SEEK_SET);
        //readBytes stores read bytes
        size_t readBytes;
        int tempFirstBlock;
        int FirstBlockAllocationFlag = 1;
        int indexFileList;
        unsigned int prevBlock;
        unsigned int total_bytes = 0;
        unsigned int OFFSET = 0;

        while((readBytes = fread(buffer, 1, sizeof(buffer), in_File)) > 0){

            int indexFAT = 0;
            //if whole chunk is read
            if(readBytes > 0)
            {
                //find an empty block via FAT table
                for(int i=0; i<4096; i++){
                    if(FS_Obj.HeaderObj.FAT[i] == 0){
                        indexFAT = i;
                        break;
                    }
                }
                if(indexFAT==0){
                    printf("file is full\n");
                    exit(1);
                }

                    if(FirstBlockAllocationFlag == 1){
                        tempFirstBlock = indexFAT;

                        //find empty file list
                        for(int j=0; j<128; j++){
                            if(FS_Obj.HeaderObj.FileListObj[j].FirstBofFileInFAT == 0){                                indexFileList = j;
                                indexFileList=j;
                                break;
                            }
                        }
                        FS_Obj.HeaderObj.FileListObj[indexFileList].FirstBofFileInFAT = tempFirstBlock;//update first block information
                        //update file name
                            for(int ing=0; ing<strlen(destFileName); ing++){
                            FS_Obj.HeaderObj.FileListObj[indexFileList].FileName[ing] = destFileName[ing];
                            }

                        FS_Obj.HeaderObj.FileListObj[indexFileList].FileName[strlen(destFileName)] = '\0';
                        FirstBlockAllocationFlag = 0;
                        prevBlock = indexFAT;
                    }

                    //the last chunk of the file is read
                    if(readBytes < 512)
                    {
                        FS_Obj.HeaderObj.FAT[prevBlock] = indexFAT;
                        FS_Obj.HeaderObj.FAT[indexFAT] = 0xFFFFFFFF;//last FAT entry
                        total_bytes =total_bytes + readBytes;
                        FS_Obj.HeaderObj.FileListObj[indexFileList].FileSize = total_bytes;//update File size
                        fwrite(buffer, 1, readBytes, FileDest);
                        fclose(FileDest);//close files
                        fclose(in_File);
                        FILE *tempHeaderF;
                        tempHeaderF = fopen("./disk/FSHeader_info.dat", "w+");//update info
                        if(tempHeaderF != NULL){
                            fwrite(&FS_Obj.HeaderObj, sizeof(FS_Obj.HeaderObj), 1, tempHeaderF);
                        }
                        break;//go out from the while due to last chunk
                    }
                    //ordinary chunk is read
                    else
                    {
                        FS_Obj.HeaderObj.FAT[prevBlock] = indexFAT;//update FAT
                        prevBlock = indexFAT;//update prevFAT
                        fwrite(buffer, 1, sizeof(buffer), FileDest);
                        OFFSET=OFFSET+512;
                        total_bytes =total_bytes + 512;
                        //Update current pointers
                        fseek(in_File, OFFSET, SEEK_SET);
                        fseek(FileDest, OFFSET, SEEK_SET);
                    }
            }

            else{
                printf("error source file not opened\n");
            }
        }
    }
    else{
        printf("File write error\n");

    }
}

//read file function
void Read(char *srcFileName, char *destPath){
    int MatchFlag = 0;
    int i;
    for(i=0; i<128; i++){
        if(strcmp(FS_Obj.HeaderObj.FileListObj[i].FileName, srcFileName) == 0){
            MatchFlag = 1;
            break;
        }
    }
    if(MatchFlag == 1){
        char nameKeeper[256] = "";
        strcat(nameKeeper,"./disk/");//add to nameKeeper string
        strcat(nameKeeper, srcFileName);
        FILE *tempFile;
        tempFile = fopen(nameKeeper, "r");
        fseek(tempFile, 0, SEEK_SET);
        if(tempFile != NULL){
            FILE *tempDestFile;
            // destination file opens
            tempDestFile = fopen(destPath, "w+");
            fseek(tempDestFile, 0, SEEK_SET);
            if(tempDestFile != NULL){
                unsigned int offset = 0;
                size_t count;
                //read from the source then write the destination
                while((count = fread(&buffer, 1, sizeof(buffer), tempFile)) > 0){
                    if(count != 512){
                        fwrite(&buffer, count, 1, tempDestFile);
                        fclose(tempDestFile);
                        fclose(tempFile);
                        
                    }
                    else{
                        fwrite(&buffer, sizeof(buffer), 1, tempDestFile);
                        offset = offset+512;
                        fseek(tempFile, offset, SEEK_SET);
                        fseek(tempDestFile, offset, SEEK_SET);
                        
                    }
                }
            }
        }
    }else{
        printf("there is no such a file in the disk: %s\n", srcFileName);
    }

}

//delete file function
void Delete(char *FileName){
    char DelNameKeeper[256] = "";
    strcat(DelNameKeeper,"./disk/");
    strcat(DelNameKeeper,FileName);
    int checkDelete;
    if((checkDelete = remove(DelNameKeeper)) != 0){
        printf("delete is not possible.\n");
    }

    //find the index of file which is matching
    int indexMatch;
    for(int i=0; i<128; i++){
        if(strcmp(FS_Obj.HeaderObj.FileListObj[i].FileName, FileName) == 0){
            indexMatch = i;
            break;
        }
    }

    //clear the FAT table until the -1
    unsigned int tempFirstBlock = FS_Obj.HeaderObj.FileListObj[indexMatch].FirstBofFileInFAT;
    unsigned int nextBlock= 0;
    unsigned int temp = tempFirstBlock;
    while(nextBlock != 0xFFFFFFFF){
        nextBlock = FS_Obj.HeaderObj.FAT[temp];
        FS_Obj.HeaderObj.FAT[temp] = 0;
        temp = nextBlock;
    }

    //FileListObj is updated
    FS_Obj.HeaderObj.FileListObj[indexMatch].FileName[0] = 0;
    FS_Obj.HeaderObj.FileListObj[indexMatch].FileSize = 0;
    FS_Obj.HeaderObj.FileListObj[indexMatch].FirstBofFileInFAT = 0;

    //Update FSHeader_info file
    FILE *FileDeleteTemp;
    FileDeleteTemp = fopen("./disk/FSHeader_info.dat", "w+");
    if(FileDeleteTemp != NULL){
        fwrite(&FS_Obj.HeaderObj, sizeof(FS_Obj.HeaderObj), 1, FileDeleteTemp);
    }
}


//list files function
void List(){
    printf("file name                       file size\n");
    for(int i=0; i< 128; i++){
        if(FS_Obj.HeaderObj.FileListObj[i].FileSize > 0){
            printf("%s", FS_Obj.HeaderObj.FileListObj[i].FileName);
            printf("                       ");
            printf("%d\n", FS_Obj.HeaderObj.FileListObj[i].FileSize);
        }
    }
}



void Defragment(){
   /* FileSystem CopyFileSystemObj;
    //copy header side
    for(int i=0;i<128;i++){
    strcpy(CopyFileSystemObj.HeaderObj.FileListObj[i].FileName,FS_Obj.HeaderObj.FileListObj[i].FileName);
    CopyFileSystemObj.HeaderObj.FileListObj[i].FirstBofFileInFAT= FS_Obj.HeaderObj.FileListObj[i].FirstBofFileInFAT;
    CopyFileSystemObj.HeaderObj.FileListObj[i].FileSize= FS_Obj.HeaderObj.FileListObj[i].FileSize;
    }
    //copy data part
    for(int k=0;k<4096;k++){
        strcpy(CopyFileSystemObj.DataPart[k],FS_Obj.DataPart[k]);
    }
    
    //format the disk
    Format();
    char *FileNameFlag;*/
    char nameKeeper[256] = "";
    strcat(nameKeeper,"./disk/");//add to nameKeeper string
    int fileNumber=0;

    for(int c=0;c<128;c++){
        if(FS_Obj.HeaderObj.FileListObj[c].FileName[0]!=0){
            fileNumber++;
        }
    }
    FILE *tempFile[fileNumber];//file array
    char name[fileNumber][256];
    FILE *tempFileDest;
    int c=0;
    char ch;

    for(int i=0;i<128;i++){
        //that means there is a file, file list entry not void
        if(FS_Obj.HeaderObj.FileListObj[i].FileName[0]!= 0){
        strcat(nameKeeper, FS_Obj.HeaderObj.FileListObj[i].FileName); //add the file name end of the string
        tempFileDest = fopen(nameKeeper, "r");
        //names of the files in "name" string array
        strcpy(name[c],FS_Obj.HeaderObj.FileListObj[i].FileName);
        tempFile[c] = fopen(name[c], "w");

        // Read contents from file copy the files
        ch = fgetc(tempFileDest);
        while (ch != EOF)
         {
        fputc(ch, tempFile[c]);
        ch = fgetc(tempFileDest);
        }

        fclose(tempFile[c]);
        fclose(tempFileDest);
         c++;
        }
  

    }

    Format(); //Format the disk

    //then rewrite the files into the disk,
    //while writing again the files into the disk, free spaces are eliminated
    for(int kc=0;kc<fileNumber;kc++){

    Write(name[kc], name[kc]); //accroding to the write operation there is no free space anymore
    //all spaces are occupied

    }
    
}

//main function
int main(int argc, char** argv){
    FILE *in_File;
    FILE *out_File;
    char *srcFile;
    char *destFile;
    disk = argv[1];

    in_File = fopen("./disk/FSHeader_info.dat","rb"); //READ BINARY
    if(in_File == NULL){
        
        for(int i=0; i<4096; i++){
            if(i==0){
                FS_Obj.HeaderObj.FAT[0] = 0xFFFFFFFF;
            }
            else{
                FS_Obj.HeaderObj.FAT[i] = 0;
            }
            
            FS_Obj.HeaderObj.FileListObj[i].FileName[0] = 0;
            FS_Obj.HeaderObj.FileListObj[i].FileSize = 0;
            FS_Obj.HeaderObj.FileListObj[i].FirstBofFileInFAT = 0;
        }

        out_File = fopen("./disk/FSHeader_info.dat", "w+");
        if(out_File != NULL){
            fwrite(&FS_Obj.HeaderObj, sizeof(FS_Obj.HeaderObj), 1, out_File);
            fclose(out_File);//close file
        }
        printf("FSHeader_info.dat file is created.\n");

    }
    else
    {
        //if file exists already
        while(fread(&FS_Obj.HeaderObj, sizeof(FS_Obj.HeaderObj), 1, in_File));
        fclose(in_File);
    }

    //Read arguments and run corresponding function.
    if(argv[2] != NULL){
        if(strcmp(argv[2],"-format") == 0){
            Format();
        }
        else if(strcmp(argv[2],"-write") == 0){
            if(argv[3]!=NULL & argv[4]!=NULL){
                srcFile = argv[3];
                destFile = argv[4];
                Write(srcFile, destFile);
        }
        else{
            printf("enter file name\n");

        }
        }
        else if(strcmp(argv[2],"-read") == 0){
            if(argv[3]!=NULL & argv[4]!=NULL){
                srcFile = argv[3];
                destFile = argv[4];
                Read(srcFile, destFile);
        }
        else{
            printf("enter file name\n");

        }
        
        }
        else if(strcmp(argv[2],"-delete") == 0){
            if(argv[3]!=NULL){
                srcFile = argv[3];
                Delete(srcFile);
            }
            else{
            printf("enter file name\n");

        }
        }
        else if(strcmp(argv[2],"-list") == 0){

                List();
        }

        else if(strcmp(argv[2],"-defragment") == 0){

                Defragment();
        }
    }

    return 0;
}