# Syscall-Communication
Server-client based calculator, where communication is handled with syscalls and interrupts.
A shared buffer text file is used for communication between the processes.

### Client 
The client process receives as arguments the server PID, two numbers and an operator represented as number(1-4),
corresponding to addition, subtraction, multiplication, division.

### Server
The server process assigns signal handlers and awaits incoming signals from client processes.
When an interrupt is received, the correct handler is called and a child process which calculates the result and writes it to the buffer is created.
