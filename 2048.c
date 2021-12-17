#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define HEIGHT 635
#define WIDTH 500

typedef struct chronometre chronometre;
struct chronometre{
    int active, minutes, secondes;
};

int startChrono(chronometre * chrono){
    // Lancement du chronomètre.
    chrono->active = 1;
    return 0;
}

int stopChrono(chronometre * chrono){
    // Arrêt du chronomètre.
    chrono->active = 0;
    return 0;
}

int resetChrono(chronometre * chrono){
    // Réinitalisation du chronomètre.
    chrono->active = 0;
    chrono->minutes = 0;
    chrono->secondes = 0;
    return 0;
}

int addSecondChrono(chronometre * chrono){
    // Ajout d'une seconde au chronomètre.
    if(chrono->active){
        chrono->secondes += 1;
        if(chrono->secondes == 60){
            chrono->secondes = 0;
            chrono->minutes += 1;
        }
    }
    return 0;
}

int saveGame(char * saveName, int ** gameBoard, int n, int * score, chronometre * chrono){
    // Cette fonction permet de sauvegarder les données de la partie actuelle dans un fichier texte. La partie pourra ensuite être chargée par la fonction loadGame().

    strcat(saveName, ".txt"); // On ajoute l'extension .txt au nom de la sauvegarde.

    // Création du fichier texte de sauvegarde. Si celui-ci existe déjà on le remplace.
    FILE * file = fopen(saveName, "w+");
    if(file==NULL){ return -1; }

    // Ecriture du score actuel à la première ligne du fichier texte.
    if(fprintf(file, "%i\n", *score) == -1){
        // Si erreur, suppression du fichier et renvoie -1.
        remove(saveName);
        return -1;
    }

    // Ecriture des minutes du temps actuel du chronomètre à la deuxième ligne du fichier texte.
    if(fprintf(file, "%i\n", chrono->minutes) == -1){
        // Si erreur, suppression du fichier et renvoie -1.
        remove(saveName);
        return -1;
    }

    // Ecriture des secondes du temps actuel du chronomètre à la troisième ligne du fichier texte.
    if(fprintf(file, "%i\n", chrono->secondes) == -1){
        // Si erreur, suppression du fichier et renvoie -1.
        remove(saveName);
        return -1;
    }

    // On parcourt le gameBoard de gauche à droite et de haut en bas et on copie la valeur de chaque case une par une en lignes dans le fichier. (une ligne = une case)
    int x, y;
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(fprintf(file, "%i\n", gameBoard[x][y]) == -1){
                remove(saveName);
                return -1;
            }
        }
    }

    fclose(file);

    return 0;
}

int loadGame(char * saveName, int ** gameBoard, int n, int * score, chronometre * chrono){
    // Cette fonction permet de charger une partie à partir d'un fichier de sauvegarde créé à l'aide de la fonction saveGame().

    // Ouverture du fichier de sauvegarde.
    FILE * file = fopen(saveName, "r");
    if(file==NULL){
        // S'il n'existe pas on renvoie le code d'erreur 10.
        return 10;
    }

    char c; // Création de la variable c qui contiendra le caractère lu.
    char buffer[10]; // Création d'un buffer qui contiendra la ligne lue.
    int i, j, line, x , y;
    line = i = x = y = 0;
    
    // Récupération des données dans le fichier texte et mise à jour des données de la partie.
    while(fread(&c, sizeof(char), 1, file) != 0){
        if(c == '\n'){
            if(line == 0){
                *score = atoi(buffer);
            }else if(line == 1){
                chrono->minutes = atoi(buffer);
            }else if(line == 2){
                chrono->secondes = atoi(buffer);
            }else{
                gameBoard[x%n][y%n] = atoi(buffer);
                y++;
                if((y%n)==0){
                    x++;
                }
            }

            line++;
            i=0;
            for(j=0; j<10; j++){
                buffer[j] = '\0';
            }
        }else{
            buffer[i] = c;
            i++;
        }
    }

    return 0;
}

int ** create2DTab(int x, int y){
    // Cette fonction créé un tableau à 2 dimensions de x lignes et y colonnes et initialise les cases à 0;

    int ** tab = malloc(sizeof(int *)*x);
    if(tab==NULL){return NULL;}
    int i, j;
    for(i=0; i<x; i++){
        tab[i] = malloc(sizeof(int)*y);
        if(tab[i]==NULL){
            int f;
            for(f=0; f<i; f++){
                free(tab[f]);
            }
            free(tab);
            return NULL;
        }
        for(j=0; j<y; j++){
            tab[i][j] = 0;
        }
    }
    return tab;
}

int free2DTab(int ** tab, int n){
    // Cette fonction libère l'espace alloué pour un tableau à 2 dimensions de n lignes.

    int i;
    if(tab != NULL){
        for(i=0; i<n; i++){
            if(tab[i] != NULL){
                free(tab[i]);
            }
        }
        free(tab);
    }
    return 0;
}

int displayGameBoard(int ** gameBoard, int n){
    // Cette fonction affiche le gameBoard dans le terminal.

    int x, y;
    printf("_____________________\n");

    for(x=0; x<n; x++){
        printf("|");
        for(y=0; y<n; y++){
            if(gameBoard[x][y]!=0){
                if(gameBoard[x][y]<10){
                    printf(" %d  |", gameBoard[x][y]);
                }else if(gameBoard[x][y]<100){
                    printf(" %d |", gameBoard[x][y]);
                }else if(gameBoard[x][y]<1000){
                    printf("%d |", gameBoard[x][y]);
                }else{
                    printf("%d|", gameBoard[x][y]);
                }
            }else{
                printf("    |");
            }

            if(y==3){
                printf("\n|____|____|____|____|");
            }
        }
        printf("\n");
    }

    return 0;
}

int countFreeBoxGameBoard(int ** gameBoard, int n){
    // Cette fonction compte le nombre de cases vides restantes sur le gameBoard et renvoie la réponse.

    int x, y, countFreeBox;
    countFreeBox = 0;
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(gameBoard[x][y]==0){
                countFreeBox++;
            }
        }
    }

    return countFreeBox;
}

