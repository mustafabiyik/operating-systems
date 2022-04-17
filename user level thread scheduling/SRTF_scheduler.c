/*
for compailer
gcc -o SRTF_scheduler SRTF_scheduler.c

for output printing
./SRTF_scheduler 2 1 5 9
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <time.h>


//parameter define
#define STACK_SIZE 10240
#define ARRAY_LENGTH 5

//error define
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0);

//global variables define
int cur_ctx = 0;  // current context
int threadkeeper=0;

//struct THREADINFO
typedef struct ThreadInfo {
ucontext_t context;
int state;//three different states: ready if 1, running if 2, or finished if 0
char stack[STACK_SIZE]; //each thread has stack
int CPU_weight;
int tid;
}ThreadInfo;

static ThreadInfo arThreads[ARRAY_LENGTH]; //first element for the main func.
void runThread(int j);

//free the memory exit from thread
void exitThread(int context_id){
arThreads[context_id].CPU_weight=0;   
arThreads[context_id].state=0;
free(arThreads[context_id].context.uc_stack.ss_sp);

}


//4 thread function for each of them there is one function
static void Threadfunc1(void){
    printf("running> T%d        ",arThreads[1].tid);
   
    printf("ready>");
     for(int gy=1;gy<=4;gy++){
         if(arThreads[gy].tid!=arThreads[1].tid){
        if(arThreads[gy].state==1){
            printf("T%d, ",arThreads[gy].tid);
            }
             else{
                    printf("     ");

            }}
    }
    printf("        ");
    //print finished threads
    printf("finished>");
       for(int y=1;y<=4;y++){
        if(arThreads[y].state==0){
            printf("T%d, ",arThreads[y].tid);
            }
    }
    
    //new Iine
    printf("\n");

       
        for (int i=1;i<=arThreads[1].CPU_weight;i++)
        {
                 /* code */
            for (int j=1;j<=arThreads[1].tid;j++)//# of tab equals to the thread number
        {
            printf("    "); 
        }
            printf("%d \n",i);
            sleep(1);
        }
        //exit thread
      exitThread(1);
    }
  static void Threadfunc2(void){
        printf("running> T%d        ",arThreads[2].tid);
   
    printf("ready>");
     for(int gy=1;gy<=4;gy++){
         if(arThreads[gy].tid!=arThreads[2].tid){
        if(arThreads[gy].state==1){
            printf("T%d, ",arThreads[gy].tid);
            } else{
                    printf("    ");

            }}
    }
    printf("        ");
    //print finished threads
    printf("finished>");
       for(int y=1;y<=4;y++){
        if(arThreads[y].state==0){
            printf("T%d, ",arThreads[y].tid);
            }
    }
    
    //new Iine
    printf("\n");
      
       
        for (int i=1;i<=arThreads[2].CPU_weight;i++)
        {
                 /* code */
            for (int j=1;j<=arThreads[2].tid;j++)
        {
            printf("    "); 
        }
            printf("%d \n",i);
            sleep(1);
        }
        //exit thread
        exitThread(2);
    }
static void Threadfunc3(void){
       
         printf("running> T%d        ",arThreads[3].tid);
   
    printf("ready>");
     for(int gy=1;gy<=4;gy++){
          if(arThreads[gy].tid!=arThreads[3].tid){
        if(arThreads[gy].state==1){
            printf("T%d, ",arThreads[gy].tid);
            }
             else{
                    printf("    ");

            }}
    }
    printf("        ");
    //print finished threads
    printf("finished>");
       for(int y=1;y<=4;y++){
        if(arThreads[y].state==0){
            printf("T%d, ",arThreads[y].tid);
            }
    }
    
    //new Iine
    printf("\n");
        for (int i=1;i<=arThreads[3].CPU_weight;i++)
        {
                 /* code */
            for (int j=1;j<=arThreads[3].tid;j++)
        {
            printf("    "); 
        }
            printf("%d \n",i);
            sleep(1);
        }
        //exit thread
       exitThread(3);
    }
static void Threadfunc4(void){
      printf("running> T%d        ",arThreads[4].tid);
   
    printf("ready>");
     for(int gy=1;gy<=4;gy++){
         if(arThreads[gy].tid!=arThreads[4].tid){
        if(arThreads[gy].state==1){
            printf("T%d, ",arThreads[gy].tid);
            }
            else{
                    printf("    ");

            }
            
            
            }
    }
    printf("        ");
    //print finished threads
    printf("finished>");
       for(int y=1;y<=4;y++){
        if(arThreads[y].state==0){
            printf("T%d, ",arThreads[y].tid);
            }
    }
    
    //new Iine
    printf("\n");
       
        for (int i=1;i<=arThreads[4].CPU_weight;i++)
        {
            /* code */
            for (int j=1;j<=arThreads[4].tid;j++)
        {//tab number according to the CPU 
            printf("    "); 
        }
            
            printf("%d \n",i);
            sleep(1);
        }
        //exit thread
       exitThread(4);
    }

//global variable for the functions
void (*Tfunctions[4])()={Threadfunc1,Threadfunc2,Threadfunc3,Threadfunc4};//array of function pointers

void initializeThread(int threadID){//it takes the thread number and then it initialize it

  //memset(&arThreads[threadID].context,0,sizeof(arThreads[threadID].context));//memory set for the the thread
  getcontext(&arThreads[threadID].context);  // get the cuurent content of the specified threadID
  arThreads[threadID].context.uc_stack.ss_sp = malloc(STACK_SIZE);  // set stack size
  arThreads[threadID].context.uc_stack.ss_size = STACK_SIZE;
  arThreads[threadID].context.uc_link = &arThreads[0].context;//link with the main context
 
}

