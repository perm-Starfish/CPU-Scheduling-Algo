import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import numpy as np
import os # For checking file existence

def plot_gantt_chart_from_csv(data_file, title, output_filename):
    """
    Reads Gantt chart data from a CSV file and plots a bar-style Gantt chart.
    The CSV file is expected to have 'Time' and 'ProcessID' columns.
    """
    if not os.path.exists(data_file):
        print(f"Warning: Data file '{data_file}' not found. Skipping plot for {title}.")
        return

    # 1. Read Gantt Data
    df = pd.read_csv(data_file)

    if df.empty:
        print(f"Warning: No Gantt data found in '{data_file}'. Skipping plot for {title}.")
        return

    # 2. Process Gantt Data into Segments
    gantt_segments = []
    
    # Sort by time to ensure correct segment processing
    df = df.sort_values(by='Time').reset_index(drop=True)

    if not df.empty:
        current_proc_id = df['ProcessID'].iloc[0]
        segment_start_time = df['Time'].iloc[0]

        for i in range(1, len(df)):
            time_point = df['Time'].iloc[i]
            proc_id = df['ProcessID'].iloc[i]

            if proc_id != current_proc_id:
                gantt_segments.append({
                    'process_id': current_proc_id,
                    'start_time': segment_start_time,
                    'end_time': time_point
                })
                current_proc_id = proc_id
                segment_start_time = time_point
        
        # Add the last segment
        # The end time of the last segment is the last recorded time point + 1ms (duration)
        gantt_segments.append({
            'process_id': current_proc_id,
            'start_time': segment_start_time,
            'end_time': df['Time'].iloc[-1] + 1 
        })

    # 3. Prepare Plotting
    fig, ax = plt.subplots(figsize=(15, 4)) # Adjusted figure size for no info box

    # Get unique process IDs (excluding idle)
    unique_proc_ids = sorted(list(set(segment['process_id'] for segment in gantt_segments if segment['process_id'] != -1)))
    
    # Create a colormap for processes
    colors = plt.cm.get_cmap('tab10', len(unique_proc_ids)) # 'tab10' for up to 10 distinct colors
    process_color_map = {pid: colors(i) for i, pid in enumerate(unique_proc_ids)}
    process_color_map[-1] = 'lightgray' # Color for idle CPU

    # Determine y-axis positions for processes
    # Instead of a single y_pos, let's map each process ID to its own y-level
    # This will create a multi-row Gantt chart similar to the example image.
    
    # Identify all process IDs that appear in the Gantt chart (excluding idle)
    all_active_pids = sorted(list(set(s['process_id'] for s in gantt_segments if s['process_id'] != -1)))
    
    # Create a mapping from process ID to y-index
    # We want 'P1' at the bottom, 'P2' above it, etc., similar to the example.
    # Matplotlib plots from bottom up for barh, so reverse the order for display.
    pid_to_y_index = {pid: i for i, pid in enumerate(all_active_pids)}
    y_tick_labels = [f'P{pid}' for pid in all_active_pids]

    # Max process ID for proper y-axis scaling
    max_pid_for_y = len(all_active_pids) 
    
    # 4. Plotting Gantt Bars
    for segment in gantt_segments:
        proc_id = segment['process_id']
        start_time = segment['start_time']
        end_time = segment['end_time']
        duration = end_time - start_time

        color = process_color_map.get(proc_id, 'black')
        label_text = f'P{proc_id}' if proc_id != -1 else 'Idle'

        # Get the y-position for this process
        current_y_pos = pid_to_y_index.get(proc_id, -1) # Default to -1 for idle or unmapped
        
        if current_y_pos != -1: # Only plot active processes in their respective rows
            ax.barh(current_y_pos, duration, left=start_time, height=0.8, color=color, edgecolor='black', linewidth=1)
            
            # Add text label in the center of the bar
            if duration > 0: # Only add label if bar has width
                ax.text(start_time + duration / 2, current_y_pos, label_text,
                        ha='center', va='center', color='black', fontsize=9, weight='bold')
        elif proc_id == -1: # Handle Idle as a separate row if desired, or skip
            # For simplicity, if Idle is a long continuous block, you might want to show it.
            # If idle blocks are short and interspersed, might be better to visually leave blank.
            # For this multi-row chart, let's just not plot explicit Idle bars.
            pass


    # 5. Customize Plot
    # Set y-axis ticks and labels based on processed IDs
    ax.set_yticks(np.arange(max_pid_for_y))
    ax.set_yticklabels(y_tick_labels)
    ax.set_ylabel('Process', fontsize=12) # Label for y-axis

    # Set Y-axis limits to provide padding
    ax.set_ylim(-0.5, max_pid_for_y - 0.5) # Adjust limits to fit bars and labels

    ax.set_xlabel('Time', fontsize=12)
    ax.set_title(title, fontsize=16, weight='bold')

    # Set X-axis limits based on max time
    max_time_point = df['Time'].iloc[-1] + 1 if not df.empty else 0
    max_x_limit = max_time_point + (5 - (max_time_point % 5) if max_time_point % 5 != 0 else 0) # Round up to nearest 5 for cleaner ticks
    ax.set_xlim(0, max_x_limit)
    ax.set_xticks(np.arange(0, max_x_limit + 1, 5)) # Tick every 5ms

    # Add grid lines for time
    ax.xaxis.grid(True, linestyle='--', alpha=0.7)
    ax.set_axisbelow(True) # Ensure grid is behind the bars
    
    # Remove top and right spines for cleaner look
    ax.spines['top'].set_visible(False)
    ax.spines['right'].set_visible(False)
    ax.spines['left'].set_visible(False) # Hide left Y-axis line
    ax.tick_params(axis='y', length=0) # Hide Y-axis ticks

    plt.tight_layout()
    plt.savefig(output_filename, dpi=300) # Save as high-resolution PNG
    plt.close(fig) # Close the figure to free up memory

if __name__ == "__main__":
    # Define the algorithms and their corresponding CSV file names and output image names
    algorithms = [
        {"name": "Preemptive SJF", "csv": "preeSJF_gantt_data.csv", "output": "preeSJF_gantt_chart.png"},
        {"name": "RR + Non-Preemptive Priority", "csv": "RR_priority_gantt_data.csv", "output": "RR_priority_gantt_chart.png"},
        {"name": "Multilevel Feedback Queue", "csv": "MLFQ_gantt_data.csv", "output": "MLFQ_gantt_chart.png"},
        {"name": "Custom Algorithm", "csv": "custom_gantt_data.csv", "output": "custom_gantt_chart.png"},
    ]

    print("Generating Gantt charts from CSV data (without results in plot)...")
    for algo in algorithms:
        plot_gantt_chart_from_csv(algo["csv"], f'{algo["name"]} Gantt Chart', algo["output"])

    print("\nAll Gantt charts processing complete.")
