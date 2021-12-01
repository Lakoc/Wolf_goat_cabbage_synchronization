//   IOS Projekt 2
//   Alexander Polok(xpolok03)
//   18.4.2019
//   River Crossing Problem

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>

//pointers for senaphores and shared memory
sem_t *sem_wait_for_all = NULL;
sem_t *sem_print = NULL;
sem_t *sem_hack_count = NULL;
sem_t *sem_serf_count = NULL;
sem_t *sem_op_count = NULL;
sem_t *semid_hack = NULL;
sem_t *semid_serf = NULL;
sem_t *sem_molo = NULL;
sem_t *wait_for_captain = NULL;
sem_t *wait_for_crew = NULL;
sem_t *sem_cruise = NULL;
sem_t *boarding = NULL;
sem_t *change_passenger_count = NULL;
sem_t *hackers_board = NULL;
sem_t *serfers_board = NULL;
sem_t *sem_processes = NULL;

int *linenum = NULL;
int *processcounter = NULL;
int *processidhacker = NULL;
int *processidserfer = NULL;
int *processes_on_molo = NULL;
int *hack_on_molo = NULL;
int *serf_on_molo = NULL;
int *cruise = NULL;
int *op_count = NULL;
int *hack_to_boat = NULL;
int *serf_to_boat = NULL;

//file for output
FILE *output;

//struct for params from input
typedef struct
{
    long count;
    long new_procestimeh;
    long new_procestimes;
    long sail_time;
    long molo_return;
    long molo_cap;
} Parameters;

