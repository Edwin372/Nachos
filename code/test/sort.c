/* sort.c
 *    Test program to sort a large number of integers.
 *
 *    Intention is to stress virtual memory system.
 *
 *    Ideally, we could read the unsorted array off of the file system,
 *	and store the result back to the file system!
 */

/*
#define UNIX
#define UNIX_DEBUG
*/

#ifdef UNIX
#include <stdio.h>
#define Exit exit
#else
#include "syscall.h"
#endif /* UNIX */


#define MAX_SIZE 100


int main()
{
    int n, i, j, tmp, A[MAX_SIZE];
    PrintString("Hello, please input your array's size n (n < 100): \n");
    n = ReadNum();
    PrintString("Current array size: ");
    PrintNum(n);

    if(n >= MAX_SIZE) {
        PrintString("Invalid array size!");
        Halt();
    }
    
    for (i = 0; i < n; i++)
    {
        PrintString("Please input your next integer:");
        A[i] = ReadNum();
    }

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < (n - 1); j++)
        {
            if (A[j] > A[j + 1])
            { /* out of order -> need to swap ! */
                tmp = A[j];
                A[j] = A[j + 1];
                A[j + 1] = tmp;
            }
        }
    }
    PrintString("Here is the sorted array:\n");
    for (i =0 ; i<n; i++){
        PrintNum(A[i]);
    }
    Halt();
}
