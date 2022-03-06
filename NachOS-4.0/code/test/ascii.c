/* help.c 
 *    Test program to print ascii table.
 */
#include "syscall.h"

int main() {
   int i;
   PrintString("Character \t ASCII Value\n");
   //Print ASCII Values
   for (i = 33; i < 127; i++) {
      PrintChar((char)i);
      PrintString("\t\t  ");
      PrintNum(i);
   }
   Halt();
}