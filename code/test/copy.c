/* copy.c
 *
 *	Simple program to copy a file.
 *
 *
 */
#include "syscall.h"

int main()
{
    int fileID;
    // int counter;
    char *buffer;
    char *fName;
    char *fNameCopy;
    int targetFileID;
    // char *str1;
    PrintString("hello, pls input filename: \n");
    ReadString(fName, 255);
    // counter = 0;
    fileID = Open(fName);
    if (fileID == -1)
    {
        PrintString("The file you try to copy doesn't exist ! \n");
    }
    else
    {
        Read(buffer, 255, fileID);
        Close(fileID);
        // counter = Add(counter, 1);
        // PrintNum(counter);
        // ConvertIntToString(str1, counter);
        // PrintString(str1);
        // ConcatString(fName," copy",1024);
        // PrintString(fName);
        // PrintString("hello, pls input copy filename: \n");
        // ReadString(fNameCopy, 255);
        fNameCopy = "an1.txt";
        if (Create(fNameCopy) != -1)
        {
            PrintString("You have successfully copied ");
            PrintString(fNameCopy);
            PrintString(" ! \n");
            targetFileID = Open(fNameCopy);
            if (targetFileID != 1) {
                PrintString(buffer);
                Write(buffer, 1000, targetFileID);
                Close(fileID);
            }
            else
                PrintString("Failed Open File \n");
        }
        else
        {
            PrintString("Failed creating file");
        }
    }
    Halt();
}