int fillInRandomBox(int ** gameBoard, int n, int choice){
    // Cette fonction affecte à une case vide aléatoire du gameBoard soit 2 soit 4 (ou seulement 2 selon le paramètre "choice" donné).

    int countFreeBox = countFreeBoxGameBoard(gameBoard, n);
    if(countFreeBox==0){ return 10; } // Si le plateau de jeu est complet, il retourne le code 10.

    // Le tableau coordTab contiendra les coordonnées x et y de toutes les cases vides du gameBoard.
    int ** coordTab = create2DTab(countFreeBox, 2);
    if(coordTab == NULL){ return -1; }

    // On rempli ce tableau en vérifiant pour chaque case du gameBoard si elle est vide. Si oui, les coordonnées sont ajoutées à "coordTab".
    int i, x, y;
    i=0;
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(gameBoard[x][y]==0){
                coordTab[i][0] = x;
                coordTab[i][1] = y;
                i++;
            }
        }
    }

    // On choisi une case au hasard dans le tableau "coordTab".
    srand(time(NULL));
    int randomFreeBox = rand()%countFreeBox;
    x = coordTab[randomFreeBox][0];
    y = coordTab[randomFreeBox][1];

    /*
    Si "choice" vaut 2, ont affecte à la case choisie la valeur 2.
    Si "choice" vaut 4, ont affecte à la case choisie soit 2 soit 4.
    */
    if(choice == 2){
        gameBoard[x][y] = 2;
    }else if(choice == 4){
        srand(time(NULL));
        gameBoard[x][y] = ((rand()%2)+1)*2;
    }else{
        free2DTab(coordTab, countFreeBox);
        return -1;
    }

    free2DTab(coordTab, countFreeBox);
    return 0;
}

int rightMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers la droite en effectuant la fusion des cases adjacentes horizontalement de même valeur.

    // On traite le gameBoard ligne par ligne.
    int x, y, t, stop;
    for(y=0; y<n; y++){
        /*
        On affecte à t la valeur n-1. La variable t désignera la case vide la plus à droite de la ligne.
        On affecte à stop la valeur n. Cette variable nous servira à ne pas fusionner 2 fois une même case dans un même mouvement. La valeur n signifie qu'aucune case n'a été fusionnée.
        On traite la ligne de la dernière case à la première.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case vide la plus à droite de la ligne:
                    On déplace la case traitée à la place de la case vide la plus à droite de la ligne.
                Si la case que l'on vient de déplacer n'est pas la dernière case de la ligne:
                    Si la case à droite de la case déplacée est de même valeur et qu'elle n'a pas déjà été fusionné lors de ce mouvement:
                        On multiplie la valeur de la case à droite par 2
                        On mets la valeur de la case déplacée à 0
                        On incrémente le score
                        On vérifie si la partie est gagnée
                        On affecte à stop la valeur t+1 qui signifie que cette case a été fusionnée et ne doit plus l'être
                    Sinon:
                        On soustrait 1 à t pour modifier les coordonnées de la case vide la plus à droite de la ligne.
                Sinon:
                    On soustrait 1 à t pour modifier les coordonnées de la case vide la plus à droite de la ligne.
        */
        t = n-1;
        stop = n;
        for(x=n-1; x>=0; x--){
            if(gameBoard[y][x] != 0){
                if(x!=t){
                    gameBoard[y][t] = gameBoard[y][x];
                    gameBoard[y][x] = 0;
                }
                if(t!=(n-1)){
                    if(gameBoard[y][t] == gameBoard[y][t+1] && (t+1)<stop){
                        gameBoard[y][t+1] *= 2;
                        gameBoard[y][t] = 0;
                        *score += gameBoard[y][t+1]; // On ajoute au score la valeur calculée pour la case traitée.
                        // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                        if(gameBoard[y][t+1]==2048){
                            *win = 1;
                        }
                        stop = t+1;
                    }else{
                        t--;
                    }
                }else{
                    t--;
                }
            }
        }
    }

    return 0;
}

int leftMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers la gauche en effectuant la fusion des cases adjacentes horizontalement de même valeur.

    // On traite le gameBoard ligne par ligne.
    int x, y, t, stop;
    for(y=0; y<n; y++){
        /*
        On affecte à t la valeur 0. La variable t désignera la case vide la plus à gauche de la ligne.
        On affecte à stop la valeur -1. Cette variable nous servira à ne pas fusionner 2 fois une même case dans un même mouvement. La valeur -1 signifie qu'aucune case n'a été fusionnée.
        On traite la ligne de la première case à la dernière.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case vide la plus à gauche de la ligne:
                    On déplace la case traitée à la place de la case vide la plus à gauche de la ligne.
                Si la case que l'on vient de déplacer n'est pas la première case de la ligne:
                    Si la case à droite de la case déplacée est de même valeur et qu'elle n'a pas déjà été fusionné lors de ce mouvement:
                        On multiplie la valeur de la case à gauche par 2
                        On mets la valeur de la case déplacée à 0
                        On incrémente le score
                        On vérifie si la partie est gagnée
                        On affecte à stop la valeur t-1 qui signifie que cette case a été fusionnée et ne doit plus l'être
                    Sinon:
                        On ajoute 1 à t pour modifier les coordonnées de la case vide la plus à gauche de la ligne.
                Sinon:
                    On ajoute 1 à t pour modifier les coordonnées de la case vide la plus à gauche de la ligne.
        */
        t = 0;
        stop = -1;
        for(x=0; x<n; x++){
            if(gameBoard[y][x] != 0){
                if(x!=t){
                    gameBoard[y][t] = gameBoard[y][x];
                    gameBoard[y][x] = 0;
                }
                if(t!=0){
                    if(gameBoard[y][t] == gameBoard[y][t-1] && (t-1)>stop){
                        gameBoard[y][t-1] *= 2;
                        gameBoard[y][t] = 0;
                        *score += gameBoard[y][t-1]; // On ajoute au score la valeur calculée pour la case traitée.
                        // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                        if(gameBoard[y][t-1]==2048){
                            *win = 1;
                        }
                        stop = t-1;
                    }else{
                        t++;
                    }
                }else{
                    t++;
                }
            }
        }
    }

    return 0;
}

int upMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers le haut en effectuant la fusion des cases adjacentes verticalements de même valeur.

    // On traite le gameBoard colonne par colonne.
    int x, y, t, stop;
    for(x=0; x<n; x++){
        /*
        On affecte à t la valeur 0. La variable t désignera la case vide la plus haute de la colonne.
        On affecte à stop la valeur -1. Cette variable nous servira à ne pas fusionner 2 fois une même case dans un même mouvement. La valeur -1 signifie qu'aucune case n'a été fusionnée.
        On traite la colonne de la première case à la dernière.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case vide la plus haute de la colonne:
                    On déplace la case traitée à la place de la case vide la plus haute de la colonne.
                Si la case que l'on vient de déplacer n'est pas la première case de la colonne:
                    Si la case au dessus de la case déplacée est de même valeur et qu'elle n'a pas déjà été fusionné lors de ce mouvement:
                        On multiplie la valeur de la case au dessus par 2
                        On mets la valeur de la case déplacée à 0
                        On incrémente le score
                        On vérifie si la partie est gagnée
                        On affecte à stop la valeur t-1 qui signifie que cette case a été fusionnée et ne doit plus l'être
                    Sinon:
                        On ajoute 1 à t pour modifier les coordonnées de la case vide la plus haute de la colonne.
                Sinon:
                    On ajoute 1 à t pour modifier les coordonnées de la case vide la plus haute de la colonne.
        */
        t = 0;
        stop = -1;
        for(y=0; y<n; y++){
            if(gameBoard[y][x] != 0){
                if(y!=t){
                    gameBoard[t][x] = gameBoard[y][x];
                    gameBoard[y][x] = 0;
                }
                if(t!=0){
                    if(gameBoard[t][x] == gameBoard[t-1][x] && (t-1)>stop){
                        gameBoard[t-1][x] *= 2;
                        gameBoard[t][x] = 0;
                        *score += gameBoard[t-1][x]; // On ajoute au score la valeur calculée pour la case traitée.
                        // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                        if(gameBoard[t-1][y]==2048){
                            *win = 1;
                        }
                        stop = t-1;
                    }else{
                        t++;
                    }
                }else{
                    t++;
                }
            }
        }
    }

    return 0;
}

int downMove(int ** gameBoard, int n, int * score, int * win){
    // Cette fonction déplace toutes les cases du gameBoard vers le haut en effectuant la fusion des cases adjacentes verticalements de même valeur.

    // On traite le gameBoard colonne par colonne.
    int x, y, t, stop;
    for(x=0; x<n; x++){
        /*
        On affecte à t la valeur n-1. La variable t désignera la case vide la plus haute de la colonne.
        On affecte à stop la valeur n. Cette variable nous servira à ne pas fusionner 2 fois une même case dans un même mouvement. La valeur n signifie qu'aucune case n'a été fusionnée.
        On traite la colonne de la dernière case à la première.
            Si la case traitée possède une valeur non nulle:
                Si la case traitée n'est pas la case vide la plus basse de la colonne:
                    On déplace la case traitée à la place de la case vide la plus basse de la colonne.
                Si la case que l'on vient de déplacer n'est pas la dernière case de la colonne:
                    Si la case du dessous de la case déplacée est de même valeur et qu'elle n'a pas déjà été fusionné lors de ce mouvement:
                        On multiplie la valeur de la case du dessous par 2
                        On mets la valeur de la case déplacée à 0
                        On incrémente le score
                        On vérifie si la partie est gagnée
                        On affecte à stop la valeur t+1 qui signifie que cette case a été fusionnée et ne doit plus l'être
                    Sinon:
                        On soustrait 1 à t pour modifier les coordonnées de la case vide la plus basse de la colonne.
                Sinon:
                    On soustrait 1 à t pour modifier les coordonnées de la case vide la plus basse de la colonne.
        */
        t = n-1;
        stop = n;
        for(y=n-1; y>=0; y--){
            if(gameBoard[y][x] != 0){
                if(y!=t){
                    gameBoard[t][x] = gameBoard[y][x];
                    gameBoard[y][x] = 0;
                }
                if(t!=(n-1)){
                    if(gameBoard[t][x] == gameBoard[t+1][x] && (t+1)<stop){
                        gameBoard[t+1][x] *= 2;
                        gameBoard[t][x] = 0;
                        *score += gameBoard[t+1][x]; // On ajoute au score la valeur calculée pour la case traitée.
                        // On teste si la nouvelle case vaut 2048. Si oui, la variable win est passée à 1.
                        if(gameBoard[t+1][y]==2048){
                            *win = 1;
                        }
                        stop = t+1;
                    }else{
                        t--;
                    }
                }else{
                    t--;
                }
            }
        }
    }

    return 0;
}

int continuationOfTheGame(int ** gameBoard, int n){
    // Cette fonction teste si le joueur peut encore jouer.

    if(countFreeBoxGameBoard(gameBoard, n) > 0){
        // S'il reste des cases vides, le joueur peut encore jouer.
        return 1;
    }else{
        int x, y;
        for(x=0; x<n; x++){
            for(y=0; y<n; y++){
                // Test d'égalité verticale.
                if(x!=(n-1)){
                    if(gameBoard[x][y] == gameBoard[x+1][y]){
                        return 1;
                    }
                }
                // Test d'égalité horizontale
                if(y!=(n-1)){
                    if(gameBoard[x][y] == gameBoard[x][y+1]){
                        return 1;
                    }
                }
            }
        }
    }

    return 0; // On retourne 0 si aucune case adjacente n'est identique : le joueur ne peux plus jouer.
}

