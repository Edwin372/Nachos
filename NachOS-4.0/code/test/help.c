/* help.c 
 *    Test program to print group and test program infomations.
 */
#include "syscall.h"

int main()
{
    int result;
    
    int value;
    char ch;
    char string[11];
    PrintString("_____________________________________________________________________\n");
    PrintChar('\n');
    PrintString("Hello, This is our group's info: \n");
    PrintString("_____________________________________________________________________\n");
    PrintChar('\n');
    PrintString("Name: Tran Hoai Chau; MSSV: 18125067; Percent of contribution: 0.8\n");
    PrintChar('\n');
    PrintString("Name: Van Duy An; MSSV: 18125059; Percent of contribution: 0.2\n");
    PrintString("_____________________________________________________________________\n");
    PrintChar('\n');
    PrintString("Program Descriptions: \n");
    PrintString("_____________________________________________________________________\n");
    PrintChar('\n');
    PrintString("Name: Sort\n");
    PrintChar('\n');
    PrintString("Description: Allow user to input an array of n integer (n <= 100),\n");
    PrintString("             use bubble sort algorithms to sort the array\n");
    PrintChar('\n');
    PrintString("----------------------------------------------------------------------\n");
    PrintString("Name: Ascii\n");
    PrintChar('\n');
    PrintString("Description: Print ascii table\n");
    PrintString("_____________________________________________________________________\n");
    Halt();
}
