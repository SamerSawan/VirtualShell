#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "shellmemory.h"
#include "shell.h"

//this has been a very long assignment, so some stuff is all over the place
//because I'm trying to finish it quickly enough for the deadline
//I will explain every single function with comments for further clarity.
//Thank you for your patience.
int RRRun();
int ProgramPID = 0;
int frame_id = 0;
int k = 0;



struct PCB {
char scriptname[100];
int PID;
int startpos;
int len;
int counter;
int age;
int pagetable[10];
int totalpages;
int numloadedpages;
int currpage;
int inpage;
int nextemptypage;
struct PCB* next;

};

struct PCB *queueHead = NULL;

//Adds a node to the end of the current ReadyQueue
void addProcess(struct PCB *newProcess){


    if (queueHead == NULL){
        queueHead = newProcess;
    }

    else{
        struct PCB *curr = queueHead;
        while (curr->next != NULL){
            curr = curr->next;
        }
        curr->next = newProcess;
    }
    
};

//Remove the head of the ReadyQueue and free the memory
void removeProcess(){
    struct PCB *temp = queueHead;
    queueHead = queueHead->next;
    free(temp);
}

int load_page(char* script, int pagenumber, struct PCB *program){
    
    char line[1000];
    int start = pagenumber*3; //keep track of which line we should process next
    int count = 0; // counter to catch up to where we are in the file
    int lines = 0; // counter to make sure we add only 3 lines at a time
    
    program->numloadedpages = program->numloadedpages + 1;

    FILE *p = fopen(script,"rt");
    int i = 0; //keeps track of which address to assign each line

    //catch up to where we are in the file
    while (!feof(p) && count <= start){
        i += 1;
        count += 1;
        fgets(line,999,p);
    }
    i -= 1; //this is because we iterate one time too many in the while loop

    //incase there is only one line in the file
    char *linestore[3];
    for(int i=0; i < lines; i++){
        linestore[i] = NULL;
    }
    char address[100];
    linestore[lines] = strdup(line);
    frame_id += 1;
    lines += 1;
    i+=1;
    
    //add 2 lines of text to the framestore and address
    while (!feof(p) && lines < 3){

        fgets(line,999,p);
        linestore[lines] = strdup(line);
        frame_id += 1;
        lines += 1;
        i+=1;
    }
    //if frame is not 3 lines long, add empty strings to frame
    while (lines < 3){
        linestore[lines] = NULL;
        lines += 1;
    }
    int temp = add_to_frame(linestore, 3);
    program->pagetable[program->nextemptypage] = temp/3;
    fclose(p);
    program->nextemptypage = program->nextemptypage + 1;
    return 0;
}


//Loads a script into the memory so that we can run it at a further date
int load_file(char* script){
    char dest[100];
    char str[100];

    sprintf(dest, "./backingstore/file%i", ProgramPID);
    sprintf(str, "cp %s ./backingstore/file%i", script, ProgramPID);
    system(str);
    

    //Open file and allocate the right amount of memory for the program.
    FILE *p = fopen(dest,"rt");
    char line[1000];
    struct PCB *program = malloc(sizeof(struct PCB));
    strcpy(program->scriptname, dest);
    fgets(line,999,p);
    int n = 0;

	while(1){
        n+=1;
        if(feof(p)){
			break;
		}
        fgets(line,999,p);
    }
    
    program->totalpages = n/3 + 1;
    fclose(p);
    //Var i keeps track of the length of the program
    int i = 0;

    //block of code initializes some fields in the PCB struct for our program
    program->PID = ProgramPID;
    program->startpos = atoi(ProgramPID + "" + i);
    program->counter = 0;
    program->next = NULL;

    int pagenumber = 0;
    
    int c = 0;
    while(c < 10){
        program->pagetable[c] = -1;
        c++;
    }
    program->nextemptypage = 0;
    program->numloadedpages = 0;
    while (pagenumber < 2 && pagenumber != program->totalpages){
        load_page(dest, pagenumber, program);
        pagenumber += 1;
    }

    //set age and length of program to "i"
    program->len = n;
    program->age = n;
    program->currpage = 0;
    program->inpage = 0;

    //close file, increment ProgramPID, add the program to the ReadyQueue
    //fclose(p);
    ProgramPID += 1;
    addProcess(program);

    return 0;
}



//runProcess runs the program at the head of the queue
int runProcess(){
    RRRun();
    /*int i = 0; //line tracker to not exceed file size
    int errCode = 0;
    char* line;
    while (i < queueHead->len){
        int frame = queueHead->pagetable[queueHead->currpage]; // frame num
        int inpage = queueHead->inpage; //index within frame 0-2 range
        int index = frame*3 + inpage; //index for get address

        queueHead->inpage = queueHead->inpage + 1; //increment inpage
        queueHead->counter = queueHead->counter + 1; //increment counter
        //THE BUG IS DUE TO NOT ADDING MORE THAN 2 PAGES, CHECK RRRUN FOR HOW TO FIX THAT.
        

        errCode = parseInput(get_address(index)); //get info
        

        //int PID = queueHead->PID;
        //char address[100];
       //snprintf(address, sizeof(address), "%d%d", PID, i);
        //line = mem_get_value(address);
        //errCode = parseInput(line);
        i += 1;
    }
    removeProcess();*/
    return 0;
}

