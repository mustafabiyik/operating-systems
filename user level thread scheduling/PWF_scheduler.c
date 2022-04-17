/*
the first 5 steps are printed but other steps are not printed on the screen, i do not understand why, i spent 10 hours on that problem but i could not handle, 


gcc -o PWF_scheduler PWF_scheduler.c
./PWF_scheduler 2 2 4 2 
if one of the cpu_weight is 4 or 3 it works perfectly 
however, ./PWF_scheduler 2 2 6 2 in this case 5 and 6 are not written on the screen

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

//struct THREADINFO
typedef struct ThreadInfo {
ucontext_t context;
int state;//three different states: ready if 1, running if 2, or finished if 0
char stack[STACK_SIZE]; //each thread has stack
int CPU_weight;
int tid;
}ThreadInfo;
int cur_ctx=0;

static ThreadInfo arThreads[ARRAY_LENGTH]; //first element for the main func.
//functions in the file
void runThread(int j);
void PWF_scheduler();
static void Threadfunc1(void);
static void Threadfunc2(void);
static void Threadfunc3(void);
static void Threadfunc4(void);
void exitThread(int context_id);
void initializeThread(int threadID);
int createThread(int ti_d,int CPU_w);

//run thread function
void runThread(int j){
//0 empty or finished
//1 reads and full
getcontext(&arThreads[0].context);
        if(arThreads[j].state){
            swapcontext(&arThreads[0].context,&arThreads[j].context);
            
            cur_ctx=j;
       }
       else{
           return ;
       }

}

void PWF_scheduler(){//round robin scheduler if the state of the thread is 1 it is runned

   // getcontext(&arThreads[0].context);
    //if the flag_var is positive then that means process continues
    while (1)
    {
        runThread(1);
        runThread(2);
        runThread(3);
        runThread(4);
        
    }
    


}

//global variables for the thread functions
int th_var1=1;
int th_var2=1;
int th_var3=1;
int th_var4=1;

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
    int count=0;
       int i;
        for (i=th_var1;i<=arThreads[1].CPU_weight;i++)
        {
        count++;
                        //space arrangement
                            for (int j=1;j<=arThreads[1].tid;j++)//# of tab equals to the thread number
                        {
                            printf("    "); 
                        }
            printf("%d \n",i);
            sleep(1);
            if(count==2){
                th_var1=i+1;
               //runThread(2);
                break;
            }
        }
        //exit thread
        if(i==arThreads[1].CPU_weight | i==(arThreads[1].CPU_weight+1)){
     exitThread(1);
        }
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
      
       
    int count=0;
       int i;
        for (i=th_var2;i<=arThreads[2].CPU_weight;i++)
        {
        count++;
                        //space arrangement
                            for (int j=1;j<=arThreads[2].tid;j++)//# of tab equals to the thread number
                        {
                            printf("    "); 
                        }
            printf("%d \n",i);
            sleep(1);
            if(count==2){
                th_var2=i+1;
                //runThread(3);
                break;
            }
        }
        if(i==arThreads[2].CPU_weight | i==(arThreads[2].CPU_weight+1)){
     exitThread(2);
        }
    }


static void Threadfunc3(void){
      int i; 
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
         int count=0;
       
        for (i=th_var3;i<=arThreads[3].CPU_weight;i++)
        {
        count++;
                        //space arrangement
                            for (int j=1;j<=arThreads[3].tid;j++)//# of tab equals to the thread number
                        {
                            printf("    "); 
                        }
            printf("%d \n",i);
            sleep(1);
            if(count==2){
                
                th_var3=i+1;
                //runThread(4);
                break;
            }
        }
        //exit thread
           if(i==arThreads[3].CPU_weight | i==(arThreads[3].CPU_weight+1)){
     exitThread(3);
        }
    }
static void Threadfunc4(void){
      printf("running> T%d        ",arThreads[4].tid);
   int i;
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
       
        int count=0;
       
        for (i=th_var4;i<=arThreads[4].CPU_weight;i++)
        {
        count++;
                        //space arrangement
                            for (int j=1;j<=arThreads[4].tid;j++)//# of tab equals to the thread number
                        {
                            printf("    "); 
                        }
            printf("%d \n",i);
            sleep(1);
            if(count==2){
                th_var4=i+1;
                //runThread(1);
                break;
            }
        }
        //exit thread
             if(i==arThreads[4].CPU_weight | i==(arThreads[4].CPU_weight+1)){
     exitThread(4);
        }
    }

//global variable for the functions
void (*Tfunctions[4])()={Threadfunc1,Threadfunc2,Threadfunc3,Threadfunc4};//array of function pointers




//exit thread free the memory
void exitThread(int context_id){
arThreads[context_id].CPU_weight=0;   
arThreads[context_id].state=0;
free(arThreads[context_id].context.uc_stack.ss_sp);


}

//initialize thread function
void initializeThread(int threadID){//it takes the thread number and then it initialize it

  //memset(&arThreads[threadID].context,0,sizeof(arThreads[threadID].context));//memory set for the the thread
  getcontext(&arThreads[threadID].context);  // get the cuurent content of the specified threadID
  arThreads[threadID].context.uc_stack.ss_sp = malloc(STACK_SIZE);  // set stack size
  arThreads[threadID].context.uc_stack.ss_size = STACK_SIZE;
  arThreads[threadID].context.uc_link = &arThreads[0].context;//link with the main context
 
}

//create thread function
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
        createThread(1, argc_arr[1]);
        createThread(2, argc_arr[2]);
        createThread(3, argc_arr[3]);
        createThread(4, argc_arr[4]);
        //call the scheduler
        PWF_scheduler();

    }
   






    else {printf("Error.\n");}
    


return 0;
}