int displaySDLWinWindow(SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Cette fonction affiche le pop up de victoire de la partie.

    // Ajout d'une fenêtre translucide par dessus le gameBoard
    SDL_Surface * winWindowBackground = NULL;
    winWindowBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 450,450, 32,0,0,0,0);
    SDL_SetAlpha(winWindowBackground,SDL_SRCALPHA, 120); // Définition de la valeur alpha de la surface. La valeur alpha gère la transparence de la surface.
    if(winWindowBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(winWindowBackground,NULL,SDL_MapRGB(winWindowBackground->format,255,255,0));
    SDL_Rect posWinWindowBackground;
    posWinWindowBackground.x=25;
    posWinWindowBackground.y=100;
    SDL_BlitSurface(winWindowBackground,NULL,ecran,&posWinWindowBackground);
    SDL_FreeSurface(winWindowBackground);

    // Initialisation de la couleur des textes
    SDL_Color colorTextWinWindow= {255, 255, 255};

    // Ajout du texte "Partie gagnée!"
    SDL_Surface * winWindowText1 = TTF_RenderText_Blended(arialBold35, "Partie gagnee!", colorTextWinWindow);
    if(winWindowText1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posWinWindowText1;
    posWinWindowText1.x=131;
    posWinWindowText1.y=280;
    SDL_BlitSurface(winWindowText1,NULL,ecran,&posWinWindowText1);
    SDL_FreeSurface(winWindowText1);

    // Ajout du texte "Appuyez sur Q pour quitter le jeu"
    SDL_Surface * winWindowText2 = TTF_RenderText_Blended(arial20, "Appuyez sur Q pour quitter le jeu", colorTextWinWindow);
    if(winWindowText2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posWinWindowText2;
    posWinWindowText2.x=100;
    posWinWindowText2.y=340;
    SDL_BlitSurface(winWindowText2,NULL,ecran,&posWinWindowText2);
    SDL_FreeSurface(winWindowText2);

    SDL_Flip(ecran);
    return 0;
}

int displaySDLGameOverWindow(SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Cette fonction affiche le pop up de defaite de la partie.

    // Ajout d'une fenêtre translucide par dessus le gameBoard
    SDL_Surface * gameOverWindowBackground = NULL;
    gameOverWindowBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 450,450, 32,0,0,0,0);
    SDL_SetAlpha(gameOverWindowBackground,SDL_SRCALPHA, 120); // Définition de la valeur alpha de la surface. La valeur alpha gère la transparence de la surface.
    if(gameOverWindowBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(gameOverWindowBackground,NULL,SDL_MapRGB(gameOverWindowBackground->format,255,0,0));
    SDL_Rect posGameOverWindowBackground;
    posGameOverWindowBackground.x=25;
    posGameOverWindowBackground.y=100;
    SDL_BlitSurface(gameOverWindowBackground,NULL,ecran,&posGameOverWindowBackground);
    SDL_FreeSurface(gameOverWindowBackground);

    // Initialisation de la couleur des textes
    SDL_Color colorTextGameOverWindow= {255, 255, 255};

    // Ajout du texte "Partie perdue!"
    SDL_Surface * gameOverWindowText1 = TTF_RenderText_Blended(arialBold35, "Partie perdue!", colorTextGameOverWindow);
    if(gameOverWindowText1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posGameOverWindowText1;
    posGameOverWindowText1.x=131;
    posGameOverWindowText1.y=280;
    SDL_BlitSurface(gameOverWindowText1,NULL,ecran,&posGameOverWindowText1);
    SDL_FreeSurface(gameOverWindowText1);

    // Ajout du texte "Appuyez sur Q pour quitter le jeu"
    SDL_Surface * gameOverWindowText2 = TTF_RenderText_Blended(arial20, "Appuyez sur Q pour quitter le jeu", colorTextGameOverWindow);
    if(gameOverWindowText2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posGameOverWindowText2;
    posGameOverWindowText2.x=100;
    posGameOverWindowText2.y=340;
    SDL_BlitSurface(gameOverWindowText2,NULL,ecran,&posGameOverWindowText2);
    SDL_FreeSurface(gameOverWindowText2);

    SDL_Flip(ecran);
    return 0;
}

int winGame(int * play, int score, SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Cette fonction déclenche les évènements de victoire de la partie.

    *play = 0;
    printf("Félicitation! Vous avez gagné la partie!\n");
    printf("Vous avez obtenu un score de %i points.\n", score);

    if(displaySDLWinWindow(ecran, arial20, arialBold35)==-1){
        return -1;
    }

    // Si l'utilisateur appuis sur "q", on quitte le jeu.
    int cont = 1;
    SDL_Event event;
    while(cont){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case 'q':
                        cont = 0;
                        break;
                    default:
                        cont = 1;
                }
                break;
            case SDL_QUIT:
                cont = 0;
                break;
            default:
                cont = 1;
        }
    }

    return 0;
}

int gameOver(int * play, int score, SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Cette fonction déclenche les évènements de defaite de la partie.

    *play = 0;
    printf("Game over! Le jeu n'est plus possible.\n");
    printf("Vous avez obtenu un score de %i points.\n", score);

    if(displaySDLGameOverWindow(ecran, arial20, arialBold35)==-1){
        return -1;
    }

    // Si l'utilisateur appuis sur "q", on quitte le jeu.
    int cont = 1;
    SDL_Event event;
    while(cont){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case 'q':
                        cont = 0;
                        break;
                    default:
                        cont = 1;
                }
                break;
            case SDL_QUIT:
                cont = 0;
                break;
            default:
                cont = 1;
        }
    }

    return 0;
}

int testWinOrGameOver(int win, int * play, int score, int ** gameBoard, int n, SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Cette fonction teste si la partie est gagnée ou perdu ou aucun des deux. 

    /*
    Si "win" vaut 1 alors la partie est gagnée.
    Sinon et que le jeu n'est plus possible alors on appelle la fonction GameOver.
    */
    if(win){
        if(winGame(play, score, ecran, arial20, arialBold35)==-1){
            return -1;
        }
    }else if(!(continuationOfTheGame(gameBoard, n))){
        if(gameOver(play, score, ecran, arial20, arialBold35)==-1){
            return -1;
        }
    }

    return 0;
}

