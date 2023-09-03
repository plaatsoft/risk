// main.c 
//
// Function:
//    Core of Risk Game Engine
//
// History:
//    01-01-2001  WVDP  Start of programming  
//    05-01-2001  WVDP  Add Attack succesfull card functions 
//    06-01-2001  WVDP  Add complet map

#include <stdio.h>
#include <curses.h>

/* General defines */
#define OUTPUT_MODE    2
#define SLEEP          0
#define TRUE           1
#define FALSE          0

/* Max defines */
#define MAX_LANDS      42
#define MAX_CONTINENTS 6
#define MAX_ARMIES     42 
#define MAX_PLAYERS    3
#define MAX_CARDS      10 
#define MAX_BORDER     10 
#define MAX_NAME       32

/* Extra armes for cards */
#define THREE_A        8
#define THREE_B        6
#define THREE_C        4 
#define ONE_ABC        10

/* Extra armies for continent */
#define N_AMERICA      5
#define S_AMERICA      2
#define AFRICA         3
#define ASIA           7
#define EUROPE         5
#define AUSTRALIA      2

/* ---------------------------------------------- */
/* Typedefs                              
/* ---------------------------------------------- */

typedef struct 
{
   int nr;
   int x;
   int y;
   char name[MAX_NAME];
   int continent;  
   int border[MAX_BORDER];
   int army;
   int owner;
   int ready;
}
LAND;

typedef struct
{
   long game;
   long round;
   long attack;
   long win;
   long lose;
}
INFO;

typedef struct 
{
   int nr;
   char name[MAX_NAME];
   int army;
   int amount_lands;
   int card[MAX_CARDS];
   int attack_succesfull;
} 
PLAYER;

/* ---------------------------------------------- */
/* Variables                              
/* ---------------------------------------------- */

int startup;
WINDOW *win;
INFO info;
PLAYER player[MAX_PLAYERS];
LAND land[MAX_LANDS]=
{
  { 0,  1, 1, "Alaska",                  0, 1, 3, 29, -1 },
  { 1,  2, 1, "Noord-West-Territoria",   0, 0, 2, 3, 4, -1 },
  { 2,  3, 1, "Groenland",               0, 1, 4, 13, -1 },
  { 3,  1, 2, "Alberta",                 0, 0, 1, 4, 6, -1 },
  { 4,  2, 2, "Ontario",                 0, 1, 2, 5, 7, 6, 3, -1 },
  { 5,  3, 2, "Quebec",                  0, 4, 7, -1 },
  { 6,  1, 3, "Verenigde-Staten (West)", 0, 3, 4, 7, 8, -1 },
  { 7,  2, 3, "Verenigde-Staten (Oost)", 0, 4, 5, 6, 8, -1 }, 
  { 8,  1, 4, "Midden-Amerika",          0, 6, 7, 9, -1 },

  { 9,  1, 6, "Venezuela",               1, 8, 10, 11, -1 },
  { 10, 1, 7, "Peru",                    1, 9, 11, 12, -1 },
  { 11, 2, 7, "Brazilie",                1, 9, 10, 12, 20, -1 },
  { 12, 1, 8, "Argentinie",              1, 10, 11, 41, -1 },

  { 13, 5, 2, "Ijsland",                 2, 2, 14, 15, -1 },
  { 14, 5, 3, "Scandinavie",             2, 13, 15, 16, 17, -1 },
  { 15, 4, 4, "Groot-Brittannie",        2, 13, 18, 16, 14, -1 },
  { 16, 5, 4, "Noord Europa",            2, 14, 17, 19, 18, 15, -1 },
  { 17, 6, 4, "Oost Europa",             2, 14, 16, 19, 26, 33, -1 },
  { 18, 5, 5, "West Europa",             2, 15, 19, 20, -1 },
  { 19, 6, 5, "Zuid Europa",             2, 18, 16, 17, 20, 21, 35, -1 },
  
  { 20, 4, 7, "Noord-Afrika",            3, 18, 21, 22, 23, -1 },
  { 21, 5, 7, "Egypte",                  3, 19, 35, 22, 20, -1 },
  { 22, 6, 8, "Oost-Afrika",             3, 21, 25, 24, 23, 20, 35, -1 },
  { 23, 5, 8, "Central-Afrika",          3, 20, 22, 24, -1 },
  { 24, 5, 9, "Zuid-Afrika",             3, 23, 22, 25, -1 },
  { 25, 6, 9, "Madagascar",              3, 24, 22, -1 },

  { 26, 8, 3, "Oeral",                   4, 17, 27, 32, 33, -1 },
  { 27, 9, 2, "Siberie",                 4, 28, 30, 31, 32, 26, -1 },
  { 28, 10,2, "Jakoetsk",                4, 27, 29, 30, -1 },
  { 29, 11,3, "Kamtsjaka",               4, 0, 28, 30, 31, 34, -1 },
  { 30, 10,3, "Tsjita",                  4, 27, 28, 29, 31, -1 },
  { 31, 10,4, "Mongolie",                4, 27, 30, 29, 34, 32, -1 },
  { 32, 9, 5, "China",                   4, 33, 26, 27, 31, 37, 36, -1 },
  { 33, 8, 4, "Kazachstan",              4, 17, 26, 32, 36, 35, -1 },
  { 34, 11,5, "Japan",                   4, 29, 31, -1 },
  { 35, 8, 5, "Midden-Oosten",           4, 17, 19, 33, 36, 22, 21, -1 },
  { 36, 9, 6, "India",                   4, 35, 33, 32, 37, -1 },
  { 37, 10,6, "Zuid-Oost Azie",          4, 36, 32, 38, -1 },

  { 38, 10,8, "Indonesie",               5, 37, 41, 39, -1 },
  { 39, 11,8, "Nieuw-Guinea",            5, 38, 40, 41, -1 },
  { 40, 10,9, "West-Australia",          5, 38, 39, 41, -1 },
  { 41, 11,9, "Oost-Australia",          5, 40, 39, 12, -1 }
};

