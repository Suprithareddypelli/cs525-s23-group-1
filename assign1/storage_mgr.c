#include<stdlib.h>
#include<stdio.h>
#include<sys/stat.h>
#include "storage_mgr.h"
#include<string.h>
#include "dberror.h"

#define openDataFile(fileName,mode) fopen(fileName, mode)
#define closeDataFile(openFile) fclose(openFile)

/*******************************************************************************************************************************************************************
 * Authors:
 *
 * NAME		               EMAIL			IMPLEMENTATION
 * --------------------------  -----------------------	------------------------------------------------------------------------------------------------------------
 * Stella Ramola Erdani        <serdani@hawk.iit.edu>	initStorageManager(), createPageFile(), openPageFile(), closePageFile(), destroyPageFile(), readBlock();
 * Supritha Reddypelli         <sreddypelli@hawk.iit.edu>	getBlockPos(), readFirstBlock(), readPreviousBlock(), readCurrentBlock(), readNextBlock(), readLastBlock();
 * Nirmala Thurputi            <nthurpati@hawk.iit.edu>   writeBlock(), writeCurrentBlock(), appendEmptyBlock(), ensureCapacity();
 *******************************************************************************************************************************************************************/

void initStorageManager(void){
  printf("Group1 Storage Manager Initialized!\n");
}

//Method for creating a page file
RC createPageFile(char *fileName) {
FILE *createFile = openDataFile(fileName, "w");
RC status = RC_OK;

if (createFile == NULL) {
  status = RC_FILE_NOT_FOUND;
} 
else {
  fseek(createFile, PAGE_SIZE, SEEK_SET);
  putc('\0', createFile);

int closeResult = closeDataFile(createFile);
if (closeResult != 0) {
  status = RC_FILE_CLOSE_FAILURE;
}
return status;
}
}

//Method for opening a page file
RC openPageFile(char *fileName, SM_FileHandle *fHandle) {
FILE *openFile = openDataFile(fileName, "r+");
RC status = RC_OK;
if (openFile != NULL) {
  struct stat openFlStat;
  stat(fileName, &openFlStat);
  fHandle->mgmtInfo = openFile;
  fHandle->curPagePos = ftell(openFile) / PAGE_SIZE;
  fHandle->totalNumPages = (int)(openFlStat.st_size / PAGE_SIZE) ? (int)(openFlStat.st_size / PAGE_SIZE) : 0;
  fHandle->fileName = fileName;
} 
else {
  status = RC_FILE_NOT_FOUND;
  return status;
}
int closeResult = closeDataFile(openFile);
if (closeResult != 0) {
  status = RC_FILE_CLOSE_FAILURE;
}
return status;
}

//Method for closing a page file
RC closePageFile(SM_FileHandle *fHandle) {
RC status = RC_OK;
if (fHandle != NULL) {
  FILE *openFile = openDataFile(fHandle->fileName,"r");
  if(openFile == NULL){
    status = RC_FILE_NOT_FOUND;
    return status;
  }

  int closeResult = closeDataFile(fHandle->mgmtInfo);
  if (closeResult != 0) {
    status = RC_FILE_CLOSE_FAILURE;
  }
  return RC_OK;
}
return RC_FILE_HANDLE_NOT_INIT;
}

//Method to destroy a page file
RC destroyPageFile(char *fileName) {
FILE *destroyFile = openDataFile(fileName, "r");
if (destroyFile == NULL) {
  return RC_FILE_NOT_FOUND;
}
int closeStatus = closeDataFile(destroyFile);
if (closeStatus != 0) {
  return RC_FILE_CLOSE_FAILURE;
}
int removeStatus = remove(fileName);
if (removeStatus == 0) {
  return RC_OK;
}
}



//Method to read a block in a page file
RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
if (fHandle == NULL)
  return RC_FILE_HANDLE_NOT_INIT;

FILE *openFile = openDataFile(fHandle->fileName,"r+");
if(openFile == NULL)
  return RC_FILE_NOT_FOUND;

int totalPages = fHandle->totalNumPages ? fHandle->totalNumPages : 0;
if(pageNum >= totalPages)
  return RC_READ_NON_EXISTING_PAGE;

int positionSetSuccess = fseek(fHandle->mgmtInfo , (pageNum*PAGE_SIZE) , SEEK_SET);
if(positionSetSuccess != 0)
  return RC_READ_NON_EXISTING_PAGE;

size_t readSize = fread(memPage, 1, PAGE_SIZE, fHandle->mgmtInfo);
if(readSize != PAGE_SIZE)
  return RC_READ_FILE_ERROR;

fHandle->curPagePos = (ftell(fHandle->mgmtInfo)/PAGE_SIZE) ? (ftell(fHandle->mgmtInfo)/PAGE_SIZE) : 0;
int closeFileStatus = closeDataFile(openFile);
if(closeFileStatus != 0)
  return RC_FILE_CLOSE_FAILURE;
return RC_OK;
}






