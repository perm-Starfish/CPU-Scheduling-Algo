#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>

struct Process {
    int id;
    int arrival_time;  // (ms)
    int burst_time;    // (ms)
    int priority;      // 優先級 (數字越小優先級越高)

    // 執行過程中的狀態變數
    int remaining_burst_time; // 剩餘的CPU時間
    int start_time;           // 第一次獲得CPU的時間
    int completion_time;      // 完成執行時間
    int last_run_time;        // 上次停止執行時的時間 (用於計算等待時間，特別是老化機制)
    bool is_started;          // 是否已經開始執行過

    Process(int _id, int _arrival, int _burst, int _priority);
    Process(const Process& other) = default;
};

// generate random process
std::vector<Process> generate_processes(int num_processes);

#endif // PROCESS_H