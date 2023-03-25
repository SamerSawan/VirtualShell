#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *framestore[XVAL];
int LRUSize = (XVAL/3);


struct LRUNode {
    int framenum;
    struct LRUNode* next;
    struct LRUNode* prev;
};

struct LRUNode *LRUHead = NULL;


void insert(int framenum){


    struct LRUNode* new = malloc(sizeof(struct LRUNode));
	new->framenum = framenum;
    new->next = LRUHead;
    new->prev = NULL;
	
    if (LRUHead != NULL){
        LRUHead->prev = new;
    }

    LRUHead = new;
}

void removeNode(int framenum){
    struct LRUNode* temp = LRUHead;
	//if head is empty, return
    if (temp == NULL){
        return;
    }
    int i = 0;
	//attempt to find correct frame
    while(temp->next != NULL){
        if (temp->framenum == framenum){
            break;
        }
        if (i > LRUSize){
            return;
        }
        i++;
        temp = temp->next;
    }
	//if frame not found, return
	if (temp->framenum != framenum){
		return;
	}
	//if frame to remove is the head, make the head the "next" element
    if (LRUHead == temp){
        LRUHead = temp->next;
        
    } 
	//if frame is not the last element, adjust the pointers between next and prev
	if (temp->next != NULL) {
        temp->next->prev = temp->prev;
        
    }
	//if frame is not the first element, adjust the pointers between next and prev 
	if (temp->prev != NULL) {
        temp->prev->next = temp->next;
        
    }
    
}

int removeLast(){
	
    struct LRUNode* temp = LRUHead;
	//iterate till the end of the list
    while(temp != NULL && temp->next != NULL){
        temp = temp->next;
    }
	//if the head is the last element, set the head to temp->next
    if (LRUHead == temp){
        LRUHead = temp->next;
    }
	//adjust the pointers between prev and next
    temp->prev->next = temp->next;
    
	
    return temp->framenum;
}



struct memory_struct
{
	char *var;
	char *value;
};

struct memory_struct shellmemory[YVAL];

// Helper functions
int match(char *model, char *var)
{
	int i, len = strlen(var), matchCount = 0;
	for (i = 0; i < len; i++)
		if (*(model + i) == *(var + i))
			matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model)
{
	char token = '='; // look for this to find value
	char value[1000]; // stores the extract value
	int i, j, len = strlen(model);
	for (i = 0; i < len && *(model + i) != token; i++)
		; // loop till we get there
	// extract the value
	for (i = i + 1, j = 0; i < len; i++, j++)
		value[j] = *(model + i);
	value[j] = '\0';
	return strdup(value);
}

// Shell memory functions

void mem_init()
{
	
	int i;
	for (i = 0; i < YVAL; i++)
	{
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}
	for (i = 0; i < XVAL; i++){
		framestore[i] = NULL;
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in)
{

	int i;

	for (i = 0; i < YVAL; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{
			shellmemory[i].value = strdup(value_in);
			return;
		}
	}

	// Value does not exist, need to find a free spot.
	for (i = 0; i < YVAL; i++)
	{
		if (strcmp(shellmemory[i].var, "none") == 0)
		{
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		}
	}

	return;
}

// get value based on input key
char *mem_get_value(char *var_in)
{
	int i;

	for (i = 0; i < 1000; i++)
	{
		if (strcmp(shellmemory[i].var, var_in) == 0)
		{

			return strdup(shellmemory[i].value);
		}
	}
	return "Variable does not exist";
}

int evictFrame(){
	
	int target = removeLast();
	

	int index = target*3;
	printf("Page fault! Victim page contents:\n");
	for (int i = 0; i<3; i++){
		printf("%s", framestore[i+index]);
		framestore[i+index] = NULL;
	}
	printf("End of victim page contents.\n");
	return index;
}

int add_to_frame(char *s[], int len){
	int i = 0;
	while(framestore[i] != NULL){
		i+=3;
	}
	if (i >= XVAL){
		i = evictFrame();
	}
	for (int j=0; j < len; j++){
		framestore[i+j] = s[j];
	}
	//printf("PRINTING STORED ADDRESS: %s AND INDEX: %i\n PRINTING FRAMESTOREINDEX: %s\n", s, index, framestore[index]);
	return i;
}



char * get_address(int index){
	
	return framestore[index];
}



void var_mem_reset()
{
	memset(shellmemory, 0, 100);
	return;
}