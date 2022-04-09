/* concatenate.c
 *
 *	Simple program to concat 2 files on the system.
 *
 *
 */
#include "syscall.h"

int main()
{
    int fileID1;
    int fileID2;
    char *fName1;
    char *fName2;
    char *buffer1;
    char *buffer2;
    // PrintString("hello, pls input filename 1: \n");
    // ReadString(fName1, 255);
    // PrintString(fName1);
    fileID1 = Open("an.txt");
    // PrintString("hello, pls input filename 2: \n");
    // ReadString(fName2, 255);
    // PrintString(fName2);
    fileID2 = Open("chau.txt");
    if(fileID1 != -1)
    {
        // PrintString(buffer1);
        if(fileID2 != -1)
        {
            Read(buffer2, 255, fileID2);
            ConcatString(buffer1,buffer2,1024);
            Write(buffer1, 1000, fileID1);
            Close(fileID2);
        }
        else
        {
            PrintString("The file 2 you try to open doesn't exist ! \n");
        }
        Close(fileID1);
    }
    else
    {
        PrintString("The file 1 you try to open doesn't exist ! \n");
    }
    Halt();
}