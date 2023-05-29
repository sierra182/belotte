#include <stdio.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <time.h>

#define amountTeams 2
#define teamsPlayers 2
#define players 4
#define totalCards 32
#define playersCards 8 
#define trickCards 4
#define firstDealMaxCards 5
#define firstDealMaxCards1 3
#define firstDealMaxCards2 2
#define startNumberColorSequence 7
#define endNumberColorSequence 14
#define amountColor 4

#define modelOrder 7, 8, 9, 11, 12, 13, 10, 14
#define trumpModelOrder 7, 8, 12, 13, 10, 14, 9, 11

typedef struct {

    int number;
    int color;
} card;

typedef struct {

    card tricksStack[totalCards];
    card playerHand[teamsPlayers][playersCards];      
} team;

typedef struct {

    card deck[totalCards];
    card trick[trickCards];
    card trump;
    int team;
    int player;    
} gameDirector;

void initCard(card* c) {

    c->number = 0;
    c->color = 0;
}

void initDeckBelotteStyle(card deck[totalCards]) {

    for (int i = 0, j = startNumberColorSequence, color = 1; i < totalCards; i++, j++) {
    
        if (j > endNumberColorSequence) {
            
            j = startNumberColorSequence;
            color++;

            if (color > amountColor) {
                color = 1;
            }
        }

        deck[i].number = j;
        deck[i].color = color;
    }   
}

void initDeck(card deck[totalCards]) {

    for (int i = 0; i < totalCards; i++) {
    
       initCard(&deck[i]);        
    }

    initDeckBelotteStyle(deck);
}

void initPlayerHand(card* playerCard) {
 
        playerCard->number = 0;
        playerCard->color = 0;    
}

void initTricksStack(card tricksStack[totalCards]) {

    for (int i = 0; i < totalCards; i++) {
    
        tricksStack[i].number = 0;
        tricksStack[i].color = 0;
    }
}

void initTrick(card trick[trickCards]) {

    for (int i = 0; i < trickCards; i++) {
    
        trick[i].number = 0;
        trick[i].color = 0;
    }
}

void initTeams(team teams[amountTeams]) {

    for(int i = 0; i < amountTeams; i++) {

        for(int j = 0; j < teamsPlayers; j++) {

            for(int k = 0; k < playersCards; k++) {
                
                initPlayerHand(&teams[i].playerHand[j][k]);
            }
        }
         
        initTricksStack(teams[i].tricksStack);
    }     
}

void initGameDirector(gameDirector* gd) {
   
    gd->player = 0;
    gd->team = 0;    
    initTrick(gd->trick);
    initDeck(gd->deck);  
    initCard(&gd->trump); 
}

void initDivideByColorResult(card divideByColorResult[amountColor][playersCards]) {

    for (int i = 0; i < amountColor; i++) {

        for (int j = 0; j < playersCards; j++) {
                   
            divideByColorResult[i][j].color = 0;
            divideByColorResult[i][j].number = 0;
        }        
    }
}

