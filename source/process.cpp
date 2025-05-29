#include "process.h"
#include <random>

// Process constructor
Process::Process(int _id, int _arrival, int _burst, int _priority)
    : id(_id), arrival_time(_arrival), burst_time(_burst), priority(_priority),
      remaining_burst_time(_burst), start_time(-1), completion_time(-1),
      last_run_time(_arrival), is_started(false) {}

std::vector<Process> generate_processes(int num_processes) {
    std::vector<Process> processes;
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister algo: generate random number

    std::vector<int> arrival_options = {0, 5, 10}; // arrival time options
    std::uniform_int_distribution<> burst_dist(6, 28); // burst time [6, 28]
    std::uniform_int_distribution<> priority_dist(1, 3); // priority [1, 3]

    for (int i = 0; i < num_processes; i++) {
        int id = i + 1;
        int arrival = arrival_options[gen() % arrival_options.size()];
        int burst = burst_dist(gen);
        int priority = priority_dist(gen);
        processes.emplace_back(id, arrival, burst, priority);
    }
    return processes;
}
