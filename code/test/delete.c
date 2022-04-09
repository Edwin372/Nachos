/* delete.c
 *
 *	Simple program to delete a file and print.
 *
 *
 */
#include "syscall.h"

int main()
{
    int fileID;
    char *fName;
    PrintString("hello, pls input filename: \n");
    ReadString(fName, 255);
    fileID = Open(fName);
    if(fileID == -1)
    {
        PrintString("The file you try to delete doesn't exist ! \n");
    }
    else
    {
        if(Remove(fName) != -1)
        {
            PrintString("You have successfully remove the file ! \n");
        }
        else
        {
            PrintString("Removing file failed ! \n");
        }
    }
    Halt();
}   