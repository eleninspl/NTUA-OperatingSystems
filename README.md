# Operating Systems Lab 

## Course Information
- **Course Title:** Operating Systems  
- **Semester:** 6th (Flow Y)  
- **Instructor:** Panayiotis Tsanakas  

## Overview
This repository contains solutions and code implementations for various lab exercises focused on system-level programming in C. The labs cover essential topics such as file operations, process management, signal handling, inter-process communication, and network programming. Each lab is designed to provide practical experience and deepen understanding of operating system concepts.

## Labs Included

### Lab 1: File Operations and Process Management
- **Objective:** Demonstrate file creation, process forking, and logging process information.
- **Key Features:**
  - Functions to ensure filenames have a specific extension.
  - Logging of parent and child process IDs to a specified or default file.

### Lab 2: Signal Handling in UNIX
- **Objective:** Explore UNIX signal handling using `sigaction` for multiple processes.
- **Key Features:**
  - Communication between parent and child processes using signals (SIGUSR1, SIGUSR2, SIGTERM).
  - Child processes toggle states ("gates open" and "gates closed") based on received signals.
  - The parent manages child processes and monitors their status.

### Lab 3: Inter-Process Communication (IPC) with Pipes and Select
- **Objective:** Implement IPC using pipes and handle multiple I/O streams with `select()`.
- **Key Features:**
  - Parent sends integers to child processes via pipes.
  - Child processes modify the integers and send them back to the parent.
  - Efficient handling of user commands and child responses using non-blocking I/O multiplexing with `select()`.

### Lab 4: Network Programming with Sockets
- **Objective:** Establish a TCP connection to a server and implement debugging features.
- **Key Features:**
  - Connection to a specified host and port for data exchange.
  - Debugging functionality to show detailed transmission information.
  - Utility functions to validate received data and check for spaces in strings.

## Compilation and Execution
Each lab's code can be compiled using a C compiler (e.g., `gcc`). To run the executables, use the appropriate command-line arguments as described in the individual lab instructions.
