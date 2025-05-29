# CPU-Scheduling-Algo
> **Operating Systems - Assignment 2**  
> Implement several CPU scheduling algorithms and propose an own one.

## Project Overview

This repository contains a C++ implementation of various CPU scheduling algorithms. The primary goal is to simulate how a CPU scheduler manages the execution of multiple processes, demonstrating their behavior through Gantt charts and evaluating their performance metrics.

The project schedules 10 processes, each with a randomly generated arrival time (0, 5, or 10 ms), burst time (6 to 28 ms), and priority (1 to 3, where 1 is highest).

## Implemented Algorithms

The following CPU scheduling algorithms are implemented:

* **Preemptive SJF (Shortest Job First) Scheduling**: Prioritizes processes with the shortest remaining burst time, preempting if a new process arrives with a shorter remaining burst time.
* **Round-Robin (RR) + Non-Preemptive Priority Scheduling**: Processes are selected based on priority (non-preemptive between priorities). Within the same priority level, Round-Robin scheduling is used with a time quantum of 5ms.
* **Multilevel Feedback Queue (MLFQ) Scheduling**: Utilizes three queues ($Q_0$, $Q_1$, $Q_2$) with different scheduling policies:
    * $Q_0$: Round-Robin with a time quantum of 5ms.
    * $Q_1$: Round-Robin with a time quantum of 10ms.
    * $Q_2$: FCFS (First-Come, First-Served) scheduling.
    Processes are demoted to lower priority queues if they exhaust their time quantum.
* **Custom Scheduling Algorithm: Priority-based SJF with Aging**: This algorithm combines features of SJF and priority scheduling. It is a preemptive algorithm that prioritizes processes based on a combination of priority (higher priority first) and remaining burst time (shorter burst time first). An "aging" mechanism is incorporated to prevent starvation: processes waiting in the ready queue for an extended period (e.g., 15ms) will have their priority incrementally boosted. This aims to improve fairness and reduce overall waiting times compared to a pure SJF or priority approach, especially when dealing with processes of varying priorities and burst times. This algorithm demonstrates an advantage in potentially achieving better average waiting and turnaround times by preventing low-priority processes from being indefinitely delayed.

## How to Compile and Run

To compile and run this project, you will need a C++ compiler (e.g., g++).

1.  **Navigate to the project root directory**:
    ```bash
    cd path/to/your_project_root
    ```

2.  **Compile the source files**:
    Use the following `g++` command. Ensure your compiler supports C++11 or later.
    ```bash
    g++ -std=c++11 -Wall ^
       -Iheader ^
       source\process.cpp ^
       source\utils.cpp ^
       source\preeSJF.cpp ^
       source\RR_priority.cpp ^
       source\MLFQ.cpp ^
       source\custom.cpp ^
       source\main.cpp ^
       -o scheduling_simulator.exe
    ```
    * `-std=c++11`: Specifies C++11 standard.
    * `-Wall`: Enables all common warnings.
    * `-Iheader`: Tells the compiler to look for header files in the `header/` directory.
    * `-o scheduling_simulator`: Names the output executable file.

3.  **Run the executable**:
    ```bash
    scheduling_simulator.exe
    ```
    The program will output the initial process list, followed by turnaround times, waiting times, and context switch count for each implemented algorithm.
    Additionally, it'll output files includes `preeSJF_gantt_data.csv`, `RR_priority_gantt_data.csv`, `MLFQ_gantt_data.csv`, `custom_gantt_data.csv`. These files are the Gantt chart data of each scheduling algorithms, and we use them to generate Gantt chart pictures.

4.   **Run the `.py` for Gantt charts**:
     If you haven't install `pandas` and `matplotlib`, please install them:
     ```bash
     pip install pandas matplotlib
     ```
     Run the `.py`:
     ```bash
     python plot_gantt.py
     ```
     This will generate the Gantt chart images in PNG format (e.g. `preemptive_sjf_gantt_chart.png`, etc.) in the root directory of your project based on the data in `.csv`s.

## Performance Metrics

For each algorithm, the following metrics are displayed:

* **Gantt Chart**: A visual representation of CPU allocation over time.
* **Turnaround Time ($T_i$)**: The total time from process arrival to its completion.
    Formula: $T_i = \text{Completion Time}_i - \text{Arrival Time}_i$ 
* **Waiting Time ($W_i$)**: The total time a process spends waiting in the ready queue.
    Formula: $W_i = T_i - \text{Burst Time}_i$ 
* **Context Switches**: The number of times the CPU switches from executing one process to another.
