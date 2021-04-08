#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

/* global variable declaration here */
//If you want to update a variable in another function, you will need to pass a pointer to it.
//when the function modifies the variable it's only modifying its own local copy and not the global variable.
int time = 0;
int total_process_num = 0;
int finished_process = 0;
float total_overhead_time = 0.0;
float total_turnaround_time = 0.0;
float max_overhead_time = 0.0;

//define a data structure here
// structure: a type that has members; members can be other types
// syntax: struct some_name
//          { type_name member_name_1;
//            type_name member_name_2;
//            ......... .............}
typedef struct
{
    int time_arrived;
    int process_id;
    int execution_time;
    char parallelisable;
    int state;
    int remain_time;
    // int time_process_complete;
    // int time_turnaround;
    // int time_overhead;
} process_t;

typedef struct
{
    process_t **cpu_process;
    int cpu_process_len;
    int cpu_id;
    int cpu_total_remaining_time;
} cpu_t;

void func_one_cpu(process_t **all_process, int total_process_num);
void func_two_cpu(process_t **all_process, int total_process_num);
void func_n_cpu(process_t **all_process, int total_process_num, int cpu_n_number);
void func_sort_queue(process_t **current_queue, int low, int high);
int partition(process_t **current_queue, int low, int high);
void func_run_cpu(process_t **cpu0, int *length0, process_t **cpu1, int length1, int which_cpu);
void func_into_cpu(process_t **first_cpu, int *first_len, process_t **sec_cpu, int *sec_len, process_t *coming_process);
bool func_has_sub(int id, process_t **cpu1, int length1);
void func_finish_process(process_t **cpu0, int *length0, process_t **cpu1, int length1, int which_cpu);
void func_into_n_cpu(cpu_t **info_all_cpus, process_t *coming_process, int cpu_n_number);
void run_nth_cpu(cpu_t **info_all_cpus, int j, int cpu_n_number);
void finish_nth_cpu(cpu_t **info_all_cpus, int j, int cpu_n_number);
void func_sort_cpus(cpu_t **info_all_cpus, int cpu_n_number);
void func_into_current_cpu(cpu_t *this_cpu, process_t *the_subprocess);
void func_quicksort_cpu(cpu_t **info_all_cpus, int low, int high);
int partition_cpu(cpu_t **info_all_cpus, int low, int high);
int func_distinct_id(cpu_t **info_all_cpus, int cpu_n_number);
int func_distinct_id_two(process_t **cpu0, int length0, process_t **cpu1, int length1);

