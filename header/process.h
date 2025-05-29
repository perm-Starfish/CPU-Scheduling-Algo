#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

struct Process {
    int id;
    int arrival_time;  // (ms)
    int burst_time;    // (ms)
    int priority;      // (數字越小 priority 越高)

    int remaining_burst_time; // 剩餘的CPU時間
    int start_time;           // 第一次獲得CPU的時間
    int completion_time;      // 完成執行時間
    int last_run_time;        // 上次停止執行時的時間 (calculate waiting time, especially aging 機制)
    bool is_started;          // 是否已經開始執行過

    Process(int _id, int _arrival, int _burst, int _priority);
    Process(const Process& other) = default;
};

// generate random process
std::vector<Process> generate_processes(int num_processes);

#endif // PROCESS_H
