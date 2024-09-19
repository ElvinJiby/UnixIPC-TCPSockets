#ifndef _QUIZDB_H
#define _QUIZDB_H

/*************************************
 *         Quiz Questions            *
 *   Used by the server program      *
 *************************************/

char* QuizQ[] = {
"In a 32-bit system architecture, each process can address 4 Giga bytes of memory. Y or N?",
"Is Stack a section of a process's address space? (Y or N)",
"Is Heap a section of a process's address space? (Y or N)",
"Is Environment a section of a process's address space? (Y or N)",
"What is the program that allows users to run programs with security privileges of another user? (Hint: Answer is a 4-letter word.)",
"chown is a user command to change file mode bits. Y or N?",
"chmod is a user command to change file owner and group. Y or N?",
"What is the standard C library function to open a file?",
"What is the standard C library function to close a file?",
"A global variable in a C program is visible to all functions. Y or N?",
"What does the malloc() library function return on failure? (Hint: A four-letter word)",
"What does the calloc() library function return on failure? (Hint: A four-letter word)",
"Does the free() library function return a value? (Y or N)?",
"The size of int* in 64-bit system architecture is 4 bytes. (Y or N)?",
"The size of int* in 64-bit system architecture is 8 bytes. (Y or N)?",
"Is malloc() a system call? (Y or N)",
"Is malloc() a C library function? (Y or N)",
"Is fork() a system call? (Y or N)",
"In compilation, what converts assembly code into machine code? (Hint: a 8-letter word.)",
"Is preprocessing a phase of compilation? (Y or N)",
"Is linking a phase of compilation? (Y or N)",
"A static library is an archive of object files. Y or N?",
"A shared library is included in an executable. Y or N?",
"A pthread is a light-weight process. Y or N?",
"In a process, two pointers having the same value point to the same data. Y or N?",
"What is the system call that returns two completely independent copies of the original process? (Hint: A four-letter word)",
"All the threads inside a process have access to the same global, shared memory. Y or N?",
"What is the software interrupt sent to a program to indicate that an important event has happened? (Hint: a six-letter word)",
"What integer represents SIGINT?",
"What integer represents SIGTERM?",
"What integer represents SIGSEGV?",
"What integer represents SIGCHLD?",
"ssh is used for secure data communication between two networked computers. Y or N?",
"An IPv4 address consists of four bytes. Y or N?",
"An IPv6 address consists of eight bytes. Y or N?",
"What is the interprocess communication (IPC) mechanism that allows data to be exchanged between applications on different hosts connected by a network? (Hint: a six-letter word.)",
"A socket is a file. Y or N?",
"Stream sockets provide a reliable, bidirectional, byte-stream communication channel. Y or N?",
"Datagram sockets provide a reliable communication channel. Y or N?",
"Datagram sockets are connectionless sockets. Y or N?",
"Stream sockets are connectionless sockets. Y or N?",
"Stream sockets are connection-oriented sockets. Y or N?",
"127.0.0.1 is called a loopback IP address. Y or N?"
};

/*************************************
 *         Quiz Answers              *
 *************************************/

char* QuizA[] = {
"Y",
"Y",
"Y",
"Y",
"sudo",
"N",
"N",
"fopen",
"fclose",
"Y",
"NULL",
"NULL",
"N",
"N",
"Y",
"N",
"Y",
"Y",
"assembler",
"Y",
"Y",
"Y",
"N",
"Y",
"Y",
"fork",
"Y",
"signal",
"2",
"15",
"11",
"17",
"Y",
"Y",
"N",
"socket",
"Y",
"Y",
"N",
"Y",
"N",
"Y",
"Y"
};

#endif /* _QUIZDB_H */