int createThread(int ti_d,int CPU_w){
    int u;
    int counter_c=0;
    //search available space for the thread
for(u=1;u<=4;u++){
    if(arThreads[u].state!=1){
        break;
    }
counter_c++;
}
if(counter_c==4){
    return -1; //no available space for the thread
}
initializeThread(u);
makecontext(&arThreads[u].context,(void*)Tfunctions[u-1],0);
arThreads[u].state=1;
arThreads[u].CPU_weight=CPU_w;
arThreads[u].tid=ti_d;
return u;//context array location from 1 to 4
//end of the creaation of the thread

}



//there is no need for the interrupt shortest remaining time run first
typedef struct SORT_AR
{
    /* data */
    int data;//keep the CPU weight of the the contexts which include threads
    int whichThread;//priority in the queue
}SORT_AR;
SORT_AR sorted_array[4];//for the srtf

void swap(SORT_AR *xp, SORT_AR *yp) 
{ 
    SORT_AR temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 
  
void SRTF_scheduler(){
    
    //assign data and cpu_weight to the sorted_Array 
   sorted_array[0].data=arThreads[1].CPU_weight;
   sorted_array[0].whichThread=1;
   sorted_array[1].data=arThreads[2].CPU_weight;
   sorted_array[1].whichThread=2;
   sorted_array[2].data=arThreads[3].CPU_weight;
   sorted_array[2].whichThread=3;
   sorted_array[3].data=arThreads[4].CPU_weight;
   sorted_array[3].whichThread=4;

//selecion sort algorithm
    int i, j, min_idx; 
  
    for (i = 0; i < 3; i++) 
    { 
        min_idx = i; 
        for (j = i+1; j < 4; j++) 
        if (sorted_array[j].data < sorted_array[min_idx].data) 
            min_idx = j; 
  
        swap(&sorted_array[min_idx], &sorted_array[i]); 
    } 


    
    //sorted array increasing order
    //1 2 3 4 
//after finding next content then swap with current content

   //set context
   
   /*  
   print the sorted data of the cpu_weight on the screen
   printf("%d \n",sorted_array[0].data);
   printf("%d \n",sorted_array[1].data);
   printf("%d \n",sorted_array[2].data);
   printf("%d \n",sorted_array[3].data);*/
   //run thread according to the lowest 
    runThread(sorted_array[0].whichThread);
    runThread(sorted_array[1].whichThread);
    runThread(sorted_array[2].whichThread);
    runThread(sorted_array[3].whichThread);
   
    
}
    
//for the signal initialization signal_action is required
void signal_action(){
    
    int ctx_cur=cur_ctx;
    int ctx_next=(cur_ctx+1)%5;
    cur_ctx=(cur_ctx+1)%5;
    if(cur_ctx==0){
        cur_ctx=1;
    }
    if(swapcontext(&arThreads[ctx_cur].context,&arThreads[ctx_next].context)==-1){
        handle_error("swapcontext");
    }
}

 


void runThread(int j){

        struct  sigaction struct_sigaction;
        memset(&struct_sigaction,0,sizeof(struct_sigaction));
        sigemptyset(&struct_sigaction.sa_mask);//masking
        struct_sigaction.sa_sigaction=signal_action;//assign function
        struct_sigaction.sa_flags=SA_SIGINFO;
        sigaction(SIGALRM,&struct_sigaction,NULL);
        struct itimerval struct_itimerval;//create the struct
        //initiLİZE its variables
        //there is no need to change threads while they are running
        //beacuse shortest remaining time run first
        struct_itimerval.it_value.tv_sec=sorted_array[3].data+1;
        struct_itimerval.it_value.tv_usec=0;
        struct_itimerval.it_interval.tv_sec=0;
        struct_itimerval.it_interval.tv_usec=0;
        //printf("dew");
        setitimer(ITIMER_REAL,&struct_itimerval,NULL);
        swapcontext(&arThreads[0].context,&arThreads[j].context);


}





int main(int argc, char *argv[])
{

    // parsing command line arguments
    if (argc <= 5){
        
        int total_CPU_w = 0;
        int argc_arr[argc];
        char* char_arr[argc];
        
        for(int i=1;i<argc;i++){
            argc_arr[i] = atoi(argv[i]);//argc keeps the cpu wieghts of the threads
            total_CPU_w += argc_arr[i]; 
            char_arr[i] = argv[i];//char_arr keeps the entered command line arguments as character
            
        };

//print share
        printf("\n Share: \n");

        for(int j = 1;j<argc;j++)
        {
            
            printf(" %d/%d \t ",argc_arr[j],total_CPU_w);
        };

//print threads
        printf("\n\n Threads:\n");

        
        for(int k=1;k<argc;k++)
        {
            
            printf(" T%d \t",k);
        };

        printf("\n");
        //print running steady and finished at the bottom

    


    
    //create theads and print scheduling results
     //multiple threads can be created there are four avalailable space
        createThread(1, argc_arr[1]);
        createThread(2, argc_arr[2]);
        createThread(3, argc_arr[3]);
        createThread(4, argc_arr[4]);
        //tab numbers are specified according to the thread number
        /*
        createThread(9, argc_arr[4]);
        if(createThread(10,4)==-1){
            printf("error no available space");
        }
        //also can be created other threads
*/
        SRTF_scheduler();

                         
}




    else {printf("Error.\n");}
    


return 0;
}