/* ---------------------------------------------- */
/* Functions                              
/* ---------------------------------------------- */

int init_game(void)
{
   if (OUTPUT_MODE==1) printf("init_game: enter\n");
   startup=TRUE;
   info.round=0;
   info.attack=0;
   info.win=0;
   info.lose=0;
   if (OUTPUT_MODE==2) 
   {
       win=initscr();
       werase(win);
       refresh();
   }
   if (OUTPUT_MODE==1) printf("init_game: leave \n");
}

int create_players(void)
{
   int i,j;
   if (OUTPUT_MODE==1) printf("Creating players ");
   for(i=0;i<MAX_PLAYERS;i++)
   {
     if (OUTPUT_MODE==1) printf(".");
     player[i].nr=i;
     player[i].army=MAX_ARMIES;
     player[i].amount_lands=0;
     player[i].attack_succesfull=FALSE; 
     for(j=0;j<MAX_CARDS;j++) player[i].card[j]=0;
   }
   if (OUTPUT_MODE==1) printf("\n");
}

int create_planet(void)
{
   int i;
   int owner=0;
   if (OUTPUT_MODE==1) printf("Creating planet ");
   for(i=0;i<MAX_LANDS;i++)
   {
      if (OUTPUT_MODE==1) printf(".");
      land[i].nr=i;
      land[i].army=1;
      land[i].owner=owner;
      player[owner].amount_lands++;
      player[owner].army--;
      if (++owner>=MAX_PLAYERS) owner=0;
   }
   if (OUTPUT_MODE==1) printf("\n");
};

int place_armies(int owner)
{
   int i,j,k;
   int value,match;
   int *p;
   
   if (OUTPUT_MODE==1) printf("%d: Placing armies ",owner); 

   i=0;
   while(player[owner].army>0)
   {
     if(owner==land[i].owner) 
     {
       /* Search land with enemy at the border */
       p=land[i].border;
       k=0;
       match=FALSE;
       while (p[k]!=-1)
       {
          match|=(land[p[k]].owner != owner);
          if (match) break;
          k++;
       }

       /* if found place new armies */
       if (match)
       {
          value=(random() % ((player[owner].army/2)+1))+1;
          if (value>player[owner].army) value=player[owner].army;
          for(j=0;j<value;j++) if (OUTPUT_MODE==1) printf(".");
          land[i].army+=value;
          player[owner].army-=value;
       }
     }
     if (i<(MAX_LANDS-1)) i++; else i=0;
   } 
   if (OUTPUT_MODE==1) printf("\n");
}

