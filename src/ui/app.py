import streamlit as st
import pandas as pd

# Global variables to store process data
processes = []
process_id_counter = 1

# Define a function to add a new process
def add_process():
    global process_id_counter
    process_id = process_id_counter
    burst_time = st.number_input("Enter Burst Time:")
    arrival_time = st.number_input("Enter Arrival Time:")
    priority = st.number_input("Enter Priority:", min_value=1)
    processes.append({"ID": process_id, "Burst Time": burst_time, "Arrival Time": arrival_time, "Priority": priority})
    process_id_counter += 1

# Define a function to edit a process
def edit_process():
    selected_process = st.selectbox("Select Process to Edit:", options=[process["ID"] for process in processes])
    index_to_edit = [index for index, process in enumerate(processes) if process["ID"] == selected_process][0]
    burst_time = st.number_input("Enter Burst Time:", value=processes[index_to_edit]["Burst Time"])
    arrival_time = st.number_input("Enter Arrival Time:", value=processes[index_to_edit]["Arrival Time"])
    priority = st.number_input("Enter Priority:", min_value=1, value=processes[index_to_edit]["Priority"])
    processes[index_to_edit] = {"ID": selected_process, "Burst Time": burst_time, "Arrival Time": arrival_time, "Priority": priority}

# Define a function to delete selected processes
def delete_processes():
    selected_processes = st.multiselect("Select Process(es) to Delete:", options=[process["ID"] for process in processes])
    for process_id in selected_processes:
        processes[:] = [process for process in processes if process["ID"] != process_id]

# Define a function to run the selected algorithm
def run_algorithm():
    algorithm = st.selectbox("Select Algorithm:", options=["FCFS", "SJF", "Round Robin"])
    if algorithm == "Round Robin":
        time_quantum = st.number_input("Enter Time Quantum:", min_value=1)
        # Add logic for Round Robin with time quantum
        st.write("Round Robin selected with Time Quantum:", time_quantum)
    else:
        # Add logic for other algorithms (FCFS, SJF)
        st.write(f"{algorithm} selected")

# Main UI
def main():
    st.title("CPU Scheduling Algorithms Visualizer")
    st.header("Process Management")

    if st.button("Add Process", key="add_btn"):
        add_process()

    if st.button("Edit Process", key="edit_btn"):
        edit_process()

    if st.button("Delete Process", key="delete_btn"):
        delete_processes()

    st.header("Process Table")
    df = pd.DataFrame(processes)
    st.dataframe(df)

    st.header("Run Algorithm")
    run_algorithm()

if __name__ == "__main__":
    main()
