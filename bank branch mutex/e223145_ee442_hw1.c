//README
/*
first run that command on the terminal
gcc -o e223145_ee442_hw1 e223145_ee442_hw1.c -lpthread -lm -lrt

second run that command on the terminal
./e223145_ee442_hw1 -c 7 -n 2 -q 2 -g 25 -d 5
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <math.h>

//global variables
//deafult values of the variables
int NUM_CLIENTS=20;
int NUM_DESKS=4;
int QUEUE_SIZE=3;
float DURATION_RATE=10.000;
float GENERATION_RATE=100.000;

int update_info=0;
int flag=0;

//semophores
//each desk has own queue and they has own semaphores separetly
sem_t *semEmpty;//create semEmpty sem
sem_t *semFull;//create semFull sem
sem_t semFullProducer;
sem_t semEmptyProducer;

//mutexes
pthread_mutex_t mutexBuffer; //lock-unlock while adding-subtracting client to the queue then 
pthread_mutex_t infoMutex; //while printing serving information about the client, it is locked and unlocked


//STRUCTS & QUEUE FUNC.
//typdef is used for the simplicity
        typedef struct BankClient {
        int id; // Client ID
        double duration; // banking transaction duration
        }BankClient;

//specify the bank client array length


        //it is declared with typedef to easiness
        typedef struct Information {
        int paydeskNo;
        BankClient client;
        }Information;
        Information info;//create info

        typedef struct ClientQueue {
        BankClient *array;
        int maxSize; //capacity
        int currentSize;
        int frontIndex;
        int rearIndex;
        int no;
        }ClientQueue;

// create queue function return pointer to the created queue
        ClientQueue* createQueue(int capacity)
        {
                ClientQueue *Q;//create queue
                Q = (ClientQueue*)malloc(sizeof(ClientQueue));
                //properties
                Q->array = (BankClient*)malloc(sizeof(BankClient)*capacity);
                Q->currentSize = 0;
                Q->maxSize = capacity;
                Q->frontIndex = 0;
                Q->rearIndex = -1;
                return Q; //return the queue
        }

// the element which is front of the queue is removed with popFront function
        void PopFront(ClientQueue *Q)
        {
                if(Q->currentSize==0) //pop does not occur
                {
                        printf("Empty Q\n");
                        return;
                }
                else  //remove the element from the front of the queue
                {
                        Q->currentSize--;
                        Q->frontIndex++; //increase the front means remove front    element
                        /* As we fill elements in circular fashion */
                        if(Q->frontIndex==Q->maxSize){
                                Q->frontIndex=0;
                        }
                }
                return;
        }


//returns the Front client of the queue
        BankClient FrontClient(ClientQueue *Q)
        {
                if(Q->currentSize==0)
                {
                        //printf("Empty\n");
                        //exit(0);
                }
                else
                {
                        return Q->array[Q->frontIndex];
                }

        }


//insert element to the queue
        void insertRear(ClientQueue *Q,BankClient newClient)
        {
                //chech full or not
                if(Q->currentSize == Q->maxSize)
                {
                       // printf("Full Q");
                        return;
                }
                else
                {
                        Q->currentSize=Q->currentSize+1;
                        Q->rearIndex=Q->rearIndex+1;
                        //circular queue
                        if(Q->rearIndex == Q->maxSize)
                        {
                                Q->rearIndex = 0;
                        }
                        //inser newClient
                        Q->array[Q->rearIndex] = newClient;
                        return;
                }

        }



//to reach from the supervisor thread also

        // exponential distribution function//
        /* example code borrowed from
        https://stackoverflow.com/questions/34558230/generating-random-numbers-of-exponential-distribution */
        double exp_distr(double lambda){
            double x;
            x = rand()/(RAND_MAX+1.0);
            return -log(1-x)/lambda;
        }

//pay Desk functions
//during paydesk operation decreases the waiting clients 
//decreases client in the queue
//consumer part
void* PayDesks(void* args) {

ClientQueue *Q4D;//create queue
Q4D = (ClientQueue*)malloc(sizeof(ClientQueue));//mem. allocation for the queue
Q4D=(ClientQueue*) args;

    while(1) {
                       
        // Remove from queue
        sem_wait(&semFull[Q4D->no]); //queue empty then wait
        pthread_mutex_lock(&mutexBuffer);

        //info->client keeps the leaving client in it
        info.client=FrontClient(Q4D);
        //assign the the client no to the paydesk
        info.paydeskNo=Q4D->no;
        PopFront(Q4D);
        

        pthread_mutex_lock(&infoMutex);
        //to be printed by the supervisor
        update_info=1;

        /*if(info.client.id==(NUM_CLIENTS-1)){
                flag=1;//that means all of the clients are created and also they are being served
                printf("flag %d \n ",flag);
                
        //cancel the thread
                         pthread_cancel(pthread_self());             
                        return NULL;
            
        }*/

        pthread_mutex_unlock(&infoMutex);
      

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmptyProducer);//also subtract from the main semaphore
        sem_post(&semEmpty[Q4D->no]); //increase the empty number of the specified desk
        
        sleep(info.client.duration); //sleep duration of the leaving client
       
    }

pthread_exit(0);
return NULL;
}

//it takes the arguman info and print the screen
void *SuperVisor(void *arg){
while(1){

pthread_mutex_lock(&mutexBuffer);
pthread_mutex_lock(&infoMutex);

if(update_info==1){
printf("Desk %d served Client %d in %f seconds. \n" ,info.paydeskNo,info.client.id,info.client.duration);
}
//if all of the clients are served then exit
if(info.client.id==(NUM_CLIENTS-1)){

exit(0);
}

update_info=0; //assign the update_info 0 to reenable the supervisor
pthread_mutex_unlock(&infoMutex);
pthread_mutex_unlock(&mutexBuffer);
//unlock the mutexes after exiting critical region to overcome race condition
}

}