int show_board(int owner)
{
   int i,j;
   int flag=TRUE;

   for (j=0;j<MAX_PLAYERS;j++)
   {
     for (i=0;i<MAX_LANDS;i++)
     {
       if(land[i].owner==j)
       {
         if (flag) 
         {
           if (OUTPUT_MODE==1) printf("\n");
           flag=FALSE;
         }
         if (OUTPUT_MODE==1)
         {
            printf("Land=%s  Armies=%d  Owner=%d\n",
               land[i].name,land[i].army,land[i].owner);
         }
         if (OUTPUT_MODE==2)
         {
            char hulpstr[100];
            sprintf(hulpstr," %d|%d ",land[i].army,land[i].owner);
            mvwaddstr(win,land[i].y*2,land[i].x*5,hulpstr);
         }
       }
     }
     flag=TRUE;
   }
   if (OUTPUT_MODE==1) printf("\n");
   if (OUTPUT_MODE==2) 
   {
      char hulpstr[100];
      sprintf(hulpstr,"Game   = %d ",info.game);
      mvwaddstr(win,1,65,hulpstr);
      sprintf(hulpstr,"Round  = %d ",info.round);
      mvwaddstr(win,2,65,hulpstr);
      sprintf(hulpstr,"Player = %d ",owner);
      mvwaddstr(win,3,65,hulpstr);
      sprintf(hulpstr,"Attack = %d ",info.attack);
      mvwaddstr(win,5,65,hulpstr);
      sprintf(hulpstr,"Win    = %d ",info.win);
      mvwaddstr(win,6,65,hulpstr);
      sprintf(hulpstr,"Lose   = %d ",info.lose);
      mvwaddstr(win,7,65,hulpstr);
      refresh();
   }
}

int dice(void)
{
   return((random()%6)+1);
}

int get_card(int owner)
{
   int i=0;
   while(player[owner].card[i]!=0) i++;
   player[owner].card[i]=(random()%3)+1;
}

int clear_card(int owner,int card)
{
   int i=0;
   while(player[owner].card[i]!=card) i++;
   player[owner].card[i]=0;
}

void check_card(int owner)
{
   int a=0,b=0,c=0,i;
   for(i=0; i<MAX_CARDS;i++)
   {
      switch (player[owner].card[i])
      {
         case 1: a++;
                 break;
         case 2: b++;
                 break;
         case 3: c++;
                 break;
      }
   }  
   if (OUTPUT_MODE==1) printf("%d: Check Card %d %d %d\n",owner,a,b,c);
   if ((a>0) && (b>0) && (c>0))
   {
      player[owner].army+=ONE_ABC;
      clear_card(owner,1);
      clear_card(owner,2);
      clear_card(owner,3);
      if (OUTPUT_MODE==1) 
      {
         printf("%d: %d extra armies (special)\n",owner,ONE_ABC);
      }
      return;
   }
   if (c>2) 
   {
      player[owner].army+=THREE_C;
      clear_card(owner,3);
      clear_card(owner,3);
      clear_card(owner,3);
      if (OUTPUT_MODE==1) 
      {
        printf("%d: %d extra armies (3 artillerie)\n",owner,THREE_C);
      }
      return;
   }
   if (b>2) 
   {
      player[owner].army+=THREE_B;
      clear_card(owner,2);
      clear_card(owner,2);
      clear_card(owner,2);
      if (OUTPUT_MODE==1) 
      {
         printf("%d: %d extra armies (3 infanterie)\n",owner,THREE_B);
      }
      return;
   }
   if (a>2) 
   {
      player[owner].army+=THREE_A;
      clear_card(owner,1);
      clear_card(owner,1);
      clear_card(owner,1);
      if (OUTPUT_MODE==1) 
      {
        printf("%d: %d extra armies (3 cavalerie)\n",owner,THREE_A);
      }
   }
}

int find_strong_land(int owner)
{
   int i;
   int nr=-1;
   int army=1;
   for(i=0;i<MAX_LANDS;i++)
   {
      if ((land[i].owner==owner) && 
          (land[i].ready==FALSE) &&
          (land[i].army>army)) 
      {
          army=land[i].army;
          nr=i;
      }
   }
   return(nr); 
}

int find_weak_land(int owner,int attack_nr)
{
   int i=0;
   int nr=-1;
   int army=100;
   int *p;

   p=land[attack_nr].border;
   while(p[i]!=-1)
   {
      if ((land[ p[i] ].owner!=owner) && 
        (land[ p[i] ].army<army))
      {
          army=land[ p[i] ].army;
          nr=p[i];
      }
      i++;
   }
   return(nr);
}

int calculate_new_armies(int owner)
{
  int value;
  value=(player[owner].amount_lands / 3);
  player[owner].army+=value;
  if (OUTPUT_MODE==1) 
  {
     printf("%d: %d extra armies (land)\n",owner,value);
  }
}  

