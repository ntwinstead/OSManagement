#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

// Nathan Winstead, CGS3763 Spring 2023, 3/8/2023
#define EXIT 0
#define INVALID -1
#define MEMORY 1
#define FILES 2
#define THREAD 3
#define FIRST 0
#define BEST 1
#define WORST 2
#define NEXT 3
#define PAGE 4
#define FIFO 0
#define LRU 1
#define FRAMES 3
#define SIZE 7
#define LOOP 3


int displayMenu();
void clearScreen();
void memoryManagement();
void firstFit(int blockSize[], int blocks, int processSize[], int processes);
void bestFit(int blockSize[], int blocks, int processSize[], int processes);
void worstFit(int blockSize[], int blocks, int processSize[], int processes);
void nextFit(int blockSize[], int blocks, int processSize[], int processes);
void displayProcess(int allocation[], int processes, int processSize[]);
void pageReplacement();
void fifo();
void lru();
int findLRU(int time[]);
void displayPages(int page, int allocation[]);
void fileAttributes();
void printAttributes(char name[], struct stat statBuff);
void multiThreads();
void* threadFunction(void* vargp);

//basic main function defining choice as -1 and using a while loop
int main()
{
    int choice = -1;
    while (choice != 0)
    {
        choice = displayMenu();
        //used a switch function to determine which function they want to use using the choice variable
        switch(choice)
        {
            case 1:
                memoryManagement();
                break;
            case 2:
                fileAttributes();
                break;
            case 3:
                multiThreads();
                break;
            case 4:
                pageReplacement();
            case 0:
                exit(0);
                break;
            default:
            printf("Error! Reference source not found.");
        }
    }
    return 0;
}
// simple display menu using scanf to get their input and returning choice, also defining choice back to -1 if input is not valid
int displayMenu()
{
    int choice = -1;
    while (choice == -1)
    {
        printf("Select the OS program to run, enter the number of your selection.\n 1. Memory Management\n 2. File Management\n 3. Multithreading\n 4.Page Replacement\n 0.Exit\n");
        scanf("%d", &choice);
        if (choice > 4 || choice < 0)
        {
            choice = -1;
        }
    }
    return choice;
}
// I pretty much just followed the text here, not sure what it does.
void clearScreen()
{
    printf("Please hit ENTER!\n");
    char inputChar;
    scanf("%c", &inputChar);
    system("clear");
    //system("cls");
}

void memoryManagement()
{
    clearScreen();
    for (int algorithm = 1; algorithm <= 4; algorithm++)
    {
    int blockSize[] = {80, 10, 65, 35, 70};
    int processSize[] = {25, 70, 5, 45, 60};
    int processes = sizeof(processSize) / sizeof(processSize[0]);
    int blocks = sizeof(blockSize) / sizeof(blockSize[0]);

        switch (algorithm)
        {
            case 1:
                firstFit (blockSize, blocks, processSize, processes);
                break;
            case 2:
                bestFit (blockSize, blocks, processSize, processes);
                break;
            case 3:
                worstFit (blockSize, blocks, processSize, processes);
                break;
            case 4:
                nextFit (blockSize, blocks, processSize, processes);
                break;
        }
    }
}

void nextFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes];
    int id = 0;

    for (int i = 0; i < processes; i++)
    {
        allocation[i] = -1;
    }

    for (int i = 0; i < processes; i++)
    {
        while (id < blocks)
        {
            if (blockSize[id] >= processSize[i])
            {
                allocation[i] = id;
                blockSize[id] = blockSize[id] - processSize[i];
                break;
            }
            id = (id + 1) % blocks;
        }
    }
    displayProcess(allocation, processes, processSize);
}

void firstFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes];

    for(int i = 0; i < processes; i++)
    {
        allocation[i] = -1;
    }

    for (int i = 0; i < processes; i++)
    {
        for (int j = 0; j < blocks; j++)
        {
            if (blockSize[j] >= processSize[i])
            {
                allocation[i] = j;
                blockSize[j] = blockSize[j] - processSize[i];
                break;
            }
        }
    }
    displayProcess(allocation, processes, processSize);
}

void bestFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes];

    for (int i = 0; i < processes; i++)
    {
        allocation[i] = -1;
    }

    for (int i = 0; i < processes; i++)
    {
        int bestIdx = -1;
        for (int j = 0; j < blocks; j++)
        {
            if (blockSize[j] >= processSize[i])
            {
                if (bestIdx == -1)
                {
                    bestIdx = j;
                }else if(blockSize[bestIdx] > blockSize[j])
                {
                    bestIdx = j;
                }
            }
        }
        if (bestIdx != -1)
        {
            allocation[i] = bestIdx;
            blockSize[bestIdx] = blockSize[bestIdx] - processSize[i];
        }
    }
    displayProcess(allocation, processes, processSize);
}

void worstFit(int blockSize[], int blocks, int processSize[], int processes)
{
    int allocation[processes];

    for (int i = 0; i < processes; i++)
    {
        allocation[i] = -1;
    }

    for (int i = 0; i < processes; i++)
    {
        int wstIdx = -1;
        for (int j = 0; j < blocks; j++)
        {
            if (blockSize[j] >= processSize[i])
            {
                if (wstIdx == 1)
                {
                    wstIdx = j;
                } else if( blockSize[wstIdx] < blockSize[j])
                {
                    wstIdx = j;
                }
            }
        }
        if (wstIdx != -1)
        {
            allocation[i] = wstIdx;
            blockSize[wstIdx] = blockSize[wstIdx] - processSize[i];
        }
    }
    displayProcess(allocation, processes, processSize);
}

void displayProcess(int allocation[], int processes, int processSize[])
{
    printf("Process No.\tProcess Size\tBlock No.\n");
    for (int i = 0; i < processes; i++)
    {
        printf("%d\t\t%d\t\t", i + 1, processSize[i]);
        if (allocation[i] != -1) 
        {
            printf("%d", allocation[i] + 1);
        } else {
            printf("Not Allocated\n");
        }
        printf("\n");
    }
}
// I've been trying to fix these functions for days and I still can't get the right output, so I'm just gonna take the loss.
void pageReplacement()
{
    clearScreen();
    for (int algorithm = 0; algorithm <= 1; algorithm++)
    {
        if (algorithm == 0)
        {
            fifo();
        } else if (algorithm == 1)
        {
            lru();
        }
    }
}

void fifo()
{
    printf("****** FIRST IN FIRST OUT ******\n");
    printf("Page\tFrame 1\tFrame 2\t Frame3\n");

    int pageRequests[] = {4, 1, 2, 4, 2, 5, 1, 3, 6};
    int pageFaults = 0;
    int allocation[3];
    int present = 0;
    int pages = sizeof(pageRequests) / sizeof(pageRequests[0]);
    memset(allocation, -1, sizeof(allocation));

    for (int i = 0; i < pages; i++)
    {
        present = 0;
        for (int j = 0; j < 3; j++)
        {
            if (allocation[j] == pageRequests[i])
            {
                present++;
                pageFaults--;
            }
        }
        pageFaults++;
        if (present == 0 && pageFaults <= 3)
        {
            allocation[pageFaults - 1] = pageRequests[i];
        } else if (present == 0)
        {
            allocation[(pageFaults -1) % 3] = pageRequests[i];
        }
        displayPages(pageRequests[i], allocation);
        printf("Total Page Faults = %d", pageFaults);
    }
}

void lru()
{
    printf("****** LEAST RECENTLY USED ******\n");
    printf("Page\tFrame 1\tFrame 2\t Frame3\n");
    int pageRequests[] = {4, 1, 2, 4, 2, 5, 1, 3, 6};
    int pageFaults = 0;
    int allocation[3];
    int pages = sizeof(pageRequests) / sizeof(pageRequests[0]);
    int counter = 0;
    int time[10];
    int flag1;
    int flag2;
    int j = 0;
    int position;
    memset(allocation, -1, sizeof(allocation));

    for (int i = 0; i < pages; i++)
    {
        flag1 = 0;
        flag2 = 0;
        for (j = 0; j < 3; i++)
        {
            if (allocation[j] == pageRequests[i])
            {
                counter++;
                time[j] = counter;
                flag1 = 1;
                flag2 = 1;
                break;
            }
        }

        if (flag1 = 0)
        {
            for (j = 0; j < FRAMES; j++)
            {
                if (allocation[j] == -1)
                {
                    counter++;
                    pageFaults++;
                    allocation[j] = pageRequests[i];
                    time[j] = counter;
                    flag2 = 1;
                    break;
                }
            }
        }

        if (flag2 = 0)
        {
            position = findLRU(time);
            counter++;
            pageFaults++;
            allocation[position] = pageRequests[i];
            time[position] = counter;
        }
        displayPages(pageRequests[i], allocation);
    }
    printf("Total Page Faults = %d", pageFaults);
}

