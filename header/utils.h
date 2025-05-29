#ifndef UTILS_H
#define UTILS_H

#include "Process.h"
#include <vector>
#include <string>
#include <map>

void print_results(const std::vector<Process>& processes, int context_switches, const std::string& algo_name);
void print_gantt_chart(const std::map<int, int>& gantt_chart_data);
void save_gantt_chart_data_to_csv(const std::map<int, int>& gantt_chart_data, const std::string& filename);

#endif // UTILS_H
