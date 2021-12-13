#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define HEIGHT 635
#define WIDTH 500

typedef struct chronometre chronometre;
struct chronometre{
    int active, minutes, secondes;
};

int startChrono(chronometre * chrono){
    chrono->active = 1;
    return 0;
}

int stopChrono(chronometre * chrono){
    chrono->active = 0;
    return 0;
}

int resetChrono(chronometre * chrono){
    chrono->active = 0;
    chrono->minutes = 0;
    chrono->secondes = 0;
    return 0;
}

int addSecondChrono(chronometre * chrono){
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
    // Cette fonction permet de sauvegarder les données de la partie actuelle dans un fichier texte. La partie pourra ensuite être chargé par la fonction loadGame().

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

    fclose(file); // Fermeture du fichier texte.

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

    char * c = malloc(sizeof(char)); // Création de la variable c qui contiendra le caractère lu.
    char * buffer = malloc(10*sizeof(char)); // Création d'un buffer qui contiendra la ligne lue.
    int i, j, line, x , y;
    line = i = x = y = 0;
    /*
    La boucle traite chaque caractère du fichier un par un.
    Elle ajoute chaque caractère lu au buffer.
    Lorsque le caractère \n est rencontré:
        Si c'est la première ligne qui est traitée alors la valeur est affectée au score.
        Si c'est un autre ligne, la valeur est affectée à la case du gameBoard correspondante.
        Enfin, on reinitialise le buffer pour traiter la ligne suivante.
    */
    while(fread(c, sizeof(char), 1, file) != 0){
        if(*c == '\n'){
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
            buffer[i] = *c;
            i++;
        }
    }

    // On libère l'espace alloué par les malloc.
    free(c);
    free(buffer);

    return 0;
}

int ** create2DTab(int x, int y){
    // Cette fonction créé un tableau à 2 dimension de taille x lignes et y colonnes.

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
    // Cette fonction affiche le gameBoard du jeu.

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
    // Cette fonction compte le nombre de case vide restante sur le gameBoard et renvoie la réponse.

    int x, y, countFreeBox;
    countFreeBox = 0;
    // La boucle traite le gameBoard case par case et chaque case vide trouvée ajoute 1 à countFreeBox.
    for(x=0; x<n; x++){
        for(y=0; y<n; y++){
            if(gameBoard[x][y]==0){
                countFreeBox++;
            }
        }
    }

    // On renvoie finalement la variable countFreeBox.
    return countFreeBox;
}

int fillInRandomBox(int ** gameBoard, int n, int choice){
    // Cette fonction affecte à une case vide aléatoire du gameBoard soit 2 soit 4 (ou seulement 2 selon le paramètre "choice" donné).

    int countFreeBox = countFreeBoxGameBoard(gameBoard, n);
    if(countFreeBox==0){ return 10; } // On teste si le gameBoard est déjà complet ou non. Si il l'est on renvoie le code d'erreur 10.

    // On créé un tableau qui contiendra les coordonnées x et y de toute les cases vide du gameBoard.
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

    // On libère le tableau des coordonnées créé.
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
    // Cette fonction teste si le jeu peut continuer lorsque le plateau est plein

    // On teste si il reste des cases vides. Si oui, on retourne la valeur 1 qui signifie que le joueur peut encore jouer.
    if(countFreeBoxGameBoard(gameBoard, n) > 0){
        return 1;
    }else{
        // On teste chaque case du gameBoard une par une.
        int x, y;
        for(x=0; x<n; x++){
            for(y=0; y<n; y++){
                /*
                Pour toute les cases sauf celle de la dernière ligne on teste si la case du dessous est la même:
                    Si oui, on retourne la valeur 1 qui signifie que le joueur peut encore jouer.
                */
                if(x!=(n-1)){
                    if(gameBoard[x][y] == gameBoard[x+1][y]){
                        return 1;
                    }
                }
                /*
                Pour toute les cases sauf celle de la dernière colonne on teste si la case de droite est la même:
                    Si oui, on retourne la valeur 1 qui signifie que le joueur peut encore jouer.
                */
                if(y!=(n-1)){
                    if(gameBoard[x][y] == gameBoard[x][y+1]){
                        return 1;
                    }
                }
            }
        }
    }

    return 0; // On retourne 0 si aucune case adjacente n'est identique. Le joueur ne peux plus jouer.
}

