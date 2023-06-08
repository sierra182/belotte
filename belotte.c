#include <stdio.h>
#include <SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <math.h>

#define amountTeams 2
#define teamsPlayers 2
#define players 4
#define totalCards 32
#define playersCards 8 
#define tricks 8
#define trickCards 4
#define firstDealMaxCards 5
#define firstDealMaxCards1 3
#define firstDealMaxCards2 2
#define startNumberColorSequence 7
#define endNumberColorSequence 14
#define amountColor 4

#define modelOrder 7, 8, 9, 11, 12, 13, 10, 14
#define modelOrderValues 0, 0, 0, 2, 3, 4, 10, 11

#define trumpModelOrder 7, 8, 12, 13, 10, 14, 9, 11
#define trumpModelOrderValues 0, 0, 3, 4, 10, 11, 14, 20
#define trumpIndex (firstDealMaxCards * players)

typedef enum {

    SortAll,
    FirstDeal,
    TrumpChoice,
    SecondDeal,
    Play,
    CheckTrickResult,
    FinalCheckTricksResults,
    Default  
     
} gameState;

typedef struct {

    SDL_Texture* texture; 
    int number;
    int color;

} card;

typedef struct {

    SDL_Rect tricksStackDestRect;
    SDL_Rect playerHandDestRects[teamsPlayers][playersCards];

    card tricksStack[totalCards];
    card playerHand[teamsPlayers][playersCards];   
        
    int score;
    int tricksStackIndex;

} team;

typedef struct {

    SDL_Rect deckDestRect;
    SDL_Rect trickDestRects[trickCards];    

    card deck[totalCards];
    card trick[trickCards];
    card trump;  

    int team;
    int player;
    int teamEvent;
    int playerEvent;
    int choosenCardIndex;

} gameDirector;

