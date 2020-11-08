#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>

void red() {printf("\033[1;31m");}
void lightred() {printf("\033[0;31m");}
void green() {printf("\033[0;32m");}        //
void yellow() {printf("\033[0;33m");}       //
void blue() {printf("\033[0;34m");}
void magenta() {printf("\033[1;35m");}
void cyan() {printf("\033[1;36m");}
void reset() {printf("\033[0m");}

int genRandomInRange(int l, int r) {
    return rand() % (r - l + 1) + l;
}
int isEligible(char instr,char stage)
{
    if(stage == 'a') {
        if(instr != 'b') return 1;
        else return 0;
    }
    if(stage == 'e')
    {
        if(instr != 'v') return 1;
        else return 0;
    }
}
struct timespec *timePassed(int T)
{
    int num = T;
    struct timespec *timeSpend = malloc(sizeof(struct timespec));
    clock_gettime(0, timeSpend);
    timeSpend->tv_sec = timeSpend->tv_sec + num;
    return timeSpend;
}
sem_t acousticPfmr;
sem_t electricPfmr;

sem_t perfDone;

int no_performer,no_acstic,no_elec,no_coord,t1,t2,waiting_time;
typedef struct performer{
    char status;
    char name[1024];
    int reachTime;
    char instr;
    int perfomTime;
    pthread_mutex_t pfmrMutex; 
    pthread_t threadid;
    sem_t semPlayer;
    struct timespec timeSpend;
}performer;

typedef struct coordinator{

    pthread_mutex_t cordiMutex;
    pthread_t threadid;
}coordinator;

typedef struct stage
{
    char status;          // 0 if stage is filled else 1
    char type;             // can be acoustic or electric
    pthread_mutex_t stgMutex;
    pthread_t threadid;
}stage;
performer *performers; coordinator *coordinators; stage *stages;


void *onStage(void *args)
{
    stage *stg = (stage *) args;
    while (1)
    {
        for(int i=1;i<=no_performer;i++)
        {
            pthread_mutex_lock(&(performers[i].pfmrMutex));
            if(isEligible(performers[i].instr,stg->type) && performers[i].status == 'p' && stg->status==0)
            {
                stg->status = 1;
                clock_gettime(0,&(performers[i].timeSpend));
                performers[i].perfomTime = genRandomInRange(t1,t2);
                blue(); printf("%s peforming %c at %c stage for %d sec.\n",performers[i].name,performers[i].instr,stg->type,performers[i].perfomTime);
                pthread_mutex_unlock(&(performers[i].pfmrMutex));
                sem_post(&(performers[i].semPlayer));
                sleep(performers[i].perfomTime);
                cyan(); printf("%s performance at %c stage ended.\n",performers[i].name,stg->type);
                sem_post(&(performers[i].semPlayer));
                stg->status =0;
                break;
            }
            pthread_mutex_unlock(&(performers[i].pfmrMutex));
        }
        if(stg->type == 'a') sem_wait(&acousticPfmr);
        else sem_wait(&electricPfmr);
    }
    
}