int init()
{
    // make shared memory
    if ((linenum = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *linenum = 1;

    if ((processcounter = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *processcounter = 0;

    if ((processidhacker = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *processidhacker = 1;

    if ((processidserfer = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *processidserfer = 1;

    if ((processes_on_molo = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *processes_on_molo = 0;

    if ((hack_on_molo = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *hack_on_molo = 0;

    if ((serf_on_molo = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *serf_on_molo = 0;

    if ((cruise = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *cruise = 0;

    if ((op_count = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *op_count = 0;

    if ((hack_to_boat = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *hack_to_boat = 0;

    if ((serf_to_boat = mmap(NULL, __SIZEOF_POINTER__, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 1;
    *serf_to_boat = 0;

    //open semaphores(check errors)
    if ((sem_wait_for_all = sem_open("/xpolok03.ios.proj2.sem_wait_for_all", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        return 1;

    if ((sem_print = sem_open("/xpolok03.ios.proj2.sem_print", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((semid_hack = sem_open("/xpolok03.ios.proj2.semid_hack", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((semid_serf = sem_open("/xpolok03.ios.proj2.semid_serf", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((sem_molo = sem_open("/xpolok03.ios.proj2.sem_molo", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((wait_for_captain = sem_open("/xpolok03.ios.proj2.wait_for_captain", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        return 1;

    if ((sem_hack_count = sem_open("/xpolok03.ios.proj2.sem_hack_count", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((sem_serf_count = sem_open("/xpolok03.ios.proj2.sem_serf_count", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((wait_for_crew = sem_open("/xpolok03.ios.proj2.wait_for_crew", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        return 1;

    if ((sem_cruise = sem_open("/xpolok03.ios.proj2.sem_cruise", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((sem_op_count = sem_open("/xpolok03.ios.proj2.sem_op_count", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((boarding = sem_open("/xpolok03.ios.proj2.boarding", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((change_passenger_count = sem_open("/xpolok03.ios.proj2.change_passenger_count", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    if ((hackers_board = sem_open("/xpolok03.ios.proj2.hackers_board", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        return 1;

    if ((serfers_board = sem_open("/xpolok03.ios.proj2.serfers_board", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
        return 1;

    if ((sem_processes = sem_open("/xpolok03.ios.proj2.sem_processes", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
        return 1;

    return 0;
}

//clean everything
void clean()
{
    fclose(output);

    //unmap shared memory
    munmap(linenum, __SIZEOF_POINTER__);
    munmap(processcounter, __SIZEOF_POINTER__);
    munmap(processidhacker, __SIZEOF_POINTER__);
    munmap(processidserfer, __SIZEOF_POINTER__);
    munmap(processes_on_molo, __SIZEOF_POINTER__);
    munmap(hack_on_molo, __SIZEOF_POINTER__);
    munmap(serf_on_molo, __SIZEOF_POINTER__);
    munmap(cruise, __SIZEOF_POINTER__);
    munmap(op_count, __SIZEOF_POINTER__);
    munmap(hack_to_boat, __SIZEOF_POINTER__);
    munmap(serf_to_boat, __SIZEOF_POINTER__);

    //close and unlink sems
    sem_close(sem_wait_for_all);
    sem_unlink("/xpolok03.ios.proj2.sem_wait_for_all");

    sem_close(sem_print);
    sem_unlink("/xpolok03.ios.proj2.sem_print");

    sem_close(semid_hack);
    sem_unlink("/xpolok03.ios.proj2.semid_hack");

    sem_close(semid_serf);
    sem_unlink("/xpolok03.ios.proj2.semid_serf");

    sem_close(sem_molo);
    sem_unlink("/xpolok03.ios.proj2.sem_molo");

    sem_close(wait_for_captain);
    sem_unlink("/xpolok03.ios.proj2.wait_for_captain");

    sem_close(sem_hack_count);
    sem_unlink("/xpolok03.ios.proj2.sem_hack_count");

    sem_close(sem_serf_count);
    sem_unlink("/xpolok03.ios.proj2.sem_serf_count");

    sem_close(wait_for_crew);
    sem_unlink("/xpolok03.ios.proj2.wait_for_crew");

    sem_close(sem_cruise);
    sem_unlink("/xpolok03.ios.proj2.sem_cruise");

    sem_close(sem_op_count);
    sem_unlink("/xpolok03.ios.proj2.sem_op_count");

    sem_close(boarding);
    sem_unlink("/xpolok03.ios.proj2.boarding");

    sem_close(change_passenger_count);
    sem_unlink("/xpolok03.ios.proj2.change_passenger_count");

    sem_close(hackers_board);
    sem_unlink("/xpolok03.ios.proj2.hackers_board");

    sem_close(serfers_board);
    sem_unlink("/xpolok03.ios.proj2.serfers_board");

    sem_close(sem_processes);
    sem_unlink("/xpolok03.ios.proj2.sem_processes");
}

int get_Params(int argc, char *argv[], Parameters *params)
//Function get paramaters from input, checks if they are ok and then saves them to structure params.
{
    //auxilliary pointer for strtol
    char *ptr;
    //number of arguments should be 7
    if (argc == 7)
    {
        //check if number is int -> make long int -> check if it is in range of long int -> check value -> save
        if ((strtol(argv[1], &ptr, 10) >= 2) && (strtol(argv[1], &ptr, 10) % 2 == 0) && (*ptr == '\0') && (errno != ERANGE))
            params->count = strtol(argv[1], &ptr, 10);
        else
            return 1;

        if ((strtol(argv[2], &ptr, 10) >= 0) && (strtol(argv[2], &ptr, 10) <= 2000) && (*ptr == '\0') && (errno != ERANGE))
            params->new_procestimeh = strtol(argv[2], &ptr, 10);
        else
            return 1;

        if ((strtol(argv[3], &ptr, 10) >= 0) && (strtol(argv[3], &ptr, 10) <= 2000) && (*ptr == '\0') && (errno != ERANGE))
            params->new_procestimes = strtol(argv[3], &ptr, 10);
        else
            return 1;

        if ((strtol(argv[4], &ptr, 10) >= 0) && (strtol(argv[4], &ptr, 10) <= 2000) && (*ptr == '\0') && (errno != ERANGE))
            params->sail_time = strtol(argv[4], &ptr, 10);
        else
            return 1;

        if ((strtol(argv[5], &ptr, 10) >= 20) && (strtol(argv[5], &ptr, 10) <= 2000) && (*ptr == '\0') && (errno != ERANGE))
            params->molo_return = strtol(argv[5], &ptr, 10);
        else
            return 1;

        if ((strtol(argv[6], &ptr, 10) >= 5) && (*ptr == '\0') && (errno != ERANGE))
            params->molo_cap = strtol(argv[6], &ptr, 10);
        else
            return 1;
    }

    else
    {
        return 1;
    }

    return 0;
}

int molo(char *proctype, int processidentify, Parameters params)
{

    //infinite loop (try to get on molo)
    while (1)
    {
        //only one process could try to access molo at one time and also cannot access molo when processes are boarding (atomic situation)
        sem_wait(sem_molo);
        sem_wait(boarding);
        //molo is not full
        if (*processes_on_molo < params.molo_cap)
        {
            //asap inc processes on molo
            (*processes_on_molo)++;

            //print that process is on molo
            sem_wait(sem_print);
            //inc proccess type  counter
            if (strcmp(proctype, "HACK") == 0)
            {
                sem_wait(sem_hack_count);
                (*hack_on_molo)++;
                sem_post(sem_hack_count);
            }
            else if (strcmp(proctype, "SERF") == 0)
            {
                sem_wait(sem_serf_count);
                (*serf_on_molo)++;
                sem_post(sem_serf_count);
            }
            fprintf(output, "%d      : %s %d       : waits              : %d        : %d\n", *linenum, proctype, processidentify, *hack_on_molo, *serf_on_molo);
            (*linenum)++;
            sem_post(sem_print);
            sem_post(boarding);
            sem_post(sem_molo);

            //loop for processess attempting to reach boat( if on cruise they have to wait)
            while (1)
            {
                //to be sure that captain starts cruise and none can join at the same time
                sem_wait(sem_cruise);
                //if on cruise cannot board
                if (*cruise == 0)
                {
                    sem_wait(change_passenger_count);
                    //if proccesses on molo could make group -> make captain -> boards -> sail -> exit
                    if (((*hack_to_boat == 3) && (strcmp(proctype, "HACK") == 0)) || ((*serf_to_boat == 3) && (strcmp(proctype, "SERF") == 0)) || ((*serf_to_boat >= 1) && (*hack_to_boat >= 2) && (strcmp(proctype, "SERF") == 0)) || ((*serf_to_boat >= 2) && (*hack_to_boat >= 1) && (strcmp(proctype, "HACK") == 0)))
                    {
                        //sem_wait(captain_on_board);
                        *cruise = 1;
                        sem_post(sem_cruise);
                        //boarding starts
                        sem_wait(boarding);
                        sem_post(change_passenger_count);

                        //deppending on situation let crew board
                        if ((*hack_to_boat >= 3) && (strcmp(proctype, "HACK") == 0))
                        {
                            sem_post(hackers_board);
                            sem_post(hackers_board);
                            sem_post(hackers_board);
                        }
                        else if ((*serf_to_boat >= 3) && (strcmp(proctype, "SERF") == 0))
                        {
                            sem_post(serfers_board);
                            sem_post(serfers_board);
                            sem_post(serfers_board);
                        }
                        else if ((*serf_to_boat >= 1) && (*hack_to_boat >= 2) && (strcmp(proctype, "SERF") == 0))
                        {
                            sem_post(hackers_board);
                            sem_post(hackers_board);
                            sem_post(serfers_board);
                        }
                        else if ((*serf_to_boat >= 2) && (*hack_to_boat >= 1) && (strcmp(proctype, "HACK") == 0))
                        {
                            sem_post(hackers_board);
                            sem_post(serfers_board);
                            sem_post(serfers_board);
                        }

                        //wait for all crew members
                        sem_wait(wait_for_crew);

                        //print that crew is boarding now
                        sem_wait(sem_print);
                        sem_wait(change_passenger_count);
                        if (strcmp(proctype, "HACK") == 0)
                        {
                            sem_wait(sem_hack_count);
                            (*hack_on_molo)--;
                            sem_post(sem_hack_count);
                        }
                        else if (strcmp(proctype, "SERF") == 0)
                        {
                            sem_wait(sem_serf_count);
                            (*serf_on_molo)--;
                            sem_post(sem_serf_count);
                        }

                        fprintf(output, "%d      : %s %d       : boards              : %d        : %d\n", *linenum, proctype, processidentify, *hack_on_molo, *serf_on_molo);
                        //4 processes boarded so now change count of processes on the molo
                        *processes_on_molo = *processes_on_molo - 4;
                        (*linenum)++;
                        sem_post(sem_print);
                        sem_post(boarding);

                        sem_post(change_passenger_count);

                        //sail for some time
                        usleep(random() % (params.sail_time*1000 + 1));
                        //let processes exit
                        sem_post(wait_for_captain);
                        sem_post(wait_for_captain);
                        sem_post(wait_for_captain);
                        //wait for them all
                        sem_wait(wait_for_crew);

                        //captain exits last
                        sem_wait(sem_print);
                        fprintf(output, "%d      : %s %d       : captain exits               : %d        : %d\n", *linenum, proctype, processidentify, *hack_on_molo, *serf_on_molo);
                        (*linenum)++;
                        sem_post(sem_print);

                        //end cruise
                        *cruise = 0;
                        return 0;
                    }

                    //wait for captain
                    else
                    {
                        sem_post(sem_cruise);

                        //increase counts  and wait for captain to let them join cruise (depending on proc type)
                        if (strcmp(proctype, "HACK") == 0)
                        {
                            (*hack_to_boat)++;
                            sem_post(change_passenger_count);
                            sem_wait(hackers_board);
                            //boarding
                            sem_wait(change_passenger_count);
                            (*hack_to_boat)--;
                            sem_wait(sem_hack_count);
                            (*hack_on_molo)--;
                            sem_post(sem_hack_count);
                            sem_post(change_passenger_count);
                        }

                        else if (strcmp(proctype, "SERF") == 0)
                        {
                            (*serf_to_boat)++;
                            sem_post(change_passenger_count);
                            sem_wait(serfers_board);
                            sem_wait(change_passenger_count);
                            (*serf_to_boat)--;
                            sem_wait(sem_serf_count);
                            (*serf_on_molo)--;
                            sem_post(sem_serf_count);
                            sem_post(change_passenger_count);
                        }

                        //last one inform captain they are ready to sail
                        sem_wait(sem_op_count);
                        (*op_count)++;
                        if (*op_count == 3)
                        {
                            sem_post(wait_for_crew);
                            *op_count = 0;
                        }
                        sem_post(sem_op_count);

                        //wait to reach destination -> exit boat
                        sem_wait(wait_for_captain);
                        sem_wait(sem_print);
                        fprintf(output, "%d      : %s %d       : member exits              : %d        : %d\n", *linenum, proctype, processidentify, *hack_on_molo, *serf_on_molo);
                        (*linenum)++;
                        sem_post(sem_print);

                        //last one inform captain -> that he is last one on the boat
                        sem_wait(sem_op_count);
                        (*op_count)++;
                        if (*op_count == 3)
                        {
                            sem_post(wait_for_crew);
                            *op_count = 0;
                        }
                        sem_post(sem_op_count);
                        return 0;
                    }
                }
                //open sem
                sem_post(sem_cruise);
            }
        }
        //molo is full
        //print message -> let other proccesses access molo
        sem_wait(sem_print);
        fprintf(output, "%d      : %s %d       : leaves queue              : %d        : %d\n", *linenum, proctype, processidentify, *hack_on_molo, *serf_on_molo);
        (*linenum)++;
        sem_post(boarding);
        sem_post(sem_print);
        sem_post(sem_molo);

        //sleep
        usleep(random() % (params.molo_return*1000 + 1));

        //wake up -> print -> go back to loop
        sem_wait(sem_print);
        fprintf(output, "%d      : %s %d       : is back\n", *linenum, proctype, processidentify);
        (*linenum)++;
        sem_post(sem_print);
    }

    return 0;
}

//childprocesses
void start(char *proctype, int processidentify, Parameters params)
{
    //print that process is alive
    sem_wait(sem_print);
    fprintf(output, "%d      : %s %d       : starts\n", *linenum, proctype, processidentify);
    (*linenum)++;
    sem_post(sem_print);
    //go to molo
    molo(proctype, processidentify, params);
    //decrase processcounter and end(if last open sem)
    sem_wait(sem_processes);
    (*processcounter)--;
    if (*processcounter == 1)
    {
        sem_post(sem_processes);
        sem_post(sem_wait_for_all);
        exit(0);
    }
    sem_post(sem_processes);
    exit(0);
}

//generate children
int genprocesses(Parameters params)
{
    int processidentify;
    char *proctype;

    //child has 0, parent same
    //generate hackers
    pid_t hacker = fork();
    *processcounter = params.count * 2;

    //fork error
    if (hacker < 0)
    {
        perror("Fork error!\n");
        return 1;
    }
    //making thousands of children
    if (hacker == 0)
    {

        //make x child processes
        for (int i = 0; i < params.count; i++)
        {
            pid_t hackerchilds = fork();
            if (hackerchilds < 0)
            {
                perror("Fork error!\n");
                return 1;
            }
            if (hackerchilds > 0)
            {
                sem_wait(semid_hack);
                processidentify = *processidhacker;
                (*processidhacker)++;
                sem_post(semid_hack);
                proctype = "HACK";
                if (params.new_procestimeh != 0)
                    if (usleep(random() % (params.new_procestimeh*1000 + 1)) == -1)
                        return 1;
                start(proctype, processidentify, params);
            }           
        }
        exit(0);
    }

    //same as hackers
    pid_t serfer = fork();
    if (serfer < 0)
    {
        perror("Fork error!\n");
        return 1;
    }

    if (serfer == 0)
    {

        for (int i = 0; i < params.count; i++)
        {
            pid_t serferchilds = fork();
            if (serferchilds < 0)
            {
                perror("Fork error!\n");
                return 1;
            }
            if (serferchilds > 0)
            {
                sem_wait(semid_serf);
                processidentify = *processidserfer;
                (*processidserfer)++;
                sem_post(semid_serf);
                proctype = "SERF";
                if (params.new_procestimes != 0)
                    if (usleep(random() % (params.new_procestimes*1000 + 1)) == -1)
                        return 1;
                start(proctype, processidentify, params);
            }            
        }
        exit(0);
    }
    //only main process leaves
    return 0;
}

int main(int argc, char *argv[])
{
    Parameters params;
    //get and check params
    if (get_Params(argc, argv, &params) == 1)
    {
        perror("Invalid parametres!\n");
        return 1;
    }

    //initialization
    if (init() != 0)
    {
        perror("Initialization error\n");
        return 1;
    }

    //open file for output
    if ((output = fopen("proj2.out", "w")) == NULL)
    {
        perror("Could not open output file!\n");
        return 1;
    }

    setbuf(output, NULL);
    
    //generate proccesses and try to cruise
    if (genprocesses(params) != 0)
        return 1;

    //wait for all processesto finish
    sem_wait(sem_wait_for_all);

    //clean what u make
    clean();

    return 0;
}