void initCard(card* c) {

    c->texture = NULL; 
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

void initRect(SDL_Rect* rect) {

    rect->w = 0;
    rect->h = 0;
    rect->x = 0;
    rect->y = 0;
} 

void initTeams(team teams[amountTeams]) {

    for(int i = 0; i < amountTeams; i++) {

        for(int j = 0; j < teamsPlayers; j++) {

            for(int k = 0; k < playersCards; k++) {
                
                initPlayerHand(&teams[i].playerHand[j][k]);
                initRect(&teams[i].playerHandDestRects[j][k]);
            }
        }

        initRect(&teams[i].tricksStackDestRect);
        initTricksStack(teams[i].tricksStack);        
        teams[i].score = 0;
        teams[i].tricksStackIndex = 0;               
    }     
}

void initTrickDestRectsToZero(gameDirector** gd) {

    for (int i = 0; i < trickCards; i++) {

        initRect(&(*gd)->trickDestRects[i]);
    }
}

void initGameDirector(gameDirector* gd) {
   
    gd->player = 0;
    gd->team = 0; 
    gd->playerEvent = 0;
    gd->teamEvent = 0;   
    gd->choosenCardIndex = 0;

    initDeck(gd->deck);  
    initCard(&gd->trump);  
    initTrick(gd->trick);
    initRect(&gd->deckDestRect);
    initTrickDestRectsToZero(&gd);  
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
    
    for (int i = 0; i < totalCards; i++) {

        card temp = deck[i];
        deck[i] = temp;       
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

void setActualPlayerWithEvent(gameDirector* gd) {

    gd->team = gd->teamEvent;
    gd->player = gd->playerEvent;
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

void modifyTrump(gameDirector* gd) {
  
    gd->trump = gd->deck[trumpIndex];
}

void chooseTrump(gameDirector* gd, team teams[amountTeams], int resultTrumpPlayer[2]) {

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

int getPlayerFromGameDirector(gameDirector gd) {

    int result = 0;
    int p = gd.player - 1;
    int t = gd.team - 1;
    
    for (int i = p; i < teamsPlayers; i++)
    {
        for (int j = t; j < amountTeams; j++)
        { 
           result++;
        }
        t = 0;
    }   
    return players - result + 1;    
}

int chooseCard(gameDirector gd) {

    int player = getPlayerFromGameDirector(gd); 
    printf("\nJoueur %d, Choisir quelle carte jouer:\n", player);

    a:
    int input;
    scanf("%d", &input);
    getchar();
    
    switch (input) {
        case 1:
            printf("Vous avez saisi 1.\n"); 
            return 0;           
        case 2:
            printf("Vous avez saisi 2.\n");       
            return 1;
        case 3:
            printf("Vous avez saisi 3.\n");      
            return 2;
        case 4:
            printf("Vous avez saisi 4.\n");        
            return 3;
        case 5:
            printf("Vous avez saisi 5.\n");        
           return 4;
        case 6:
            printf("Vous avez saisi 6.\n");      
            return 5;
        case 7:
            printf("Vous avez saisi 7.\n");      
            return 6;
        case 8:
            printf("Vous avez saisi 8.\n");        
            return 7;
      
        default:
            printf("Entier non valide.\n");
            goto a;           
    } 
}

void addCardToTrick(card c, card trick[trickCards], int trickIndex) {

    trick[trickIndex] = c;   
}

void playCard(gameDirector* gd, card playerHand[playersCards], int trickIndex, int pHIndex) {
    
    addCardToTrick(playerHand[pHIndex], gd->trick, trickIndex);  
    removeCardFromPlayerHand(playerHand, pHIndex);  
}

int play(gameDirector* gd, team teams[amountTeams], int trickIndex) {

    int isPlayerTurn = 0;
    if (gd->team == gd->teamEvent && gd->player == gd->playerEvent) {

       isPlayerTurn = 1; 
    } 
    else {
        isPlayerTurn = 0;
    }    

    if (isPlayerTurn) {  

        playCard(gd, teams[gd->team-1].playerHand[gd->player-1], trickIndex, gd->choosenCardIndex); 
        return isPlayerTurn;
    }
    return isPlayerTurn;  
}

void playConsole(gameDirector* gd, team teams[amountTeams]) {

    int choosenCardIndex = 0;    
    int trickIndex = 0;

    initTrick(gd->trick);

    for (int i = trickIndex; i < players; i++) {
       
        choosenCardIndex = chooseCard(*gd);                    
        playCard(gd, teams[gd->team-1].playerHand[gd->player-1], i, choosenCardIndex);    
        nextPlayer(gd);        
    }   
}

void copyTrickToTeam(gameDirector gd, team* t) {

    int temp = t->tricksStackIndex;
        for (int i = 0; i < trickCards; i++, temp++) {

            t->tricksStack[temp] = gd.trick[i];            
        }
    t->tricksStackIndex = temp;
}

int compareTrickCardWithWinningCard(card trickCard, card* winningCard, int model[playersCards]) {
   
    card temp[2] = {0, 0};  

    for (int i = 0, j = 0; i < playersCards; i++) {
        
        if (model[i] == winningCard->number) {          
            temp[j] = *winningCard;          
            j++;
        }
        else if (model[i] == trickCard.number) {
          
            temp[j] = trickCard;          
            j++;
        }        
    }
    
    if (winningCard->number != temp[1].number) {

        winningCard->color = temp[1].color;
        winningCard->number = temp[1].number;  
        return 1;
    }    
    return 0;
}

void testCompareTrickCardWithWinningCard() { 
    
    int model[playersCards] = {modelOrder};  
    int trumpModel[playersCards] = {trumpModelOrder};

    card deck1[playersCards];    
    card deck2[playersCards];
    for (int i = 0; i < playersCards; i++) {

        deck1[i].number = i + 7;
        deck1[i].color = 1;
        deck2[i].number = i + 7;
        deck2[i].color = 1;
    }
   
    for (int i = 0; i < playersCards; i++) {

        for (int j = 0; j < playersCards; j++) {
            
            if (i == j) {
                break;
            }
            card temp = deck2[j];
            printf("trickCard: %d:%d, winningCard: %d:%d, model\n", deck1[i].color, deck1[i].number, deck2[j].color, deck2[j].number);
            int result = compareTrickCardWithWinningCard(deck1[i], &deck2[j], trumpModel);
            printf("trickCard: %d:%d, winningCard: %d:%d, model, result:%d \n\n", deck1[i].color, deck1[i].number, deck2[j].color, deck2[j].number, result); 
            deck2[j] = temp;
        }
    }
}

int calculateScore(gameDirector gd) {

    int score = 0;

    int trumpModel[playersCards] = {trumpModelOrder};
    int trumpModelValues[playersCards] = {trumpModelOrderValues};

    int Model[playersCards] = {modelOrder};
    int ModelValues[playersCards] = {modelOrderValues};

    if (gd.trump.color == gd.trick[0].color) {

        for (int i = 0; i < trickCards; i++) {

            for (int j = 0; j < playersCards; j++) {

                if (gd.trick[i].number == trumpModel[j]) {

                    score += trumpModelValues[j];
                }
            }
             
        }
    }
    else {

        for (int i = 0; i < trickCards; i++) {

            for (int j = 0; j < playersCards; j++) {

                if (gd.trick[i].number == Model[j]) {

                    score += ModelValues[j];
                }
            }             
        }
    }
    return score;
}

void checkTrickResult(gameDirector* gd, team teams[amountTeams]) {

    int model[playersCards] = {modelOrder};
    int trumpModel[playersCards] = {trumpModelOrder}; 

    card winningCard = gd->trick[0];  
    int winningTeam = gd->team;
    int winningPlayer = gd->player;
    
    int trickIndex = 1;
    int isReplaced = 0;

    nextPlayer(gd);  

    if (gd->trick[0].color == gd->trump.color) {

        for (int i = 1; i < trickCards; i++) {            
 
            if (gd->trick[i].color == winningCard.color) {            
                isReplaced = compareTrickCardWithWinningCard(gd->trick[i], &winningCard, trumpModel);
                if (isReplaced) {

                    winningTeam = gd->team;
                    winningPlayer = gd->player;
                }
            }
            nextPlayer(gd);
        }    
    }    
    else {

        for (int i = 1; i < trickCards; i++) {           

            if (gd->trick[i].color != gd->trump.color) {

                if (winningCard.color == gd->trick[i].color) {
                   
                    isReplaced = compareTrickCardWithWinningCard(gd->trick[i], &winningCard, model);                    
                    if (isReplaced) {

                        winningTeam = gd->team;
                        winningPlayer = gd->player;
                    }
                }
            }
            else { 

                if (winningCard.color == gd->trump.color) {
                    
                    isReplaced = compareTrickCardWithWinningCard(gd->trick[i], &winningCard, trumpModel);
                    
                    if (isReplaced) {

                        winningTeam = gd->team;
                        winningPlayer = gd->player;
                    }    
                }
                else {

                    winningTeam = gd->team;
                    winningPlayer = gd->player;
                    winningCard = gd->trick[i];
                }                
            }
            nextPlayer(gd);
        }
    }    
   
    teams[winningTeam - 1].score += calculateScore(*gd);     
    copyTrickToTeam(*gd, &teams[winningTeam - 1]);  

    gd->team = winningTeam;
    gd->player = winningPlayer;    
} 

int initWindow(SDL_Window** window, SDL_Renderer** renderer, int width, int height) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) { 

        printf("Erreur d'initialisation de SDL: %s\n", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Belotte",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              width,
                              height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                           
    if (*window == NULL) {
      
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (*renderer == NULL) {      

        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return 1;
    }    
    return 0;
}

void getPositionFromCenter(int centerX, int centerY, int width, int height, double* x, double* y) {

    *x = centerX - (width/2.0);
    *y = centerY - (height/2.0);  
}

void initPlaymatTexture(SDL_Texture** playmatTexture, SDL_Renderer** renderer) {

    SDL_Surface* playmatSurface = IMG_Load("C:\\belotte\\playmat.png");    
    *playmatTexture = SDL_CreateTextureFromSurface(*renderer, playmatSurface);
    SDL_FreeSurface(playmatSurface);
}

void initPlaymatDestRect(SDL_Rect* playmatDestRect, int windowWidth, int windowHeight) {

    playmatDestRect->w = .7 * windowHeight;
    playmatDestRect->h = .7 * windowHeight;
    playmatDestRect->x = windowWidth/2 - playmatDestRect->w/2;
    playmatDestRect->y = windowHeight/2 - playmatDestRect->h/2;
}

void initCardTextures(SDL_Texture* textureCard[amountColor][playersCards], SDL_Renderer** renderer) {
    
    char filePath[50]; 
    for (int i = 1; i <= amountColor; i++) {
        
        for (int j = 1; j <= playersCards; j++) {

            sprintf(filePath, "C:\\belotte\\cards\\%dcard%d.png", i, j + 6);
            SDL_Surface* surfaceCard = IMG_Load(filePath);         
            textureCard[i - 1][j - 1] = SDL_CreateTextureFromSurface(*renderer, surfaceCard); 
            SDL_FreeSurface(surfaceCard);           
        }
    }
}

void addCardTexturesToCards(SDL_Texture* cardTextures[amountColor][playersCards], gameDirector* gd) {

    for (int i = 0, k = 0; i < amountColor; i++) {

        for (int j = 0; j < playersCards; j++, k++) {
            
            gd->deck[k].texture = cardTextures[i][j];                       
        }
    }
}

void initDeckDestRect(SDL_Rect* cardDestRect, int windowWidth, int windowHeight) {

    double x, y;
    int width, height;
    int primaryCenterX, primaryCenterY;

    width = .08 * windowHeight;
    height = 3 * width / 2;
    primaryCenterX = windowWidth / 2;
    primaryCenterY = windowHeight / 2;
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  

    cardDestRect->w = width;
    cardDestRect->h = height; 
    cardDestRect->x = x;
    cardDestRect->y = y;    
}

void initDeckDestRects(SDL_Rect deckDestRects[amountColor][playersCards], int windowWidth, int windowHeight) {
    
    for (int i = 0; i < amountColor; i++){  

        for (int j = 0; j < playersCards; j++){

            initDeckDestRect(&deckDestRects[i][j], windowWidth, windowHeight);
        }
    }     
}

void addDeckDestRectsToDeck(SDL_Rect deckDestRects[amountColor][playersCards], gameDirector* gd) {

    for (int i = 0; i < amountColor; i++) {

        for (int j = 0; j < playersCards; j++) {
            
            gd->deckDestRect = deckDestRects[i][j];            
        }
    } 
}

void initTrickDestRect(SDL_Rect* trickDestRect, int windowWidth, int windowHeight, double* offset) {

    double x, y;
    int primaryCenterX, primaryCenterY;
    int width = .08 * windowHeight;
    int height = 3 * width / 2;
    int widthTrickContainer = .7 * windowHeight /2;
    double cardSpace = widthTrickContainer / 4.0 / 2.0;
    primaryCenterY = windowHeight / 2;
    primaryCenterX = (windowWidth / 2) - (widthTrickContainer / 2) + cardSpace;
    
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);

    trickDestRect->w = width;
    trickDestRect->h = height;
    trickDestRect->x = x + *offset;
    trickDestRect->y = y;
    *offset += 2 * cardSpace;
}

void initTrickDestRects(SDL_Rect trickDestRects[trickCards], int windowWidth, int windowHeight) {

    double offset = 0.0;
    for (int i = 0; i < trickCards ; i++) {

        initTrickDestRect(&trickDestRects[i], windowWidth, windowHeight, &offset);
    }
}

void addTrickDestRectsToGameDirector(SDL_Rect trickDestRects[trickCards], gameDirector* gd) {

    for(int i = 0; i < trickCards; i++) { 

        gd->trickDestRects[i] = trickDestRects[i];
    }
}

void initTeam1TricksStackDestRects(SDL_Rect* tricksStackDestRect, int windowWidth, int windowHeight) {

    double x, y;
    int primaryCenterX, primaryCenterY;

    int width = .08 * windowHeight;
    int height = 3 * width / 2;

    primaryCenterY = .3 * windowHeight;
    primaryCenterX = windowWidth / 2;
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  

    tricksStackDestRect->w = width;
    tricksStackDestRect->h = height;
    tricksStackDestRect->x = x;
    tricksStackDestRect->y = y;
}

void initTeam2TricksStackDestRects(SDL_Rect* tricksStackDestRect, int windowWidth, int windowHeight) {

    double x, y;
    int primaryCenterX, primaryCenterY;

    int width = .08 * windowHeight;
    int height = 3 * width / 2;

    primaryCenterY = .7 * windowHeight;
    primaryCenterX = windowWidth / 2;
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  

    tricksStackDestRect->w = width;
    tricksStackDestRect->h = height;
    tricksStackDestRect->x = x;
    tricksStackDestRect->y = y;
}

void initTeamsTricksStacksDestRects(SDL_Rect tricksStacksDestRects[amountTeams][totalCards], int windowWidth, int windowHeight) {

    for (int i = 0; i < totalCards; i++) {
       
        initTeam1TricksStackDestRects(&tricksStacksDestRects[0][i], windowWidth, windowHeight);
        initTeam2TricksStackDestRects(&tricksStacksDestRects[1][i], windowWidth, windowHeight);
    }
}

void addTricksStacksDestRectsToTeams(SDL_Rect tricksStacksDestRects[amountTeams][totalCards], team teams[amountTeams]) {
    
    for(int i = 0, l = 0; i < amountTeams; i++) {

        for(int j = 0; j < totalCards; j++) {           
             
            teams[i].tricksStackDestRect = tricksStacksDestRects[i][j];   
        }
    }  
}

void initPlayerHand1DestRect(SDL_Rect* destRectCard, int windowWidth, int windowHeight, double* offset) {
    
    double x, y;
    int primaryCenterX, primaryCenterY; 
    
    double cardSpace = (.7 * windowHeight) / 8.0 / 2.0;
    double matStart = ((windowWidth / 2.0) - ((.7 * windowHeight) / 2.0));
    primaryCenterX =(int) matStart + cardSpace;

    int width = .08 * windowHeight;
    int height = 3 * width / 2; 
    primaryCenterY = (windowHeight / 2) + ((windowHeight *.7) / 2) + (height / 2) + (.01 * windowHeight);         
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  

    destRectCard->w = width;
    destRectCard->h = height;
    destRectCard->x = x + (int) *offset;
    destRectCard->y = y;
    *offset += 2 * cardSpace;
}

void initPlayerHand2DestRect(SDL_Rect* destRectCard, int windowWidth, int windowHeight, double* offset, SDL_Point* rotationCenter) {
    
    double x, y;
    int primaryCenterX, primaryCenterY;    

    int width = .08 * windowHeight;
    int height = 3 * width / 2;

    primaryCenterX = (windowWidth / 2) - ((windowHeight *.7) / 2) - (height / 2) - (.01 * windowHeight);
    double cardSpace = (.7 * windowHeight) / 8.0 / 2.0;
    double matStart = ((windowHeight / 2.0) - ((.7 * windowHeight) / 2.0));
   
    primaryCenterY =(int) matStart + cardSpace;
    int centerX = width / 2;
    int centerY = height / 2; 
    rotationCenter->x = centerX ;
    rotationCenter->y = centerY;     
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  
 
    destRectCard->w = width;
    destRectCard->h = height;
    destRectCard->x = x;
    destRectCard->y = y + *offset;
    *offset += 2 * cardSpace;
}

void initPlayerHand3DestRect(SDL_Rect* destRectCard, int windowWidth, int windowHeight, double* offset) {
    
    double x, y;
    int primaryCenterX, primaryCenterY;    

    double cardSpace = (.7 * windowHeight) / 8.0 / 2.0;
    double matStart = ((windowWidth / 2.0) - ((.7 * windowHeight) / 2.0));
    primaryCenterX =(int) matStart + cardSpace;

    int width = .08 * windowHeight;
    int height = 3 * width / 2;
    primaryCenterY = (windowHeight / 2) - ((windowHeight *.7) / 2) - (height / 2) - (.01 * windowHeight);           
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  

    destRectCard->w = width;
    destRectCard->h = height;
    destRectCard->x = x + (int) *offset;
    destRectCard->y = y;
    *offset += 2 * cardSpace;
}

void initPlayerHand4DestRect(SDL_Rect* destRectCard, int windowWidth, int windowHeight, double* offset, SDL_Point* rotationCenter) {
    
    double x, y;
    int primaryCenterX, primaryCenterY; 
 
    int width = .08 * windowHeight;
    int height = 3 * width / 2;

    double cardSpace = (.7 * windowHeight) / 8.0 / 2.0;
    double matStart = ((windowHeight / 2.0) - ((.7 * windowHeight) / 2.0));
   
    primaryCenterY =(int) matStart + cardSpace;
    primaryCenterX = (windowWidth / 2) + ((windowHeight *.7) / 2) + (height / 2) + (.01 * windowHeight);

    int centerX = width / 2;
    int centerY = height / 2; 
    rotationCenter->x = centerX ;
    rotationCenter->y = centerY;     
    getPositionFromCenter(primaryCenterX, primaryCenterY, width, height, &x, &y);  
 
    destRectCard->w = width;
    destRectCard->h = height;
    destRectCard->x = x;
    destRectCard->y = y + *offset;
    *offset += 2 * cardSpace;
}

void initPlayerHandsDestRects(team teams[amountTeams], SDL_Rect destRectsPlayerHand[players][playersCards], int windowWidth, int windowHeight, SDL_Point rotationCenters[teamsPlayers][playersCards]) {

    double offset[players] = {0, 0, 0, 0};   
       
    for (int j = 0; j < playersCards; j++) {

        if(teams[0].playerHand[0][j].number != 0) {
      
            initPlayerHand1DestRect(&destRectsPlayerHand[0][j], windowWidth, windowHeight, &offset[0]);
        }
        if(teams[1].playerHand[0][j].number != 0) {
     
            initPlayerHand2DestRect(&destRectsPlayerHand[1][j], windowWidth, windowHeight, &offset[1], &rotationCenters[0][j]);
        } 
        if(teams[0].playerHand[1][j].number != 0) {
     
            initPlayerHand3DestRect(&destRectsPlayerHand[2][j], windowWidth, windowHeight, &offset[2]);
        } 
        if(teams[1].playerHand[1][j].number != 0) {
        
            initPlayerHand4DestRect(&destRectsPlayerHand[3][j], windowWidth, windowHeight, &offset[3], &rotationCenters[1][j]);
        }           
    }           
}

void addPlayerHandsDestRectsToTeams(SDL_Rect destRectsPlayersHands[players][playersCards], team teams[amountTeams]) {
    
    for(int i = 0, l = 0; i < amountTeams; i++) {

        for(int j = 0; j < teamsPlayers; j++, l++) {

            for(int k = 0; k < playersCards; k++) {
             
                teams[i].playerHandDestRects[j][k] = destRectsPlayersHands[l][k];                
            }
        }
    }  
}

void initCrossTexture(SDL_Texture** crossTexture, SDL_Renderer** renderer) {

    SDL_Surface* crossSurface = IMG_Load("C:\\belotte\\cross.png");    
    *crossTexture = SDL_CreateTextureFromSurface(*renderer, crossSurface);
    SDL_FreeSurface(crossSurface);;
}

void initCircleTexture(SDL_Texture** circleTexture, SDL_Renderer** renderer) {

    SDL_Surface* circleSurface = IMG_Load("C:\\belotte\\circle.png");    
    *circleTexture = SDL_CreateTextureFromSurface(*renderer, circleSurface);
    SDL_FreeSurface(circleSurface);;
}

void  initCrossAndCircleDestRects(SDL_Rect crossAndCircleDestRects[2], double x, double y, int containerWidth, int containerHeight, int elementSize) {

    crossAndCircleDestRects[0].x = x;
    crossAndCircleDestRects[0].y = y;
    crossAndCircleDestRects[0].w = elementSize;
    crossAndCircleDestRects[0].h = elementSize;

    if (containerWidth < containerHeight) {
        
        crossAndCircleDestRects[1].x = x;
        crossAndCircleDestRects[1].y = y + elementSize;
    } 
    else {

        crossAndCircleDestRects[1].x = x + elementSize;
        crossAndCircleDestRects[1].y = y; 
    }     
    crossAndCircleDestRects[1].w = elementSize;
    crossAndCircleDestRects[1].h = elementSize;
}

void initCrossAndCircleContainerDestRect(SDL_Rect crossAndCircleDestRects[2], int positionMultiplicator, int positionInverter, int windowWidth, int windowHeight) {

    double x, y;
    int primaryCenterX, primaryCenterY;
    int containerWidth, containerHeight;
    int globalCenterX, globalCenterY;
      
    int elementSize = .05 * windowHeight;    
    containerWidth = containerHeight = elementSize; 

    globalCenterX = windowWidth / 2;
    globalCenterY = windowHeight / 2;

    int mat = .7 * windowHeight;  
    int positionCalcul = mat / 5;
    int signedPositionCalcul;

    if (positionInverter == 1) {
        
        signedPositionCalcul = globalCenterY + positionCalcul * positionMultiplicator;
        primaryCenterX = globalCenterX;
        primaryCenterY = signedPositionCalcul;
        containerWidth *= 2; 
    } 
    else if (positionInverter == 0) {

        signedPositionCalcul = globalCenterX + positionCalcul * positionMultiplicator;
        primaryCenterY = globalCenterY;        
        primaryCenterX = signedPositionCalcul;      
        containerHeight *= 2;  
    }
    getPositionFromCenter(primaryCenterX, primaryCenterY, containerWidth, containerHeight, &x, &y);

    initCrossAndCircleDestRects(crossAndCircleDestRects, x, y, containerWidth, containerHeight, elementSize);  
}

void initCrossAndCircleContainerDestRects(SDL_Rect crossAndCircleDestRects[players][2],  int windowWidth, int windowHeight) {

    int crossAndCircleDestRectsIndex = 0, positionMultiplicator = 0, positionInverter = 0;
    
    for (int i = positionMultiplicator, k = crossAndCircleDestRectsIndex; i < 2 ; i ++) {

        for (int j = positionInverter; j < 2 ; j++, k++) {

            if (i == 0)
            {
                positionMultiplicator = -1;
            }
            else {
                positionMultiplicator = 1;
            }
            initCrossAndCircleContainerDestRect(crossAndCircleDestRects[k], positionMultiplicator, j, windowWidth, windowHeight);
        }          
    }           
    
}

void deckRenderCopy(SDL_Renderer* renderer, gameDirector* gd) {

    for (int i = 0; i < totalCards; i++) {   

        SDL_RenderCopy(renderer, gd->deck[i].texture, NULL,  &gd->deckDestRect);                   
    }
}

void playerHandsRenderCopy(SDL_Renderer* renderer, team teams[amountTeams], double angle, SDL_Point rotationCenters[teamsPlayers][playersCards]) {
        
    for(int h = 0; h < amountTeams; h++) {

        for (int i = 0; i < teamsPlayers; i++) {
            
            for (int j = 0; j < playersCards; j++) {
               
                if (teams[h].playerHand[i][j].color != 0) {
                    
                    if (i == 1) {

                        SDL_RenderCopyEx(renderer, teams[h].playerHand[i][j].texture, NULL,  &teams[h].playerHandDestRects[i][j], angle, &rotationCenters[i][j], SDL_FLIP_NONE); 
                    }
                    else {                   
                        SDL_RenderCopy(renderer, teams[h].playerHand[i][j].texture, NULL,  &teams[h].playerHandDestRects[i][j]);   
                    }
                }
            }
        }
    } 
}

void trickRenderCopy(SDL_Renderer* renderer, gameDirector* gd) {

    for(int i = 0; i < trickCards; i++) { 

        if(gd->trick[i].color != 0) {

            SDL_RenderCopy(renderer, gd->trick[i].texture, NULL, &gd->trickDestRects[i]);
        }               
    }
} 

void tricksStacksRenderCopy(SDL_Renderer* renderer, team teams[amountTeams]) {

    for(int i = 0; i < amountTeams; i++) { 

        for(int j = 0; j < totalCards; j++) { 

            if (teams[i].tricksStack[j].color != 0) {

                SDL_RenderCopy(renderer, teams[i].tricksStack[j].texture, NULL, &teams[i].tricksStackDestRect);
            }               
        }
    }
}

Uint32 myCallbackFirst(Uint32 interval, void* param) {
    
    SDL_TimerID* timer_id = (SDL_TimerID*)param;
    
    //SDL_Init(SDL_INIT_TIMER);    
    //SDL_TimerID myTimer = SDL_AddTimer(5000, myCallback, &myTimer);

    SDL_RemoveTimer(*timer_id);
    return 0;  
}

int SDL_main(int argc, char *argv[]) {

   // printf("hello world!\n\n"); 

    int windowWidth = 600;
    int windowHeight = 600;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    
    if (initWindow(&window, &renderer, windowWidth, windowHeight)) {
        return 1;
    }
  
    team teams[amountTeams]; 
    initTeams(teams);

    gameDirector gd;
    initGameDirector(&gd);
    gd.team = 1;
    gd.player = 1;

    gameState gs = FirstDeal;
    gameState gsNextState = Default; 

    shuffleCards(gd.deck);

    {
    //shuffleCards(gd.deck); 
    /*
    //

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
        printf("\n\nnow secondDeal\n\n"); 
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

    sortAllPlayersHands(gd, teams);                

    for (int i = 0; i < totalCards / trickCards; i++) {
    
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
                printf("i:%d, \"%d:%d\" - ", i, gd.trick[i].color, gd.trick[i].number);
            }
        }
        {
            printf("\n\nnow play\n\n"); 
        }
        {
            printf("team: %d, player: %d\n\n", gd.team, gd.player);    
        }
        
        play(&gd, teams);

        {
            printf("A :team: %d, player: %d\n\n", gd.team, gd.player);    
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
            printf("\n\ntrick:\n");
            for (int i = 0; i < trickCards; i++) {
                printf("i:%d, \"%d:%d\" - ", i, gd.trick[i].color, gd.trick[i].number);
            }
        }

        checkTrickResult(&gd, teams);

        {
            printf("B team: %d, player: %d\n\n", gd.team, gd.player);    
        }
        {
            printf("\n\naftercheck");
        }
        {   
            printf("\n\ntricksStack team 0:\n");
            for (int i = 0; i < totalCards; i++) {
                printf("i:%d,%d \"%d:%d\" - ", i, teams[0].tricksStackIndex, teams[0].tricksStack[i].color, teams[0].tricksStack[i].number);
            }
            printf("team 0 score:%d\n", teams[0].score);
        }
        {   
        printf("\n\ntricksStack team 1:\n");
            for (int i = 0; i < totalCards; i++) {
                printf("i:%d,%d \"%d:%d\" - ", i, teams[1].tricksStackIndex, teams[1].tricksStack[i].color, teams[1].tricksStack[i].number);
        }
            printf("team 1 score:%d\n", teams[1].score);
        }        
    }
    */
    }

    SDL_Texture* playmatTexture;
    initPlaymatTexture(&playmatTexture, &renderer); 
    SDL_Rect playmatDestRect;          
    initPlaymatDestRect(&playmatDestRect, windowWidth, windowHeight);
       
    SDL_Texture* cardTextures[amountColor][playersCards];
    initCardTextures(cardTextures, &renderer);
    addCardTexturesToCards(cardTextures, &gd);      
   
    SDL_Rect deckDestRects[amountColor][playersCards];    
    initDeckDestRects(deckDestRects, windowWidth, windowHeight);
    addDeckDestRectsToDeck(deckDestRects, &gd);
    
    SDL_Rect trickDestRects[trickCards]; 
    initTrickDestRects(trickDestRects, windowWidth, windowHeight);
    addTrickDestRectsToGameDirector(trickDestRects, &gd); 

    SDL_Rect tricksStacksDestRects[amountTeams][totalCards]; 
    initTeamsTricksStacksDestRects(tricksStacksDestRects, windowWidth, windowHeight); 
    addTricksStacksDestRectsToTeams(tricksStacksDestRects, teams); 

    SDL_Texture* crossTexture;
    initCrossTexture(&crossTexture, &renderer); 
    SDL_Texture* circleTexture;
    initCircleTexture(&circleTexture, &renderer);

    SDL_Rect crossAndCircleDestRects[players][2];          
    initCrossAndCircleContainerDestRects(crossAndCircleDestRects, windowWidth, windowHeight);

    double angle = 90.0;
    SDL_Point rotationCenters[teamsPlayers][playersCards];
    SDL_Rect destRectsPlayersHands[players][playersCards];

    SDL_RenderCopy(renderer, playmatTexture, NULL, &playmatDestRect);   
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
   
    int countCheckTrickResult = 0;
    int countPlay = 0;
    int trickIndex = 0;

    SDL_Event event;
    int quit = 0;    
    while (!quit) {

        SDL_Delay(16);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);     

        while (SDL_PollEvent(&event) != 0) {

            if (event.type == SDL_QUIT) {
                quit = 1;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
           
                int newWidth = windowWidth = event.window.data1;
                int newHeight = windowHeight = event.window.data2;

                initPlaymatDestRect(&playmatDestRect, newWidth, newHeight); 

                initDeckDestRects(deckDestRects, newWidth, newHeight);
                addDeckDestRectsToDeck(deckDestRects, &gd);

                initPlayerHandsDestRects(teams, destRectsPlayersHands, newWidth, newHeight, rotationCenters);                  
                addPlayerHandsDestRectsToTeams(destRectsPlayersHands, teams); 

                initTrickDestRects(trickDestRects, newWidth, newHeight);
                addTrickDestRectsToGameDirector(trickDestRects, &gd);

                initTeamsTricksStacksDestRects(tricksStacksDestRects, newWidth, newHeight); 
                addTricksStacksDestRectsToTeams(tricksStacksDestRects, teams); 

                initCrossAndCircleContainerDestRects(crossAndCircleDestRects, newWidth, newHeight);
            }

            else if (event.type == SDL_MOUSEBUTTONDOWN) {

                int x, y;
                SDL_GetMouseState(&x, &y);  

                /*if (SDL_PointInRect(&((SDL_Point){x, y}),&crossDestRect) ) {  //verif si ne bug pas lors de lacces a un destrect inexistant
                                
                    if (gsNextState == Play) {

                                    gd.teamEvent = h;
                                    gd.playerEvent = i;
                                    gd.choosenCardIndex = j;
                                    gs = Play; 
                                }                               
                            }
                }*/

                for (int h = 0; h < amountTeams; h++) {

                    for (int i = 0; i < teamsPlayers; i++) {

                        for (int j = 0; j < playersCards; j++) {

                            if (SDL_PointInRect(&((SDL_Point){x, y}), &teams[h].playerHandDestRects[i][j]) ) {  //verif si ne bug pas lors de lacces a un destrect inexistant
                                
                                if (gsNextState == Play) {

                                    gd.teamEvent = h;
                                    gd.playerEvent = i;
                                    gd.choosenCardIndex = j;
                                    gs = Play; 
                                }                               
                            }
                        }
                    }   
                }
            }
        } 
        
        switch (gs) {

            case SortAll:
             
                sortAllPlayersHands(gd, teams);

                gs = Default; 
                break;

            case FirstDeal:

                firstDeal(&gd, teams);  

                gs = SortAll;
                gsNextState = TrumpChoice;
                break;
            
            case TrumpChoice:
               
                // dans event gd.team player = ?
                modifyTrump(&gd);
                setActualPlayerWithEvent(&gd);
                addCardToPlayerHand(teams[gd.team].playerHand[gd.player], playersCards - 1,  gd.deck, trumpIndex);     
                
                gs = SecondDeal;
                break;    

            case SecondDeal:

                int trumpPlayer[2] = {gd.team,gd.player};
                secondDeal(&gd, teams, trumpPlayer);

                gs = SortAll;
                gsNextState = Play;  
                break;

            case Play:

                // dans event gd.choosenCardIndex, gd.teamEvent gd.playerEvent = ?               
                int isPlayerTurn = play(&gd, teams, trickIndex);               
                if (isPlayerTurn) { 

                    countPlay++;
                    trickIndex++;
                    nextPlayer(&gd);
                }
                
                if (countPlay >= players) {

                    countPlay = 0;
                    gs = CheckTrickResult;
                }                
                break;

            case CheckTrickResult:

                checkTrickResult(&gd, teams);
                countCheckTrickResult++;
                initTrick(gd.trick);

                gs = Play;
                if (countCheckTrickResult >= tricks) {
                    countCheckTrickResult = 0;
                   gs = FinalCheckTricksResults; 
                }                
                break;

            case FinalCheckTricksResults:
                break;

            default:          
                break;
        }
         
        initPlayerHandsDestRects(teams, destRectsPlayersHands, windowWidth, windowHeight, rotationCenters);                  
        addPlayerHandsDestRectsToTeams(destRectsPlayersHands, teams);  
        SDL_RenderCopy(renderer, playmatTexture, NULL, &playmatDestRect);        
        deckRenderCopy(renderer, &gd);
        playerHandsRenderCopy(renderer, teams, angle, rotationCenters); 
        trickRenderCopy(renderer, &gd);
        tricksStacksRenderCopy(renderer, teams);  
        tricksStacksRenderCopy(renderer, teams); 
        
        for (int i = 0; i < players; i++) {

            SDL_RenderCopy(renderer, crossTexture, NULL, &crossAndCircleDestRects[i][0]);
            SDL_RenderCopy(renderer, circleTexture, NULL, &crossAndCircleDestRects[i][1]);
        }
        

        SDL_RenderPresent(renderer);   
    }
   
    for (int i = 0; i <totalCards; i++) {
        SDL_DestroyTexture(gd.deck[i].texture);
    }
    SDL_DestroyTexture(crossTexture);   
    SDL_DestroyTexture(circleTexture);  
    SDL_DestroyTexture(playmatTexture);   
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window); 
    SDL_Quit();       
    return 0;
}