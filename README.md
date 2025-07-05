# CPU_design

RISC-V Assembly to Machine Code Converter with 5-Stage Pipelined CPU Simulation
This project C++ implementation that performs two major tasks:

1. Converts RISC-V assembly instructions to binary machine code

2. Simulates a 5-stage pipelined CPU architecture (Fetch, Decode, Execute, Memory Access, and Write Back)

How It Works
Input: Provide a list of RISC-V assembly instructions in the code (assemblycode vector).

Conversion: The AssemblyConverter class tokenizes and translates each instruction into machine code.

Pipeline Execution: Simulates each pipeline stage over clock cycles, handling control and data dependencies.

Output:

Printed binary instructions

Register state

Data memory contents