//argv is the pointer to an array of strings that represents the arguments
//access using argv[i]
int main(int argc, char **argv)
{

    //pointer: some_type* pointer_name;
    // we use * after some_type to signal the pointer type
    // example: int x = 123; int* p = &x; *p = 456; then x is 456 instead
    // example: &arr[0] == arr which is the address of first element of array
    // int arr = {10, 20, 30, 40, 50}; int* p = arr; p+=2; *p is now 30

    //int *process_num;
    int INIT = 1000;
    //an array that stores all processes
    //sizeof tells the size of data structure process_t in bytes
    //-> so malloc arrange a space for INIT * sizeof(process_t*)
    process_t **all_process = (process_t **)malloc(sizeof(process_t *) * INIT);
    for (int i = 0; i < INIT; i++)
    {
        all_process[i] = (process_t *)malloc(sizeof(process_t));
    }
    assert(all_process != NULL);

    char *filename;
    char *cpu_number_char;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-f") == 0)
        {
            filename = argv[i + 1];
            //printf("%s", filename);
        }
        if (strcmp(argv[i], "-p") == 0)
        {
            cpu_number_char = argv[i + 1];
            //printf("process number is %s\n", cpu_number_char);
        }
    }

    //If you fopen and don't close, then that descriptor won't be cleaned up, and will persist until the program closes.
    char line[50];
    FILE *fp = fopen(filename, "r");
    //process_t *one_process = malloc(sizeof(process_t));
    const char s[2] = " ";
    char *token;

    int processes_i = -1;
    while (fgets(line, sizeof(line), fp))
    {
        processes_i += 1;
        token = strtok(line, s);
        if (token != NULL)
        {
            //int atoi(const char *str) converts the string argument str to an integer (type int).
            all_process[processes_i]->time_arrived = atoi(token);
            //printf("index is %d  ------time arrived is %d\n", processes_i, all_process[processes_i]->time_arrived);
        }
        token = strtok(NULL, s);
        if (token != NULL)
        {
            all_process[processes_i]->process_id = atoi(token);
            //printf("index is %d  ------process_id is %d\n", processes_i, all_process[processes_i]->process_id);
        }
        token = strtok(NULL, s);
        if (token != NULL)
        {
            all_process[processes_i]->execution_time = atoi(token);
            all_process[processes_i]->remain_time = atoi(token);
            //printf("index is %d  ------execution_time is %d\n", processes_i, all_process[processes_i]->execution_time);
            //printf("index is %d  ------remain_time is %d\n", processes_i, all_process[processes_i]->remain_time);
        }
        token = strtok(NULL, s);
        if (token != NULL)
        {
            all_process[processes_i]->parallelisable = token[0];
            //printf("index is %d  ------parallel is %c\n", processes_i, all_process[processes_i]->parallelisable);
        }
        all_process[processes_i]->state = 0;
    }

    //how many lines of processes in that file
    total_process_num = processes_i + 1;
    fclose(fp);

    // printf("all process in the main fucntion\n");
    // for (int j = 0; j < total_process_num; j++)
    // {
    //     printf("time arrived is %d\n", all_process[j]->time_arrived);
    //     printf("process_id is %d\n", all_process[j]->process_id);
    //     printf("execution time is %d\n", all_process[j]->execution_time);
    //     printf("remaining time is %d\n", all_process[j]->remain_time);
    //     printf("parallel is %c\n", all_process[j]->parallelisable);
    // }

    if (strcmp(cpu_number_char, "1") == 0)
    {
        func_one_cpu(all_process, total_process_num);
    }
    else if (strcmp(cpu_number_char, "2") == 0)
    {
        func_two_cpu(all_process, total_process_num);
    }
    else
    {
        //printf("yes n cpus here %s\n", cpu_number_char);
        int cpu_n_number = atoi(cpu_number_char);
        func_n_cpu(all_process, total_process_num, cpu_n_number);
    }

    return 0;
}

void func_one_cpu(process_t **all_process, int total_process_num)
{
    //printf("first line of func_one_cpu, total process number is %d\n", total_process_num);
    // printf("all process when entering the func_one_cpu\n");
    // for (int j = 0; j < total_process_num; j++)
    // {
    //     printf("process_id is %d\n", all_process[j]->process_id);
    //     printf("execution time is %d\n", all_process[j]->execution_time);
    //     printf("remaining time is %d\n", all_process[j]->remain_time);
    //     printf("tim arrived is %d\n", all_process[j]->time_arrived);
    // }

    //array of processes
    process_t **current_queue = (process_t **)malloc(sizeof(process_t *) * total_process_num);
    for (int i = 0; i < total_process_num; i++)
    {
        current_queue[i] = (process_t *)malloc(sizeof(process_t));
    }
    assert(current_queue != NULL);

    int current_queue_i = 0;
    //add all process with arrive_time = 0
    for (int i = 0; i < total_process_num; i++)
    {
        if (all_process[i]->time_arrived == 0)
        {
            current_queue[current_queue_i] = all_process[i];
            current_queue_i++;
        }
    }
    //sort queue based on remaining time of process
    func_sort_queue(current_queue, 0, current_queue_i - 1);
    // printf("\n");
    // printf("sorted by quicksort");
    // for (int j = 0; j < current_queue_i; j++)
    // {
    //     printf("remaining time is %d\n", current_queue[j]->remain_time);
    // }
    process_t *least_remain_process;

    while (1)
    {
        least_remain_process = current_queue[0];
        if (least_remain_process->state == 0)
        {
            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", time, least_remain_process->process_id, least_remain_process->remain_time, 0);
            least_remain_process->state = 1;
        }
        least_remain_process->remain_time -= 1;
        time += 1;
        //printf("time is %d", time);

        if (least_remain_process->remain_time == 0)
        {
            float turnaround_time = (float)(time - least_remain_process->time_arrived);
            total_turnaround_time += turnaround_time;
            float overhead_time = (float)(turnaround_time / least_remain_process->execution_time);
            total_overhead_time += overhead_time;
            if (overhead_time > max_overhead_time)
            {
                max_overhead_time = overhead_time;
            }
            int id = least_remain_process->process_id;
            //remove_head(current_queue, &current_queue_i);
            current_queue[0] = current_queue[current_queue_i - 1];
            current_queue_i -= 1;
            finished_process += 1;
            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, id, total_process_num - finished_process);
        }
        if (finished_process == total_process_num)
        {

            printf("Turnaround time %d\n", (int)ceil(total_turnaround_time / total_process_num));
            printf("Time overhead %0.9g %0.9g\n", ceil(max_overhead_time * 100) / 100, ceil(100 * total_overhead_time / total_process_num) / 100);
            printf("Makespan %d\n", time);
            return;
        }
        for (int j = 0; j < total_process_num; j++)
        {
            if (all_process[j]->time_arrived == time)
            {
                current_queue[current_queue_i] = all_process[j];
                current_queue_i++;
            }
        }
        func_sort_queue(current_queue, 0, current_queue_i - 1);
        // printf("\n");
        // printf("check if sorted by quicksort\n");

        // for (int j = 0; j < current_queue_i; j++)
        // {
        //     printf("remaining time is %d\n", current_queue[j]->remain_time);
        // }

        if (current_queue[0] != least_remain_process && least_remain_process != NULL)
        {
            least_remain_process->state = 0;
        }
    }
}