int displaySDLWinWindow(SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Ajout d'une fenêtre translucide par dessus le gameBoard
    SDL_Surface * winWindowBackground = NULL;
    winWindowBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 450,450, 32,0,0,0,0);
    SDL_SetAlpha(winWindowBackground,SDL_SRCALPHA, 120); // Définition de la valeur alpha de la surface. La valeur alpha gère la transparence de la surface.
    if(winWindowBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(winWindowBackground,NULL,SDL_MapRGB(winWindowBackground->format,255,255,0));
    SDL_Rect positionWinWindowBackground;
    positionWinWindowBackground.x=25;
    positionWinWindowBackground.y=100;
    SDL_BlitSurface(winWindowBackground,NULL,ecran,&positionWinWindowBackground);
    SDL_FreeSurface(winWindowBackground);

    // Ajout du texte "Partie gagnée!"
    SDL_Color colorTexte= {255, 255, 255};
    SDL_Surface * Texte = TTF_RenderText_Blended(arialBold35, "Partie gagnee!", colorTexte);
    if(Texte==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte;
    positionTexte.x=131;
    positionTexte.y=280;
    SDL_BlitSurface(Texte,NULL,ecran,&positionTexte);
    SDL_FreeSurface(Texte);

    // Ajout du texte "Appuyez sur Q pour quitter le jeu"
    SDL_Color colorTexte2= {255, 255, 255};
    SDL_Surface * Texte2 = TTF_RenderText_Blended(arial20, "Appuyez sur Q pour quitter le jeu", colorTexte2);
    if(Texte2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte2;
    positionTexte2.x=100;
    positionTexte2.y=340;
    SDL_BlitSurface(Texte2,NULL,ecran,&positionTexte2);
    SDL_FreeSurface(Texte2);

    SDL_Flip(ecran);
    return 0;
}

int displaySDLGameOverWindow(SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Ajout d'une fenêtre translucide par dessus le gameBoard
    SDL_Surface * winWindowBackground = NULL;
    winWindowBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 450,450, 32,0,0,0,0);
    SDL_SetAlpha(winWindowBackground,SDL_SRCALPHA, 120); // Définition de la valeur alpha de la surface. La valeur alpha gère la transparence de la surface.
    if(winWindowBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(winWindowBackground,NULL,SDL_MapRGB(winWindowBackground->format,255,0,0));
    SDL_Rect positionWinWindowBackground;
    positionWinWindowBackground.x=25;
    positionWinWindowBackground.y=100;
    SDL_BlitSurface(winWindowBackground,NULL,ecran,&positionWinWindowBackground);
    SDL_FreeSurface(winWindowBackground);

    // Ajout du texte "Partie perdue!"
    SDL_Color colorTexte= {255, 255, 255};
    SDL_Surface * Texte = TTF_RenderText_Blended(arialBold35, "Partie perdue!", colorTexte);
    if(Texte==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte;
    positionTexte.x=131;
    positionTexte.y=280;
    SDL_BlitSurface(Texte,NULL,ecran,&positionTexte);
    SDL_FreeSurface(Texte);

    // Ajout du texte "Appuyez sur Q pour quitter le jeu"
    SDL_Color colorTexte2= {255, 255, 255};
    SDL_Surface * Texte2 = TTF_RenderText_Blended(arial20, "Appuyez sur Q pour quitter le jeu", colorTexte2);
    if(Texte2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte2;
    positionTexte2.x=100;
    positionTexte2.y=340;
    SDL_BlitSurface(Texte2,NULL,ecran,&positionTexte2);
    SDL_FreeSurface(Texte2);

    SDL_Flip(ecran);
    return 0;
}

int winGame(int * play, int score, SDL_Surface * ecran, TTF_Font * arial20, TTF_Font * arialBold35){
    // Cette fonction est appelée quand la partie est gagnée.

    *play = 0; // Stop la partie
    printf("Félicitation! Vous avez gagné la partie!\n");
    printf("Vous avez obtenu un score de %i points.\n", score); // Affichage du score

    if(displaySDLWinWindow(ecran, arial20, arialBold35)==-1){
        return -1;
    }

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
    // Cette fonction est appelée quand la partie est perdue.

    *play = 0; // Stop la partie
    printf("Game over! Le jeu n'est plus possible.\n");
    printf("Vous avez obtenu un score de %i points.\n", score); // Affichage du score

    if(displaySDLGameOverWindow(ecran, arial20, arialBold35)==-1){
        return -1;
    }

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
    /*
    Si "win" vaut 1 alors la partie est gagnée.
    Sinon, on teste s'il reste des cases vides sur le gameBoard. Si non, on regarde s'il est possible de continuer le jeu. Si non, game over.
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

int initGameboard(int ** gameBoard, int n){
    // Initialisation du plateau de jeu pour le début de la partie

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
    //création de la surface principale
    SDL_Surface * ecran = NULL ;
    if((ecran = SDL_SetVideoMode(WIDTH,HEIGHT,32, SDL_HWSURFACE))==NULL){
        fprintf(stderr,"Erreur VideoMode %s\n",SDL_GetError());
        return NULL;
    }

    // Légende de la fenêtre
    SDL_WM_SetCaption("2048",NULL);

    //Remplissage de la fenêtre
    SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format, 247,217,166));

    SDL_Flip(ecran);

    return ecran;
}

int initSDLScreenGame(SDL_Surface * ecran, TTF_Font * arial12, TTF_Font * arial14){
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
    SDL_Rect positionScoreBackground;
    positionScoreBackground.x=25;
    positionScoreBackground.y=25;
    SDL_BlitSurface(scoreBackground,NULL,ecran,&positionScoreBackground);
    SDL_FreeSurface(scoreBackground);

    // Ajout du fond du chronomètre
    SDL_Surface * chronoBackground = NULL;
    chronoBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(chronoBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(chronoBackground,NULL,SDL_MapRGB(chronoBackground->format,105,91,69));
    SDL_Rect positionChronoBackground;
    positionChronoBackground.x=350;
    positionChronoBackground.y=25;
    SDL_BlitSurface(chronoBackground,NULL,ecran,&positionChronoBackground);
    SDL_FreeSurface(chronoBackground);

    // Ajout du fond du gameBoard
    SDL_Surface * gameBoardBackground = NULL;
    gameBoardBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 450,450, 32,0,0,0,0);
    if(gameBoardBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(gameBoardBackground,NULL,SDL_MapRGB(gameBoardBackground->format,105,91,69));
    SDL_Rect positionGameBoardBackground;
    positionGameBoardBackground.x=25;
    positionGameBoardBackground.y=100;
    SDL_BlitSurface(gameBoardBackground,NULL,ecran,&positionGameBoardBackground);
    SDL_FreeSurface(gameBoardBackground);

    // Ajout des indications de commande
    SDL_Color colorTexte= {50,50,50};
    // Affichage du texte "Utilisez les flèches directionnelles pour déplacer les cases."
    SDL_Surface * Texte1 = TTF_RenderText_Blended(arial14, "Utilisez les fleches directionnelles pour deplacer les cases.", colorTexte);
    if(Texte1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte1;
    positionTexte1.x=35;
    positionTexte1.y=565;
    SDL_BlitSurface(Texte1,NULL,ecran,&positionTexte1);
    SDL_FreeSurface(Texte1);

    // Affichage du texte "Appuyez sur S pour sauvegarder votre partie ou Q pour quitter."
    SDL_Surface * Texte2 = TTF_RenderText_Blended(arial14, "Appuyez sur S pour sauvegarder votre partie ou Q pour quitter.", colorTexte);
    if(Texte2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte2;
    positionTexte2.x=35;
    positionTexte2.y=590;
    SDL_BlitSurface(Texte2,NULL,ecran,&positionTexte2);
    SDL_FreeSurface(Texte2);

    // Affichage du texte "Pour la sauvagarde, saisissez le nom de la sauvegarde dans le terminal."
    SDL_Surface * Texte3 = TTF_RenderText_Blended(arial12, "Pour la sauvagarde, saisissez le nom de la sauvegarde dans le terminal.", colorTexte);
    if(Texte3==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte3;
    positionTexte3.x=45;
    positionTexte3.y=610;
    SDL_BlitSurface(Texte3,NULL,ecran,&positionTexte3);
    SDL_FreeSurface(Texte3);

    SDL_Flip(ecran);
    return 0;
}

int refreshSDLScreenGame(SDL_Surface * ecran, int ** gameBoard, int n, int score, TTF_Font * police){
    // Mise à jour du score
    // Suppression de l'ancien score
    SDL_Surface * scoreBackground = NULL;
    scoreBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(scoreBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(scoreBackground,NULL,SDL_MapRGB(scoreBackground->format,105,91,69));
    SDL_Rect positionScoreBackground;
    positionScoreBackground.x=25;
    positionScoreBackground.y=25;
    SDL_BlitSurface(scoreBackground,NULL,ecran,&positionScoreBackground);
    SDL_FreeSurface(scoreBackground);
    
    // Affichage du score actuel
    SDL_Color colorScore= {255,255,255};
    char string[6];
    sprintf(string, "%d", score);
    SDL_Surface * Texte = TTF_RenderText_Blended(police, string, colorScore);
    if(Texte==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positonScore;
    positonScore.x=35;
    positonScore.y=40;
    SDL_BlitSurface(Texte,NULL,ecran,&positonScore);
    SDL_FreeSurface(Texte);


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

            //Ajout case selon les coordonnées
            SDL_Surface * caseGameBoard = NULL;
            caseGameBoard = SDL_CreateRGBSurface(SDL_HWSURFACE, 100,100, 32,0,0,0,0);
            if(caseGameBoard == NULL){
                fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
                return -1;
            }
            SDL_Rect positionCase;
            positionCase.x=x;
            positionCase.y=y;

            if(gameBoard[caseY][caseX]==0){
                // Case vide
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 166, 151, 128));
            }else{
                SDL_FillRect(caseGameBoard,NULL,SDL_MapRGB(caseGameBoard->format, 235, 164, 52));
            }
            SDL_BlitSurface(caseGameBoard,NULL,ecran,&positionCase);
            SDL_FreeSurface(caseGameBoard);

            // Ajout du nombre dans la case si necessaire
            if(gameBoard[caseY][caseX]!=0){
                SDL_Color colorTextCase= {255,255,255};
                char string[6];
                sprintf(string, "%d", gameBoard[caseY][caseX]);
                SDL_Surface * Texte = TTF_RenderText_Blended(police, string, colorTextCase);
                if(Texte==NULL){
                    fprintf(stderr,"erreur: %s",TTF_GetError());
                    return -1;
                }
                SDL_Rect positionTextCase;
                if(gameBoard[caseY][caseX]<10){
                    positionTextCase.x=x+45;
                }else if(gameBoard[caseY][caseX]<100){
                    positionTextCase.x=x+38;
                }else if(gameBoard[caseY][caseX]<1000){
                    positionTextCase.x=x+32;
                }else if(gameBoard[caseY][caseX]<10000){
                    positionTextCase.x=x+27;
                }else{
                    positionTextCase.x=x+23;
                }
                positionTextCase.y=y+40;
                SDL_BlitSurface(Texte,NULL,ecran,&positionTextCase);
                SDL_FreeSurface(Texte);
            }
        }
    }

    SDL_Flip(ecran);
    return 0;
}

int refreshSDLChrono(SDL_Surface * ecran, chronometre * chrono, TTF_Font * police){
    // Suppression du chronomètre affiché précédemment
    SDL_Surface * chronoBackground = NULL;
    chronoBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 125, 50, 32,0,0,0,0);
    if(chronoBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(chronoBackground,NULL,SDL_MapRGB(chronoBackground->format,105,91,69));
    SDL_Rect positionChronoBackground;
    positionChronoBackground.x=350;
    positionChronoBackground.y=25;
    SDL_BlitSurface(chronoBackground,NULL,ecran,&positionChronoBackground);
    SDL_FreeSurface(chronoBackground);

    // Affichage du score actuel
    SDL_Color colorChrono= {255,255,255};
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
    SDL_Surface * Texte = TTF_RenderText_Blended(police, strTime, colorChrono);
    if(Texte==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionChrono;
    positionChrono.x=360;
    positionChrono.y=40;
    SDL_BlitSurface(Texte,NULL,ecran,&positionChrono);
    SDL_FreeSurface(Texte);

    SDL_Flip(ecran);
    return 0;
}

int displaySDLQuitWindow(SDL_Surface * ecran, TTF_Font * police){
    // Ajout du fond de la fenêtre
    SDL_Surface * quitWindowBackground = NULL;
    quitWindowBackground = SDL_CreateRGBSurface(SDL_HWSURFACE, 375,150, 32,0,0,0,0);
    if(quitWindowBackground == NULL){
        fprintf(stderr,"Erreur CreateRGBSurface %s\n",SDL_GetError());
        return -1;
    }
    SDL_FillRect(quitWindowBackground,NULL,SDL_MapRGB(quitWindowBackground->format,66,58,44));
    SDL_Rect positionQuitWindowBackground;
    positionQuitWindowBackground.x=62;
    positionQuitWindowBackground.y=250;
    SDL_BlitSurface(quitWindowBackground,NULL,ecran,&positionQuitWindowBackground);
    SDL_FreeSurface(quitWindowBackground);

    SDL_Flip(ecran);

    // Ajout du texte "Voulez-vous vraiment quitter?"
    SDL_Color colorTexte = {255, 255, 255};
    SDL_Surface * Texte = TTF_RenderText_Blended(police, "Voulez-vous vraiment quitter?", colorTexte);
    if(Texte==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte;
    positionTexte.x=117;
    positionTexte.y=280;
    SDL_BlitSurface(Texte,NULL,ecran,&positionTexte);
    SDL_FreeSurface(Texte);

    // Ajout du texte "Appuyez sur Y pour oui ou N pour non."
    SDL_Color colorTexte2 = {255, 255, 255};
    SDL_Surface * Texte2 = TTF_RenderText_Blended(police, "Appuyez sur Y pour oui ou N pour non.", colorTexte2);
    if(Texte2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        return -1;
    }
    SDL_Rect positionTexte2;
    positionTexte2.x=75;
    positionTexte2.y=330;
    SDL_BlitSurface(Texte2,NULL,ecran,&positionTexte2);
    SDL_FreeSurface(Texte2);

    return 0;
}

int quitParty(int * play, SDL_Surface * ecran, TTF_Font * arial12, TTF_Font * arial14, TTF_Font * arial20){
    if(displaySDLQuitWindow(ecran, arial20)){
        return -1;
    }
    printf("Voulez-vous veaiment quitter la partie? Y/N\n");
    
    int cont = 1;
    SDL_Event event;
    while(cont){
        SDL_WaitEvent(&event);
        switch(event.type){
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case 'n':
                        if(initSDLScreenGame(ecran, arial12, arial14)==-1){
                            return -1;
                        }
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
    // Cette fonction lance la partie.

    int win = 0; // On initialise la varibale win à 0. Elle sera passée à 1 si le jeu est gagné.

    if(initSDLScreenGame(ecran, arial12, arial14)==-1){
        return -1;
    }

    // On affiche le gameBoard et le score dans le terminale
    displayGameBoard(gameBoard, n);
    printf("Score: %i\n", *score);

    // On affiche le gameBoard sur la fenêtre graphique SDL
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
                            upMove(gameBoard, n, score, &win);
                            // On ajoute une case de valeur 2 ou 4 à la place d'une case vide aléatoire du gameBoard. Si une erreur se produit on retourne -1.
                            if(fillInRandomBox(gameBoard, n, 4) == -1){
                                return -1;
                            }
                            // On affiche le gameBoard et le score dans le terminale
                            displayGameBoard(gameBoard, n);
                            printf("Score: %i\n", *score);
                            // On affiche le gameBoard sur la fenêtre graphique SDL
                            if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                return -1;
                            }
                            // On test si la partie est gagnée ou perdu ou si elle doit continuer.
                            if(testWinOrGameOver(win,play,*score,gameBoard,n,ecran,arial20,arialBold35)==-1){
                                return -1;
                            }
                            break;
                        case SDLK_DOWN:
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
                            if((q = quitParty(play, ecran, arial12, arial14, arial20))==1){
                                displayGameBoard(gameBoard, n);
                                printf("Score: %i\n", *score);
                                if(refreshSDLScreenGame(ecran, gameBoard, n, *score, arial20)==-1){
                                    return -1;
                                }
                            }else if(q==-1){
                                return -1;
                            }
                            startChrono(chrono);
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

        timeEnd = time(NULL);
        if(difftime(timeEnd, timeStart)>0){
            addSecondChrono(chrono);
            timeStart = time(NULL);
            refreshSDLChrono(ecran, chrono, arial20);
        }
    }

    return 0;
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

    // Chargement de la police de texte
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

    // On créé le plateau de jeu que l'on nommera gameBoard.
    int ** gameBoard = create2DTab(n, n);
    if(gameBoard == NULL){
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    int score = 0; // Initialisation de la variable de score.

    SDL_Surface * ecran = initSDLWindow();
    if(ecran == NULL){
        free2DTab(gameBoard, n);
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE); 
    }

    SDL_Color colorTexte= {135, 101, 0};

    SDL_Surface * text1 = TTF_RenderText_Blended(arial20, "Appuyez sur N pour demarrer une nouvelle partie!", colorTexte);
    if(text1==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }
    SDL_Rect positionText1;
    positionText1.x=27;
    positionText1.y=250;
    SDL_BlitSurface(text1,NULL,ecran,&positionText1);
    SDL_FreeSurface(text1);

    SDL_Surface * text2 = TTF_RenderText_Blended(arial20, "OU", colorTexte);
    if(text2==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }
    SDL_Rect positionText2;
    positionText2.x=233;
    positionText2.y=302;
    SDL_BlitSurface(text2,NULL,ecran,&positionText2);
    SDL_FreeSurface(text2);

    SDL_Surface * text3 = TTF_RenderText_Blended(arial20, "Appuyez sur L pour charger une partie enregistree!", colorTexte);
    if(text3==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }
    SDL_Rect positionText3;
    positionText3.x=22;
    positionText3.y=350;
    SDL_BlitSurface(text3,NULL,ecran,&positionText3);
    SDL_FreeSurface(text3);

    SDL_Surface * text4 = TTF_RenderText_Blended(arial12, "puis saisissez le nom de la sauvegarde dans le terminal.", colorTexte);
    if(text4==NULL){
        fprintf(stderr,"erreur: %s",TTF_GetError());
        free2DTab(gameBoard, n);
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }
    SDL_Rect positionText4;
    positionText4.x=22;
    positionText4.y=377;
    SDL_BlitSurface(text4,NULL,ecran,&positionText4);
    SDL_FreeSurface(text4);

    SDL_Flip(ecran);

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
                TTF_Quit(); // On quitte SDL_TTF
                SDL_Quit(); // On quitte SDL
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
        /*
        Si l'utilisateur entre 'n':
            On appelle la fonction "initGameboard". Si la fonction échoue, on libère l'espace alloué au plateau de jeu et retourne -1 pour mettre fin au programme.
        */
        if(initGameboard(gameBoard, n) == -1){
            printf("Erreur lors de l'initialisation du plateau de jeu.\n");
            free2DTab(gameBoard, n);
            TTF_Quit(); // On quitte SDL_TTF
            SDL_Quit(); // On quitte SDL
            exit(EXIT_FAILURE);
        }
    }else if(choice == 'l'){
        // Si l'utilisateur entre 'l':

        // On demande à l'utilisateur le nom de la sauvegarde qu'il souhaite charger.
        char saveName[24] = "";
        printf("Nom de la sauvegarde à charger : ");
        scanf(" %19s", saveName);

        strcat(saveName, ".txt"); // On ajoute l'xtension ".txt" à la fin du nom

        int r = loadGame(saveName, gameBoard, n, &score, &chrono); // On appelle la fonction "loadGame" et on affecte la valeur retournée par la fonction à r

        // On traite la valeur retournée par loadGame
        if(r == 10){
            // Si la valeur est 10 alors le nom saisie par l'utilisateur n'est pas correct. On libère l'espace alloué au plateau de jeu et retourne O pour mettre fin au programme.
            printf("Aucune sauvegarde pour le nom saisi.\n");
            free2DTab(gameBoard, n);
            TTF_Quit(); // On quitte SDL_TTF
            SDL_Quit(); // On quitte SDL
            exit(EXIT_SUCCESS);
        }else if(r == -1){
            // Si la valeur est -A alors une erreur s'est produite. On libère l'espace alloué au plateau de jeu et retourne O pour mettre fin au programme.
            printf("Erreur lors du chargement de la partie.\n");
            free2DTab(gameBoard, n);
            TTF_Quit(); // On quitte SDL_TTF
            SDL_Quit(); // On quitte SDL
            exit(EXIT_FAILURE);
        }
    }

    // On initialise la variable "play" à 1 et on appelle startGame pour lancer la partie.
    // Si une erreur se produit pendant la partie, on libère l'espace alloué au plateau de jeu et retourne -1 pour mettre fin au programme.
    int play = 1;
    if(startGame(gameBoard, n, &play, &score, ecran, arial12, arial14, arial20, arialBold35, &chrono) == -1){
        free2DTab(gameBoard, n);
        TTF_Quit(); // On quitte SDL_TTF
        SDL_Quit(); // On quitte SDL
        exit(EXIT_FAILURE);
    }

    free2DTab(gameBoard, n); // On libère l'espace alloué pour le plateau de jeu.
    TTF_Quit(); // On quitte SDL_TTF
    SDL_Quit(); // On quitte SDL
    return EXIT_SUCCESS;
}