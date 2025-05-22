#include "Process.h"
#include <random>

// Process constructor
Process::Process(int _id, int _arrival, int _burst, int _priority)
    : id(_id), arrival_time(_arrival), burst_time(_burst), priority(_priority),
      remaining_burst_time(_burst), start_time(-1), completion_time(-1),
      last_run_time(_arrival), is_started(false) {}

// 隨機生成行程的函數實作
std::vector<Process> generate_processes(int num_processes) {
    std::vector<Process> processes;
    std::random_device rd;
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 演算法生成隨機數

    std::vector<int> arrival_options = {0, 5, 10}; // 到達時間選項
    std::uniform_int_distribution<> burst_dist(6, 28); // burst time 範圍 [6, 28]
    std::uniform_int_distribution<> priority_dist(1, 3); // 優先級範圍 [1, 3]

    for (int i = 0; i < num_processes; ++i) {
        int id = i + 1;
        int arrival = arrival_options[gen() % arrival_options.size()];
        int burst = burst_dist(gen);
        int priority = priority_dist(gen);
        processes.emplace_back(id, arrival, burst, priority);
    }
    return processes;
}