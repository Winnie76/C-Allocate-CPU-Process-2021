#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

int time = 0;
int total_process_num = 0;
int finished_process = 0;
float total_overhead_time = 0.0;
float total_turnaround_time = 0.0;
float max_overhead_time = 0.0;

typedef struct
{
    int time_arrived;
    int process_id;
    int execution_time;
    char parallelisable;
    int state;
    int remain_time;

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

int main(int argc, char **argv)
{

    int INIT = 1000;
    //an array that stores all processes
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
        }
        if (strcmp(argv[i], "-p") == 0)
        {
            cpu_number_char = argv[i + 1];
        }
    }

    char line[50];
    FILE *fp = fopen(filename, "r");
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
        }
        token = strtok(NULL, s);
        if (token != NULL)
        {
            all_process[processes_i]->process_id = atoi(token);
        }
        token = strtok(NULL, s);
        if (token != NULL)
        {
            all_process[processes_i]->execution_time = atoi(token);
            all_process[processes_i]->remain_time = atoi(token);
        }
        token = strtok(NULL, s);
        if (token != NULL)
        {
            all_process[processes_i]->parallelisable = token[0];
        }
        all_process[processes_i]->state = 0;
    }

    //how many lines of processes in that file
    total_process_num = processes_i + 1;
    fclose(fp);

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

        int cpu_n_number = atoi(cpu_number_char);
        func_n_cpu(all_process, total_process_num, cpu_n_number);
    }

    return 0;
}

void func_one_cpu(process_t **all_process, int total_process_num)
{
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

    cpu_t **info_all_cpus;
    info_all_cpus = malloc(sizeof(cpu_t *) * cpu_n_number);
    for (int v = 0; v < cpu_n_number; v++)
    {
        info_all_cpus[v] = malloc(sizeof(cpu_t));
        assert(info_all_cpus[v]);

        info_all_cpus[v]->cpu_process = malloc(sizeof(process_t *) * total_process_num);
        assert(info_all_cpus[v]->cpu_process);
        info_all_cpus[v]->cpu_process_len = 0;
        info_all_cpus[v]->cpu_total_remaining_time = 0;
        info_all_cpus[v]->cpu_id = v;
    }

    process_t **coming_process = (process_t **)malloc(sizeof(process_t *) * total_process_num);
    for (int i = 0; i < total_process_num; i++)
    {
        coming_process[i] = (process_t *)malloc(sizeof(process_t));
    }
    assert(coming_process != NULL);

    while (1)
    {
        int coming_number = 0;
        for (int k = 0; k < total_process_num; k++)
        {
            if (all_process[k]->time_arrived == time)
            {

                *coming_process[coming_number] = *all_process[k];
                coming_number++;

                //put into differnent cpus
                func_into_n_cpu(info_all_cpus, coming_process[coming_number - 1], cpu_n_number);
            }
        }
        func_sort_cpus(info_all_cpus, cpu_n_number);

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

    if (coming_process->parallelisable == 'p')
    {

        int subprocess_number;
        if (cpu_n_number > coming_process->execution_time)
        {
            subprocess_number = coming_process->execution_time;
        }
        else if (cpu_n_number <= coming_process->execution_time)
        {
            subprocess_number = cpu_n_number;
        }

        func_sort_cpus(info_all_cpus, cpu_n_number);

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

    if (this_cpu_process[0]->remain_time == 0)
    {

        if (this_cpu_process[0]->parallelisable == 'p')
        {

            int id = this_cpu_process[0]->process_id;
            for (int i = 0; i < cpu_n_number; i++)
            {
                if (i != j && func_has_sub(id, info_all_cpus[i]->cpu_process, info_all_cpus[i]->cpu_process_len) == true)
                {
                    *(this_cpu_process[0]) = *(this_cpu_process[this_cpu->cpu_process_len - 1]);
                    this_cpu->cpu_process_len = this_cpu->cpu_process_len - 1;
                    func_sort_queue(this_cpu_process, 0, this_cpu->cpu_process_len - 1);

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

            int distinct_id = func_distinct_id(info_all_cpus, cpu_n_number);
            finished_process += 1;

            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, id, distinct_id);
        }
        else
        {

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

            int distinct_id = func_distinct_id(info_all_cpus, cpu_n_number);
            finished_process += 1;

            printf("%d,FINISHED,pid=%d,proc_remaining=%d\n", time, id, distinct_id);
        }
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

    this_cpu_process[0]->remain_time -= 1;
    this_cpu->cpu_total_remaining_time -= 1;

    return;
}

void func_sort_cpus(cpu_t **info_all_cpus, int cpu_n_number)
{

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

    cpu_t *pivot_process = info_all_cpus[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
    {

        if (info_all_cpus[j]->cpu_total_remaining_time < pivot_process->cpu_total_remaining_time)
        {

            i += 1;
            cpu_t *temp = info_all_cpus[j];
            info_all_cpus[j] = info_all_cpus[i];
            info_all_cpus[i] = temp;
        }
        else if (info_all_cpus[j]->cpu_total_remaining_time == pivot_process->cpu_total_remaining_time)
        {
            if (info_all_cpus[j]->cpu_id < pivot_process->cpu_id)
            {
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

    return i + 1;
}

void func_into_current_cpu(cpu_t *this_cpu, process_t *the_subprocess)
{
    this_cpu->cpu_process[this_cpu->cpu_process_len] = the_subprocess;
    this_cpu->cpu_process_len = this_cpu->cpu_process_len + 1;
    this_cpu->cpu_total_remaining_time = this_cpu->cpu_total_remaining_time + the_subprocess->remain_time;
}