void func_sort_queue(process_t **current_queue, int low, int high)
{
    int pivot_index;
    if (low >= high)
    {
        return;
    }
    pivot_index = partition(current_queue, low, high);
    func_sort_queue(current_queue, low, pivot_index - 1);
    func_sort_queue(current_queue, pivot_index + 1, high);
}

int partition(process_t **current_queue, int low, int high)
{
    process_t *pivot_process = current_queue[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (current_queue[j]->remain_time < pivot_process->remain_time)
        {
            i += 1;
            process_t *temp = current_queue[j];
            current_queue[j] = current_queue[i];
            current_queue[i] = temp;
        }
        else if (current_queue[j]->remain_time == pivot_process->remain_time)
        {
            if (current_queue[j]->process_id < pivot_process->process_id)
            {
                i += 1;
                process_t *temp = current_queue[j];
                current_queue[j] = current_queue[i];
                current_queue[i] = temp;
            }
        }
    }
    process_t *temp2 = current_queue[i + 1];
    current_queue[i + 1] = current_queue[high];
    current_queue[high] = temp2;
    return i + 1;
}

void func_two_cpu(process_t **all_process, int total_process_num)
{

    process_t **first_cpu = (process_t **)malloc(sizeof(process_t *) * total_process_num);
    process_t **sec_cpu = (process_t **)malloc(sizeof(process_t *) * total_process_num);
    //new process coming at that time
    process_t **coming_process = (process_t **)malloc(sizeof(process_t *) * total_process_num);

    for (int i = 0; i < total_process_num; i++)
    {
        first_cpu[i] = (process_t *)malloc(sizeof(process_t));
        sec_cpu[i] = (process_t *)malloc(sizeof(process_t));
        coming_process[i] = (process_t *)malloc(sizeof(process_t));
    }
    assert(first_cpu != NULL);
    assert(sec_cpu != NULL);
    assert(coming_process != NULL);

    int first_len = 0;
    int sec_len = 0;

    int original_total_process = total_process_num;
    while (1)
    {
        int coming_number = 0;
        for (int k = 0; k < original_total_process; k++)
        {
            if (all_process[k]->time_arrived == time)
            {
                *coming_process[coming_number] = *all_process[k];
                coming_number++;

                //put into differnent cpus
                func_into_cpu(first_cpu, &first_len, sec_cpu, &sec_len, coming_process[coming_number - 1]);
            }
        }
        /////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!need a function for finished print first!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        func_finish_process(first_cpu, &first_len, sec_cpu, sec_len, 0);
        func_finish_process(sec_cpu, &sec_len, first_cpu, first_len, 1);
        func_run_cpu(first_cpu, &first_len, sec_cpu, sec_len, 0);
        func_run_cpu(sec_cpu, &sec_len, first_cpu, first_len, 1);

        if (total_process_num == finished_process)
        {
            printf("Turnaround time %d\n", (int)ceil(total_turnaround_time / total_process_num));
            printf("Time overhead %0.9g %0.9g\n", ceil(max_overhead_time * 100) / 100, ceil(100 * total_overhead_time / total_process_num) / 100);
            printf("Makespan %d\n", time);
            return;
        }
        time += 1;
    }
}

