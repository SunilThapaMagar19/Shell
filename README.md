# Mav Shell (msh)

Mav Shell (msh) is a custom Unix-like command-line shell written in C. It mimics core functionality of standard shells such as `bash` and `csh` by accepting user commands, creating child processes, and executing programs using system calls.

---

## Features
- Displays a custom prompt: `msh>`
- Executes external commands using `fork()`, `execvp()`, and `wait()`
- Supports commands from `/bin`, `/usr/bin`, `/usr/local/bin`, and the current directory
- Implements built-in commands:
  - `cd` (including `cd ..`)
  - `exit` / `quit`
  - `history`
- Maintains a command history of the **last 50 commands**
- Allows re-running commands using `!#` syntax
- Supports **output redirection** using `>`  
- Supports **piping between two commands** using `|`
- Handles up to **10 command-line arguments**
- Blocks `SIGINT` (Ctrl+C) and `SIGTSTP` (Ctrl+Z) signals to prevent shell termination

---

## How It Works
1. The shell prints the `msh>` prompt and waits for user input.
2. User input is tokenized based on whitespace.
3. Built-in commands are handled directly within the shell.
4. External commands are executed by forking a child process and calling `execvp()`.
5. The parent process waits for the child to complete before displaying the next prompt.
6. Command history is stored in memory and can be viewed or re-executed.

---

## Supported Functionality

### Command Execution
- Executes valid system commands using the `exec` family of functions.
- Prints an error message for unsupported commands.

### History
- Stores up to **50 recent commands**
- Displays history using the `history` command
- Re-runs a command using `!<number>`

## Technologies Used
- C
- Linux System Calls
- `fork()`, `execvp()`, `wait()`
- Signals (`SIGINT`, `SIGTSTP`)
- File descriptors and I/O redirection
- Dynamic memory management

---