//Bubblesort for the readyQueue, sorts in ascending order by length of the program (for SJF)
void sortByLen(){
    int sorted = 0;

    while(!sorted)
    {
        struct PCB **prev = &queueHead;
        struct PCB *curr = queueHead;
        struct PCB *temp;

        sorted = 1;
        while(curr!=NULL){

            temp = curr->next;

            if(temp!=NULL && curr->len > temp->len)
            {
                curr->next = temp->next;
                temp->next = curr;
                *prev = temp;

                sorted = 0;
            }

            prev = & curr->next;
            curr = curr->next;
        }
    }
}

//removes program by PID
void removeByPID(int PID){
    struct PCB *curr = queueHead->next;
    struct PCB *prev = queueHead;
    if (prev->PID == PID){
        removeProcess();
    } else {
        while (curr!=NULL){
            if (curr->PID == PID){
                struct PCB *next = curr->next;
                prev->next = curr->next;
                free(curr);
                break;
            }
            curr = curr->next;
            prev = prev->next;
        }
    }
}

//This method is for the RR policy, uses the "counter" field of PCB to keep track of which lines
int RRRun(){
    struct PCB *curr = queueHead;
    int errCode = 0;
    if (queueHead == NULL){
        return 0;
    } else {
        while (queueHead!=NULL){
            while (curr!=NULL){
                
                int PID = curr->PID;
                char address[100];
                char* line;
                for (int i = 0;i<2;i++){
                    if (curr->counter >= curr->len){
                        break;
                    }
                    if (curr->inpage >= 3){
                        curr->currpage = curr->currpage + 1;
                        curr->inpage = 0;
                    }
                    if (curr->currpage >= curr->numloadedpages){
                        load_page(curr->scriptname, curr->currpage, curr);
                        break;
                    }
                    int frame = curr->pagetable[curr->currpage];
                    removeNode(frame);
                    insert(frame);
                    
                    int inpage = curr->inpage;
                    int index = frame*3 + inpage;
                    
                    curr->inpage = curr->inpage + 1;
                    if (get_address(index) != NULL){
                        errCode = parseInput(get_address(index));
                    }
                    curr->counter = curr->counter + 1;
                }
                if (curr->counter >= curr->len){
                    removeByPID(curr->PID);
                }
                curr = curr->next;
            }
            curr = queueHead;
        }
        
        return errCode;
    }
}

//handles ageing the ReadyQueue
void age(struct PCB *node){
    struct PCB *curr = queueHead;
    while (curr!=NULL){
        if (curr->PID == node->PID){
            curr = curr->next;
            continue;
        }
        if (curr->age != 0){
            curr->age = curr->age - 1;
        }
        curr = curr->next;
    }
}

//takes a node and sets it to the new head of the ReadyQueue while adjusting all other links such that the Queue is intact
void setNewHead(struct PCB *node){
    struct PCB *curr = queueHead->next;
    struct PCB *previous = queueHead;
    while (curr!=NULL){
        if (curr->PID == node->PID){
            struct PCB *temp = queueHead;
            previous->next = curr->next;
            queueHead = curr;
            curr->next = temp->next;
            temp->next = NULL;
            addProcess(temp);
            break;
        }
        previous = curr;
        curr = curr->next;
    }
}

//finds the node with the lowest "age" field
struct PCB * getLowestAge(){
    struct PCB *curr = queueHead;
    struct PCB *minProg;
    int min = __INT_MAX__;
    while (curr != NULL){
        if (curr->age < min){
            min = curr->age;
            minProg = curr;
        }
        curr = curr->next;
    }
    return minProg;
}

//handles executing programs with the AGING policy.
int Aging(){
    struct PCB *curr = getLowestAge(queueHead);
    setNewHead(curr);
    int errCode = 0;
    if (queueHead == NULL){
        return 0;
    } else {
        while (queueHead!=NULL){
            while (curr!=NULL){
                int PID = curr->PID;
                char address[100];
                char* line;
                if (curr->counter >= curr->len){
                    break;
                }
                snprintf(address, sizeof(address), "%d%d", PID, curr->counter);
                line = mem_get_value(address);
                errCode = parseInput(line);
                curr->counter = curr->counter + 1;
                age(curr);
                curr = getLowestAge(curr);
                setNewHead(curr);
            }
            if (curr->counter >= curr->len){
                removeByPID(curr->PID);
            }
            curr = getLowestAge(curr);
        }
    }
}