int initSDLScreenGame(SDL_Surface * ecran, TTF_Font * arial12, TTF_Font * arial14){
    // Cette fonction initialise la fenêtre SDL avec les éléments necessaires à l'affichage du jeu.

    // Suppression de tout les éléments de la fenêtre
    SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 247,217,166));

    // Ajout du fond du score
    SDL_Surface * scoreBackground = NULL;
    scoreBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(scoreBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(scoreBackground,NULL,SDL_MapRGB(scoreBackground->format,105,91,69));
    SDL_Rect posScoreBackground;
    posScoreBackground.x=25;
    posScoreBackground.y=25;
    SDL_BlitSurface(scoreBackground,NULL,ecran,&posScoreBackground);
    SDL_FreeSurface(scoreBackground);

    // Ajout du fond du chronomètre
    SDL_Surface * chronoBackground = NULL;
    chronoBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(chronoBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(chronoBackground,NULL,SDL_MapRGB(chronoBackground->format,105,91,69));
    SDL_Rect posChronoBackground;
    posChronoBackground.x=350;
    posChronoBackground.y=25;
    SDL_BlitSurface(chronoBackground,NULL,ecran,&posChronoBackground);
    SDL_FreeSurface(chronoBackground);

    // Ajout du fond du GameBoard
    SDL_Surface * gameBoardBackground = NULL;
    gameBoardBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 450,450, 32,0,0,0,0);
    if(gameBoardBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(gameBoardBackground,NULL,SDL_MapRGB(gameBoardBackground->format,105,91,69));
    SDL_Rect posGameBoardBackground;
    posGameBoardBackground.x=25;
    posGameBoardBackground.y=100;
    SDL_BlitSurface(gameBoardBackground,NULL,ecran,&posGameBoardBackground);
    SDL_FreeSurface(gameBoardBackground);

    // Ajout des indications de commande
    // Initialisation de la couleur des textes d'indications
    SDL_Color colorTextIndications= {50,50,50};

    // Affichage du texte "Utilisez les flèches directionnelles pour déplacer les cases."
    SDL_Surface * textIndications1 = TTF_RenderText_Blended(arial14, "Utilisez les fleches directionnelles pour deplacer les cases.", colorTextIndications);
    if(textIndications1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextIndications1;
    posTextIndications1.x=35;
    posTextIndications1.y=565;
    SDL_BlitSurface(textIndications1,NULL,ecran,&posTextIndications1);
    SDL_FreeSurface(textIndications1);

    // Affichage du texte "Appuyez sur S pour sauvegarder votre partie ou Q pour quitter."
    SDL_Surface * textIndications2 = TTF_RenderText_Blended(arial14, "Appuyez sur S pour sauvegarder votre partie ou Q pour quitter.", colorTextIndications);
    if(textIndications2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextIndications2;
    posTextIndications2.x=35;
    posTextIndications2.y=590;
    SDL_BlitSurface(textIndications2,NULL,ecran,&posTextIndications2);
    SDL_FreeSurface(textIndications2);

    // Affichage du texte "Pour la sauvagarde, saisissez le nom de la sauvegarde dans le terminal."
    SDL_Surface * textIndications3 = TTF_RenderText_Blended(arial12, "Pour la sauvagarde, saisissez le nom de la sauvegarde dans le terminal.", colorTextIndications);
    if(textIndications3==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextIndications3;
    posTextIndications3.x=45;
    posTextIndications3.y=610;
    SDL_BlitSurface(textIndications3,NULL,ecran,&posTextIndications3);
    SDL_FreeSurface(textIndications3);

    SDL_Flip(ecran);
    return 0;
}

int refreshSDLScreenGame(SDL_Surface * ecran, int ** gameBoard, int n, int score, TTF_Font * police){
    // Cette fonction met à jours l'affichage du score et l'affichage du plateau de jeu sur la fenêtre SDL.

    // Mise à jour du score
    // Suppression de l'ancien score
    SDL_Surface * scoreBackground = NULL;
    scoreBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(scoreBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(scoreBackground,NULL,SDL_MapRGB(scoreBackground->format,105,91,69));
    SDL_Rect posScoreBackground;
    posScoreBackground.x=25;
    posScoreBackground.y=25;
    SDL_BlitSurface(scoreBackground,NULL,ecran,&posScoreBackground);
    SDL_FreeSurface(scoreBackground);
    
    // Affichage du score actuel
    SDL_Color colorTextScore= {255,255,255};
    char string[6];
    sprintf(string, "%d", score);
    SDL_Surface * textScore = TTF_RenderText_Blended(police, string, colorTextScore);
    if(textScore==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextScore;
    posTextScore.x=35;
    posTextScore.y=40;
    SDL_BlitSurface(textScore,NULL,ecran,&posTextScore);
    SDL_FreeSurface(textScore);


    // Mise à jour du gameBoard
    int caseX, caseY, x, y;
    for(caseY=0; caseY<n; caseY++){
        for(caseX=0; caseX<n; caseX++){
            if(caseX==0 && caseY==0){
                // Première case
                x = 35;
                y = 110;
            }else if(caseX==0){
                // Nouvelle ligne
                x=35;
                y+=110;
            }else{
                // Nouvelle case de la même ligne
                x+=110;
            }

            //Ajout d'une case selon les coordonnées calculées
            SDL_Surface * caseGameBoard = NULL;
            caseGameBoard = SDL_CreateRGBSurface(SDL_HWSURFACE, 100,100, 32,0,0,0,0);
            if(caseGameBoard == NULL){
                fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
                return -1;
            }
            SDL_Rect posCase;
            posCase.x=x;
            posCase.y=y;

            // Initialisation de la couleur des textes des cases
            SDL_Color colorTextCase = {255,255,255};

            // On assigne une couleur à la case selon sa valeur.
            if(gameBoard[caseY][caseX]==0){
                // Case vide
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 166, 151, 128));
            }else if(gameBoard[caseY][caseX]==2){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 238, 228, 218));
                // Modification de la couleur du texte pour qu'il soit lisible
                colorTextCase.r = 75;
                colorTextCase.g = 75;
                colorTextCase.b = 75;
            }else if(gameBoard[caseY][caseX]==4){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 236, 224, 199));
                // Modification de la couleur du texte pour qu'il soit lisible
                colorTextCase.r = 75;
                colorTextCase.g = 75;
                colorTextCase.b = 75;
            }else if(gameBoard[caseY][caseX]==8){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 242, 177, 120));
            }else if(gameBoard[caseY][caseX]==16){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 245, 149, 99));
            }else if(gameBoard[caseY][caseX]==32){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 245, 124, 96));
            }else if(gameBoard[caseY][caseX]==64){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 228, 107, 77));
            }else if(gameBoard[caseY][caseX]==128){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 234, 204, 121));
            }else if(gameBoard[caseY][caseX]==256){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 236, 202, 97));
            }else if(gameBoard[caseY][caseX]==512){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 233, 197, 91));
            }else if(gameBoard[caseY][caseX]==1024){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 236, 198, 3));
            }else if(gameBoard[caseY][caseX]==2048){
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 231, 191, 67));
            }
            SDL_BlitSurface(caseGameBoard,NULL,ecran,&posCase);
            SDL_FreeSurface(caseGameBoard);

            // Ajout du nombre dans la case si elle n'est pas vide.
            if(gameBoard[caseY][caseX]!=0){
                char string[6];
                sprintf(string, "%d", gameBoard[caseY][caseX]);
                SDL_Surface * textCase = TTF_RenderText_Blended(police, string, colorTextCase);
                if(textCase==NULL){
                    fprintf(stderr,"erreur: %s",TTF_GetError());
                    return -1;
                }
                // Calcul de la position du nombre selon sa longueur.
                SDL_Rect posTextCase;
                if(gameBoard[caseY][caseX]<10){
                    posTextCase.x=x+45;
                }else if(gameBoard[caseY][caseX]<100){
                    posTextCase.x=x+38;
                }else if(gameBoard[caseY][caseX]<1000){
                    posTextCase.x=x+32;
                }else if(gameBoard[caseY][caseX]<10000){
                    posTextCase.x=x+27;
                }else{
                    posTextCase.x=x+23;
                }
                posTextCase.y=y+40;
                SDL_BlitSurface(textCase,NULL,ecran,&posTextCase);
                SDL_FreeSurface(textCase);
            }
        }
    }

    SDL_Flip(ecran);
    return 0;
}

