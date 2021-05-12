// Author: Mohammed Almansour


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <strings.h>



// data type for heap nodes
typedef struct HeapNode {
    // each node stores the priority (key), name, execution time,
    //  release time and deadline of one task
    int key; //key of this task
    int TaskName;  //task name
    int Etime; //execution time of this task
    int Rtime; // release time of this task
    int Dline; // deadline of this task
    int degree; // degree of node
    struct HeapNode *parent;
    struct HeapNode *sibling;
    struct HeapNode *child;

    // add additional fields here
} HeapNode;

//data type for a priority queue (heap)
typedef struct BinomialHeap{ //this is heap header
    int  size;      // count of items in the heap
    struct HeapNode *first;
    // add additional fields here
} BinomialHeap;

// create a new heap node to store an item (task)
HeapNode *newHeapNode(int k, int n, int c, int r, int d)
{ // k:key, n:task name, c: execution time, r:release time, d:deadline
    // ... you need to define other parameters yourself)
    HeapNode *new;
    new = malloc(sizeof(HeapNode));
    assert(new != NULL);
    new->key = k;
    new->TaskName = n;
    new->Etime = c;
    new->Rtime = r;
    new->Dline = d;
    new->degree = 0;
    new->parent = new->sibling = new->child = NULL;
    // initialise other fields
    return new;
}

// create a new empty heap-based priority queue
BinomialHeap *newHeap()
{ // this function creates an empty binomial heap-based priority queue
    BinomialHeap *T;
    T = malloc(sizeof(BinomialHeap));
    assert (T != NULL);
    T->size = 0;
    T->first = NULL;
    // initialise all the fields here
    return T;
}

// function to get maximum of two integers
int max(int a, int b)
{
    return (a > b)? a : b;
}

// function to merge two trees
HeapNode *merge2Trees(HeapNode *N1, HeapNode *N2) {

    // if the second key is smaller
    if (N1->key > N2->key) {
        N1->parent = N2;
        N1->sibling = N2->child;
        N2->child = N1;
        N2->degree++;
        return N2;
    }
        // if the first is smaller
    else {
        N2->parent = N1;
        N2->sibling = N1->child;
        N1->child = N2;
        N1->degree++;
        return N1;
    }
}


HeapNode *merge2Heaps(BinomialHeap *heap1, BinomialHeap *heap2) {

    if (heap1->first == NULL) { return heap2->first;}
    if (heap2->first == NULL) { return heap1->first;}

    // create temp nodes for traversal and sorting
    HeapNode *head;
    HeapNode *h1temp = heap1->first;
    HeapNode *h2temp = heap2->first;
    HeapNode *tail;

    // assign starting temp nodes based the lowest degree
    if (heap1->first->degree <= heap2->first->degree) {
        head = heap1->first;
        h1temp = h1temp->sibling;
    }
    else {
        head = heap2->first;
        h2temp = h2temp->sibling;
    }
    tail = head;

    // start merging while keeping non decreasing order
    while (h1temp != NULL && h2temp != NULL) {

        if (h1temp->degree <= h2temp->degree) {

            tail->sibling = h1temp;
            h1temp = h1temp->sibling;
        }
        else {

            tail->sibling = h2temp;
            h2temp = h2temp->sibling;
        }
        tail = tail->sibling;
    }

    // connect tail to the remaining of the longer heap
    tail->sibling = (h1temp != NULL) ? h1temp : h2temp;

    return head;
}

// this function follows O(log(n + m)) time where n and m are the size of each heap
HeapNode *union2Heaps( BinomialHeap *heap1, BinomialHeap *heap2) {

    HeapNode *newfirst = merge2Heaps( heap1, heap2);

    heap1->first = NULL;
    heap2->first = NULL;

    if (newfirst == NULL) { return NULL;}

    HeapNode *prev = NULL;
    HeapNode *current = newfirst;
    HeapNode *next = newfirst->sibling;

    while (next != NULL) {

        if ( current->degree != next->degree || (next->sibling != NULL && next->sibling->degree == current->degree)) {

            prev = current;
            current = next;
        }

        else {

            if (current->key < next->key) {

                // if next becomes a child
                current->sibling = next->sibling;
                merge2Trees(current, next);
            }
            else if (current->key == next->key && prev == NULL) {
                current->sibling = next->sibling;
                newfirst = merge2Trees( current, next);
            }
            else {

                // if the current first becomes a child reassign first
                if (prev == NULL) { newfirst = next;}
                    // if current becomes a child
                else { prev->sibling = next;}

                merge2Trees( current, next);
                current = next;
            }
        }

        next = current->sibling;
    }

    return newfirst;
}