void func_into_cpu(process_t **first_cpu, int *first_len, process_t **sec_cpu, int *sec_len, process_t *coming_process)
{

    if (coming_process->parallelisable == 'p')
    {
        int the_remain_time = coming_process->remain_time;
        the_remain_time = ceil(the_remain_time / 2) + 1;
        coming_process->remain_time = the_remain_time;
        ////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!wrong here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        *first_cpu[*first_len] = *coming_process;
        *first_len += 1;
        *sec_cpu[*sec_len] = *coming_process;
        *sec_len += 1;
        return;
    }
    else
    {
        int total_remain_time0 = 0;
        int total_remain_time1 = 0;
        for (int m = 0; m < *first_len; m++)
        {
            total_remain_time0 += first_cpu[m]->remain_time;
        }
        for (int n = 0; n < *sec_len; n++)
        {
            total_remain_time1 += sec_cpu[n]->remain_time;
        }
        if (total_remain_time0 <= total_remain_time1)
        {
            *first_cpu[*first_len] = *coming_process;
            *first_len += 1;
        }
        else
        {
            *sec_cpu[*sec_len] = *coming_process;
            *sec_len += 1;
        }
        return;
    }
}

void func_finish_process(process_t **cpu0, int *length0, process_t **cpu1, int length1, int which_cpu)
{
    if (*length0 == 0)
    {
        return;
    }

    process_t *least_remain_time_process;
    least_remain_time_process = cpu0[0];
    func_sort_queue(cpu0, 0, *length0 - 1);
    if (cpu0[0] != least_remain_time_process && least_remain_time_process != NULL)
    {
        least_remain_time_process->state = 0;
    }

    if (cpu0[0]->remain_time == 0)
    {
        if (cpu0[0]->parallelisable == 'n')
        {
            finished_process += 1;
        }
        else if (cpu0[0]->parallelisable == 'p' && func_has_sub(cpu0[0]->process_id, cpu1, length1) == true)
        {
            *cpu0[0] = *cpu0[*length0 - 1];
            *length0 = *length0 - 1;
            return;
        }
        else if (cpu0[0]->parallelisable == 'p' && func_has_sub(cpu0[0]->process_id, cpu1, length1) == false)
        {

            finished_process += 1;
        }
        float turnaround_time = (float)(time - cpu0[0]->time_arrived);
        total_turnaround_time += turnaround_time;
        float overhead_time = (float)(turnaround_time / cpu0[0]->execution_time);
        total_overhead_time += overhead_time;
        if (overhead_time > max_overhead_time)
        {
            max_overhead_time = overhead_time;
        }
        int id = cpu0[0]->process_id;
        *cpu0[0] = *cpu0[*length0 - 1];
        *length0 = *length0 - 1;
        int distinct_id = func_distinct_id_two(cpu0, *length0, cpu1, length1);
        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, id, distinct_id);
        func_sort_queue(cpu0, 0, *length0 - 1);
    }
    return;
}

