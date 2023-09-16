# One Bad Apple

**Initialization:**

- [x] Set up the basic program structure.
- [x] Include necessary header files (`stdio.h`, `stdlib.h`, `unistd.h`, etc.).
- [x] Define global variables and data structures for communication (e.g., pipes, shared memory, etc.).

**Process Management:**

- [ ] Prompt the user for the value of 'k' (the number of processes).
- [ ] Create 'k' child processes using `fork()` and set up the circular ring structure.
- [ ] Handle the case where 'k' processes form the ring, with each having a read and write connection to its neighbors.
- [ ] Implement a method for gracefully shutting down the simulation when the user presses Control-C using signal handling (`signal()` function).

**Message Passing:**

- [x] Define data structures for message passing, including message format and headers.
- [ ] Implement logic for passing the apple (synchronization).
- [ ] Implement logic for receiving and processing messages:
  - [ ] Each process should check if it has the apple.
  - [ ] If it has the apple, check if the message is intended for it.
  - [ ] If the message is intended for the process, copy and process it.
  - [ ] If the message is not intended for the process, send it to the next node with a small delay (without using `sleep`).

**User Input:**

- [ ] Prompt the user for a message to send and the node to send it to.
- [ ] Handle user input validation.

**Verbose Logging:**

- [ ] Implement verbose diagnostic messages throughout the program to track the flow of data and events.
  - [x] Display information about which process is sending/receiving data.
  - [x] Indicate the direction of data transfer.
  - [x] Log process creation/closure events, etc.

**Additional Features:**

- [x] Handle messages of more than one word.
- [ ] Implement the "bad apple" feature if extra credit is desired:
  - [ ] Assign one of the nodes as the "bad apple."
  - [ ] Modify the "bad apple" process to replace some data with random data before sending it.

**Code Quality and Documentation:**

- [x] Use descriptive variable names following the camelCase convention.
- [x] Ensure the code is self-documenting where possible, with clear comments explaining complex logic or algorithms.
- [ ] Provide a high-level overview of the program's functionality in a comment at the beginning of the code.
- [x] Keep code modular and well-organized.

**Testing:**

- [ ] Test the program thoroughly with different values of 'k,' message lengths, and bad apple scenarios.
- [ ] Check for memory leaks and ensure proper resource cleanup.

**Finalization:**

- [ ] Create a design document that clearly describes your project, including its objectives, architecture, and the implementation approach. This document should serve as a guide for anyone reviewing your code.
- [x] Organize your source code in C, ensuring it is well-documented, readable, and follows coding conventions. Avoid using zip files for code submission.
- [ ] Capture screenshots of the execution, showing the terminal output in a clear and readable format. Ensure the following is visible in the screenshots:
  - [ ] The circular ring structure with nodes (processes) labeled or numbered.
  - [ ] The location of the "apple" (i.e., which process has it at any given time).
  - [ ] Messages being sent and received between nodes, including their content and direction (e.g., from Node X to Node Y).
  - [ ] Verbose diagnostic messages and any other relevant information that demonstrates the program's functionality.
- [ ] Demonstrate the sending of at least two messages from the parent process to different nodes:
  - [ ] Send the first message, wait for it to be received and the apple to return to the parent.
  - [ ] Send a second message to a different node, following the same procedure.
- [ ] Ensure that the screenshots are taken from a terminal or console window where the output is easy to read and understand (not from an integrated development environment like Visual Studio Code).
- [ ] Organize all final project files and documentation in a clear and easily accessible manner for submission.
