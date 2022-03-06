// synchconsole.cc
//	Routines providing synchronized access to the keyboard
//	and console display hardware devices.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and lengthation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchconsole.h"



//----------------------------------------------------------------------
// SynchConsoleInput::SynchConsoleInput
//      Initialize synchronized access to the keyboard
//
//      "inputFile" -- if NULL, use stdin as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleInput::SynchConsoleInput(char *inputFile)
{
    consoleInput = new ConsoleInput(inputFile, this);
    lock = new Lock("console in");
    waitFor = new Semaphore("console in", 0);
}

//----------------------------------------------------------------------
// SynchConsoleInput::~SynchConsoleInput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleInput::~SynchConsoleInput()
{
    delete consoleInput;
    delete lock;
    delete waitFor;
}

//----------------------------------------------------------------------
// SynchConsoleInput::GetChar
//      Read a buffer typed from the keyboard and return buffer's size.
//----------------------------------------------------------------------

int SynchConsoleInput::Read(char *buffer, int numBytes)
{
    int idx;
    int isEOL = FALSE;
    char ch;
    for (idx = 0; idx < numBytes; idx++)
    {
        buffer[idx] = 0;
    }
    idx = 0;
    lock->Acquire(); // Block for a read line
    while ((idx < numBytes) && (isEOL == FALSE))
    {
        do
        {
            waitFor->P();                 // Block for single char
            ch = consoleInput->GetChar(); // Get a char (could)
        } while (ch == EOF);

        if (ch == '\n')
        {
            isEOL = TRUE;
        }
        else
        {
            buffer[idx] = ch;
            idx++;
        }
    }
    lock->Release(); // UnBLock
    return idx;      // How many did we rd
}
//----------------------------------------------------------------------
// SynchConsoleInput::GetChar
//      Read a character typed at the keyboard, waiting if necessary.
//----------------------------------------------------------------------
char SynchConsoleInput::GetChar()
{
    char ch;

    lock->Acquire();
    waitFor->P(); // wait for EOF or a char to be available.
    ch = consoleInput->GetChar();
    lock->Release();
    return ch;
}

//----------------------------------------------------------------------
// SynchConsoleInput::GetChar
//      Read an integer typed at the keyboard, waiting if necessary.
//----------------------------------------------------------------------
int SynchConsoleInput::GetInt()
{
    char *buffer;
    int MAX_BUFFER = 255;
    buffer = new char[MAX_BUFFER + 1];
    int numbytes = Read(buffer, MAX_BUFFER);
    bool isNegative = false;
    int firstIdx = 0;
    long number = 0;
    if (buffer[0] == '-')
    {
        isNegative = true;
        firstIdx = 1;
    }
    for (int i = firstIdx; i < numbytes; i++)
    {
        if (buffer[i] < '0' && buffer[i] > '9')
        {
        
            delete buffer;
            return NULL;
        }
    }
    int temp = 0;
    for (int i = firstIdx; i < numbytes; i++)
    {
        

        number = number * 10 + (long)(buffer[i] - 48);
        if (number > INT32_MAX || number < temp)
        {
            DEBUG(dbgSys, "\nThe integer number is not valid");
            delete buffer;
            return NULL;
        }
        temp = number;
    }
    if (isNegative)
    {
        number = number * -1;
    }
    delete buffer;
    return number;
}

void SynchConsoleInput::GetString(char *buffer, int length)
{
    Read(buffer, length);
    return;
}
//----------------------------------------------------------------------
// SynchConsoleInput::CallBack
//      Interrupt handler called when keystroke is hit; wake up
//	anyone waiting.
//----------------------------------------------------------------------

void SynchConsoleInput::CallBack()
{
    waitFor->V();
}

//----------------------------------------------------------------------
// SynchConsoleOutput::SynchConsoleOutput
//      Initialize synchronized access to the console display
//
//      "outputFile" -- if NULL, use stdout as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleOutput::SynchConsoleOutput(char *outputFile)
{
    consoleOutput = new ConsoleOutput(outputFile, this);
    lock = new Lock("console out");
    waitFor = new Semaphore("console out", 0);
}

//----------------------------------------------------------------------
// SynchConsoleOutput::~SynchConsoleOutput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleOutput::~SynchConsoleOutput()
{
    delete consoleOutput;
    delete lock;
    delete waitFor;
}

int SynchConsoleOutput::Write(char *buffer, int numBytes)
{
    int idx;
    lock->Acquire();
    for (idx = 0; idx < numBytes; idx++)
    {
        consoleOutput->PutChar(buffer[idx]); // Write and wait
        waitFor->P();                       // Block for a character
    }
    lock->Release(); // Free Up
    return numBytes; // Return the bytes out
}

//----------------------------------------------------------------------
// SynchConsoleOutput::PutChar
//      Write a character to the console display, waiting if necessary.
//----------------------------------------------------------------------

void SynchConsoleOutput::PutChar(char ch)
{
    lock->Acquire();
    consoleOutput->PutChar(ch);
    waitFor->P();
    lock->Release();
}

void SynchConsoleOutput::PutInt(int n)
{
    char str[11];
    int idx = 0;
    sprintf(str, "%d\n\0", n);
    lock->Acquire();
    do
    {
        consoleOutput->PutChar(str[idx]);
        idx++;
        waitFor->P();
    } while (str[idx] != '\0');
    lock->Release();
}

void SynchConsoleOutput::PutString(char* buffer)
{
    int length = 0;
    while (buffer[length] != 0){
        length++;                           
    }
    Write(buffer, length + 1); 
    delete buffer;
}

//----------------------------------------------------------------------
// SynchConsoleOutput::CallBack
//      Interrupt handler called when it's safe to send the next
//	character can be sent to the display.
//----------------------------------------------------------------------

void SynchConsoleOutput::CallBack()
{
    waitFor->V();
}