int refreshSDLChrono(SDL_Surface * ecran, chronometre * chrono, TTF_Font * police){
    // Cette fonction met à jours l'affichage du chronomètre sur la fenêtre SDL.

    // Suppression du chronomètre affiché précédemment
    SDL_Surface * chronoBackground = NULL;
    chronoBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(chronoBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(chronoBackground,NULL,SDL_MapRGB(chronoBackground->format,105,91,69));
    SDL_Rect posChronoBackground;
    posChronoBackground.x=350;
    posChronoBackground.y=25;
    SDL_BlitSurface(chronoBackground,NULL,ecran,&posChronoBackground);
    SDL_FreeSurface(chronoBackground);

    // Affichage du score actuel
    SDL_Color colorTextChrono= {255,255,255};
    char strSecondes[3];
    char strTime[10];
    if(chrono->secondes<10){
        sprintf(strSecondes, "0%d", chrono->secondes);
    }else{
        sprintf(strSecondes, "%d", chrono->secondes);
    }
    sprintf(strTime, "%d", chrono->minutes);
    strcat(strTime, ":");
    strcat(strTime, strSecondes);
    SDL_Surface * textChrono = TTF_RenderText_Blended(police, strTime, colorTextChrono);
    if(textChrono==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextChrono;
    posTextChrono.x=360;
    posTextChrono.y=40;
    SDL_BlitSurface(textChrono,NULL,ecran,&posTextChrono);
    SDL_FreeSurface(textChrono);

    SDL_Flip(ecran);
    return 0;
}

int displaySDLQuitPopUp(SDL_Surface * ecran, TTF_Font * police){
    // Cette fonction affiche la pop up pour quitter la partie.

    // Ajout du fond de la fenêtre
    SDL_Surface * quitPopUpBackground = NULL;
    quitPopUpBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 375,150, 32,0,0,0,0);
    if(quitPopUpBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(quitPopUpBackground,NULL,SDL_MapRGB(quitPopUpBackground->format,66,58,44));
    SDL_Rect posQuitPopUpBackground;
    posQuitPopUpBackground.x=62;
    posQuitPopUpBackground.y=250;
    SDL_BlitSurface(quitPopUpBackground,NULL,ecran,&posQuitPopUpBackground);
    SDL_FreeSurface(quitPopUpBackground);

    // Initialisation de la couleur des textes de la pop up
    SDL_Color colorTextQuitPopUp = {255, 255, 255};

    // Ajout du texte "Voulez-vous vraiment quitter?"
    SDL_Surface * textQuitPopUp1 = TTF_RenderText_Blended(police, "Voulez-vous vraiment quitter?", colorTextQuitPopUp);
    if(textQuitPopUp1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextQuitPopUp1;
    posTextQuitPopUp1.x=117;
    posTextQuitPopUp1.y=280;
    SDL_BlitSurface(textQuitPopUp1,NULL,ecran,&posTextQuitPopUp1);
    SDL_FreeSurface(textQuitPopUp1);

    // Ajout du texte "Appuyez sur Y pour oui ou N pour non."
    SDL_Surface * textQuitPopUp2 = TTF_RenderText_Blended(police, "Appuyez sur Y pour oui ou N pour non.", colorTextQuitPopUp);
    if(textQuitPopUp2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect posTextQuitPopUp2;
    posTextQuitPopUp2.x=75;
    posTextQuitPopUp2.y=330;
    SDL_BlitSurface(textQuitPopUp2,NULL,ecran,&posTextQuitPopUp2);
    SDL_FreeSurface(textQuitPopUp2);


    SDL_Flip(ecran);
    return 0;
}

int quitGame(int * play, SDL_Surface * ecran, TTF_Font * arial20){
    // Cette fonction déclenche les évènement permettant de quitter la partie.

    if(displaySDLQuitPopUp(ecran, arial20)){
        return -1;
    }
    printf("Voulez-vous veaiment quitter la partie? Y/N\n");
    
    // Si l'utilisateur appuis sur "Y" on quitte le jeu,
    // S'il appuis sur "N" on enlève la pop up pour quitter le jeu et la partie reprend.
    int cont = 1;
    SDL_Event event;
    while(cont){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case 'n':
                        cont = 0;
                        return 1;
                        break;
                    case 'y':
                        cont = 0;
                        *play = 0;
                        break;
                    default:
                        cont = 1;
                }
                break;
            case SDL_QUIT:
                cont = 0;
                *play = 0;
                break;
            default:
                cont = 1;
        }
    }

    return 0;
}

int startGame(int ** gameBoard, int n, int * play, int * score, SDL_Surface * ecran, TTF_Font * arial12, TTF_Font * arial14, TTF_Font * arial20, TTF_Font * arialBold35, chronometre * chrono){
    // Cette fonction lance la partie puis gère les évènements et appelle les fonctions necessaires au déroulement du jeu.

    int win = 0; // On initialise la variable win à 0. Elle sera passée à 1 si le jeu est gagné.

    if(initSDLScreenGame(ecran, arial12, arial14)==-1){
        return -1;
    }

    displayGameBoard(gameBoard, n);
    printf("Score: %i\n", *score);

    if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
        return -1;
    }

    // Lancement du chronomètre
    startChrono(chrono);
    time_t timeStart, timeEnd;
    timeStart = time(NULL);

    int q;
    char saveName[24] = "";
    SDL_Event event;
    while(*play){
        SDL_Delay(100);
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_KEYUP:
                    switch(event.key.keysym.sym){
                        case SDLK_UP:
                            // Mouvement vers le haut
                            upMove(gameBoard, n, score, &win);
                            if(fillInRandomBox(gameBoard, n, 4) == -1){
                                return -1;
                            }
                            displayGameBoard(gameBoard, n);
                            printf("Score: %i\n", *score);
                            if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                return -1;
                            }
                            if(testWinOrGameOver(win,play,*score,gameBoard,n,ecran,arial20,arialBold35)==-1){
                                return -1;
                            }
                            break;
                        case SDLK_DOWN:
                            // Mouvement vers le bas
                            downMove(gameBoard, n, score, &win);
                            if(fillInRandomBox(gameBoard, n, 4) == -1){
                                return -1;
                            }
                            displayGameBoard(gameBoard, n);
                            printf("Score: %i\n", *score);
                            if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                return -1;
                            }
                            if(testWinOrGameOver(win,play,*score,gameBoard,n,ecran,arial20,arialBold35)==-1){
                                return -1;
                            }
                            break;
                        case SDLK_RIGHT:
                            // Mouvement vers la droite
                            rightMove(gameBoard, n, score, &win);
                            if(fillInRandomBox(gameBoard, n, 4) == -1){
                                return -1;
                            }
                            displayGameBoard(gameBoard, n);
                            printf("Score: %i\n", *score);
                            if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                return -1;
                            }
                            if(testWinOrGameOver(win,play,*score,gameBoard,n,ecran,arial20,arialBold35)==-1){
                                return -1;
                            }
                            break;
                        case SDLK_LEFT:
                            //  Mouvement vers la gauche
                            leftMove(gameBoard, n, score, &win);
                            if(fillInRandomBox(gameBoard, n, 4) == -1){
                                return -1;
                            }
                            displayGameBoard(gameBoard, n);
                            printf("Score: %i\n", *score);
                            if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                return -1;
                            }
                            if(testWinOrGameOver(win,play,*score,gameBoard,n,ecran,arial20,arialBold35)==-1){
                                return -1;
                            }
                            break;
                        case 's':
                            stopChrono(chrono);
                            // On demande le nom que l'utilisateur veut donner à la sauvegarde et on appelle la fonction saveGame pour sauvegarder la partie.
                            printf("Nom de la sauvegarde : ");
                            scanf(" %19s", saveName);
                            if(saveGame(saveName, gameBoard, n, score, chrono) == 0){
                                printf("\nPartie sauvegardée avec succès\n");
                            }else{
                                printf("Erreur lors de la sauvagarde de la partie.\n");
                            }
                            startChrono(chrono);
                            break;
                        case 'q':
                            stopChrono(chrono);
                            if((q = quitGame(play, ecran, arial20))==1){
                                // Le joueur veut continuer la partie
                                if(initSDLScreenGame(ecran, arial12, arial14)==-1){
                                    return -1;
                                }
                                displayGameBoard(gameBoard, n);
                                printf("Score: %i\n", *score);
                                if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                    return -1;
                                }
                                startChrono(chrono);
                            }else if(q==-1){
                                return -1;
                            }
                            break;
                        default:
                            *play=1;
                    }
                    break;
                case SDL_QUIT:
                    *play=0;
                    break;
                default:
                    *play = 1;
            }
        }

        // Ajout d'une seconde au chronomètre si necessaire
        timeEnd = time(NULL);
        if(difftime(timeEnd, timeStart)>0){
            addSecondChrono(chrono);
            timeStart = time(NULL);
            refreshSDLChrono(ecran, chrono, arial20);
        }
    }

    return 0;
}