int func_distinct_id_two(process_t **cpu0, int length0, process_t **cpu1, int length1)
{
    int array_len = 0;
    int array_distinct_id[1000];
    for (int i = 0; i < length0; i++)
    {
        int process_id_i = cpu0[i]->process_id;
        int in_array = 0;
        for (int k = 0; k < array_len; k++)
        {
            if (process_id_i == array_distinct_id[k])
            {
                in_array = 1;
            }
        }
        if (in_array == 0)
        {
            array_distinct_id[array_len] = process_id_i;
            array_len += 1;
        }
    }
    for (int i = 0; i < length1; i++)
    {
        int process_id_i = cpu1[i]->process_id;
        int in_array = 0;
        for (int k = 0; k < array_len; k++)
        {
            if (process_id_i == array_distinct_id[k])
            {
                in_array = 1;
            }
        }
        if (in_array == 0)
        {
            array_distinct_id[array_len] = process_id_i;
            array_len += 1;
        }
    }
    return array_len;
}
void func_run_cpu(process_t **cpu0, int *length0, process_t **cpu1, int length1, int which_cpu)
{
    if (*length0 == 0)
    {
        return;
    }
    // process_t *least_remain_time_process;
    // least_remain_time_process = cpu0[0];
    // func_sort_queue(cpu0, 0, *length0 - 1);
    // if (cpu0[0] != least_remain_time_process && least_remain_time_process != NULL)
    // {
    //     least_remain_time_process->state = 0;
    // }
    // if (cpu0[0]->remain_time == 0)
    // {
    //     finished_process += 1;
    //     if (cpu0[0]->parallelisable == 'n' || (cpu0[0]->parallelisable == 'p' && func_has_sub(cpu0[0]->process_id, cpu1, length1) == false))
    //     {
    //         printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, cpu0[0]->process_id, total_process_num - finished_process);
    //     }
    //     *cpu0[0] = *cpu0[*length0 - 1];
    //     *length0 = *length0 - 1;
    //     func_sort_queue(cpu0, 0, *length0 - 1);
    // }
    func_sort_queue(cpu0, 0, *length0 - 1);
    if (cpu0[0]->state == 0)
    {
        cpu0[0]->state = 1;
        if (cpu0[0]->parallelisable == 'p')
        {
            printf("%d,RUNNING,pid=%d.%d,remaining_time=%d,cpu=%d\n", time, cpu0[0]->process_id, which_cpu, cpu0[0]->remain_time, which_cpu);
        }
        else
        {
            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", time, cpu0[0]->process_id, cpu0[0]->remain_time, which_cpu);
        }
    }

    cpu0[0]->remain_time -= 1;

    return;
}

