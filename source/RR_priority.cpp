#include "scheduling.h"
#include "utils.h"
#include <algorithm>
#include <queue>
#include <map>
#include <iostream>

using namespace std;

// RR + non-preemptive priority
void rr_priority_scheduling(vector<Process> processes) {
    // 儲存原始副本用於結果輸出，並按ID排序
    vector<Process> original_processes = processes;
    sort(original_processes.begin(), original_processes.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    int time_quantum = 5;

    // 每個優先級維護一個 FIFO 佇列，實現 RR within Priority
    vector<queue<int>> priority_queues(4); // priority 1, 2, 3 (index 1, 2, 3)

    map<int, int> gantt_chart_data;
    int current_time = 0;
    int completed_processes = 0;
    int num_processes = processes.size();
    int context_switches = 0;
    int prev_proc_id = -1;

    cout << "\n--- Starting RR + Non-Preemptive Priority Scheduling ---\n";

    while (completed_processes < num_processes) {
        // 將所有已到達且未加入佇列的行程加入對應優先級的佇列
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_burst_time > 0 && processes[i].last_run_time <= current_time) {
                // last_run_time <= current_time 用於確保未完成的行程會被重新放入隊列
                // 這裡的邏輯需要更精確，確保一個行程只被加入一次，直到它完成或被彈出
                bool in_queue = false;
                for(int p_level = 1; p_level <= 3; p_level++) {
                    queue<int> temp_q = priority_queues[p_level];
                    while(!temp_q.empty()){
                        if(temp_q.front() == processes[i].id){
                            in_queue = true;
                            break;
                        }
                        temp_q.pop();
                    }
                    if(in_queue) break;
                }
                if (!in_queue && processes[i].completion_time == -1) { // 確保未在任何佇列中且未完成
                    priority_queues[processes[i].priority].push(processes[i].id);
                    processes[i].last_run_time = current_time; // 設定上次檢查時間，防止重複加入
                }
            }
        }

        int current_proc_id = -1;
        int current_proc_idx = -1;

        // 從最高優先級的佇列開始檢查
        for (int p_level = 1; p_level <= 3; ++p_level) {
            if (!priority_queues[p_level].empty()) {
                current_proc_id = priority_queues[p_level].front();
                priority_queues[p_level].pop(); // 從佇列中取出
                // 找到對應的行程物件
                for(size_t i = 0; i < processes.size(); i++) {
                    if (processes[i].id == current_proc_id) {
                        current_proc_idx = i;
                        break;
                    }
                }
                break; // 找到最高優先級的行程
            }
        }

        if (current_proc_id == -1) { // if all queue empty -> CPU空閒
            gantt_chart_data[current_time] = -1;
            current_time++;
            prev_proc_id = -1;
            continue;
        }

        Process& current_proc = processes[current_proc_idx];

        if (prev_proc_id != -1 && prev_proc_id != current_proc.id) {
            context_switches++;
        }
        prev_proc_id = current_proc.id;

        if (!current_proc.is_started) {
            current_proc.start_time = current_time;
            current_proc.is_started = true;
        }

        // 執行一個time quantum或直到完成 (非搶佔式優先級，但 RR 內部搶佔)
        int execute_time = min(time_quantum, current_proc.remaining_burst_time);
        
        for (int i = 0; i < execute_time; i++) {
             gantt_chart_data[current_time + i] = current_proc.id;
        }

        current_proc.remaining_burst_time -= execute_time;
        current_time += execute_time;

        if (current_proc.remaining_burst_time == 0) {
            current_proc.completion_time = current_time;
            completed_processes++;
        } else {
            // 如果未完成，將其放回其優先級佇列的尾部 (RR 行為)
            priority_queues[current_proc.priority].push(current_proc.id);
            // 更新 last_run_time 避免重複加入
            processes[current_proc_idx].last_run_time = current_time;
        }
    }

    // 將排程後的結果複製回原始 process 列表，以便 print_results 函數使用
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
    string csv_filename = "RR_priority_gantt_data.csv";
    save_gantt_chart_data_to_csv(gantt_chart_data, csv_filename);
    print_results(original_processes, context_switches, "RR + Non-Preemptive Priority Scheduling");
}
