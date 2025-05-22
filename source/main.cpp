#include "Process.h"
#include "Utils.h"
#include "SchedulingAlgorithms.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    int num_processes = 10;

    // 生成一組行程
    vector<Process> initial_processes = generate_processes(num_processes);

    // 為了確保每個演算法使用完全相同的初始行程狀態，每次都複製一份
    vector<Process> processes_for_sjf = initial_processes;
    vector<Process> processes_for_rr_priority = initial_processes;
    vector<Process> processes_for_mlfq = initial_processes;
    vector<Process> processes_for_custom = initial_processes;

    // 列印初始行程資訊
    cout << "--- Initial Processes ---\n";
    cout << "Process ID | Arrival Time | Burst Time | Priority\n";
    cout << "--------------------------------------------------\n";
    for (const auto& p : initial_processes) {
        cout << setw(10) << p.id << " | "
             << setw(12) << p.arrival_time << " | "
             << setw(10) << p.burst_time << " | "
             << setw(8) << p.priority << "\n";
    }
    cout << "--------------------------------------------------\n";


    // 執行每個排程演算法
    preemptive_sjf_scheduling(processes_for_sjf); // preemptive SJF
    rr_priority_scheduling(processes_for_rr_priority); // RR + priority
    multilevel_feedback_queue_scheduling(processes_for_mlfq); // MLFQ
    custom_priority_sjf_aging_scheduling(processes_for_custom); // customize

    return 0;
}