bool func_has_sub(int id, process_t **cpu1, int length1)
{
    for (int i = 0; i < length1; i++)
    {
        if (cpu1[i]->process_id == id)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

void func_n_cpu(process_t **all_process, int total_process_num, int cpu_n_number)
{
    //printf("yes in func n cpus here\n");
    cpu_t **info_all_cpus;
    info_all_cpus = malloc(sizeof(cpu_t *) * cpu_n_number);
    for (int v = 0; v < cpu_n_number; v++)
    {
        info_all_cpus[v] = malloc(sizeof(cpu_t));
        assert(info_all_cpus[v]);
        //????????????????????????????????这里要给每个info_all_cpus[v]->cpu_process[i]创造空间吗？？？？？？？？？？？？
        info_all_cpus[v]->cpu_process = malloc(sizeof(process_t *) * total_process_num);
        assert(info_all_cpus[v]->cpu_process);
        info_all_cpus[v]->cpu_process_len = 0;
        info_all_cpus[v]->cpu_total_remaining_time = 0;
        info_all_cpus[v]->cpu_id = v;
    }
    //printf("yes finished malloc of info all cpus\n");
    process_t **coming_process = (process_t **)malloc(sizeof(process_t *) * total_process_num);
    for (int i = 0; i < total_process_num; i++)
    {
        coming_process[i] = (process_t *)malloc(sizeof(process_t));
    }
    assert(coming_process != NULL);

    while (1)
    {
        int coming_number = 0;
        //printf("total process num is %d, finished process num is %d\n", total_process_num, finished_process);
        //printf("yes inside while loop\n");
        for (int k = 0; k < total_process_num; k++)
        {
            if (all_process[k]->time_arrived == time)
            {
                //printf("yes all_process[k]->time_arrived == time\n");
                *coming_process[coming_number] = *all_process[k];
                coming_number++;

                //printf("func_into_n_cpu errrrrorrrrrrrrrrrrrr\n");

                //put into differnent cpus
                func_into_n_cpu(info_all_cpus, coming_process[coming_number - 1], cpu_n_number);
            }
        }
        func_sort_cpus(info_all_cpus, cpu_n_number);
        //printf("yes coming process put in info_all_cpus \n");
        for (int j = 0; j < cpu_n_number; j++)
        {
            finish_nth_cpu(info_all_cpus, j, cpu_n_number);
        }
        for (int j = 0; j < cpu_n_number; j++)
        {
            run_nth_cpu(info_all_cpus, j, cpu_n_number);
        }
        if (finished_process == total_process_num)
        {
            printf("Turnaround time %d\n", (int)ceil(total_turnaround_time / total_process_num));
            printf("Time overhead %.9g %.9g\n", ceil(max_overhead_time * 100) / 100, ceil(100 * total_overhead_time / total_process_num) / 100);
            printf("Makespan %d\n", time);
            return;
        }
        time += 1;
    }
}

void func_into_n_cpu(cpu_t **info_all_cpus, process_t *coming_process, int cpu_n_number)
{
    //printf("inside func_into_n_cpu \n");
    if (coming_process->parallelisable == 'p')
    {
        //printf("yes parallel\n");
        int subprocess_number;
        if (cpu_n_number > coming_process->execution_time)
        {
            subprocess_number = coming_process->execution_time;
        }
        else if (cpu_n_number <= coming_process->execution_time)
        {
            subprocess_number = cpu_n_number;
        }
        //printf("func_sort_cpus????????\n");
        func_sort_cpus(info_all_cpus, cpu_n_number);
        //printf("yessss func_sort_cpus correeeeecccctttt????????\n");
        for (int j = 0; j < subprocess_number; j++)
        {
            process_t *the_subprocess = malloc(sizeof(process_t));
            the_subprocess->time_arrived = coming_process->time_arrived;
            the_subprocess->process_id = coming_process->process_id;
            the_subprocess->execution_time = coming_process->execution_time;
            the_subprocess->remain_time = ceil(coming_process->remain_time / (double)subprocess_number) + 1;
            the_subprocess->state = coming_process->state;
            the_subprocess->parallelisable = coming_process->parallelisable;
            func_into_current_cpu(info_all_cpus[j], the_subprocess);
        }
    }
    else
    {
        func_sort_cpus(info_all_cpus, cpu_n_number);
        func_into_current_cpu(info_all_cpus[0], coming_process);
    }
}

void finish_nth_cpu(cpu_t **info_all_cpus, int j, int cpu_n_number)
{
    //printf("yes in finish_nth_cpu \n");

    if (info_all_cpus[j]->cpu_process_len == 0)
    {
        return;
    }
    cpu_t *this_cpu = info_all_cpus[j];
    process_t **this_cpu_process = this_cpu->cpu_process;
    process_t *least_remain_cpu = this_cpu_process[0];

    func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);
    if (least_remain_cpu != this_cpu_process[0])
    {
        least_remain_cpu->state = 0;
    }
    // if (this_cpu_process[0]->state != 1)
    // {
    //     this_cpu_process[0]->state = 1;
    //     if (this_cpu_process[0]->parallelisable == 'p')
    //     {
    //         printf("%d,RUNNING,pid=%d.%d,remaining_time=%d,cpu=%d\n", time, this_cpu_process[0]->process_id, this_cpu->cpu_id, this_cpu_process[0]->remain_time, this_cpu->cpu_id);
    //     }
    //     else
    //     {
    //         printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", time, this_cpu_process[0]->process_id, this_cpu_process[0]->remain_time, this_cpu->cpu_id);
    //     }
    // }
    // this_cpu_process[0]->remain_time -= 1;
    // this_cpu->cpu_total_remaining_time -= 1;
    //printf("yesss before this_cpu_process[0]->remain_time == 0\n");
    if (this_cpu_process[0]->remain_time == 0)
    {
        //printf("yes in FUNC FINISH -- this_cpu_process[0]->remain_time == 0 \n");
        if (this_cpu_process[0]->parallelisable == 'p')
        {
            //printf("yes parallisable\n");
            int id = this_cpu_process[0]->process_id;
            for (int i = 0; i < cpu_n_number; i++)
            {
                if (i != j && func_has_sub(id, info_all_cpus[i]->cpu_process, info_all_cpus[i]->cpu_process_len) == true)
                {
                    *(this_cpu_process[0]) = *(this_cpu_process[this_cpu->cpu_process_len - 1]);
                    this_cpu->cpu_process_len = this_cpu->cpu_process_len - 1;
                    func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);
                    //func_sort_cpus(info_all_cpus, cpu_n_number);
                    //printf("yes reduce sub process \n");
                    return;
                }
            }
            float turnaround_time = (float)(time - this_cpu_process[0]->time_arrived);
            total_turnaround_time += turnaround_time;
            float overhead_time = (float)(turnaround_time / this_cpu_process[0]->execution_time);
            total_overhead_time += overhead_time;
            if (overhead_time > max_overhead_time)
            {
                max_overhead_time = overhead_time;
            }
            *(this_cpu_process[0]) = *(this_cpu_process[this_cpu->cpu_process_len - 1]);
            this_cpu->cpu_process_len = this_cpu->cpu_process_len - 1;
            func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);
            //func_sort_cpus(info_all_cpus, cpu_n_number);
            int distinct_id = func_distinct_id(info_all_cpus, cpu_n_number);
            finished_process += 1;
            //printf("before finish-->finsi proces  num is %d\n", finished_process);
            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, id, distinct_id);
        }
        else
        {
            //printf("yes nnnnnnnnnnnnnnnn\n");
            float turnaround_time = (float)(time - this_cpu_process[0]->time_arrived);
            total_turnaround_time += turnaround_time;
            float overhead_time = (float)(turnaround_time / this_cpu_process[0]->execution_time);
            total_overhead_time += overhead_time;
            if (overhead_time > max_overhead_time)
            {
                max_overhead_time = overhead_time;
            }
            int id = this_cpu_process[0]->process_id;
            *(this_cpu_process[0]) = *(this_cpu_process[this_cpu->cpu_process_len - 1]);
            this_cpu->cpu_process_len = this_cpu->cpu_process_len - 1;
            func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);
            //func_sort_cpus(info_all_cpus, cpu_n_number);
            int distinct_id = func_distinct_id(info_all_cpus, cpu_n_number);
            finished_process += 1;
            //printf("before finish-->finsi proces  num is %d\n", finished_process);

            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, id, distinct_id);
        }
        // *(this_cpu_process[0]) = *(this_cpu_process[this_cpu->cpu_process_len - 1]);
        // this_cpu->cpu_process_len = this_cpu->cpu_process_len - 1;
        // func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);
        // func_sort_cpus(info_all_cpus, cpu_n_number);
    }
    return;
}