int initGameboard(int ** gameBoard, int n){
    // Cette fonction initialise le plateau de jeu avec 2 cases de valeur 2 à des coordonnées aléatoires à l'aide de la fonction fillInRandomBox().

    if(fillInRandomBox(gameBoard, n, 2) != 0){ return -1; } // Le paramètre 2 signifie ici que les cases ajoutées seront forcément de valeur 2.
    if(fillInRandomBox(gameBoard, n, 2) != 0){ return -1; }

    /*
    // Code permettant de tester la situation de victoire.
    gameBoard[0][0] = 1024;
    gameBoard[0][1] = 1024;
    */

    /*
    // Code permettant de tester la situation de défaite.
    int x, y, i;
    i=0;
    for(x=0;x<n;x++){
        for(y=0;y<n;y++){
            gameBoard[x][y] = i;
            i++;
        }
    }
    */

    return 0;
}

SDL_Surface * initSDLWindow(){
    /* Cette fonction initialise la fenêtre d'affichage SDL et renvoie un pointeur vers celle-ci. */

    // Création de la surface principale
    SDL_Surface * ecran = NULL ;
    if((ecran = SDL_SetVideoMode(WIDTH,HEIGHT,32, SDL_HWSURFACE))==NULL){
        fprintf(stderr,"Erreur VideoMode %s\n",SDL_GetError());
        return NULL;
    }

    // Légende de la fenêtre
    SDL_WM_SetCaption("2048",NULL);

    // Remplissage de la fenêtre
    SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 247,217,166));

    // Mise à jour de la fenêtre
    SDL_Flip(ecran);

    return ecran;
}