//Method to return the position of the current page
int getBlockPos(SM_FileHandle *fHandle) {
  if (fHandle == NULL) {
    return RC_FILE_HANDLE_NOT_INIT;
  }
  FILE *openFile = openDataFile(fHandle->fileName, "r");
  if (openFile == NULL) {
    return RC_FILE_NOT_FOUND;
  }
  return fHandle->curPagePos;
}

//Method to read the first block in a page file
RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
  int pageNum = 0;
  fseek(fHandle->mgmtInfo, 0, SEEK_SET);
  fHandle->curPagePos = 0;
  return readBlock(pageNum, fHandle, memPage);
}

//Function to read the previous block
RC readPreviousBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
  for (int k=0;k<1;k++){}
  FILE *openFile = openDataFile(fHandle->fileName, "r");
  if (openFile == NULL) {
    return RC_FILE_NOT_FOUND;
  }
  int curPos = fHandle->curPagePos;
  if (curPos == 0) {
    return RC_READ_NON_EXISTING_PAGE;
  }
  return readBlock(curPos - 1, fHandle, memPage);
}

//Method to read the current block
RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
  FILE *openFile = openDataFile(fHandle->fileName, "r");
  if (openFile == NULL) {
    return RC_FILE_NOT_FOUND;
  }
  int curPos = fHandle->curPagePos;
  for (int j=0;j<1;j++){ }
  return readBlock(curPos, fHandle, memPage);
}

//Method to read next block
RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
  int curPos = fHandle->curPagePos;
  if (curPos >= fHandle->totalNumPages - 1) {
    return RC_READ_NON_EXISTING_PAGE;
  }

  fseek(fHandle->mgmtInfo, curPos + 1, SEEK_CUR);
  fHandle->curPagePos = curPos + 1;

  return readBlock(curPos + 1, fHandle, memPage);
}

//Function for reading last block
RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
  for (int l=0;l<1;l++){}
  FILE *openFile = openDataFile(fHandle->fileName, "r");
  if (openFile == NULL) {
    return RC_FILE_NOT_FOUND;
  }

  int lastPos = fHandle->totalNumPages - 1;
  for (int i=0;i<1;i++){  }
  return readBlock(lastPos, fHandle, memPage);
}

// Writing a page to the disk
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
  if (fHandle == NULL) return RC_FILE_HANDLE_NOT_INIT;
  FILE *openFile = openDataFile(fHandle->fileName, "r+");
  if (openFile == NULL) return RC_FILE_NOT_FOUND;
  int totalPages;
  if (fHandle->totalNumPages != 0) {
    totalPages = fHandle->totalNumPages;
  } 
  else {
    totalPages = 0;
  }
  if (totalPages < pageNum || pageNum < 0) return RC_WRITE_FAILED;
  int success = fseek(openFile, pageNum * PAGE_SIZE, SEEK_SET);
  if (success == 0) {
    size_t writeResult = fwrite(memPage, PAGE_SIZE, 1, openFile);
    fseek(openFile, (pageNum + 1) * PAGE_SIZE, SEEK_SET);
    fHandle->curPagePos = (ftell(fHandle->mgmtInfo) / PAGE_SIZE);
    fHandle->totalNumPages = (ftell(openFile) / PAGE_SIZE);
    if (closeDataFile(openFile) != 0) return RC_FILE_CLOSE_FAILURE;
    return RC_OK;
  } else {
    return RC_WRITE_FAILED;
  }
}

// Writing a page to the disk at the current position of cursor
RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage) {
FILE *openFile = fopen(fHandle->fileName, "r+");
if (openFile) {
int currentPagePos = fHandle->curPagePos / PAGE_SIZE;
++fHandle->totalNumPages;
return writeBlock(currentPagePos, fHandle, memPage);
}
return RC_FILE_NOT_FOUND;
}

// Increasing the number of pages in the file by one
RC appendEmptyBlock(SM_FileHandle *fHandle) {
FILE *openFile = fopen(fHandle->fileName, "a");
if (openFile) {
char zeroByte = '\0';
for (int i = 0; i < PAGE_SIZE; ++i) {
fwrite(&zeroByte, 1, 1, openFile);
}
fHandle->totalNumPages = (ftell(openFile) / PAGE_SIZE);
fclose(openFile);
return RC_OK;
}
return RC_FILE_NOT_FOUND;
}

// Ensuring the capacity of the file. If number of pages are less than numberOfPages increase the size to numberOfPages
RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle) {
if (fHandle) {
FILE *openFile = fopen(fHandle->fileName, "r+");
if (openFile) {
if (numberOfPages > fHandle->totalNumPages) {
int pagesToAdd = numberOfPages - fHandle->totalNumPages;
for (int i = 0; i < pagesToAdd; ++i) {
appendEmptyBlock(fHandle);
}
fclose(openFile);
return RC_OK;
} else {
fclose(openFile);
return RC_READ_NON_EXISTING_PAGE;
}
}
return RC_FILE_NOT_FOUND;
}
return RC_FILE_HANDLE_NOT_INIT;
}