int func_distinct_id(cpu_t **info_all_cpus, int cpu_n_number)
{
    int array_len = 0;
    int array_distinct_id[1000];
    for (int i = 0; i < cpu_n_number; i++)
    {
        for (int j = 0; j < info_all_cpus[i]->cpu_process_len; j++)
        {
            int process_id_j = info_all_cpus[i]->cpu_process[j]->process_id;
            int in_array = 0;
            for (int k = 0; k < array_len; k++)
            {
                if (process_id_j == array_distinct_id[k])
                {
                    in_array = 1;
                }
            }
            if (in_array == 0)
            {
                array_distinct_id[array_len] = process_id_j;
                array_len += 1;
            }
        }
    }
    return array_len;
}

void run_nth_cpu(cpu_t **info_all_cpus, int j, int cpu_n_number)
{
    if (info_all_cpus[j]->cpu_process_len == 0)
    {
        return;
    }
    cpu_t *this_cpu = info_all_cpus[j];
    process_t **this_cpu_process = this_cpu->cpu_process;
    process_t *least_remain_cpu = this_cpu_process[0];

    func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);
    if (least_remain_cpu != this_cpu_process[0])
    {
        least_remain_cpu->state = 0;
    }
    if (this_cpu_process[0]->state != 1)
    {
        this_cpu_process[0]->state = 1;
        if (this_cpu_process[0]->parallelisable == 'p')
        {
            printf("%d,RUNNING,pid=%d.%d,remaining_time=%d,cpu=%d\n", time, this_cpu_process[0]->process_id, this_cpu->cpu_id, this_cpu_process[0]->remain_time, this_cpu->cpu_id);
        }
        else
        {
            printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n", time, this_cpu_process[0]->process_id, this_cpu_process[0]->remain_time, this_cpu->cpu_id);
        }
    }
    ////////////////////////!!!!!!!!!!!!!!!probelm is here!!!!!!!!!!!!!!!
    this_cpu_process[0]->remain_time -= 1;
    this_cpu->cpu_total_remaining_time -= 1;
    //printf("remain time is : %d ---yes reduced remain time successfully\n", this_cpu_process[0]->remain_time);
    return;
}

