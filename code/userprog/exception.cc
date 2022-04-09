// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "synchconsole.h"
#include "syscall.h"
#include "ksyscall.h"

#define PAGE_FAULT_MESS "No valueid translation found"
#define READ_ONLY_MESS "Write attempted to page marked read-only"
#define BUS_ERROR_MESS "Translation resulted invalueid physical address"
#define ADDRESS_ERROR_MESS "Unaligned reference or one that was beyond the end of the address space"
#define OVERFLOW_MESS "Integer overflow in add or sub."
#define ILLEGAL_INSTR_MESS "Unimplemented or reserved instr."
#define NUM_TYPE_MESS "Number exception types"
#define MAX_LENGTH 255
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an valueessing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
void IncreasePC()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

char *fromUserToSystem(int address, int length)
{
	char *ch = &(kernel->machine->mainMemory[address]);
	char *kBuffer = new char[length + 1];
	if (ch[0] == '\0')
	{
		return kBuffer;
	}
	memset(kBuffer, 0, length + 1);
	for (int idx = 0; idx < length; idx++)
	{
		kBuffer[idx] = ch[idx];
		if (ch[idx] == '\0')
			break;
	}
	return kBuffer;
}

void fromSystemToUser(int address, int length, char *kBuffer)
{
	if (length <= 0)
		return;
	int idx = 0;
	char ch = 0;
	while (idx < length + 1)
	{
		ch = kBuffer[idx];
		if ((ch == '\n' && kBuffer[idx+1] == '\0') || ch == '\0')
			break;
		kernel->machine->mainMemory[address + idx] = (int)ch;
		idx++;
	}
}
void concatFromSystemToUser(int address, int length, char *kBuffer, char *kBuffer2)
{
	if (length <= 0)
		return;
	int idx = 0;
	int ch = 0;
	while (idx < length + 1)
	{
		ch = (int)kBuffer[idx];
		if (ch == 0)
			break;
		kernel->machine->mainMemory[address + idx] = ch;
		idx++;
	}
	int idx2 = 0;
	while (idx < length + 1)
	{
		ch = (int)kBuffer2[idx2];
		if (ch == 0)
			break;
		kernel->machine->mainMemory[address + idx] = ch;
		idx++;
		idx2++;
	}
}

void displayExceptionMessage(char *message)
{
	DEBUG(dbgAddr, message);
	cerr << "Exception: " << message << "\n";
}

void handleMachineException(ExceptionType type)
{

	switch (type)
	{
	case PageFaultException:
		displayExceptionMessage(PAGE_FAULT_MESS);
		break;

	case ReadOnlyException:
		displayExceptionMessage(READ_ONLY_MESS);
		break;

	case BusErrorException:
		displayExceptionMessage(BUS_ERROR_MESS);
		break;

	case AddressErrorException:
		displayExceptionMessage(ADDRESS_ERROR_MESS);
		break;

	case OverflowException:
		displayExceptionMessage(OVERFLOW_MESS);
		break;

	case IllegalInstrException:
		displayExceptionMessage(ILLEGAL_INSTR_MESS);
		break;

	case NumExceptionTypes:
		displayExceptionMessage(NUM_TYPE_MESS);
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)type << "\n";
		break;
	}
}

