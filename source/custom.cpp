#include "SchedulingAlgorithms.h"
#include "Utils.h"
#include <algorithm>
#include <map>
#include <iostream>

using namespace std;

// 自訂排程演算法：Priority-based SJF with Aging
// 優勢：期望在周轉時間和等待時間上優於純 RR + 優先級，同時避免飢餓。
void custom_priority_sjf_aging_scheduling(vector<Process> processes) {
    // 儲存原始副本用於結果輸出，並按ID排序
    vector<Process> original_processes = processes;
    sort(original_processes.begin(), original_processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    vector<int> ready_queue_indices; // 儲存就緒行程在 processes 向量中的索引

    map<int, int> gantt_chart_data;
    int current_time = 0;
    int completed_processes = 0;
    int num_processes = processes.size();
    int context_switches = 0;
    int prev_proc_id = -1;

    // 老化機制參數
    const int AGING_THRESHOLD = 15; // 等待超過 15ms 觸發老化 (可調參)
    const int AGING_PRIORITY_BOOST = 1; // 優先級提升量 (數字越小優先級越高，所以是減少)

    cout << "\n--- Starting Custom: Priority-based SJF with Aging Scheduling ---\n";

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
                    // 初始化上次執行時間或加入佇列時間，用於老化計算
                    processes[i].last_run_time = current_time;
                }
            }
        }

        // 執行老化機制
        for (int idx : ready_queue_indices) {
            // 如果行程的等待時間超過閾值且不是最高優先級 (優先級 1)
            // 這裡的等待時間是：當前時間 - 上次執行/進入佇列的時間
            if (processes[idx].priority > 1 && (current_time - processes[idx].last_run_time) >= AGING_THRESHOLD) {
                processes[idx].priority = max(1, processes[idx].priority - AGING_PRIORITY_BOOST); // 提升優先級
                processes[idx].last_run_time = current_time; // 重置上次執行時間，避免立即再次老化
                // cout << "DEBUG: Process " << processes[idx].id << " priority boosted to " << processes[idx].priority << " at time " << current_time << "\n";
            }
        }

        // 依據 (優先級, 剩餘burst time) 排序就緒佇列
        // 優先級高的優先 (數字小)，其次是剩餘 burst time 短的優先
        sort(ready_queue_indices.begin(), ready_queue_indices.end(), [&](int a_idx, int b_idx) {
            const Process& pa = processes[a_idx];
            const Process& pb = processes[b_idx];
            if (pa.priority != pb.priority) {
                return pa.priority < pb.priority; // 優先級小的優先
            }
            return pa.remaining_burst_time < pb.remaining_burst_time; // 剩餘burst time 短的優先
        });

        if (ready_queue_indices.empty()) {
            gantt_chart_data[current_time] = -1; // CPU空閒
            current_time++;
            prev_proc_id = -1;
            continue;
        }

        int current_proc_idx = ready_queue_indices[0]; // 選擇最高優先級且剩餘burst time 最短的行程
        Process& current_proc = processes[current_proc_idx];

        if (prev_proc_id != -1 && prev_proc_id != current_proc.id) {
            context_switches++; // 增加上下文切換次數
        }
        prev_proc_id = current_proc.id;

        if (!current_proc.is_started) {
            current_proc.start_time = current_time;
            current_proc.is_started = true;
        }
        
        gantt_chart_data[current_time] = current_proc.id; // 記錄甘特圖

        current_proc.remaining_burst_time--; // 執行一個時間單位
        current_time++;
        current_proc.last_run_time = current_time; // 更新上次執行時間

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
    print_results(original_processes, context_switches, "Custom: Priority-based SJF with Aging Scheduling");
}