void func_sort_cpus(cpu_t **info_all_cpus, int cpu_n_number)
{
    //printf("yes in func_sort_cpus\n");
    for (int i = 0; i < cpu_n_number; i++)
    {
        int total_remain_time = 0;
        if (info_all_cpus[i]->cpu_process_len > 0)
        {
            for (int j = 0; j < info_all_cpus[i]->cpu_process_len; j++)
            {
                total_remain_time += info_all_cpus[i]->cpu_process[j]->remain_time;
            }
            info_all_cpus[i]->cpu_total_remaining_time = total_remain_time;
        }
    }
    func_quicksort_cpu(info_all_cpus, 0, cpu_n_number - 1);
}

void func_quicksort_cpu(cpu_t **info_all_cpus, int low, int high)
{
    //printf("yes in func_quicksort_cpus\n");
    int pivot_index;
    if (low >= high)
    {
        return;
    }
    pivot_index = partition_cpu(info_all_cpus, low, high);
    func_quicksort_cpu(info_all_cpus, low, pivot_index - 1);
    func_quicksort_cpu(info_all_cpus, pivot_index + 1, high);
}

int partition_cpu(cpu_t **info_all_cpus, int low, int high)
{
    //printf("yes in partition_cpus\n");
    cpu_t *pivot_process = info_all_cpus[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        //printf("yes in partition for loop\n");
        if (info_all_cpus[j]->cpu_total_remaining_time < pivot_process->cpu_total_remaining_time)
        {
            //printf("yes in partition for loop info_all_cpus[j]->cpu_total_remaining_time < pivot_process->cpu_total_remaining_time\n");
            i += 1;
            cpu_t *temp = info_all_cpus[j];
            info_all_cpus[j] = info_all_cpus[i];
            info_all_cpus[i] = temp;
        }
        else if (info_all_cpus[j]->cpu_total_remaining_time == pivot_process->cpu_total_remaining_time)
        {
            if (info_all_cpus[j]->cpu_id < pivot_process->cpu_id)
            {
                //printf("yes in partition for loop info_all_cpus[j]->cpu_total_remaining_time == pivot_process->cpu_total_remaining_time\n");

                i += 1;
                cpu_t *temp = info_all_cpus[j];
                info_all_cpus[j] = info_all_cpus[i];
                info_all_cpus[i] = temp;
            }
        }
    }
    cpu_t *temp2 = info_all_cpus[i + 1];
    info_all_cpus[i + 1] = info_all_cpus[high];
    info_all_cpus[high] = temp2;
    //printf("yes end of partition\n");
    return i + 1;
}

void func_into_current_cpu(cpu_t *this_cpu, process_t *the_subprocess)
{
    this_cpu->cpu_process[this_cpu->cpu_process_len] = the_subprocess;
    this_cpu->cpu_process_len = this_cpu->cpu_process_len + 1;
    this_cpu->cpu_total_remaining_time = this_cpu->cpu_total_remaining_time + the_subprocess->remain_time;
}

// typedef struct
// {
//     process_t **cpu_process;
//     int cpu_process_len;
//     int cpu_id;
//     int cpu_total_remaining_time;
// } cpu_t;

// typedef struct
// {
//     int time_arrived;
//     int process_id;
//     int execution_time;
//     char parallelisable;
//     int state;
//     int remain_time;
// } process_t;