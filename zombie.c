/*
    Emirhan Akturk

    Zombie Game
    Optimum doorman:2(average winning rate %75)
*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>
#include<errno.h>
/*number of zombies in the game: 
you should synchronize threads editing this variable*/
pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
int zombieCounter;
int killedZombie;

/* Keeps track of number of zombies entered.*/
void zombieEntered(){
    int s=pthread_mutex_lock(&m);
    if(s!=0) perror("pthread_mutex_lock");
    zombieCounter++;
    printf("Zombie entered,counter:%d\n",zombieCounter);
    s=pthread_mutex_unlock(&m);
    if(s!=0) perror("pthread_mutex_unlock");
}
/* Keeps track of number of zombies killed.*/
void zombieKilled(){
    int s=pthread_mutex_lock(&m);
    if(s!=0) perror("pthread_mutex_lock");
    zombieCounter--;
    killedZombie++;
    printf("Killed zombie counter:%d\n",killedZombie);
    printf("Remaining zombies:%d\n",zombieCounter);
    s=pthread_mutex_unlock(&m);
    if(s!=0) perror("pthread_mutex_unlock");
}
/* Returns true if number of zombies in the room are 
greater than or equal to 100.*/

/* Returns true if there is at least one zombies in the room.*/
int zombiesExist(){
    return zombieCounter>0;
}
/*Returns the number of zombies killed.*/
int getKilledCount(){
    return killedZombie;
}

/* Returns the number of zombies in the room.*/
int getInTheRoomCount(){
    return zombieCounter;
}
int tooManyZombiesInTheRoom(){
    return getInTheRoomCount()>=100;
}

/*Returns true if more than 100 zombies have been killed.*/
int killed100Zombies(){
    return getKilledCount()>=100;
}
/*doorman thread*/
void *doorMan(void *p){
    int isZombieEntered;
     while(!killed100Zombies() && !tooManyZombiesInTheRoom()){//The game continues until one of two situations occurs
        srand(time(0));
        isZombieEntered=rand()%2;
        if(isZombieEntered){
            zombieEntered();}
        if(usleep(2000)!=0) perror("usleep");
     }
     pthread_exit(NULL);
}

/*slayer thread*/
void *slayer(void *p){
     while(!killed100Zombies() && !tooManyZombiesInTheRoom()){//The game continues until one of two situations occurs
      if(zombiesExist()){
        zombieKilled();
        if(usleep(2000)!=0) perror("usleep");}
    }
    pthread_exit(NULL);
}

int win=0,lose=0;
void zombieGame(int doorman){
    pthread_t DoorMans[doorman];//DoorMan
    pthread_t Slayer;//Slayers
    int s;
    int d[doorman];
    for(int play=1;play<=10;play++){
        printf("\nWelcome to the zombie game\n");
        printf("Number of doorman:%d\n",doorman);
        zombieCounter=0;
        killedZombie=0;
        s=pthread_create(&Slayer,NULL,slayer,NULL);
        if(s!=0) perror("pthread_create slayer");
        for(int i=0;i<doorman;i++){
            d[i]=pthread_create(&DoorMans[i],NULL,doorMan,NULL);
            if(d[i]!=0) perror("pthread_create doorman");
        }
        s=pthread_join(Slayer,NULL);
        for(int i=0;i<doorman;i++){
            d[i]=pthread_join(DoorMans[i],NULL);
            if(d[i]!=0) perror("pthread_join doorman");
        }
        if(s!=0) perror("pthread_join slayer");
        if(killed100Zombies()){
            printf("You have killed 100 zombies,YOU WON\n\n");
            win++;
        }
        else if(tooManyZombiesInTheRoom()){
            printf("You have been killed by the zombies,YOU LOSE\n\n");
            lose++;
        }
    }
    printf("win:%d,lose:%d\n",win,lose);
}
/*simulator main thread*/
int main(int argc, char **argv){  
    int doorman=atoi(argv[1]);
    zombieGame(doorman);
    return 0;
}