int battle(int attack_nr,int defend_nr)
{
   int a1=0,a2=0,a3=0,b1=0,b2=0,tmp;
  
   if (OUTPUT_MODE==1)
   {
      printf("%s [%d|%d] attacks %s [%d|%d]\n",
        land[attack_nr].name,land[attack_nr].army,
        land[attack_nr].owner,land[defend_nr].name,
        land[defend_nr].army,land[defend_nr].owner);
   }
 
   if (OUTPUT_MODE==2)
   {
      char hulpstr[100];
      sprintf(hulpstr,"%s [%d|%d] attacks %s [%d|%d]\n",
        land[attack_nr].name,land[attack_nr].army,
        land[attack_nr].owner,land[defend_nr].name,
        land[defend_nr].army,land[defend_nr].owner);
      mvwaddstr(win,20,0,hulpstr);
      wattron(win,A_BOLD);
      sprintf(hulpstr," %d|%d ",land[attack_nr].army,land[attack_nr].owner);
      mvwaddstr(win,land[attack_nr].y*2,land[attack_nr].x*5,hulpstr);
      sprintf(hulpstr," %d|%d ",land[defend_nr].army,land[defend_nr].owner);
      mvwaddstr(win,land[defend_nr].y*2,land[defend_nr].x*5,hulpstr);
      wattroff(win,A_BOLD);
   }  
   a1=dice();
   if (land[attack_nr].army>2) a2=dice();
   if (land[attack_nr].army>3) a3=dice();
   b1=dice();
   if (land[defend_nr].army>1) b2=dice();

   /* Sort dice values */
   if (b1<b2) 
   {
     tmp=b2;
     b2=b1;
     b1=tmp;
   }
   if (a1<a2) 
   {
     tmp=a2;
     a2=a1;
     a1=tmp;
   }
   if (a2<a3) 
   {
     tmp=a3;
     a3=a2;
     a2=tmp;
   }
   if (a1<a2) 
   {
     tmp=a2;
     a2=a1;
     a1=tmp;
   }
   if (a1==0) a1=a2;
   if (a2==0) a2=a3;
   if (a1==0) a1=a2;
   if (b1==0) b1=b2;
 
   if (OUTPUT_MODE==1) 
   {
      printf("a1=[%d] a2=[%d] a3=[%d] d1=[%d] d2=[%d] ", 
        a1,a2,a3,b1,b2); 
   }
   if (OUTPUT_MODE==2) 
   {
      char hulpstr[100];
      sprintf(hulpstr,"a1=[%d] a2=[%d] a3=[%d] d1=[%d] d2=[%d] ", 
        a1,a2,a3,b1,b2); 
      mvwaddstr(win,21,0,hulpstr);
   }

   /* Calculate loss / win */
   info.attack++;
   if (a1>b1) land[defend_nr].army--; 
     else land[attack_nr].army--;
   if ((b2!=0) && (a2!=0))
   {
      if (a2>b2) land[defend_nr].army--; 
        else land[attack_nr].army--;
   }

   /* Check if land is defied */
   if (land[defend_nr].army==0)
   {
     info.win++;
     player[land[defend_nr].owner].amount_lands--;
     land[defend_nr].owner=land[attack_nr].owner;
     player[land[attack_nr].owner].amount_lands++;
   
     /* Move armies */
     if(land[attack_nr].army>4)
     {
        land[defend_nr].army=land[attack_nr].army-4;
        land[attack_nr].army=4; 
     }
     else
     {
        land[defend_nr].army=land[attack_nr].army-1;
        land[attack_nr].army=1; 
     }
     player[land[attack_nr].owner].attack_succesfull=TRUE; 
     if (OUTPUT_MODE==1) printf("WIN.\n");
     if (OUTPUT_MODE==2) 
     {
        mvwaddstr(win,21,40,"WIN");
     }
   }
   else
   {
     if (OUTPUT_MODE==1) printf("\n");
     if (OUTPUT_MODE==2) 
     {
        mvwaddstr(win,21,40,"   ");
     }
     info.lose++;
   } 
   if (OUTPUT_MODE==2)
   {
     refresh();
     sleep(SLEEP);
     show_board(land[attack_nr].owner);
   }
}

