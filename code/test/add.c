/* add.c
 *	Simple program to test whether the systemcall interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main()
{
   char *fName;

   int val1 = 372;
   int val2 = 2;
   int fileID;
   long val3 = Add(val1, val2);
   char *buffer;
   PrintNum(val3);
   PrintString("hello, pls input filename: \n");
   ReadString(fName, 255);
   
   if (Create(fName) != -1)
   {
      PrintString("Created file\n");

      fileID = Open(fName);
      if (fileID != -1)
      {
         Write("xin chao chau dep trai 123\n", 1000, fileID);
         Close(fileID);
      }
      fileID = Open(fName);
      PrintString(fName);
      PrintNum(fileID);
      
      if (fileID != -1)
      {
         Read(buffer, 255, fileID);
         PrintString("got here\n");
         PrintString(buffer);
         Close(fileID);
      }
   }

   Halt();
}
