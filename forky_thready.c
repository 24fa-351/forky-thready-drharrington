#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCESSES 256

void pattern1(int num_processes);
void pattern2(int num_processes);
void pattern3(int start, int end);

void random_sleep() {
    int sleep_time = rand() % 8 + 1;  // Sleep between 1 and 8 seconds
    sleep(sleep_time);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_processes> <pattern_number>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_processes = atoi(argv[1]);   // Number of processes
    int pattern_choice = atoi(argv[2]);  // Pattern number

    if (num_processes < 1 || num_processes > MAX_PROCESSES) {
        fprintf(stderr, "Number of processes must be between 1 and %d\n",
                MAX_PROCESSES);
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));  // Seed for random sleep

    switch (pattern_choice) {
        case 1:
            pattern1(num_processes);
            break;
        case 2:
            pattern2(num_processes);
            break;
        case 3:
            pattern3(1, num_processes);
            break;
        default:
            fprintf(stderr, "Invalid pattern number. Use 1, 2, or 3.\n");
            exit(EXIT_FAILURE);
    }

    return 0;
}

// Pattern 1: Fork all processes at once, and then wait for all to complete
void pattern1(int num_processes) {
    pid_t process_ids[MAX_PROCESSES];

    for (int i = 0; i < num_processes; ++i) {
        process_ids[i] = fork();
        if (process_ids[i] == 0) {  // Child process
            printf("Process %d (PID: %d) beginning\n", i + 1, getpid());
            random_sleep();
            printf("Process %d (PID: %d) exiting\n", i + 1, getpid());
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all children to finish
    for (int i = 0; i < num_processes; ++i) {
        waitpid(process_ids[i], NULL, 0);
        printf("Parent process waiting for Process %d (PID: %d)\n", i + 1,
               process_ids[i]);
    }
}

// Pattern 2: Create processes iteratively
void pattern2(int num_processes) {
    pid_t process_id;

    for (int i = 0; i < num_processes; ++i) {
        process_id = fork();
        if (process_id == 0) {  // Child process
            printf("Process %d (PID: %d) beginning\n", i + 1, getpid());
            random_sleep();
            if (i < num_processes - 1) {
                printf("Process %d (PID: %d) creating Process %d\n", i + 1,
                       getpid(), i + 2);
            }
            printf("Process %d (PID: %d) exiting\n", i + 1, getpid());
            exit(EXIT_SUCCESS);
        } else {
            waitpid(process_id, NULL,
                    0);  // Parent waits for the child to finish
            printf("Parent process waiting for Process %d (PID: %d)\n", i + 1,
                   process_id);
        }
    }
}

// Pattern 3: Create processes in a binary tree structure
void pattern3(int start, int end) {
    if (start > end) {
        return;
    }

    pid_t left_pid = -1, right_pid = -1;

    printf("Process %d (PID: %d) beginning\n", start, getpid());
    random_sleep();

    if (2 * start <= end) {
        left_pid = fork();
        if (left_pid == 0) {  // Left child
            printf("Process %d (PID: %d) creating Process %d\n", start,
                   getpid(), 2 * start);
            pattern3(2 * start, end);
            exit(EXIT_SUCCESS);
        }
    }

    if (2 * start + 1 <= end) {
        right_pid = fork();
        if (right_pid == 0) {  // Right child
            printf("Process %d (PID: %d) creating Process %d\n", start,
                   getpid(), 2 * start + 1);
            pattern3(2 * start + 1, end);
            exit(EXIT_SUCCESS);
        }
    }

    // Parent waits for both left and right children
    if (left_pid > 0) {
        waitpid(left_pid, NULL, 0);
        printf("Process %d (PID: %d) waited for left Process %d\n", start,
               getpid(), 2 * start);
    }
    if (right_pid > 0) {
        waitpid(right_pid, NULL, 0);
        printf("Process %d (PID: %d) waited for right Process %d\n", start,
               getpid(), 2 * start + 1);
    }

    printf("Process %d (PID: %d) exiting\n", start, getpid());
}