void shuffleCards(card deck[totalCards]) {

    srand(time(NULL)); 
    
    for (int i = 0; i < totalCards; i++) {
      
        int j = rand() % (totalCards - i) + i;  
        card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void nextPlayer(gameDirector* gd) {

    int t = gd->team - 1;
    int p = gd->player - 1;
    int count = 0;

    for (int i = 0; i < 2; i++) {    

        for(int j = p; j < teamsPlayers; j++) {              
         
            for (int k = t; k < amountTeams; k++, count++) { 

                if (count >= 2) {
                    return;
                }

                gd->team = k + 1;
                gd->player = j + 1;
            }
            t = 0;
        }
        p = 0;
    }  
}

void firstDealBis(gameDirector* gd, team teams[amountTeams]) {   // Inutile

    int t = gd->team - 1;
    int p = gd->player - 1;      
    int count = 0;

    for (int i = 0, d = 0; i < firstDealMaxCards; ) {
          
        for(int j = p; j < teamsPlayers; j++) {              
         
            for (int k = t; k < amountTeams; k++, count++, d++) {   
               
                if (d >= firstDealMaxCards * players) {
                    return;
                }
                
                if (count >= players) {
                    count = 0;
                    i++;                   
                } 

                teams[k].playerHand[j][i] = gd->deck[d];
                initCard(&gd->deck[d]);                             
            } 
            t = 0;
        }  
        p = 0;             
    }    
}

void firstDeal(gameDirector* gd, team teams[amountTeams]) { 

    int t = gd->team - 1;
    int p = gd->player - 1;    
    int playerHandIndex = 0;
    int deckIndex = 0;

    for (int i = 0, d = deckIndex; i < 2; i++ ) {
          
        for(int j = p; j < teamsPlayers; j++) {              
         
            for (int k = t; k < amountTeams; k++) {  

                if (d >= firstDealMaxCards1 * players) {

                    deckIndex = d;                                   
                    goto a;                    
                } 

                for (int l = 0; l < firstDealMaxCards1; l++, playerHandIndex = l,d++) {
                    
                    teams[k].playerHand[j][l] = gd->deck[d];
                    initCard(&gd->deck[d]);                    
                }                           
            }
            t = 0;
        }
        p = 0; 
    }
    
    a:
    
    t = gd->team - 1;
    p = gd->player - 1;

    for (int i = 0, d = deckIndex; i < 2; i++ ) {
       
        for(int j = p; j < teamsPlayers; j++) {              
            
            for (int k = t; k < amountTeams; k++) {  
                
                if (d >= firstDealMaxCards * players) {
                   
                    return;
                } 

                for (int l = playerHandIndex ; l < playersCards - playerHandIndex; l++, d++) {
                  
                    teams[k].playerHand[j][l] = gd->deck[d];
                    initCard(&gd->deck[d]);
                }                                         
            }
            t = 0;
        }
        p = 0; 
    }
}

void insertionSortAscending(int array[], int size) {            // inutile

    int i = 0, key = 0, j = 0;

    for (i = 1; i < size; i++) {

        key = array[i];
        j = i - 1;

        while (j >= 0 && array[j] > key) {

            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
    }
}

void sortNumberByModel(int model [playersCards], card playerHand[playersCards]) {

    card temp[playersCards] = {0,0,0,0,0,0,0,0};
    for (int i = 0, k = 0; i < playersCards; i++) {
    
        for (int j = 0; j < playersCards; j++) {

            if (model[i] == playerHand[j].number) {

              
                temp[k] = playerHand[j];
                k++;
            }
        }
    }

    for (int i = 0; i < playersCards; i++) {
        playerHand[i] = temp[i];
    }   
}

void divideByColor(gameDirector gd, card playerHand[playersCards], card resultDivideByColor[amountColor][playersCards]) {
    
    for (int i = 0; i < playersCards; i++) {

        for (int j = 1; j < amountColor + 1; j++) {

            if ( playerHand[i].color == j) {

                resultDivideByColor[j - 1][i] = playerHand[i];          
            }    
        }
    }

    int model[playersCards] = {modelOrder};
    int trumpModel[playersCards] = {trumpModelOrder};

    for (int i = 0; i < amountColor; i++) {
       
        if (gd.trump.color != 0 && gd.trump.color == i + 1) {
           
            sortNumberByModel(trumpModel, resultDivideByColor[i]); 
        }
        else {  
            sortNumberByModel(model, resultDivideByColor[i]); 
        }
    }      
} 

void mergeByColor(gameDirector gd, card playerHand[playersCards], card divideByColorResult[amountColor][playersCards]) {

    int playerHandIndex = 0;

    for (int i = 0, k = 0; i < amountColor; i++) {

        for (int j = 0; j < playersCards; j++) {
          
                if (divideByColorResult[i][j].color != 0 && gd.trump.color != i + 1) {

                playerHand[k] = divideByColorResult[i][j];
                playerHandIndex = ++k;     
            }  
            
              
        }
    }  

     for (int i = 0, k = playerHandIndex; i < amountColor; i++) {

        for (int j = 0; j < playersCards; j++) {
          
                if (divideByColorResult[i][j].color != 0 && gd.trump.color == i + 1) {

                playerHand[k] = divideByColorResult[i][j];
                k++;     
            }   
        }
    } 
}

void sortPlayerHand(gameDirector gd, card playerHand[playersCards]) {

    card divideByColorResult[amountColor][playersCards];
    initDivideByColorResult(divideByColorResult);
  
    divideByColor(gd, playerHand, divideByColorResult);
   
    mergeByColor(gd, playerHand, divideByColorResult);     
}

void sortAllPlayersHands(gameDirector gd, team teams[amountTeams]) {

    for (int i = 0; i < amountTeams; i++) {

        for (int j = 0; j < teamsPlayers; j++) {   
            sortPlayerHand(gd, teams[i].playerHand[j]);
        }
    }
}

void addCardToPlayerHand(card playerHand[playersCards], int pHindex, card deck[totalCards], int cardIndex) {

    playerHand[pHindex] = deck[cardIndex];
    initCard(&deck[cardIndex]);
}

void removeCardFromPlayerHand(card playerHand[playersCards], int pHIndex) {
    
    initCard(&playerHand[pHIndex]);
}

void setActualPlayer(gameDirector* gd, int team, int player) {

    gd->team = team;
    gd->player = player;
}

void modifyTrump(gameDirector* gd) {
  
    gd->trump = gd->deck[firstDealMaxCards * players];
}

int chooseTrump(gameDirector* gd, team teams[amountTeams], int resultTrumpPlayer[2]) {

    modifyTrump(gd);
    int trump = firstDealMaxCards * players;
    printf("\n\nAtout: c: %d, n: %d\n", gd->deck[trump].color, gd->deck[trump].number);    
    printf("Choisir quel joueur prend l'Atout:\n");

    a:
    int input;
    scanf("%d", &input);
    getchar();
    
    switch (input) {
        case 1:
            printf("Vous avez saisi 1.\n");           
            addCardToPlayerHand(teams[0].playerHand[0], playersCards - 1,  gd->deck, trump);
            resultTrumpPlayer[0] = 0;
            resultTrumpPlayer[1] = 0;
            // setActualPlayer(&gd, 0, 0);
            break;
        case 2:
            printf("Vous avez saisi 2.\n");
            addCardToPlayerHand(teams[1].playerHand[0], playersCards - 1,  gd->deck, trump);
            resultTrumpPlayer[0] = 1;
            resultTrumpPlayer[1] = 0;
            // setActualPlayer(&gd, 1, 0);
            break;
        case 3:
            printf("Vous avez saisi 3.\n");
            addCardToPlayerHand(teams[0].playerHand[1], playersCards - 1,  gd->deck, trump);
            resultTrumpPlayer[0] = 0;
            resultTrumpPlayer[1] = 1;
            // setActualPlayer(&gd, 0, 1);
            break;
        case 4:
            printf("Vous avez saisi 4.\n");
            addCardToPlayerHand(teams[1].playerHand[1], playersCards - 1,  gd->deck, trump);
            resultTrumpPlayer[0] = 1;
            resultTrumpPlayer[1] = 1;
            // setActualPlayer(&gd, 1, 1);
            break;
        default:
            printf("Entier non valide.\n");
            goto a;
            break;
    }          
}

void secondDeal(gameDirector* gd, team teams[amountTeams], int trumpPlayer[2]) {   
    
    int t = gd->team - 1;
    int p = gd->player - 1;      
    int count = 0;
    int deckIndex = firstDealMaxCards * players + 1;
    
    for (int i = firstDealMaxCards, d = deckIndex; i < playersCards; ) {
          
        for(int j = p; j < teamsPlayers; j++) {              
         
            for (int k = t; k < amountTeams; k++, count++, d++) {   
           
                if (d >= totalCards ) {
                    return;
                }
           
                if (count >= players) {
                    count = 0;
                    i++;                   
                } 

                if (!(trumpPlayer[0] == k && trumpPlayer[1] == j && i == playersCards - 1)) { 
               
                    teams[k].playerHand[j][i] = gd->deck[d];
                    initCard(&gd->deck[d]);                    
                }
                else { 
                    d--;
                }                         
            } 
            t = 0;
        }  
        p = 0;             
    }    
}

int chooseCard(int player) {


    return 1;
}

void addCardToTrick(card c, card trick[trickCards], int trickIndex) {

    trick[trickIndex] = c;
}

void playCard(gameDirector* gd, card playerHand[playersCards], int trickIndex, int pHIndex) {
    
    addCardToTrick(playerHand[pHIndex], gd->trick, trickIndex);
    removeCardFromPlayerHand(playerHand, pHIndex);
}

void play(gameDirector* gd, team teams[amountTeams]) {

    int choosenCardIndex = 0;
    int player = 1;
    for (int i = 0, k = 0; i < teamsPlayers; i++) {

        for (int j = 0; j < amountTeams; j++, k++, player++) {  

            choosenCardIndex = chooseCard(player);
            playCard(gd, teams[j].playerHand[i], k, choosenCardIndex);
            nextPlayer(gd);
        }
    }   
}

int initWindow(SDL_Window* window, SDL_Renderer* renderer, int width, int height) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) { 

        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Belotte",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN);
                           
    if (window == NULL) {
      
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (renderer == NULL) {      

        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }    
    return 0;
}

int SDL_main(int argc, char *argv[]) {

    printf("hello world!\n\n");

    const int windowWidth = 500;
    const int windowHeight = 500;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    if (initWindow(window, renderer, windowWidth, windowHeight)) {
        return 1;
    }

    team teams[amountTeams]; 
    initTeams(teams);
    
    gameDirector gd;
    initGameDirector(&gd);
  
    shuffleCards(gd.deck); 
    
    gd.team = 2;
    gd.player = 1;

    {
        printf("team: %d, player: %d\n\n", gd.team, gd.player);    
    }
    
    {   
        for (int i = 0; i < totalCards; i++) {
            printf("*%d \"%d:%d\" ",i,  gd.deck[i].color, gd.deck[i].number);
        }         
        printf("\n\nplayer1: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[0][i].color, teams[0].playerHand[0][i].number);
        }        
        printf("\nplayer2: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[0][i].color, teams[1].playerHand[0][i].number);
        }        
        printf("\nplayer3: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[1][i].color, teams[0].playerHand[1][i].number);
        }        
        printf("\nplayer4: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[1][i].color, teams[1].playerHand[1][i].number);
        }  
    }  

    {
        printf("\n\nnow firstDeal\n\n"); 
    }
    
    firstDeal(&gd, teams);  

    {   
        for (int i = 0; i < totalCards; i++) {
            printf("*%d \"%d:%d\" ",i,  gd.deck[i].color, gd.deck[i].number);
        }         
        printf("\n\nplayer1: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[0][i].color, teams[0].playerHand[0][i].number);
        }        
        printf("\nplayer2: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[0][i].color, teams[1].playerHand[0][i].number);
        }        
        printf("\nplayer3: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[1][i].color, teams[0].playerHand[1][i].number);
        }        
        printf("\nplayer4: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[1][i].color, teams[1].playerHand[1][i].number);
        }  
    } 

    int trumpPlayer[2] = {0,0};       
    chooseTrump(&gd, teams, trumpPlayer); 
    
    {
        printf("\nAtout: %d, %d", gd.trump.color, gd.trump.number);
    }
    {
        printf("\n\nnow sortPlayerHand\n\n"); 
    }  

    secondDeal(&gd, teams, trumpPlayer);

    {   
        for (int i = 0; i < totalCards; i++) {
            printf("*%d \"%d:%d\" ",i,  gd.deck[i].color, gd.deck[i].number);
        }         
        printf("\n\nplayer1: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[0][i].color, teams[0].playerHand[0][i].number);
        }        
        printf("\nplayer2: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[0][i].color, teams[1].playerHand[0][i].number);
        }        
        printf("\nplayer3: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[1][i].color, teams[0].playerHand[1][i].number);
        }        
        printf("\nplayer4: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[1][i].color, teams[1].playerHand[1][i].number);
        }  
    } 
    {
        printf("\n\nnow sortALLHands\n\n"); 
    }

    sortAllPlayersHands(gd, teams);                //// test

    {   
        for (int i = 0; i < totalCards; i++) {
            printf("*%d \"%d:%d\" ",i,  gd.deck[i].color, gd.deck[i].number);
        }         
        printf("\n\nplayer1: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[0][i].color, teams[0].playerHand[0][i].number);
        }        
        printf("\nplayer2: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[0][i].color, teams[1].playerHand[0][i].number);
        }        
        printf("\nplayer3: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[1][i].color, teams[0].playerHand[1][i].number);
        }        
        printf("\nplayer4: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[1][i].color, teams[1].playerHand[1][i].number);
        }  
    }  
    {   
        printf("\n\ntrick:\n");
        for (int i = 0; i < trickCards; i++) {
            printf("i:%d, \"%d:%d\" - ", i, gd.trick->color, gd.trick->number);
        }
    }
    {
        printf("\n\nnow sortALLHands\n\n"); 
    }

    play(&gd, teams);

    {   
        for (int i = 0; i < totalCards; i++) {
            printf("*%d \"%d:%d\" ",i,  gd.deck[i].color, gd.deck[i].number);
        }         
        printf("\n\nplayer1: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[0][i].color, teams[0].playerHand[0][i].number);
        }        
        printf("\nplayer2: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[0][i].color, teams[1].playerHand[0][i].number);
        }        
        printf("\nplayer3: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[0].playerHand[1][i].color, teams[0].playerHand[1][i].number);
        }        
        printf("\nplayer4: ");
        for (int i = 0; i < playersCards; i++) {        
            printf("\"%d:%d\" - ", teams[1].playerHand[1][i].color, teams[1].playerHand[1][i].number);
        }  
    }  
    {   
        printf("\n\ntrick:\n");
        for (int i = 0; i < trickCards; i++) {
            printf("i:%d, \"%d:%d\" - ", i, gd.trick->color, gd.trick->number);
        }
    }
     
    SDL_Event event;
    int quit = 0;    
    while (!quit) {

        SDL_Delay(16);
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        SDL_RenderPresent(renderer);
    }
       
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    getchar();
    return 0;
}