int check_continents(int owner)
{
   int i,j;
   int match;
   for (i=0; i<MAX_CONTINENTS; i++)
   {
     match=TRUE;
     for(j=0; j<MAX_LANDS; j++)
     {
        if (land[j].continent==i) 
        {
           match&=(land[j].owner == owner);
        }
        if (match==FALSE) break;
     }
     if (match)
     {
        switch( i )
        {
           case 0 : player[owner].army+=N_AMERICA;
                    if (OUTPUT_MODE==1) 
                    { 
                       printf("%d: %d extra armies (Nord-America)\n",
                         owner, N_AMERICA);
                    }
                    break;
           case 1 : player[owner].army+=S_AMERICA;
                    if (OUTPUT_MODE==1) 
                    {
                       printf("%d: %d extra armies (South-America)\n",
                          owner, S_AMERICA);
                    }
                    break;
           case 2 : player[owner].army+=EUROPE;
                    if (OUTPUT_MODE==1)
                    {
                       printf("%d: %d extra armies (Europa)\n",
                          owner, EUROPE);
                    }
                    break;
           case 3 : player[owner].army+=AFRICA;
                    if (OUTPUT_MODE==1)
                    {
                       printf("%d: %d extra armies (Africa)\n",
                          owner, AFRICA);
                    }
                    break;
           case 4 : player[owner].army+=ASIA;
                    if (OUTPUT_MODE==1)
                    {
                       printf("%d: %d extra armies (Asia)\n",
                          owner, ASIA);
                    }
                    break;
           case 5 : player[owner].army+=AUSTRALIA;
                    if (OUTPUT_MODE==1) 
                    {
                       printf("%d: %d extra armies (Australia)\n",
                         owner, AUSTRALIA);
                    }
                    break;
        }
      }
   }
}

int get_player(int owner)
{
   int flag=TRUE;
    
   while( flag )
   {
      owner++;
      if (owner>=MAX_PLAYERS) 
      {
         owner=0;
         startup=FALSE;
      }
      player[owner].attack_succesfull=FALSE; 
      
      /* Check if owner has land else skip player */
      flag = player[owner].amount_lands == 0;
   }
   return(owner);
}

int game_engine(void)
{
  int owner=-1;
  int defend_nr;
  int attack_nr;
  int go;
  int i;
  int active=TRUE;
  
  while(active)
  {
    /* New round */
    info.round++;
    if (OUTPUT_MODE==1) printf("Round [%d]\n",info.round);
   
    /* Select Player */ 
    owner=get_player(owner);
    if (OUTPUT_MODE==1) printf("Player [%d] active.\n",owner); 
    
    /* Clear land battle flags */
    for (i=0;i<MAX_LANDS;i++) land[i].ready=FALSE;
    
    /* Process new armies */
    if (!startup)
    {
       calculate_new_armies(owner);
       check_continents(owner);
       check_card(owner);
       place_armies(owner);
    }
    go=TRUE;
   
    /* Show game board */ 
    show_board(owner);
  
    /* Player attack */ 
    while(go)
    {
       attack_nr=find_strong_land(owner); 
       if (attack_nr!=-1)
       {
         /* Found attact land with more then 1 army */
         /* Find weaker land */
         defend_nr=find_weak_land(owner,attack_nr); 
         if (defend_nr!=-1)
         {
            battle(attack_nr,defend_nr); 
         }
         else
         {
            land[attack_nr].ready=TRUE;
         }
       }
       else
       {
         int stop=TRUE;
         go=FALSE;
      
         if (player[owner].attack_succesfull)
         {
            get_card(owner);
            if (OUTPUT_MODE==1) printf("\n");
         }   
         
         /* Check if game ready  */
         for (i=0; i<MAX_LANDS; i++)
         {
           stop &= owner==land[i].owner;
         }
         if (stop==TRUE) active=FALSE;
      }
    }
  }

  if (OUTPUT_MODE==1) 
  {
     printf("Game over\n");
  }

  show_board(owner);

  if (OUTPUT_MODE==1) 
  {
     printf("The End [%d|%d|%d]\n",info.round,info.win,info.lose);
  }
} 

int close_game(void)
{
  if (OUTPUT_MODE==2)
  {
     delwin(win);
     endwin();
  }
}

int main(void)
{
   int i;
   srandom((int)time());

   /* Create Game board */   
  
   info.game=0; 
   while(TRUE)
   {
      info.game++;
      
      /* Create Game board */   
      init_game();
      create_players();
      create_planet();
      for (i=0;i<MAX_PLAYERS;i++) place_armies(i);
      if (OUTPUT_MODE==1) printf("\n");  
   
      /* Play the game */
      game_engine();
      if (OUTPUT_MODE==1) break;  
   }
   close_game();
}

/* ---------------------------------------------- */
/* The End                              
/* ---------------------------------------------- */

