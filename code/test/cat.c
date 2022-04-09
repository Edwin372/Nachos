/* cat.c
 *
 *	Simple program to open a file and print.
 *
 *
 */
#include "syscall.h"

int main()
{
    int fileID;
    char *fName;
    char *buffer;
    PrintString("hello, pls input filename: \n");
    ReadString(fName, 255);
    fileID = Open(fName);

    if (fileID != -1)
    {
        Read(buffer, 255, fileID);
        PrintString(buffer);
        Close(fileID);
    }
    else
    {
        PrintString("The file you try to open doesn't exist ! \n");
    }
    Halt();
}   