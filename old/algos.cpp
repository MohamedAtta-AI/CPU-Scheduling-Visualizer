#include "LL.cpp"


int* fcfs(TSortedList<Process> ready, int& curr_time) {
    int total_waiting, total_turnaround, flag;
    total_waiting = total_turnaround = flag = 0;
    cout << "TIME: " << curr_time << endl;
    // cout << "Ready: "; ready.print(); cout << " SIZE: " << ready.getSize() << endl;
    Process curr_p;
    if (ready.getSize()) {
        for (int i = 0; i < ready.getSize(); i++) {
            curr_p = ready.at(i);
            for (int j = 0; j < i; j++) {
                Process prev_p = ready.at(j);
                if (curr_p.arrival == prev_p.arrival)
                    curr_p.waiting += prev_p.burst;
                else
                    cout << "Current time: " << curr_time << " Arrival: " << curr_p.arrival << endl;
                    curr_p.waiting = curr_time - curr_p.arrival;
            }
            cout << "Curr waiting: ID:" << curr_p.id << " Wait: " << curr_p.waiting << endl;
            total_waiting += curr_p.waiting;
            curr_p.turnaround = curr_p.waiting + curr_p.burst;
            total_turnaround += curr_p.turnaround;
            flag = 1;
        }
    } else {
        curr_time++;
    }
    if (flag) curr_time += curr_p.burst;

    static int out[2] = { total_waiting, total_turnaround };
    return out;
}


void run_algorithm(TSortedList<Process> processes) {
    int curr_time = 0; int* out;

    TSortedList<Process> ready;
    TSortedList<Gantt> gantt;

    while (processes.getSize()) {
        // cout << "Processes size: " << processes.getSize() << " Content: "; processes.print(); cout << endl;
        // cout << "Time: " << curr_time << endl;
        for (int i = 0; i < processes.getSize(); i++) {
            // cout << "T0" << endl;
            Process p = processes.at(i);
            if (curr_time >= p.arrival) {
                // cout << "T1" << endl;
                ready.addToTail(p);
                processes.removeFromHead();
                // cout << "T2" << endl;
            }
            // cout << "T3" << endl;
        }
        out = fcfs(ready, curr_time);
        // for (int i = 0; i < ready.getSize(); i++) ready.removeFromHead();
    }
    //print totals
    for (int i = 0; i < 2; i++)
        cout << "Totals: " << out[i] << " ";
    cout << endl;
}


int main() {
    TSortedList<Process> processes;
    for (int i = 0; i < 3 ; i++) {
        Process data;
        cin >> data.id >> data.arrival >> data.burst;
        processes.addToTail(data);
    }
    processes.InsertionSort();
    run_algorithm(processes);
}