// put the time complexity analysis for Insert() here
// this function follows O(log(n)) time
void Insert(BinomialHeap *T, int k, int n, int c, int r, int d)
{ // k: key, n: task name, c: execution time, r: release time, d:deadline
    // You don't need to check if this task already exists in T
    //put your code here
    if (T->first == NULL) {
        T->first = newHeapNode( k, n, c, r, d);
        T->size++;
        return;
    }

    // create a new node and put it in a heap
    BinomialHeap *new = newHeap();
    new->first = newHeapNode( k, n, c, r, d);
    new->size++;

    // perform a heap union
    T->first = union2Heaps( T, new);
    T->size++;
}


// a function that inserts a node into a heap given the node and the heap
void Insert2(BinomialHeap *T, HeapNode *N) {

    BinomialHeap *new = newHeap();
    new->first = N;

    // perform a heap union
    T->first = union2Heaps( T, new);
    T->size++;
    return;
}

// function that removes the designated node given the heap and the previous node
void removeNode( BinomialHeap *heap, HeapNode *node, HeapNode *prevNode) {

    // if the node to be removed is the first
    if (node == heap->first) { heap->first = node->sibling;}
        // if not
    else { prevNode->sibling = node->sibling;}

    HeapNode *newFirst = NULL;
    HeapNode *child = node->child;

    // rearrange the children trees in non decreasing order
    while (child != NULL) {

        HeapNode *next = child->sibling;
        child->sibling = newFirst;
        child->parent = NULL;
        newFirst = child;
        child = next;
    }

    // create a new heap and put the children in it
    BinomialHeap *temp = newHeap();
    temp->first = newFirst;

    // get the union of the two heaps
    heap->first = union2Heaps( heap, temp);

    // decrease the heap size by one
    heap->size--;

    // free the temp heap
    free(temp);
}


// put your time complexity for RemoveMin() here
// this function follows O(log(n)) time
HeapNode *RemoveMin(BinomialHeap *T)
{
    // put your code here
    if (T->first == NULL) { return NULL;}

    if (T->size == 1) {
        HeapNode *temp = T->first;
        T->first = NULL;
        return temp;
    }

    // find the min node and its previous
    HeapNode *min = T->first;
    HeapNode *minPrev = NULL;
    HeapNode *current = min->sibling;
    HeapNode *prev = min;

    while (current != NULL) {

        if (current->key < min->key) {
            min = current;
            minPrev = prev;
        }

        prev = current;
        current = current->sibling;
    }

    // call the remove function to remove that node
    removeNode( T, min, minPrev);
    min->degree = 0;
    min->sibling = min->child = min->parent = NULL;
    return min;
}



// function that returns the minimum key in the heap
// this function follows O(log(n)) time
int Min(BinomialHeap *T)
{
    // put your code here
    int min;
    HeapNode *N = T->first;
    min = N->key;
    while (N != NULL) {
        if (N->key < min) {
            min = N->key;
        }
        N = N->sibling;
    }
    return min;
}