void ExceptionHandler(ExceptionType eType)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << eType << " type: " << type << "\n");
	switch (eType)
	{
	case NoException:
	{
		return;
	}
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
			ASSERTNOTREACHED();
			IncreasePC();
			return;
		}
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
			/* Process SysAdd Systemcall*/
			int result, val1, val2;
			val1 = (int)kernel->machine->ReadRegister(4);
			val2 = (int)kernel->machine->ReadRegister(5);
			result = SysAdd(/* int op1 */ val1,
							/* int op2 */ val2);
			if (result > INT32_MAX || result < val1 || result < val2)
			{
				handleMachineException(NumExceptionTypes);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_Create:
		{
			int value = kernel->machine->ReadRegister(4);
			{
				char *filename = &(kernel->machine->mainMemory[value]);
				bool status = kernel->fileSystem->Create(filename, 0);
				kernel->machine->WriteRegister(2, (int)status);
			}

			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_PrintNum:
		{
			DEBUG(dbgSys, "Printing integer:\n");
			int value = kernel->machine->ReadRegister(4);
			kernel->synchConsoleOut->PutInt(value);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_ReadNum:
		{
			DEBUG(dbgSys, "Reading integer:\n");
			int value = kernel->synchConsoleIn->GetInt();
			DEBUG(dbgSys, value << ":\n");

			if (value == NULL)
			{
				handleMachineException(NumExceptionTypes);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			kernel->machine->WriteRegister(2, value);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_ReadChar:
		{
			DEBUG(dbgSys, "Reading char: \n");
			char value = kernel->synchConsoleIn->GetChar();
			kernel->machine->WriteRegister(2, value);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_PrintChar:
		{
			DEBUG(dbgSys, "Printing char: \n");
			char value = kernel->machine->ReadRegister(4);
			kernel->synchConsoleOut->PutChar(value);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_PrintString:
		{
			DEBUG(dbgSys, "Printing string: \n")
			int address = kernel->machine->ReadRegister(4);
			char *kBuffer = fromUserToSystem(address, MAX_LENGTH);
			kernel->synchConsoleOut->PutString(kBuffer);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_ReadString:
		{
			DEBUG(dbgSys, "Reading string: \n")
			int address = kernel->machine->ReadRegister(4);
			// DEBUG(dbgSys, address <<": \n")
			int length = kernel->machine->ReadRegister(5);
			// DEBUG(dbgSys, length <<": \n")
			char *kBuffer = fromUserToSystem(address, length);
			kernel->synchConsoleIn->GetString(kBuffer, length);
			fromSystemToUser(address, length, kBuffer);
			kernel->machine->WriteRegister(2,1);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_ConcatString:
		{
			DEBUG(dbgSys, "Concatenating string: \n")
			int address = kernel->machine->ReadRegister(4);
			int address2 = kernel->machine->ReadRegister(5);
			// DEBUG(dbgSys, address <<": \n")
			int length = kernel->machine->ReadRegister(6);
			// DEBUG(dbgSys, length <<": \n")
			char *kBuffer = fromUserToSystem(address, length);
			DEBUG(dbgSys, kBuffer <<": \n")
			char *kBuffer2 = fromUserToSystem(address2, length);
			DEBUG(dbgSys, kBuffer2 <<": \n")
			strcat (kBuffer, kBuffer2);
			DEBUG(dbgSys, kBuffer <<": \n")
			fromSystemToUser(address,  length,  kBuffer);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_RandomNum:
		{
			DEBUG(dbgSys, "Generating random integer: \n");
			int value = rand();
			kernel->machine->WriteRegister(2, value);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_Open:
		{
			DEBUG(dbgSys, "Opening a file: \n");

			int address = kernel->machine->ReadRegister(4);
			int type = kernel->machine->ReadRegister(5);
			char *kBuffer;

			// if already opened 10 files
			if (kernel->fileSystem->index > 10)
			{
				kernel->machine->WriteRegister(2, -1);
				delete[] kBuffer;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			// if open stdin or stdout, number of openingFileLists dont increase
			kBuffer = fromUserToSystem(address, MAX_LENGTH + 1);
			printf(kBuffer);
			if (strcmp(kBuffer, "stdin") == 0)
			{
				printf("Stdin mode\n");
				kernel->machine->WriteRegister(2, 0);
				delete[] kBuffer;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			else if (strcmp(kBuffer, "stdout") == 0)
			{
				printf("Stdout mode\n");
				kernel->machine->WriteRegister(2, 1);
				delete[] kBuffer;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			if ((kernel->fileSystem->openingFileList[kernel->fileSystem->index] = kernel->fileSystem->Open(kBuffer)) != NULL)
			{

				printf("\nOpen file success '%s'\n", kBuffer);
				kernel->machine->WriteRegister(2, kernel->fileSystem->index);
			}
			else
			{
				printf("Can not open file '%s'", kBuffer);
				kernel->machine->WriteRegister(2, -1);
			}
			delete[] kBuffer;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_Close:
		{
			DEBUG(dbgSys, "Closing a file: \n");
			int openFileId = kernel->machine->ReadRegister(4);
			int i = kernel->fileSystem->index;
			// if openFileID is out of range of opening file list
			if (i < openFileId)
			{
				printf("Invalid file id\n");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			// handling file close successfully
			// we remove it from opening file list

			kernel->fileSystem->openingFileList[openFileId] = NULL;
			delete kernel->fileSystem->openingFileList[openFileId];
			kernel->machine->WriteRegister(2, 0);
			printf("Close file success\n");
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_Read:
		{
			DEBUG(dbgSys, "Reading a file: \n");
			int address = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int openFileId = kernel->machine->ReadRegister(6);
			int i = kernel->fileSystem->index;
			// if the file id is a specical file id such as std out or the id is invalid
			if (openFileId > i || openFileId < 0 || openFileId == 1)
			{
				printf("The file is not opened");
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			if (kernel->fileSystem->openingFileList[openFileId] == NULL)
			{
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			char *kBuffer = fromUserToSystem(address, size);

			// read from stdin
			if (openFileId == 0)
			{
				int content = kernel->synchConsoleIn->Read(kBuffer, size);
				fromSystemToUser(address, content, kBuffer);
				kernel->machine->WriteRegister(2, content);

				delete[] kBuffer;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			int initPos = kernel->fileSystem->openingFileList[openFileId]->getSeekPosition();
			if ((kernel->fileSystem->openingFileList[openFileId]->Read(kBuffer, size)) > 0)
			{
				int curPos = kernel->fileSystem->openingFileList[openFileId]->getSeekPosition();
				fromSystemToUser(address, size, kBuffer);
				kernel->machine->WriteRegister(2, curPos - initPos + 1); // after & before just used for returning
			}
			else
			{
				kernel->machine->WriteRegister(2, -1);
			}
			delete[] kBuffer;
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_Write:
		{
			DEBUG(dbgSys, "Writing a file: \n");
			int address = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int openFileId = kernel->machine->ReadRegister(6);
			int i = kernel->fileSystem->index;

			if (openFileId > i || openFileId < 0 || openFileId == 0) // `out of domain` filesys + try to write to stdin
			{
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			else if (kernel->fileSystem->openingFileList[openFileId] == NULL)
			{
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			// write to console
			int initPos = kernel->fileSystem->openingFileList[openFileId]->getSeekPosition();
			char *kBuffer = fromUserToSystem(address, size);

			if (openFileId == 1)
			{
				int i = 0;
				while (kBuffer[i] != '\0' && kBuffer[i] != '\n')
				{
					kernel->synchConsoleOut->Write(kBuffer + i, 1);
					i++;
				}
				kBuffer[i] = '\n';
				kernel->synchConsoleOut->Write(kBuffer + i, 1); // write last character

				kernel->machine->WriteRegister(2, i - 1);
				delete[] kBuffer;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			// handle normal write file

			if ((kernel->fileSystem->openingFileList[openFileId]->Write(kBuffer, size)) != 0)
			{
				int curPos = kernel->fileSystem->openingFileList[openFileId]->getSeekPosition();
				fromSystemToUser(address, curPos - initPos, kBuffer);
				kernel->machine->WriteRegister(2, curPos - initPos + 1);
				delete[] kBuffer;
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
		}
		case SC_Seek:
		{
			DEBUG(dbgSys, "Seeking a file: \n");
			int position = kernel->machine->ReadRegister(4);
			int openFileId = kernel->machine->ReadRegister(5);

			// handle when if opening file is stdio or not exist
			if (openFileId < 1 || openFileId > kernel->fileSystem->index || kernel->fileSystem->openingFileList[openFileId] == NULL)
			{
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}

			int length = kernel->fileSystem->openingFileList[openFileId]->Length();
			// hanlde when seek position is larger than file length
			if (position > length)
			{
				kernel->machine->WriteRegister(2, -1);
				IncreasePC();
				return;
				ASSERTNOTREACHED();
			}
			// Seek file at given position
			kernel->fileSystem->openingFileList[openFileId]->Seek(position);
			kernel->machine->WriteRegister(2, position);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_Remove:
		{
			DEBUG(dbgSys, "Removing a file: \n");
			int address = kernel->machine->ReadRegister(4);
			// copy filename buffer from the address from user space to kernel space
			char *kBuffer;
			kBuffer = fromUserToSystem(address, MAX_LENGTH);
			// handle remove failed
			if (!kernel->fileSystem->Remove(kBuffer))
			{
				printf("\nFailed to remove file");
				kernel->machine->WriteRegister(2, -1);
			} // handle remove successuly
			else
			{
				printf("\n Successfully removed '%s' \n", kBuffer);
				kernel->machine->WriteRegister(2, 1);
			}

			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		case SC_ConvertIntToString:
		{
			DEBUG(dbgSys, "Converting: \n");

			int num = kernel->machine->ReadRegister(4);
			int address = kernel->machine->ReadRegister(5);
			
			char kBuffer[5];
			snprintf( kBuffer, 5, "%d", num );
			DEBUG(dbgSys, "Converting: \n" << num);
			DEBUG(dbgSys, "Converting: \n" << kBuffer);
			fromSystemToUser(address, 3, kBuffer);
			kernel->machine->WriteRegister(2, 1);
			IncreasePC();
			return;
			ASSERTNOTREACHED();
		}
		default:
		{
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		}
		IncreasePC();
		return;
		ASSERTNOTREACHED();
		break;
	default:
	{
		handleMachineException(eType);
		SysHalt();
	}
	}
	ASSERTNOTREACHED();
}
