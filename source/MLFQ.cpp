#include "SchedulingAlgorithms.h"
#include "Utils.h"
#include <algorithm>
#include <queue>
#include <map>
#include <iostream>

using namespace std;

// 多級回饋佇列排程
void multilevel_feedback_queue_scheduling(vector<Process> processes) {
    // 儲存原始副本用於結果輸出，並按ID排序
    vector<Process> original_processes = processes;
    sort(original_processes.begin(), original_processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    // 定義三個佇列 [cite: 8]
    queue<int> q0; // RR with quantum 5ms
    queue<int> q1; // RR with quantum 10ms
    queue<int> q2; // FCFS

    int quantum_q0 = 5;  // Q0 時間量子
    int quantum_q1 = 10; // Q1 時間量子

    map<int, int> gantt_chart_data;
    int current_time = 0;
    int completed_processes = 0;
    int num_processes = processes.size();
    int context_switches = 0;
    int prev_proc_id = -1;

    cout << "\n--- Starting Multilevel Feedback Queue Scheduling ---\n"; [cite: 8]

    // 追蹤每個行程所在的佇列 (0, 1, 2, -1表示已完成或未到達)
    vector<int> process_queue_level(num_processes + 1, -1); // 索引對應 process ID

    while (completed_processes < num_processes) {
        // 將所有已到達且未完成的行程加入最高優先級佇列 (Q0)
        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_burst_time > 0) {
                // 如果行程尚未進入任何佇列 (或已完成但被重置)，則加入 Q0
                // 這裡的 -2 用於標記已添加到 Q0 但還未被處理的行程，避免重複添加
                if (process_queue_level[processes[i].id] == -1) {
                    q0.push(processes[i].id);
                    process_queue_level[processes[i].id] = 0; // 標記為 Q0
                }
            }
        }

        int current_proc_id = -1;
        int current_proc_idx = -1; // 儲存 processes 向量中的索引
        int current_quantum = 0;
        int current_queue_level = -1;

        // 檢查佇列優先順序：Q0 -> Q1 -> Q2
        if (!q0.empty()) {
            current_proc_id = q0.front();
            q0.pop();
            current_quantum = quantum_q0;
            current_queue_level = 0;
        }
        else if (!q1.empty()) {
            current_proc_id = q1.front();
            q1.pop();
            current_quantum = quantum_q1;
            current_queue_level = 1;
        }
        else if (!q2.empty()) {
            current_proc_id = q2.front();
            q2.pop();
            // FCFS 執行到完成，時間量子為剩餘 burst time
            // 在實際中，這裡可以設定一個非常大的時間量子，或者根據剩餘 burst time 來
            current_quantum = 99999; // 模擬無限大，即 FCFS 直到完成
            current_queue_level = 2;
        } else {
            // CPU 空閒
            gantt_chart_data[current_time] = -1;
            current_time++;
            prev_proc_id = -1;
            continue;
        }

        // 找到對應的行程物件
        for(size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].id == current_proc_id) {
                current_proc_idx = i;
                break;
            }
        }
        Process& current_proc = processes[current_proc_idx];

        if (prev_proc_id != -1 && prev_proc_id != current_proc.id) {
            context_switches++; // 增加上下文切換次數
        }
        prev_proc_id = current_proc.id;

        if (!current_proc.is_started) {
            current_proc.start_time = current_time;
            current_proc.is_started = true;
        }
        
        int execute_time = min(current_quantum, current_proc.remaining_burst_time);

        for (int i = 0; i < execute_time; ++i) {
            gantt_chart_data[current_time + i] = current_proc.id; // 記錄甘特圖
        }

        current_proc.remaining_burst_time -= execute_time;
        current_time += execute_time;

        if (current_proc.remaining_burst_time == 0) {
            current_proc.completion_time = current_time;
            completed_processes++;
            process_queue_level[current_proc.id] = -1; // 已完成
        } else {
            // 降級
            if (current_queue_level == 0) {
                q1.push(current_proc.id);
                process_queue_level[current_proc.id] = 1;
            } else if (current_queue_level == 1) {
                q2.push(current_proc.id);
                process_queue_level[current_proc.id] = 2;
            } else {
                // 如果在 Q2 尚未完成，則放回 Q2 隊列尾部 (FCFS)
                q2.push(current_proc.id);
                process_queue_level[current_proc.id] = 2;
            }
        }
    }

    // 將排程後的結果複製回原始行程列表，以便 print_results 函數使用
    for (size_t i = 0; i < original_processes.size(); ++i) {
        for (const auto& p : processes) {
            if (original_processes[i].id == p.id) {
                original_processes[i].completion_time = p.completion_time;
                break;
            }
        }
    }

    print_gantt_chart(gantt_chart_data);
    print_results(original_processes, context_switches, "Multilevel Feedback Queue Scheduling");
}