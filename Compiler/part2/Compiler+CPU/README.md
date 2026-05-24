# Custom Compiler & CPU Simulator

A complete end-to-end compiler and CPU simulator written in C++. This project implements the full compilation pipeline from source code to machine code execution on a custom CPU.

## 📌 Overview

This project demonstrates a deep understanding of how programming languages and computers work internally. It takes source code written in a custom programming language, compiles it through multiple stages, and finally executes the generated machine code on a simulated CPU.
SOURCE CODE → Lexer → Parser → AST → IR → Assembler → Machine Code → CPU


---

## 🔧 Compilation Pipeline Stages

### 1. Lexical Analyzer (Lexer)
- Reads source code character by character
- Groups characters into meaningful **tokens** (keywords, identifiers, numbers, operators, etc.)
- Removes whitespace and comments
- Reports lexical errors (invalid characters)

### 2. Parser
- Takes tokens from the lexer
- Builds an **Abstract Syntax Tree (AST)** based on grammar rules
- Checks syntax correctness
- Reports syntax errors (missing parentheses, incorrect statement order, etc.)

### 3. Abstract Syntax Tree (AST)
- An in-memory tree representation of the code structure
- Each node represents a construct (variable, operator, function call, etc.)
- Used for semantic analysis and further transformations

### 4. Intermediate Representation (IR)
- Converts AST to a lower-level, linear representation
- Uses **three-address code** format: `t1 = a + b`
- Simplifies optimization and code generation

### 5. Assembler
- Translates IR instructions into **assembly language**
- Maps virtual registers to real or stack locations
- Handles labels and jumps

### 6. Machine Code Generator
- Converts assembly to **binary machine code**
- Each instruction is encoded into bytes
- Generates a binary file that the CPU understands

### 7. CPU Simulator
- **Fetch** – reads instruction from memory
- **Decode** – determines instruction type and operands
- **Execute** – performs the operation (ALU, memory access, branching)
- Simulates registers, RAM, program counter, and flags
- Supports step-by-step execution for debugging

---

## 📁 Project Structure
custom-compiler/
├── src/
│ ├── lexer.cpp
│ ├── parser.cpp
│ ├── ast.cpp
│ ├── ir.cpp
│ ├── assembler.cpp
│ ├── machine_code.cpp
│ └── cpu.cpp
├── include/
│ └── (header files)
├── tests/
│ └── (test programs)
├── examples/
│ └── sample.myLang
└── README.md

text

---

## 🚀 Features

| Stage | Features |
|-------|----------|
| **Lexer** | Token recognition, error reporting, whitespace skipping |
| **Parser** | Recursive descent parsing, AST construction, syntax checking |
| **AST** | Tree traversal, semantic validation |
| **IR** | Three-address code generation |
| **Assembler** | Register allocation, label resolution |
| **Machine Code** | Binary encoding, instruction packing |
| **CPU** | Fetch-decode-execute cycle, register file, ALU, memory, branching |

MOV  R1, 5      ; R1 = 5
MOV  R2, 3      ; R2 = 3
ADD  R3, R1, R2 ; R3 = R1 + R2
SUB  R4, R1, R2 ; R4 = R1 - R2
MUL  R5, R1, R2 ; R5 = R1 * R2
DIV  R6, R1, R2 ; R6 = R1 / R2
CMP  R1, R2     ; compare R1 and R2
JMP  label      ; unconditional jump
JE   label      ; jump if equal
JNE  label      ; jump if not equal
HLT             ; halt execution
📝 Example Program
Source code (custom language):

text
a = 5 + 3;
b = a * 2;
print(b);
Compilation & execution:

text
[Lexer]     Tokens: ID(a) ASSIGN NUM(5) PLUS NUM(3) SEMICOLON ...
[Parser]    AST built successfully
[IR]        t1 = 5 + 3
            a = t1
            t2 = a * 2
            b = t2
[Assembler] MOV R1, 5
            MOV R2, 3
            ADD R3, R1, R2
            ...
[CPU]       Output: 16
[CPU]       Halted
🛠️ Requirements
C++11 or higher

No external dependencies (pure C++ standard library)

🔨 Build & Run
bash
# Clone the repository
git clone https://github.com/yourusername/custom-compiler.git
cd custom-compiler

# Compile
g++ -std=c++11 src/*.cpp -o compiler

# Run with source file
./compiler examples/sample.myLang

# Run in debug mode (step-by-step CPU execution)
./compiler examples/sample.myLang --debug
📚 What I Learned
How lexers and parsers work (tokenization, recursive descent)

Abstract Syntax Tree (AST) representation and traversal

Intermediate Representation (IR) design

Assembly and machine code generation

CPU architecture (registers, ALU, control unit, memory)

Fetch-decode-execute cycle simulation

End-to-end system integration

### 🎯 Why This Project Matters
Building a compiler from scratch demonstrates:

### Skill	Evidence
Algorithms & Data Structures	Trees (AST), graphs (control flow), symbol tables
Low-level understanding	Assembly, machine code, CPU internals
System design	Pipeline architecture, modular design
Problem solving	Debugging complex interactions across stages
C++ proficiency	Pointers, memory management, OOP

### ⭐ Future Improvements
Add more data types (float, arrays)

Implement optimization passes (constant folding, dead code elimination)

Add a linker for multiple files

Create a web-based visualizer for AST and CPU state

Extend CPU to support interrupts and I/O