int main(int argc, char* argv[]) {

//take input from the user
int option;

while((option=getopt(argc,argv,"cnqgd"))!=-1){

switch(option){
    case 'c': //client number
       NUM_CLIENTS=atoi(argv[2]);
        break;

    case 'n': //desk arrangement
         NUM_DESKS=atoi(argv[4]);
        break;

    case 'q': //queue size
         QUEUE_SIZE=atoi(argv[6]);
        break;

    case 'g': //generation time
         GENERATION_RATE=atof(argv[8]);
        break;

    case 'd': //duration time
         DURATION_RATE=atof(argv[10]);
        break;

    default:
            printf("error");
        break;

    }



}
    printf("NUM_CLIENTS     : %d\n",NUM_CLIENTS);
    printf("NUM_DESKS       : %d\n",NUM_DESKS);
    printf("QUEUE_SIZE      : %d\n",QUEUE_SIZE);
    printf("DURATION_RATE   : %f\n",DURATION_RATE);
    printf("GENERATION_RATE : %f\n",GENERATION_RATE);
       
        
        //thread declaretions
        pthread_t thDesk [NUM_DESKS];
        pthread_t SuperVisorThread;
        
        //inialize the mutex and sem.
        pthread_mutex_init(&mutexBuffer, NULL);
        pthread_mutex_init(&infoMutex, NULL);


        //initialize the semophores
        semEmpty = (sem_t*)malloc(sizeof(sem_t)*NUM_DESKS);
        semFull = (sem_t*)malloc(sizeof(sem_t)*NUM_DESKS);
        int yo;
                for(yo=0;yo<NUM_DESKS;yo++)
                {
                        //at the beginning all of the sem are free
                        sem_init(&semEmpty[yo], 0, QUEUE_SIZE);
                        sem_init(&semFull[yo], 0, 0);
                }

        //desk*queue size can be imagined as 2d array
        //desks are pointer for the queue
        //the capacity of the bank is QUEUE_SIZE*NUM_DESKS
        //initially there is no client in the bank therefore initialized 
        //as following 
        sem_init(&semEmptyProducer,0,QUEUE_SIZE*NUM_DESKS);
        sem_init(&semFullProducer,0,0);
        
    
        
        //create desk queues
        ClientQueue *Q4Desks[NUM_DESKS];
        for (int ii = 0; ii < NUM_DESKS; ii++)
        {
              Q4Desks[ii] = (ClientQueue*)malloc(sizeof(ClientQueue));
        }

        //create clients as a number of specified by user
        BankClient *newClients;//create newClients
        //allocate space which can handle BankClient struct
        newClients = (BankClient*)malloc(sizeof(BankClient)*NUM_CLIENTS);

        

        //thread creation
        pthread_create(&SuperVisorThread,NULL,&SuperVisor,NULL);
        int j;

        for(j=0;j<NUM_DESKS;j++)
        {
                //crate the paydesk threads

                Q4Desks[j]=createQueue(QUEUE_SIZE); //Queue for the j th is created with size QUEUE_SIZE
                Q4Desks[j]->no=j;

                //checking the desk thread creation ****
                if(pthread_create(&thDesk[j],NULL,&PayDesks,Q4Desks[j])==0){
                        printf("Desk %d is created. \n",j);
                }
                else{

                        printf("Desk %d is NOT created\n",j);
                }

        }

        
        int i;
    for(i=0;i<NUM_CLIENTS;i++) {
        // Produce

        double duration=exp_distr(DURATION_RATE);

        sleep(exp_distr(GENERATION_RATE));

        // Add to the buffer
        sem_wait(&semEmptyProducer);//if full then wait
        pthread_mutex_lock(&mutexBuffer);

        newClients[i].id=i;
        newClients[i].duration=duration;
       


//   //find the minimum numbered queue then placed it
        int min=QUEUE_SIZE;
        int lem;
        for(lem=0; lem<NUM_DESKS;lem++){
                //control the clients on the queues
                //find the queue which has minimum elemnents
                if(Q4Desks[lem]->currentSize < min){
                    min=Q4Desks[lem]->currentSize;
                }
        }


        for(lem=0; lem<NUM_DESKS;lem++){
                //insert the new client to the queue which has lowest element number
                if(Q4Desks[lem]->currentSize == min){
                    insertRear(Q4Desks[lem],newClients[i]);//new client is inserted
                       sem_post(&semFull[lem]); //increase the semaphore corresponding queue
                         
                         //printf("Client %d arrived to desk %d. \n", i, lem);
                         printf("Client %d arrived. \n", i);
                         break;//break after adding created client to the least number of elements desk
                
                }
        }


//after adding new client to the queue unlock to mutex to enable removing queue capability
//to prevent race condition
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFullProducer);//increase full by 1
   
    }


//join thread
    for (int iiii = 0; iiii < NUM_DESKS; iiii++) {
        if (pthread_join(thDesk[iiii], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
        pthread_join(SuperVisorThread,NULL);
//destroy the semophores exiting stage of the process
    sem_destroy(&semEmptyProducer);
    sem_destroy(&semFullProducer);

         for(yo=0;yo<NUM_DESKS;yo++){
    sem_destroy(&semEmpty[yo]);
    sem_destroy(&semFull[yo]);
    }
    
    pthread_mutex_destroy(&infoMutex);
    pthread_mutex_destroy(&mutexBuffer);

 //free the memory
        for (int ii = 0; ii < NUM_DESKS; ii++)
        {
         free(Q4Desks[ii]);
        }
        free(newClients);
    return 0;
}