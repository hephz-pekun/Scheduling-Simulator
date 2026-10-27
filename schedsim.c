// C program for implementation of Simulation
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "process.h"
#include "util.h"

// Function to find the waiting time for all processes (Round Robin)
void findWaitingTimeRR(ProcessType plist[], int n, int quantum)
{
    int rem_bt[n];

    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int t = 0;

    while (1) {
        int done = 1;
        int executed = 0;

        for (int i = 0; i < n; i++) {

            if (plist[i].art <= t && rem_bt[i] > 0) {

                done = 0;
                executed = 1;

                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                }
                else {
                    t += rem_bt[i];

                    plist[i].wt =
                        t -
                        plist[i].bt -
                        plist[i].art;

                    if (plist[i].wt < 0)
                        plist[i].wt = 0;

                    rem_bt[i] = 0;
                }
            }
        }

        if (done)
            break;

        if (!executed)
            t++;
    }
}

// Function to find the waiting time for all processes (SJF/SRTF)
void findWaitingTimeSJF(ProcessType plist[], int n)
{
    int rt[n];

    for (int i = 0; i < n; i++)
        rt[i] = plist[i].bt;

    int complete = 0;
    int t = 0;
    int shortest = 0;
    int finish_time;
    int minm;
    int found;

    while (complete != n) {

        minm = INT_MAX;
        found = 0;

        for (int j = 0; j < n; j++) {

            if (plist[j].art <= t &&
                rt[j] > 0 &&
                rt[j] < minm) {

                minm = rt[j];
                shortest = j;
                found = 1;
            }
        }

        if (!found) {
            t++;
            continue;
        }

        rt[shortest]--;

        if (rt[shortest] == 0) {

            complete++;

            finish_time = t + 1;

            plist[shortest].wt =
                finish_time -
                plist[shortest].bt -
                plist[shortest].art;

            if (plist[shortest].wt < 0)
                plist[shortest].wt = 0;
        }

        t++;
    }
}

// FCFS waiting time
void findWaitingTime(ProcessType plist[], int n)
{
    plist[0].wt = 0;

    int current_time = plist[0].art + plist[0].bt;

    for (int i = 1; i < n; i++) {

        if (current_time < plist[i].art)
            current_time = plist[i].art;

        plist[i].wt = current_time - plist[i].art;

        if (plist[i].wt < 0)
            plist[i].wt = 0;

        current_time += plist[i].bt;
    }
}

// Function to calculate turnaround time
void findTurnAroundTime(ProcessType plist[], int n)
{
    for (int i = 0; i < n; i++)
        plist[i].tat = plist[i].bt + plist[i].wt;
}

// Sort by arrival time
int arrival_comparer(const void *a, const void *b)
{
    ProcessType *p1 = (ProcessType *)a;
    ProcessType *p2 = (ProcessType *)b;

    return p1->art - p2->art;
}

// Sort by priority descending, arrival ascending
int my_comparer(const void *this, const void *that)
{
    ProcessType *p1 = (ProcessType *)this;
    ProcessType *p2 = (ProcessType *)that;

    if (p1->pri != p2->pri)
        return p2->pri - p1->pri;

    return p1->art - p2->art;
}

// FCFS
void findavgTimeFCFS(ProcessType plist[], int n)
{
    qsort(plist, n, sizeof(ProcessType), arrival_comparer);

    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);

    printf("\n*********\nFCFS\n");
}

// SJF
void findavgTimeSJF(ProcessType plist[], int n)
{
    qsort(plist, n, sizeof(ProcessType), arrival_comparer);

    findWaitingTimeSJF(plist, n);
    findTurnAroundTime(plist, n);

    printf("\n*********\nSJF\n");
}

// RR
void findavgTimeRR(ProcessType plist[], int n, int quantum)
{
    findWaitingTimeRR(plist, n, quantum);
    findTurnAroundTime(plist, n);

    printf("\n*********\nRR Quantum = %d\n", quantum);
}

// Priority
void findavgTimePriority(ProcessType plist[], int n)
{
    qsort(plist, n, sizeof(ProcessType), my_comparer);

    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);

    printf("\n*********\nPriority\n");
}

void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0;
    int total_tat = 0;
    float awt, att;

    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n");

    for (int i = 0; i < n; i++) {
        total_wt += plist[i].wt;
        total_tat += plist[i].tat;

        printf("\t%d\t\t%d\t\t%d\t\t%d\n",
               plist[i].pid,
               plist[i].bt,
               plist[i].wt,
               plist[i].tat);
    }

    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);

    printf("\nAverage waiting time = %.2f", awt);
    printf("\nAverage turn around time = %.2f\n", att);
}

ProcessType *initProc(char *filename, int *n)
{
    FILE *input_file = fopen(filename, "r");

    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    ProcessType *plist = parse_file(input_file, n);

    fclose(input_file);

    return plist;
}

// Driver code
int main(int argc, char *argv[])
{
    int n;
    int quantum = 2;

    ProcessType *proc_list;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }

    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimeSJF(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // Priority
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimePriority(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);

    findavgTimeRR(proc_list, n, quantum);
    printMetrics(proc_list, n);
    free(proc_list);

    return 0;
}