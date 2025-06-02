#include "scheduling.h"
#include "utils.h"
#include <algorithm>
#include <map>
#include <iostream>

using namespace std;

// custom: Priority-based SJF with Aging
// 優勢：期望在 turnaround time 和 waiting time 上優於純 RR + priority，同時避免 starvation
void custom_priority_sjf_aging_scheduling(vector<Process> processes) {
    // 儲存原始副本用於結果輸出，並按ID排序
    vector<Process> original_processes = processes;
    sort(original_processes.begin(), original_processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    vector<int> ready_queue_indices;

    map<int, int> gantt_chart_data;
    int current_time = 0;
    int completed_processes = 0;
    int num_processes = processes.size();
    int context_switches = 0;
    int prev_proc_id = -1;

    // aging parameters
    const int AGING_THRESHOLD = 15; // 等待超過 15ms 觸發 aging (可調參)
    const int AGING_PRIORITY_BOOST = 1; // priority 提升量 (數字越小 priority 越高，所以是減少)

    cout << "\n--- Starting Custom: Priority-based SJF with Aging Scheduling ---\n";

    while (completed_processes < num_processes) {
        // 將所有已到達的行程加入就緒佇列
        for (int i = 0; i < num_processes; i++) {
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
                    // 初始化上次執行時間或加入queue時間 for aging calculation
                    processes[i].last_run_time = current_time;
                }
            }
        }

        // aging
        for (int idx : ready_queue_indices) {
            // 如果行程的等待時間超過 threshold 且不是最高優先級 (priority 1)
            // 這裡的等待時間是：目前時間 - 上次執行/進入queue的時間
            if (processes[idx].priority > 1 && (current_time - processes[idx].last_run_time) >= AGING_THRESHOLD) {
                processes[idx].priority = max(1, processes[idx].priority - AGING_PRIORITY_BOOST); // boost priority
                processes[idx].last_run_time = current_time; // 重置上次執行時間，避免立即再次 aging
                // cout << "DEBUG: Process " << processes[idx].id << " priority boosted to " << processes[idx].priority << " at time " << current_time << "\n";
            }
        }

        // 依據 (priority, remaining burst time) 排序就緒 queue
        // priority高的優先 (數字小)，其次是剩餘 burst time 短的優先
        sort(ready_queue_indices.begin(), ready_queue_indices.end(), [&](int a_idx, int b_idx) {
            const Process& pa = processes[a_idx];
            const Process& pb = processes[b_idx];
            if (pa.priority != pb.priority) {
                return pa.priority < pb.priority; // priority 小的優先
            }
            return pa.remaining_burst_time < pb.remaining_burst_time; // 剩餘 burst time 短的優先
        });

        if (ready_queue_indices.empty()) {
            gantt_chart_data[current_time] = -1; // CPU空閒
            current_time++;
            prev_proc_id = -1;
            continue;
        }

        int current_proc_idx = ready_queue_indices[0]; // 選擇最高priority且剩餘burst time最短的行程
        Process& current_proc = processes[current_proc_idx];

        if (prev_proc_id != -1 && prev_proc_id != current_proc.id) {
            context_switches++;
        }
        prev_proc_id = current_proc.id;

        if (!current_proc.is_started) {
            current_proc.start_time = current_time;
            current_proc.is_started = true;
        }
        
        gantt_chart_data[current_time] = current_proc.id;

        current_proc.remaining_burst_time--; 
        current_time++;
        current_proc.last_run_time = current_time; // update

        if (current_proc.remaining_burst_time == 0) {
            current_proc.completion_time = current_time;
            completed_processes++;
            // 將完成的行程從就緒queue中移除
            ready_queue_indices.erase(remove(ready_queue_indices.begin(), ready_queue_indices.end(), current_proc_idx), ready_queue_indices.end());
        }
    }

    // 將排程後的結果複製回原始行程列表，以便 print_results 函數使用
    for (size_t i = 0; i < original_processes.size(); i++) {
        for (const auto& p : processes) {
            if (original_processes[i].id == p.id) {
                original_processes[i].completion_time = p.completion_time;
                break;
            }
        }
    }

    // discard: print_gantt_chart(gantt_chart_data);
    // 新增：將甘特圖數據保存到 CSV
    string csv_filename = "custom_gantt_data.csv";
    save_gantt_chart_data_to_csv(gantt_chart_data, csv_filename);
    print_results(original_processes, context_switches, "Custom: Priority-based SJF with Aging Scheduling");
}
