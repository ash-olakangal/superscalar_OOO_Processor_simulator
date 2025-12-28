
---

# Superscalar Out-of-Order Processor Simulator

A cycle-accurate C++ simulator that models a high-performance, superscalar processor. It simulates **dynamic scheduling** using a centralized Issue Queue and a Reorder Buffer (ROB) to support out-of-order execution and in-order retirement.

## Features

* **Superscalar Execution**: Configurable dispatch, issue, and retirement widths.
* **Dynamic Scheduling**: Implements Tomasulo’s algorithm-like logic with an **Issue Queue (IQ)**.
* **Register Renaming**: Uses a **Register Alias Table (RAT)** to eliminate WAR and WAW hazards.
* **Speculative Execution**: Supports a **Reorder Buffer (ROB)** for precise state recovery and in-order commitment.
* **Trace-Driven**: Analyzes instruction traces to report IPC (Instructions Per Cycle) and total execution time.

---

## Installation & Building

1. **Clone the repository:**
```bash
git clone https://github.com/ash-olakangal/superscalar_OOO_Processor_simulator.git
cd superscalar_OOO_Processor_simulator

```


2. **Build the simulator:**
```bash
make

```


This will generate an executable named `sim`.
3. **Clean build files:**
```bash
make clean

```



---

## Usage

The simulator is configured via command-line arguments to define the hardware constraints.

### Command Syntax

```bash
./sim <S> <N> <B> <trace_file>

```

### Parameter Breakdown

| Parameter | Description |
| --- | --- |
| **S** | **Issue Queue Size:** Maximum number of instructions waiting to be issued. |
| **N** | **Width:** The superscalar width (how many instructions can be Dispatched/Issued/Retired per cycle). |
| **B** | **ROB Size:** The number of entries in the Reorder Buffer. |
| **trace_file** | Path to the instruction trace file. |

**Example:**
To simulate a 4-wide processor with a 32-entry Issue Queue and a 64-entry ROB:

```bash
./sim 32 4 64 traces/gcc_trace.txt

```

---

## Processor Architecture & Stages

The simulator processes instructions through a multi-stage pipeline. Every cycle, the simulator performs these steps in **reverse order** (Retire → Writeback → Execute → Issue → Dispatch → Fetch) to ensure that instruction movement between stages is handled correctly within a single clock cycle.

### 1. Pipeline Stages

1. **Fetch**: Reads the next  instructions from the trace file and places them into the **Dispatch Queue**.
2. **Dispatch**:
* Moves instructions from the Dispatch Queue to the **Issue Queue** and **ROB**.
* **Renaming**: Renames architectural source registers to ROB entries using the **RAT**.


3. **Issue**:
* Looks at instructions in the Issue Queue.
* If all source operands are "Ready" (data is available), the instruction is issued to the Functional Units.


4. **Execute**:
* Simulates the execution latency of instructions (e.g., Simple ALU, Mult, Load/Store).


5. **Writeback**:
* Instructions finish execution.
* They broadcast their result to the **Issue Queue** (to wake up dependent instructions) and mark the **ROB** entry as ready.


6. **Retire**:
* Instructions are committed in-order from the head of the **ROB**.
* The **RAT** is updated if necessary, and the ROB entry is freed.



---

## 🗃 Key Data Structures

### **Reorder Buffer (ROB)**

The ROB is a circular buffer that ensures instructions "retire" (commit to the architectural state) in the order they were fetched. It allows the processor to execute instructions out-of-order while maintaining the illusion of in-order execution for the programmer.

### **Issue Queue (IQ)**

A "waiting room" for instructions. Instructions sit here until their data dependencies (source registers) are satisfied. Once ready, they are picked for execution.

### **Register Alias Table (RAT)**

The RAT tracks which ROB entry will produce the value for a specific architectural register.

* If `RAT[R1] = 5`, it means architectural register `R1` is currently being computed by the instruction in `ROB entry 5`.
* This mechanism effectively performs **Register Renaming**.

---

## Performance Metrics

The simulator provides a detailed summary at the end of the run:

* **Total Cycles**: Number of clock cycles required to complete the trace.
* **Total Instructions**: Total count of instructions processed.
* **IPC (Instructions Per Cycle)**: The primary measure of throughput.
* **Timing Per Instruction**: Displays exactly which cycle each instruction entered every stage.

---

## Trace Format

Each line in the trace file represents one instruction:
`<PC> <operation_type> <dest_reg> <src1_reg> <src2_reg>`

* Example: `401234 0 1 2 3`
* `0`: Operation type (e.g., ALU).
* `1`: Destination Register.
* `2`, `3`: Source Registers.



---

## ⚖️ License

This project is for educational purposes in Computer Architecture. Please refer to the repository's license for more information.