void *gettingReward(void *args)
{
    coordinator *cordi = (coordinator *)args;
    while(1)
    {
        // int flag =1;
        sem_wait(&perfDone);
        pthread_mutex_lock(&(cordi->cordiMutex));
        for(int i=1;i<=no_performer;i++)
        {
            pthread_mutex_lock(&(performers[i].pfmrMutex));
            if(performers[i].status=='w')   //waiting for reward
            {
                performers[i].status = 'r';
                pthread_mutex_unlock(&(performers[i].pfmrMutex));
                sleep(2);
                sem_post(&(performers[i].semPlayer));
                magenta(); printf("%s collected TShirt.\n",performers[i].name);
                break;
            }
            pthread_mutex_unlock(&(performers[i].pfmrMutex));
        }
        pthread_mutex_unlock(&(cordi->cordiMutex));
    }
}
void *performer_came(void *args)
{
    performer *Performer = (performer *)args;
    sleep(Performer->reachTime);

    pthread_mutex_lock(&Performer->pfmrMutex);
    green(); printf("%s %c arrived.\n",Performer->name,Performer->instr);
    Performer->status = 'p'; // waiting to perform
    pthread_mutex_unlock(&Performer->pfmrMutex);

    if(Performer->instr == 'p' || Performer->instr == 'g' || Performer->instr == 's')
    {
        sem_post(&acousticPfmr);
        sem_post(&electricPfmr);
    } 
    else if(Performer->instr == 'b')
        sem_post(&electricPfmr);
    else if(Performer->instr == 'v')
        sem_post(&acousticPfmr);
    
    if(sem_timedwait(&(Performer->semPlayer),timePassed(waiting_time))==-1)
    {
        yellow(); printf("%s %c left because of impatience.\n",Performer->name,Performer->instr);
        return NULL;
    }
    sem_wait(&(Performer->semPlayer));

    pthread_mutex_lock(&Performer->pfmrMutex);
    Performer->status = 'w'; //waiting for reward
    pthread_mutex_unlock(&Performer->pfmrMutex);
    sem_post(&perfDone);
    sem_wait(&(Performer->semPlayer));
    pthread_mutex_unlock(&Performer->pfmrMutex);
    return NULL;
}
int main()
{
    srand(time(0));
    cyan(); printf("\n\nEnter num_performers, num_acoustic_stages, num_electric_stages, num_coordinatiors, min and max performance time, the waiting time:\n");
    scanf("%d %d %d %d %d %d %d",&no_performer,&no_acstic,&no_elec,&no_coord,&t1,&t2,&waiting_time);
    
    performers = (performer *)malloc((no_performer+3)*sizeof(performer));
    coordinators = (coordinator *)malloc((no_coord+3)*sizeof(coordinator));
    stages = (stage *)malloc((no_acstic+no_elec+3)*sizeof(stage));

    sem_init(&acousticPfmr,0,0);
    sem_init(&electricPfmr,0,0);
    for(int i=1;i<=no_performer;++i)
    {
        performers[i].status = 'n';        //not arrived
        sem_init(&(performers[i].semPlayer),0 ,0);
        pthread_mutex_init(&(performers[i].pfmrMutex),NULL);
        scanf("%s %c %d",performers[i].name,&performers[i].instr,&performers[i].reachTime);
    }
    for(int i=1;i<=no_coord;++i)
        pthread_mutex_init(&(coordinators[i].cordiMutex),NULL);
    for(int i=1;i<=(no_elec+no_acstic);++i)
    {
        if(i<=no_acstic) stages[i].type = 'a';
        else stages[i].type = 'e';
        stages[i].status = 0;     //empty
        pthread_mutex_init(&(stages[i].stgMutex),NULL);
    }
    sem_init(&perfDone,0,0);

    red(); printf("\n\nStarted Simulation.\n");
    for(int i=1;i<=(no_acstic+no_elec);i++)
        pthread_create(&(stages[i].threadid),NULL,onStage,&stages[i]);
    for(int i=1;i<=no_performer;i++)
        pthread_create(&(performers[i].threadid),NULL,performer_came,&performers[i]);
    for(int i=1;i<=no_coord;i++)
        pthread_create(&(coordinators[i].threadid),NULL,gettingReward,&coordinators[i]);
    
    for(int i=1;i<=no_performer;++i)
    {
        pthread_join(performers[i].threadid,0);
    }
    red(); printf("Finished");
    for(int i=1;i<=(no_acstic+no_elec);++i)
        pthread_mutex_destroy(&(stages[i]).stgMutex);
    for(int i=1;i<=no_coord;++i)
        pthread_mutex_destroy(&(coordinators[i]).cordiMutex);
    for(int i=1;i<=no_performer;++i)
        pthread_mutex_destroy(&(performers[i]).pfmrMutex);
    return 0;
}