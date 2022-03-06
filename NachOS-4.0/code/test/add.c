/* add.c
 *	Simple program to test whether the systemcall interface works.
 *
 *	Just do a add syscall that adds two values and returns the result.
 *
 */

#include "syscall.h"

int main()
{
   int val1 = 372;
   int val2 = RandomNum();
   long val3 = Add(val1, val2);
   Halt(); 
}