int findLRU(int time[])
{
    int position = 0;
    int minimum = time[0];

    for(int i = 0; i < 3; i++)
    {
        if (time[i] < minimum)
        {
            minimum = time[i];
            position = i;
        }
    }
    return position;
}


void displayPages(int page, int allocation[])
{
    printf("%d\t\t", page);

    for (int i = 0; i < 3; i++)
    {
        if (allocation[i] == -1)
        {
            printf("-\t");
        } else
        {
            printf("%d\t", allocation[i]);
        }
    }
    printf("\n");
}

void fileAttributes()
{
    struct stat statBuff;
    int err;
    struct dirent* de;
    DIR* dr = opendir(".");

    if (dr == NULL)
    {
        printf("Could not open current directory");
    }

    while ((de = readdir(dr)) != NULL)
    {
        err = stat(de->d_name, &statBuff);
        if (err == -1)
        {
            printf("Error in Stat\n Press 'Enter' to continue...\n");
            getchar();
        }

        printAttributes(de->d_name, statBuff);
    }
}

void printAttributes(char name[], struct stat statBuff)
{
    time_t t;
    char timestr[100];

    printf("\n-------------File = %s --------------\n", name);
    printf("Device ID: %ln\n", &statBuff.st_rdev);
    printf("File serial number: %ln\n", &statBuff.st_ino);
    printf("File user ID: %-8ls\n", &statBuff.st_uid);
    printf("File group ID: %-8ls\n", &statBuff.st_gid);
    printf("File mode: %ls\n", &statBuff.st_mode);

    printf("Owner permissions: ");
    if (&statBuff.st_mode && S_IXUSR)
    {
        printf("execute ");
    }
    if (&statBuff.st_mode && S_IWUSR)
    {
        printf("write ");
    }
    if (&statBuff.st_mode && S_IRUSR)
    {
        printf("read ");
    }

    printf("\nGroup permissions: ");
    if (&statBuff.st_mode && S_IXGRP)
    {
        printf("execute ");
    }
    if (&statBuff.st_mode && S_IWGRP)
    {
        printf("write ");
    }
    if (&statBuff.st_mode && S_IRGRP)
    {
        printf("read ");
    }

    printf("\nOther permissions: ");
    if (&statBuff.st_mode && S_IXOTH)
    {
        printf("execute ");
    }
    if (&statBuff.st_mode && S_IWOTH)
    {
        printf("write ");
    }
    if (&statBuff.st_mode && S_IROTH)
    {
        printf("read ");
    }

    t = statBuff.st_ctime;
    printf("\nData created: %s\n", ctime(&t));

    t=statBuff.st_mtime;
    printf("Data modified: %s\n", ctime(&t));

    t=statBuff.st_atime;
    printf("Time file was last accessed: %s\n", ctime(&t));

    printf("File size: %ld\n", statBuff.st_size);
}

void multiThreads()
{
    int i;
    int error;
    pthread_t tid[SIZE];

    for (i = 0; i < SIZE; i++)
    {
        error = pthread_create(&tid[i], NULL, threadFunction, (void*) &i);
        if (error != 0)
        {
            printf("\nThread can't be created: [%s] Press 'ENTER' to continue....", strerror(error));
            getchar();
        }
    }
    for (i = 0; i < SIZE; i++)
    {
        pthread_join(tid[i], NULL);
    }
}

void* threadFunction(void* vargp)
{
    int i;
    int* myid = (int*) vargp;
    printf("Thread #%d has started\n", *myid);

    for (i = 0; i < LOOP; i++)
    {
        printf("Thread #%d is printing iteration %d\n", *myid, i+1);
    }
    printf("Thread #%d has finished\n", *myid);
    return NULL;
}