// function that reads values from txt file and fills four arrays; task name, execution time, release time, deadline
// and returns the number of tasks
int Read(char *f1, int arrV[], int arrC[], int arrR[], int arrD[]) {

    char buffer[32];
    bzero(buffer, 32);
    buffer[0] = 'n';
    char ch;
    FILE *fptr;

    // if file is not found
    if ((fptr = fopen(f1, "r")) == NULL) {
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    int v = 0; int c =0; int r =0; int d = 0;
    int i = 0;
    int k = 0;
    while (1) {
        // Reads the character where the seeker is currently
        ch = fgetc(fptr);

        // If EOF is encountered then break out of the while loop
        if (ch == EOF) {
            if (buffer != NULL) {
                if (i == 0) {arrV[v] = atoi(buffer); v++;}

                if (i == 1) {arrC[c] = atoi(buffer); c++;}

                if (i == 2) {arrR[r] = atoi(buffer); r++;}

                if (i == 3) {arrD[d] = atoi(buffer); d++;}

                i++;
                bzero(buffer, 32);
                k=0;
                break;
            }
        }

        //printf("%c\n", ch);

        // check if the current character being read is a digit
        if (isdigit(ch) != 0 || ch == '-') {
            // reads the current character
            buffer[k] = ch;
            //printf("%c\n", ch);


            // increamenting the counter so that the next
            // character is read in the next position in
            // the array of buffer
            k++;
            continue;

        }
        if (ch == ' ' || ch =='\n') {
            if (buffer[0] != 'n') {
                if (i == 0) {arrV[v] = atoi(buffer); v++;}

                if (i == 1) {
                    arrC[c] = atoi(buffer); c++;
                    if (arrC[c-1] <= 0) { printf("input error when reading the attribute of the task %d", c-1);}
                }

                if (i == 2) {
                    arrR[r] = atoi(buffer); r++;
                    if (arrR[r-1] < 0) { printf("input error when reading the attribute of the task %d", r-1);}
                }

                if (i == 3) {
                    arrD[d] = atoi(buffer); d++;
                    if (arrD[d-1] <= 0) { printf("input error when reading the attribute of the task %d", d-1);}
                }

                bzero(buffer, 32);
                buffer[0]='n';
                k=0;
                i++;
            }
        }

        // reset counter after each 4 numbers
        if (i == 4) {i=0;}

    }
    return v;
}

// function that writes the task schedule into a file
void Write(char *f2, int arrName[], int arrCore[], int arrSTime[], int arrMiss[], int end, int missedTasks) {

    FILE *fptr;

    fptr = fopen(f2, "w");

    if(fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    int i;
    for (i=0; i<end-1; i++) {
        fprintf(fptr, "%d Core%d %d ", arrName[i], arrCore[i], arrSTime[i]);
    }

    fprintf(fptr, "\n\n");

    for (i=0; i<missedTasks; i++) {
        fprintf(fptr, "Task %d missed its deadline\n", arrMiss[i]);
    }

}


// put your time complexity analysis for MyTaskScheduler here
// this function follows O(n*log(n)) time
// O(n*log(n)) for populating the releasetime heap with all the tasks
// O(n*log(n)) for the removing each task from release heap and scheduling it
int TaskScheduler(char *f1, char *f2, int m )
{
    // put your code here
    int arrV[100]; int arrC[100]; int arrR[100]; int arrD[100];
    int v;
    v = Read(f1, arrV, arrC, arrR, arrD);

    // create three new heaps
    // 1- deadline heap 2- release time heap 3- time when core is going to be available
    BinomialHeap *Deadline = newHeap();
    BinomialHeap *Releasetime = newHeap();
    BinomialHeap *Core = newHeap();

    // populate the releasetime heap
    int i;
    for (i=0; i < v-1; i++) {

        //Insert(Deadline, arrD[i], arrV[i], arrC[i], arrR[i], arrD[i]);
        Insert(Releasetime, arrR[i], arrV[i], arrC[i], arrR[i], arrD[i]);

    }


    // populate the core heap
    for (i=1;i < m+1; i++) {

        Insert(Core, 0, i, NULL,NULL,NULL);
    }

    int j;
    i = 0;
    int s = 0;
    int arrName[100]; int arrCore[100]; int arrSTime[100]; int arrMiss[50];
    HeapNode *currentCore;
    HeapNode *current;

    while (Releasetime->first != NULL) {

        for (j=0; j<m; j++) {
            if (Releasetime->first == NULL) { break;}
            HeapNode *temp = RemoveMin(Releasetime);
            temp->key = temp->Dline;
            Insert2(Deadline, temp);
        }

        for (j=0; j<m; j++) {
            if (Deadline->first == NULL) { break;}
            current = RemoveMin(Deadline);
            currentCore = RemoveMin(Core);
            arrName[i] = current->TaskName;
            arrCore[i] = currentCore->TaskName;
            arrSTime[i] = max(currentCore->key, current->Rtime);
            currentCore->key = arrSTime[i] + current->Etime;
            Insert2(Core, currentCore);
            i++;
            if (arrSTime[i-1] + current->Etime > current->Dline) {
                //printf("task %d missed its deadline \n", current->TaskName);
                arrMiss[s] = current->TaskName;
                s++;
            }
        }
    }


//    for (j=0; j<i; j++) {
//        printf("%d Core%d %d\n", arrName[j], arrCore[j], arrSTime[j]);
//    }

    // call the write function to write the results in a file
    Write(f2, arrName, arrCore, arrSTime, arrMiss, v, s);

    if (s>0) { return 0;}
    else { return 1;}
}






int main() //sample main for testing
{ int i;
    i=TaskScheduler("samplefile1.txt", "feasibleschedule1.txt", 4);
    if (i==0) printf("No feasible schedule!\n");
    /* There is a feasible schedule on 4 cores */
    i=TaskScheduler("samplefile1.txt", "feasibleschedule2.txt", 3);
    if (i==0) printf("No feasible schedule!\n");
    /* There is no feasible schedule on 3 cores */
    i=TaskScheduler("samplefile2.txt", "feasibleschedule3.txt", 5);
    if (i==0) printf("No feasible schedule!\n");
    /* There is a feasible schedule on 5 cores */
    i=TaskScheduler("samplefile2.txt", "feasibleschedule4.txt", 4);
    if (i==0) printf("No feasible schedule!\n");
    /* There is no feasible schedule on 4 cores */
    i=TaskScheduler("samplefile3.txt", "feasibleschedule5.txt", 2);
    if (i==0) printf("No feasible schedule!\n");
    /* There is no feasible schedule on 2 cores */
    i=TaskScheduler("samplefile4.txt", "feasibleschedule6.txt", 2);
    if (i==0) printf("No feasible schedule!\n");
    /* There is a feasible schedule on 2 cores */
    return 0;
}