int main(int argc, char ** argv) {
    // Initialiation de la SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) !=0){
        fprintf(stderr,"\nUnable to initialize SDL:  %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Initialisation de SDL_TTF
    if(TTF_Init() == -1){
        fprintf(stderr,"\nUnable to initialize TTF: %s\n", TTF_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // Chargement des polices de texte
    TTF_Font * arial20 = NULL ;
    arial20 = TTF_OpenFont("fonts/Arial.ttf",20);
    if(arial20==NULL){
        fprintf(stderr,"\nUnable to load TTF:  %s\n",TTF_GetError());
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    TTF_Font * arialBold35 = NULL ;
    arialBold35 = TTF_OpenFont("fonts/Arial Bold.ttf",35);
    if(arialBold35==NULL){
        fprintf(stderr,"\nUnable to load TTF:  %s\n",TTF_GetError());
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    TTF_Font * arial14 = NULL ;
    arial14 = TTF_OpenFont("fonts/Arial Bold.ttf",14);
    if(arial14==NULL){
        fprintf(stderr,"\nUnable to load TTF:  %s\n",TTF_GetError());
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    TTF_Font * arial12 = NULL ;
    arial12 = TTF_OpenFont("fonts/Arial Bold.ttf",12);
    if(arial12==NULL){
        fprintf(stderr,"\nUnable to load TTF:  %s\n",TTF_GetError());
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    int n = 4; // Taille du plateau de jeu.
    //(On stock cette valeur dans une variable pour plus de clarté étant donnée qu'elle sera réutilisée souvent dans le code)

    // On initialise le plateau de jeu que l'on nomme gameBoard.
    int ** gameBoard = create2DTab(n, n);
    if(gameBoard == NULL){
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    int score = 0; // Initialisation de la variable de score.

    // Initialisation de la fenêtre du jeu.
    SDL_Surface * ecran = initSDLWindow();
    if(ecran == NULL){
        free2DTab(gameBoard, n);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE); 
    }

    // Initialisation de la couleur des textes du menu
    SDL_Color colorTextMenu = {135, 101, 0};

    // Ajout des textes du menu sur la fenêtre
    SDL_Surface * menuText1 = TTF_RenderText_Blended(arial20, "Appuyez sur N pour demarrer une nouvelle partie!", colorTextMenu);
    if(menuText1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_Rect posMenuText1;
    posMenuText1.x=27;
    posMenuText1.y=250;
    SDL_BlitSurface(menuText1,NULL,ecran,&posMenuText1);
    SDL_FreeSurface(menuText1);

    SDL_Surface * menuText2 = TTF_RenderText_Blended(arial20, "OU", colorTextMenu);
    if(menuText2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_Rect posMenuText2;
    posMenuText2.x=233;
    posMenuText2.y=302;
    SDL_BlitSurface(menuText2,NULL,ecran,&posMenuText2);
    SDL_FreeSurface(menuText2);

    SDL_Surface * menuText3 = TTF_RenderText_Blended(arial20, "Appuyez sur L pour charger une partie enregistree!", colorTextMenu);
    if(menuText3==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_Rect posMenuText3;
    posMenuText3.x=22;
    posMenuText3.y=350;
    SDL_BlitSurface(menuText3,NULL,ecran,&posMenuText3);
    SDL_FreeSurface(menuText3);

    SDL_Surface * menuText4 = TTF_RenderText_Blended(arial12, "puis saisissez le nom de la sauvegarde dans le terminal.", colorTextMenu);
    if(menuText4==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_Rect posMenuText4;
    posMenuText4.x=22;
    posMenuText4.y=377;
    SDL_BlitSurface(menuText4,NULL,ecran,&posMenuText4);
    SDL_FreeSurface(menuText4);

    // Actualisation de la fenêtre
    SDL_Flip(ecran);

    // Attente et traitement des évènements
    char choice;
    int cont = 1;
    SDL_Event event;
    while(cont){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case 'n':
                        cont = 0;
                        choice = 'n';
                        break;
                    case 'l':
                        cont = 0;
                        choice = 'l';
                        break;
                    default:
                        cont = 1;
                }
                break;
            case SDL_QUIT:
                cont = 0;
                free2DTab(gameBoard, n);
                TTF_Quit();
                SDL_Quit();
                exit(EXIT_SUCCESS);
                break;
            default:
                cont = 1;
        }
    }

    // Initialisation du chronomètre
    struct chronometre chrono;
    resetChrono(&chrono);

    if(choice == 'n'){
        // Si l'utilisateur a entré 'n', on appelle la fonction "initGameboard".
        if(initGameboard(gameBoard, n) == -1){
            printf("Erreur lors de l'initialisation du plateau de jeu.\n");
            free2DTab(gameBoard, n);
            TTF_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
    }else if(choice == 'l'){
        // Si l'utilisateur a entré 'l'...

        // On demande à l'utilisateur le nom de la sauvegarde qu'il souhaite charger.
        char saveName[24] = "";
        printf("Nom de la sauvegarde à charger : ");
        scanf(" %19s", saveName);

        strcat(saveName, ".txt"); // On ajoute l'xtension ".txt" à la fin du nom

        // On traite la valeur retournée par loadGame.
        int r;
        if((r=(loadGame(saveName, gameBoard, n, &score, &chrono))) == 10){
            // Si la valeur est 10 alors le nom saisie par l'utilisateur n'est pas correct. On libère l'espace alloué au programme et on quitte.
            printf("Aucune sauvegarde pour le nom saisi.\n");
            free2DTab(gameBoard, n);
            TTF_Quit();
            SDL_Quit();
            exit(EXIT_SUCCESS);
        }else if(r == -1){
            // Si la valeur est -1 alors une erreur s'est produite. On libère l'espace alloué au programme et on quitte.
            printf("Erreur lors du chargement de la partie.\n");
            free2DTab(gameBoard, n);
            TTF_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
    }

    // On initialise la variable "play" à 1 et on appelle startGame pour lancer la partie.
    // Si une erreur se produit pendant la partie, on libère l'espace alloué au programme et on quitte.
    int play = 1;
    if(startGame(gameBoard, n, &play, &score, ecran, arial12, arial14, arial20, arialBold35, &chrono) == -1){
        free2DTab(gameBoard, n);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // On libère l'espace alloué au programme et on quitte.
    free2DTab(gameBoard, n);
    TTF_Quit();
    SDL_Quit();
    exit(EXIT_SUCCESS);
}