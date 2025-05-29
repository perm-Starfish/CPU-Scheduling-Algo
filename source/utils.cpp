#include "utils.h"
#include <iostream>
#include <iomanip>
#include <numeric> // For std::accumulate (if used for sums)
#include <algorithm> // For std::sort (if used for sorting results by ID)
#include <fstream> // For file operations

using namespace std;

void print_results(const vector<Process>& processes, int context_switches, const string& algo_name) {
    cout << "\n--- " << algo_name << " Results ---\n";

    double total_waiting_time = 0;
    double total_turnaround_time = 0;

    cout << "Process ID | Arrival Time | Burst Time | Completion Time | Turnaround Time | Waiting Time\n";
    cout << "-----------------------------------------------------------------------------------------\n";

    for (const auto& p : processes) {
        // Turnaround Time (Ti) = Completion Time (Ci) - Arrival Time (Ai)
        int turnaround_time = p.completion_time - p.arrival_time;

        // Waiting Time (Wi) = Turnaround Time (Ti) - Burst Time (Bi)
        int waiting_time = turnaround_time - p.burst_time;

        total_waiting_time += waiting_time;
        total_turnaround_time += turnaround_time;

        cout << setw(10) << p.id << " | "
             << setw(12) << p.arrival_time << " | "
             << setw(10) << p.burst_time << " | "
             << setw(15) << p.completion_time << " | "
             << setw(15) << turnaround_time << " | "
             << setw(12) << waiting_time << "\n";
    }

    cout << "-----------------------------------------------------------------------------------------\n";
    cout << "Average Turnaround Time: " << total_turnaround_time / processes.size() << " ms\n";
    cout << "Average Waiting Time: " << total_waiting_time / processes.size() << " ms\n";
    cout << "Context Switches: " << context_switches << "\n";
}

void print_gantt_chart(const map<int, int>& gantt_chart_data) {
    cout << "\nGantt Chart:\n";
    if (gantt_chart_data.empty()) {
        cout << "  [Empty]\n";
        return;
    }

    // 找出最大時間點
    int max_time = 0;
    for (const auto& entry : gantt_chart_data) {
        if (entry.first > max_time) {
            max_time = entry.first;
        }
    }

    cout << "Time:   ";
    for (int t = 0; t <= max_time; t++) {
        cout << setw(4) << t;
    }
    cout << "\n";

    cout << "Process:";
    for (int t = 0; t <= max_time; t++) {
        auto it = gantt_chart_data.find(t);
        int current_proc_id = (it != gantt_chart_data.end()) ? it->second : -1;

        if (current_proc_id == -1) {
            cout << setw(4) << "-"; // 表示CPU空閒
        } else {
            cout << setw(4) << current_proc_id;
        }
    }
    cout << "\n";
}
    // 新增：將甘特圖數據存到 CSV
void save_gantt_chart_data_to_csv(const map<int, int>& gantt_chart_data, const string& filename) {
    ofstream outfile(filename);
    if (!outfile.is_open()) {
        cerr << "Error: Could not open file " << filename << " for writing Gantt chart data." << endl;
        return;
    }

    outfile << "Time,ProcessID\n"; // CSV 標頭

    // 由於 map 是按鍵排序的，可以直接遍歷
    for (const auto& entry : gantt_chart_data) {
        outfile << entry.first << "," << entry.second << "\n";
    }

    outfile.close();
    cout << "Gantt chart data saved to " << filename << endl;
}
