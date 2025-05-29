#ifndef SCHEDULING_ALGORITHMS_H
#define SCHEDULING_ALGORITHMS_H

#include "process.h"
#include <vector>

void preemptive_sjf_scheduling(std::vector<Process> processes); // preemptive SJF
void rr_priority_scheduling(std::vector<Process> processes);    // RR + priority
void multilevel_feedback_queue_scheduling(std::vector<Process> processes); // MLFQ
void custom_priority_sjf_aging_scheduling(std::vector<Process> processes); // customize

#endif // SCHEDULING_ALGORITHMS_H
