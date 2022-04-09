/* createfile.c
 *
 *	Simple program to create a file on the system.
 *
 *
 */
#include "syscall.h"

int main()
{
    char *fName;
    char *fNameOpen;
    PrintString("hello, pls input filename: \n");
    ReadString(fName, 255);
    ConcatString(fName,".txt",1024);
    if(Open(fName) == -1)
    {
        if (Create(fName) != -1)
        {
            PrintString("You have successfully created " );
            PrintString(fName );
            PrintString(" ! \n" );
        }
    }
    else
    {
        PrintString("The file you try to create is already existed ! \n");
    }
    Halt();
}