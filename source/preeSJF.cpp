#include "SchedulingAlgorithms.h"
#include "Utils.h"
#include <algorithm>
#include <map>
#include <iostream>

using namespace std;

// preemptive SJF
void preemptive_sjf_scheduling(vector<Process> processes) {
    // 儲存原始副本用於結果輸出，並按ID排序
    vector<Process> original_processes = processes;
    sort(original_processes.begin(), original_processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    vector<int> ready_queue_indices; // 儲存就緒行程在 processes 向量中的索引

    map<int, int> gantt_chart_data; // 時間點 -> 執行中的行程ID
    int current_time = 0;
    int completed_processes = 0;
    int num_processes = processes.size();
    int context_switches = 0;
    int prev_proc_id = -1; // 上一個執行的行程ID

    cout << "\n--- Starting Preemptive SJF Scheduling ---\n";

    while (completed_processes < num_processes) {
        // 將所有已到達的行程加入就緒佇列
        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_burst_time > 0) {
                bool found = false;
                for (int idx : ready_queue_indices) {
                    if (idx == i) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    ready_queue_indices.push_back(i);
                }
            }
        }

        // 依據剩餘burst time排序就緒佇列
        sort(ready_queue_indices.begin(), ready_queue_indices.end(), [&](int a_idx, int b_idx) {
            return processes[a_idx].remaining_burst_time < processes[b_idx].remaining_burst_time;
        });

        if (ready_queue_indices.empty()) {
            // 如果就緒佇列為空，且仍有未完成行程，則CPU空閒
            gantt_chart_data[current_time] = -1; // -1 表示CPU空閒
            current_time++;
            prev_proc_id = -1; // 重置上一個行程ID
            continue;
        }

        int current_proc_idx = ready_queue_indices[0]; // 選擇剩餘burst time最短的行程
        Process& current_proc = processes[current_proc_idx];

        if (prev_proc_id != -1 && prev_proc_id != current_proc.id) {
            context_switches++; // 增加上下文切換次數
        }
        prev_proc_id = current_proc.id;

        // 更新行程狀態
        if (!current_proc.is_started) {
            current_proc.start_time = current_time;
            current_proc.is_started = true;
        }
        
        // 將當前時間點的行程記錄到甘特圖
        gantt_chart_data[current_time] = current_proc.id;

        current_proc.remaining_burst_time--; // 執行一個時間單位
        current_time++;

        if (current_proc.remaining_burst_time == 0) {
            current_proc.completion_time = current_time;
            completed_processes++;
            // 將完成的行程從就緒佇列中移除
            ready_queue_indices.erase(remove(ready_queue_indices.begin(), ready_queue_indices.end(), current_proc_idx), ready_queue_indices.end());
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
    print_results(original_processes, context_switches, "Preemptive SJF Scheduling");
}