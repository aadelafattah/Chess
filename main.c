#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char pieces[12][12];
char gameBoard [12][12];
char deadPieces[37];
int undo[500][6];
int redo[500][5];
int pUndo;
int pRedo;
char play[5];
int index[4];
int died=1; //deadPieces index
int diedLast;
int promotedLast;
int i,j,k,l;
int valid;
int turn;
int king1x, king1y, king2x, king2y;
int checkWhite, checkBlack;
int checkMateBlack,checkMateWhite;
int isKing;
int checkingX;
int checkingY;
FILE *fPointer;

//scanning all of the user inputs
void readLine(char s[],int n)
{
    int ch, i=0;
    while((ch=getchar()) != '\n'){
        if(i<n)
        {
            s[i++]=ch;
        }
    }
    s[i]='\0';
}

//function prototypes
void gameBoardBorders();
void checkerBoard();
void startingChessBoard();
void printGame ();
void scanAndConvert();
void checkValidMove();
void kingsIndex();
void swapElements ();
void reverseSwapElement();
void rook();
void knight();
void king();
void king();
void bishop();
void queen();
void pawn();
void moveSet();
void kingWhiteCheckMate();
void kingBlackCheckMate();
void checkMateBlackPath(int x,int y);
void checkMateWhitePath(int x,int y);
void kingWhiteStaleMate();
void kingBlackStaleMate();
void fullPieceCheckBlack(int x,int y);
void fullPieceCheckWhite(int x,int y);
void saveRedo();
void redoGame();
void saveUndo();
void undoGame();
void gameOver();
void saveGame();
void loadGame();
void checkValidBlackStalemate();
void checkValidWhiteStalemate();

int main(){
    //setting up the start board
    newGame:startingChessBoard();
    gameBoardBorders();
    checkerBoard();
    printGame();
    //pinpointing the positions of the king
    king1x=9, king1y=6, king2x=2, king2y=6;
    //setting up the parameters
    turn=0;
    diedLast=0;
    promotedLast=0;
    checkWhite=0;
    checkBlack=0;
    pUndo=0;
    pRedo=0;
    while (1){
        valid = 0;
        checkMateBlack=-1;checkMateWhite=-1;
        //white turn
        if (turn%2==0){
            while ((!valid)||(checkWhite)){
                pUndo=0;
                printf("\nWhite's turn:\t (for undo press \"U\" / for redo press \"R\" / for save press \"S\" / for load press \"L\")\n");
                scanAndConvert();
                //to save the game
                while ((play[0]=='S')&&(play[1]=='\0')){
                    saveGame();
                    printf("\nWhite's turn:\t (for undo press \"U\" / for redo press \"R\" / for save press \"S\" / for load press \"L\")\n");
                    scanAndConvert();
                }
                //to load the game
                if ((play[0]=='L')&&(play[1]=='\0')){
                    printf("Enter the file name to be loaded.\n");
                    char saveName[100];
                    readLine(saveName,100);
                    fPointer = fopen(saveName,"r");
                    if (fPointer){ //checking the file name
                        loadGame();
                        break;
                    }
                    else {
                        printf("File doesn't exist.\n");
                    }
                }
                //if player wants to undo
                else if((play[0]=='U'&&turn>0)&&(play[1]=='\0')){
                    undoGame();
                    break;
                }
                //if player wants to redo
                else if((play[0]=='R'&&pRedo-1>=0)&&(play[1]=='\0')){
                    redoGame();
                    break;
                }
                else{
                    //if the input is valid
                    if(valid){
                        checkValidMove(); //checking if the move of valid
                    }
                    if(valid){
                        moveSet(); //seeing the move set of the selected piece
                    }
                    if (valid){
                        saveUndo();
                        swapElements();//moving
                        undo[turn][4]=diedLast;
                        undo[turn][5]=promotedLast;
                    }
                    if(valid){
                        isKing=1;
                        fullPieceCheckWhite(king1x,king1y); //checking the king is in check or not
                    }
                    if ((valid)&&(checkWhite)){
                        printf("Invalid move.\n");
                        reverseSwapElement();//reversing the swap if the king becomes in check
                    }
                }
            }
            //if game is loaded start from beginning
            if (play[0]=='L'){
                continue;
            }
            printf("\n");
            printGame();
            kingWhiteStaleMate();//checking own king stalemate
            if (!checkMateBlack){
                printf("Stalemate.\n");
                gameOver();
                if (play[0]=='N'){//if the players want to start a new game after the game is over
                    goto newGame;
                }
                else if (play[0]=='L'){//if the players want to load a saved game after the game is over
                    printf("Enter the file name to be loaded.\n");
                    char saveName[100];
                    readLine(saveName,100);
                    fPointer = fopen(saveName,"r");
                    while (!fPointer){
                        printf("File doesn't exist.\n");
                        printf("Enter the file name to be loaded.\n");
                        readLine(saveName,100);
                        fPointer = fopen(saveName,"r");
                    }
                    loadGame();
                    continue;
                }
                else if (play[0]=='E'){//if the players want to exit after the game is over
                    exit(EXIT_SUCCESS);
                }
            }
            isKing=1;
            fullPieceCheckBlack(king2x,king2y);//checking the opposite player king
            if (checkMateBlack){
                kingBlackCheckMate();//checking checkmate on the opposite player
                if (!checkMateWhite){
                    printf("Check mate.\n");
                    gameOver();
                    if (play[0]=='N'){
                        goto newGame;
                    }
                    else if (play[0]=='L'){
                        printf("Enter the file name to be loaded.\n");
                        char saveName[100];
                        readLine(saveName,100);
                        fPointer = fopen(saveName,"r");
                        while (!fPointer){
                            printf("File doesn't exist.\n");
                            printf("Enter the file name to be loaded.\n");
                            readLine(saveName,100);
                            fPointer = fopen(saveName,"r");
                        }
                        loadGame();
                        continue;
                    }
                    else if (play[0]=='E'){
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            isKing=1;
            fullPieceCheckBlack(king2x,king2y);//checking again to be sure of the values
            if (!checkBlack){
                kingBlackStaleMate();//checking stalemate of the other player
                if (!checkMateWhite){
                    printf("Stalemate.\n");
                    gameOver();
                    if (play[0]=='N'){
                        goto newGame;
                    }
                    else if (play[0]=='L'){
                        printf("Enter the file name to be loaded.\n");
                        char saveName[100];
                        readLine(saveName,100);
                        fPointer = fopen(saveName,"r");
                        while (!fPointer){
                            printf("File doesn't exist.\n");
                            printf("Enter the file name to be loaded.\n");
                            readLine(saveName,100);
                            fPointer = fopen(saveName,"r");
                        }
                        loadGame();
                        continue;
                    }
                    else if (play[0]=='E'){
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            isKing=1;
            fullPieceCheckBlack(king2x,king2y);//checking again to be sure of the values
            if (checkBlack){
                printf("Check black.\n");
            }
            if(pUndo==0){//changing turn
                turn++;
                promotedLast=0;
            }
        }
        else {//Black turn same steps as the white
            while ((!valid)||(checkBlack)){
                pUndo=0;
                printf("\nBlack's turn:\t (for undo press \"U\" / for redo press \"R\" / for save press \"S\" / for load press \"L\")\n");
                scanAndConvert();
                while ((play[0]=='S')&&(play[1]=='\0')){
                    saveGame();
                    printf("\nWhite's turn:\t (for undo press \"U\" / for redo press \"R\" / for save press \"S\" / for load press \"L\")\n");
                    scanAndConvert();
                }
                if ((play[0]=='L')&&(play[1]=='\0')){
                    printf("Enter the file name to be loaded.\n");
                    char saveName[100];
                    readLine(saveName,100);
                    fPointer = fopen(saveName,"r");
                    if (fPointer){ //checking the file name
                        loadGame();
                        break;
                    }
                    else {
                        printf("File doesn't exist.\n");
                    }
                }
                else if((play[0]=='U'&&turn>0)&&(play[1]=='\0')){
                    undoGame();
                    break;
                }
                else if((play[0]=='R'&&pRedo-1>=0)&&(play[1]=='\0')){
                    redoGame();
                    break;
                }
                else{
                    if(valid){
                        checkValidMove();
                    }
                    if(valid){
                        moveSet();
                    }
                    if(valid){
                        saveUndo();
                        swapElements();
                        undo[turn][4]=diedLast;
                        undo[turn][5]=promotedLast;
                    }
                    if(valid){
                        isKing=1;
                        fullPieceCheckBlack(king2x,king2y);
                    }
                    if(checkBlack && valid){
                        printf("Invalid move.\n");
                        reverseSwapElement();
                    }
                }
            }
            if (play[0]=='L'){
                continue;
            }
            printf("\n");
            printGame();
            kingBlackStaleMate();
            if (!checkMateWhite){
                printf("Stalemate.\n");
                gameOver();
                if (play[0]=='N'){
                    goto newGame;
                }
                else if (play[0]=='L'){
                    printf("Enter the file name to be loaded.\n");
                    char saveName[100];
                    readLine(saveName,100);
                    fPointer = fopen(saveName,"r");
                    while (!fPointer){
                        printf("File doesn't exist.\n");
                        printf("Enter the file name to be loaded.\n");
                        readLine(saveName,100);
                        fPointer = fopen(saveName,"r");
                    }
                    loadGame();
                    continue;
                }
                else if (play[0]=='E'){
                    exit(EXIT_SUCCESS);
                }
            }
            isKing=1;
            fullPieceCheckWhite(king1x,king1y);
            if (checkMateWhite){
                kingWhiteCheckMate();
                if (!checkMateBlack){
                    printf("Check mate.\n");
                    gameOver();
                    if (play[0]=='N'){
                        goto newGame;
                    }
                    else if (play[0]=='L'){
                        printf("Enter the file name to be loaded.\n");
                        char saveName[100];
                        readLine(saveName,100);
                        fPointer = fopen(saveName,"r");
                        while (!fPointer){
                            printf("File doesn't exist.\n");
                            printf("Enter the file name to be loaded.\n");
                            readLine(saveName,100);
                            fPointer = fopen(saveName,"r");
                        }
                        loadGame();
                        continue;
                    }
                    else if (play[0]=='E'){
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            isKing=1;
            fullPieceCheckWhite(king1x,king1y);
            if (!checkWhite){
                kingWhiteStaleMate();
                if (!checkMateBlack){
                    printf("Stalemate.\n");
                    gameOver();
                    if (play[0]=='N'){
                        goto newGame;
                    }
                    else if (play[0]=='L'){
                        printf("Enter the file name to be loaded.\n");
                        char saveName[100];
                        readLine(saveName,100);
                        fPointer = fopen(saveName,"r");
                        while (!fPointer){
                            printf("File doesn't exist.\n");
                            printf("Enter the file name to be loaded.\n");
                            readLine(saveName,100);
                            fPointer = fopen(saveName,"r");
                        }
                        loadGame();
                        continue;
                    }
                    else if (play[0]=='E'){
                        exit(EXIT_SUCCESS);
                    }
                }
            }
            isKing=1;
            fullPieceCheckWhite(king1x,king1y);
            if (checkWhite){
                printf("Check white.\n");
            }
            if(!pUndo){
                turn++;
                promotedLast=0;
            }
        }
    }
    return 0;
}

void gameBoardBorders(){
    for (i=0;i<12;i++){
        for (j=0;j<12;j++){
            gameBoard[i][j]='\0';
        }
    }
    for (i=0;i<12;i+=11){
        for (j=2;j<10;j++){
            gameBoard[i][j]='A'+(j-2);
        }
    }
    for (j=2;j<10;j++){
        gameBoard[1][j]='-';
        gameBoard[10][j]='-';
    }
    for (i=2;i<10;i++){
        for (j=0;j<12;j+=11){
            gameBoard[i][j]='8'-(i-2);
        }
    }
    for (i=2;i<10;i++){
        gameBoard[i][1]='|';
        gameBoard[i][10]='|';
    }

}

void checkerBoard(){
    for (i=2;i<10;i++){
        for (j=2;j<10;j+=2){
            if (i%2==0){
                gameBoard[i][j]='.';
            }
            else {
                gameBoard[i][j]='=';
            }
        }
    }
    for (i=2;i<10;i++){
        for (j=3;j<10;j+=2){
            if (i%2==0){
                gameBoard[i][j]='=';
            }
            else {
                gameBoard[i][j]='.';
            }
        }
    }
}

void startingChessBoard(){
    for (i=0;i<12;i++){
        for(j=0;j<12;j++){
            pieces[i][j]='\0';
        }
    }
    pieces[2][2]= 'R';
    pieces[2][3]= 'N';
    pieces[2][4]= 'B';
    pieces[2][5]= 'Q';
    pieces[2][6]= 'K';
    pieces[2][7]= 'B';
    pieces[2][8]= 'N';
    pieces[2][9]= 'R';

    pieces[9][2]= 'r';
    pieces[9][3]= 'n';
    pieces[9][4]= 'b';
    pieces[9][5]= 'q';
    pieces[9][6]= 'k';
    pieces[9][7]= 'b';
    pieces[9][8]= 'n';
    pieces[9][9]= 'r';

    for(j=2;j<10;j++){
        pieces[3][j]= 'P';
    }

    for(j=2;j<10;j++){
        pieces[8][j]= 'p';
    }
    for(i=0;i<36;i++){
        deadPieces[i]='\0';
    }
}

void printGame(){
    system("cls");
    printf("\n\t\t\t\t\t\tDead\n");
    for (i=0;i<12;i++){
        for (j=0;j<12;j++){
            printf("%c",gameBoard[i][j]);
            printf("%c ", pieces[i][j]);
        }
        printf("\t\t%c %c %c", deadPieces[i+1], deadPieces[i+13], deadPieces[i+25]);
        printf("\n\n");
    }
    printf("\n");
}

void scanAndConvert(){
    readLine(play,5);
    if((play[0]=='U')&&(play[1]=='\0')){
        return;
    }
    else if((play[0]=='R')&&(play[1]=='\0')){
        return;
    }
    else if ((play[0]=='S')&&(play[1]=='\0')){
        return;
    }
    else if ((play[0]=='L')&&(play[1]=='\0')){
        return;
    }
    else{
        if (play[4]!='\0'){
            printf("Invalid move.\n");
            valid=0;
            return;
        }//checking the range of the input
        if ((play[0]<'A')||(play[0]>'H')||(play[1]<'1')||(play[1]>'8')||(play[2]<'A')||(play[2]>'H')||(play[3]<'1')||(play[3]>'8')){
            printf("Invalid move.\n");
            valid=0;
            return;
        }
    } //converting to have indexes
    index[0]=58-play[1];
    index[1]=play[0]-63;
    index[2]=58-play[3];
    index[3]=play[2]-63;
    valid=1;
}

void checkValidMove(){
    if (turn%2==0){
        if ((pieces[index[0]][index[1]]<'a')||(pieces[index[0]][index[1]]>'z')){
            printf("Invalid move.\n");
            valid=0;
            return;
        }
        if ((pieces[index[2]][index[3]]>='a')&&(pieces[index[2]][index[3]]<='z')){
            printf("Invalid move.\n");
            valid=0;
            return;
        }
    }
    else if (turn%2==1){
        if ((pieces[index[0]][index[1]]<'A')||(pieces[index[0]][index[1]]>'Z')){
            printf("Invalid move.\n");
            valid=0;
            return;
        }
        if((pieces[index[2]][index[3]]>='A')&&(pieces[index[2]][index[3]]<='Z')){
            printf("Invalid move.\n");
            valid=0;
            return;
        }
    }
    valid=1;
}

void kingsIndex(){
    int p,q;
    for(p=2;p<=9;p++){
        for(q=2;q<=9;q++){
            if(pieces[p][q]=='k'){
                king1x=p;
                king1y=q;
            }
            if(pieces[p][q]=='K'){
                king2x=p;
                king2y=q;
            }
        }
    }
}

void swapElements (){
    if ((pieces[index[2]][index[3]])!='\0'){//checking if the place has an opposite piece
        deadPieces[died]= pieces[index[2]][index[3]];
        died++;
        pieces[index[2]][index[3]]='\0';
        diedLast=1;
    }else{
        diedLast=0;
    }
    char temp=pieces[index[0]][index[1]];
    pieces[index[0]][index[1]]=pieces[index[2]][index[3]];
    pieces[index[2]][index[3]]=temp;
    kingsIndex();
}

void saveUndo(){ //saving the index of the turn in the undo list
    int u;
    for(u=0;u<4;u++){
        undo[turn][u]=index[u];
    }
}

void undoGame(){
    int u;
    //making the pieces index as the last turn
    for(u=0;u<4;u++){
        index[u]=undo[turn-1][u];
    }
    //checking if anything dead
    diedLast=undo[turn-1][4];
    //saving redo
    if (undo[turn-1][5]==1){
        redo[pRedo][4]=pieces[index[2]][index[3]];
        if(pieces[index[2]][index[3]]>='a'&& pieces[index[2]][index[3]]<='z'){
            pieces[index[2]][index[3]]='p';
        }
        else if(pieces[index[2]][index[3]]>='A'&& pieces[index[2]][index[3]]<='Z'){
            pieces[index[2]][index[3]]='P';
        }
    }
    saveRedo();
    reverseSwapElement();
    turn--;
    pUndo=1;
}

void saveRedo(){//if undo is done saves the move in the redo
    int r;
    for(r=0;r<4;r++){
        redo[pRedo][r]=undo[turn-1][r];
    }
    pRedo++;
}

void redoGame(){
    int r;
    for(r=0;r<4;r++){
    //making the pieces index as the last turn
        index[r]=redo[pRedo-1][r];
    }
    if (redo[pRedo-1][4]!=0){
        pieces[index[0]][index[1]]=redo[pRedo-1][4];
    }
    //saving the undo
    saveUndo();
    swapElements();
    undo[turn][4]=diedLast;
    pRedo--;
}

void reverseSwapElement(){
    if(diedLast){//if last move a piece died then we bring it back
        if(pieces[index[0]][index[1]]=='\0' || deadPieces[died-1]!='\0'){
            pieces[index[0]][index[1]]=deadPieces[died-1];
            died--;
            deadPieces[died]='\0';
        }
    }
    char temp=pieces[index[2]][index[3]];
    pieces[index[2]][index[3]]=pieces[index[0]][index[1]];
    pieces[index[0]][index[1]]=temp;
    kingsIndex();
}

void rook(){//movement of the rook
    if((index[3]-index[1])==0 && (index[2]-index[0]>0)){
        for (i=1;i<index[2]-index[0];i++){
            if (pieces[index[0]+i][index[1]]!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if(index[0]+i==index[2]){
                valid=1;
        }
    }
    else if ((index[3]-index[1])==0 && (index[0]-index[2]>0)){
        for (i=1;i<index[0]-index[2];i++){
                if (pieces[index[0]-i][index[1]]!='\0'){
                    printf("Invalid move.");
                    valid=0;
                    return;
                }
            }
            if (index[0]-i==index[2]){
                valid=1;
        }
    }
    else if ((index[3]-index[1])>0 && (index[2]-index[0]==0)){
        for (i=1;i<index[3]-index[1];i++){
            if (pieces[index[0]][index[1]+i]!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if(index[1]+i==index[3]){
            valid=1;
        }
    }
    else if ((index[1]-index[3])>0 && (index[2]-index[0]==0)){
        for (i=1;i<index[1]-index[3];i++){
            if (pieces[index[0]][index[1]-i]!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if(index[1]-i==index[3]){
            valid=1;
        }
    }
    else{
        printf("Invalid move.");
        valid=0;
        return;
    }
}

void knight(){//movement of the knight
    if ((((abs(index[3]-index[1]))==1)&&((abs(index[2]-index[0]))==2)) || (((abs(index[3]-index[1]))==2)&&((abs(index[2]-index[0]))==1)))
    {
        valid=1;
    }else{
        printf("Invalid move.");
        valid=0;
        return;
    }
}

void king(){//movement of the king
    if((((abs(index[3]-index[1]))==1) && ((abs(index[2]-index[0]))==1)) || (((abs(index[3]-index[1]))==1) && ((abs(index[2]-index[0]))==0)) || (((abs(index[3]-index[1]))==0) && ((abs(index[2]-index[0]))==1)))
    {
        valid=1;
    }else{
        printf("Invalid move.");
        valid=0;
        return;
    }
}

void bishop(){//movement of the bishop
    if (((index[0]-index[2])==(index[1]-index[3]))&&((index[0]-index[2])>0)){
        for (i=1;i<index[0]-index[2];i++){
            if ((pieces[index[0]-i][index[1]-i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]-i==index[2])&&(index[1]-i==index[3])){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[1]-index[3]))&&((index[2]-index[0])>0)){
        for (i=1;i<index[2]-index[0];i++){
            if ((pieces[index[0]+i][index[1]+i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]+i==index[2])&&(index[1]+i==index[3])){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[3]-index[1]))&&((index[0]-index[2])>0)){
        for (i=1;i<index[0]-index[2];i++){
            if ((pieces[index[0]-i][index[1]+i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]-i==index[2])&&(index[1]+i==index[3])){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[3]-index[1]))&&((index[2]-index[0])>0)){
        for (i=1;i<index[0]-index[2];i++){
            if ((pieces[index[0]+i][index[1]-i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]+i==index[2])&&(index[1]-i==index[3])){
            valid=1;
        }
    }
    else{
        printf("Invalid move.");
        valid=0;
        return;
    }
}

void queen(){//movement of the queen
    if((index[3]-index[1])==0 && (index[2]-index[0]>0)){
        for (i=1;i<index[2]-index[0];i++){
            if (pieces[index[0]+i][index[1]]!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if(index[0]+i==index[2]){
                valid=1;
        }
    }
    else if ((index[3]-index[1])==0 && (index[0]-index[2]>0)){
        for (i=1;i<index[0]-index[2];i++){
                if (pieces[index[0]-i][index[1]]!='\0'){
                    printf("Invalid move.");
                    valid=0;
                    return;
                }
            }
            if (index[0]-i==index[2]){
                valid=1;
        }
    }
    else if ((index[3]-index[1])>0 && (index[2]-index[0]==0)){
        for (i=1;i<index[3]-index[1];i++){
            if (pieces[index[0]][index[1]+i]!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if(index[1]+i==index[3]){
            valid=1;
        }
    }
    else if ((index[1]-index[3])>0 && (index[2]-index[0]==0)){
        for (i=1;i<index[1]-index[3];i++){
            if (pieces[index[0]][index[1]-i]!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if(index[1]-i==index[3]){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[1]-index[3]))&&((index[0]-index[2])>0)){
        for (i=1;i<index[0]-index[2];i++){
            if ((pieces[index[0]-i][index[1]-i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]-i==index[2])&&(index[1]-i==index[3])){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[1]-index[3]))&&((index[2]-index[0])>0)){
        for (i=1;i<index[2]-index[0];i++){
            if ((pieces[index[0]+i][index[1]+i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]+i==index[2])&&(index[1]+i==index[3])){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[3]-index[1]))&&((index[0]-index[2])>0)){
        for (i=1;i<index[0]-index[2];i++){
            if ((pieces[index[0]-i][index[1]+i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]-i==index[2])&&(index[1]+i==index[3])){
            valid=1;
        }
    }
    else if (((index[0]-index[2])==(index[3]-index[1]))&&((index[2]-index[0])>0)){
        for (i=1;i<index[0]-index[2];i++){
            if ((pieces[index[0]+i][index[1]-i])!='\0'){
                printf("Invalid move.");
                valid=0;
                return;
            }
        }
        if ((index[0]+i==index[2])&&(index[1]-i==index[3])){
            valid=1;
        }
    }
    else{
        printf("Invalid move.");
        valid=0;
        return;
    }
}

void pawn(){//movement of the pawn
    char promote[2];
        if (pieces[index[0]][index[1]]=='p'){
            if(index[0]==8){
                if(((index[2]-index[0]==-1)||(index[2]-index[0]==-2)) && (index[3]-index[1]==0) && (pieces[index[2]][index[3]]=='\0')){
                    valid = 1;
                }
                else if((pieces[index[2]][index[3]]!='\0')&& ((index[2]-index[0]==-1)&& (abs(index[3]-index[1])==1))){
                    valid = 1;
                }
                else if ((pieces[index[2]][index[3]]!='\0')&& ((index[2]-index[0]==-1)&& (abs(index[3]-index[1])==1))){
                    valid=1;
                }
                else{
                    printf("Invalid move.");
                    valid=0;
                    return;
                }
            }else{
                if((index[2]-index[0]==-1) && (index[3]-index[1]==0) && (pieces[index[2]][index[3]]=='\0')){
                    valid = 1;
                }
                else if((pieces[index[2]][index[3]]!='\0')&& ((index[2]-index[0]==-1)&& (abs(index[3]-index[1])==1))){
                    valid = 1;
                }
                else{
                    printf("Invalid move.");
                    valid=0;
                    return;
                }
            }
            if (index[2]==2){
                printf("Promote to: ");
                readLine(promote,2);
                while ((promote[1]!='\0')||((promote[0] != 'R')&&(promote[0] != 'N')&&(promote[0] != 'B')&&(promote[0] != 'Q')&&(promote[0] != 'P'))){
                    printf("Wrong promotion\n");
                    printf("Promote to: ");
                    readLine(promote,2);
                }
                if(promote[0] == 'R'){
                    pieces[index[0]][index[1]]='r';
                    valid=1;
                }
                else if(promote[0] == 'N'){
                    pieces[index[0]][index[1]]='n';
                    valid=1;
                }
                else if(promote[0] == 'B'){
                    pieces[index[0]][index[1]]='b';
                    valid=1;
                }
                else if(promote[0] == 'Q'){
                    pieces[index[0]][index[1]]='q';
                    valid=1;
                }else if(promote[0] == 'P'){
                    pieces[index[0]][index[1]]='p';
                    valid=1;
                }
                promotedLast=1;
            }
        }
        else{
            if(index[0]==3){
                if(((index[2]-index[0]==1)||(index[2]-index[0]==2)) && (index[3]-index[1]==0) && (pieces[index[2]][index[3]]=='\0')){
                    valid = 1;
                }
                else if((pieces[index[2]][index[3]]!='\0')&& ((index[2]-index[0]==-1)&& (abs(index[3]-index[1])==1))){
                    valid = 1;
                }
                else if ((pieces[index[2]][index[3]]!='\0')&& ((index[2]-index[0]==1)&& (abs(index[3]-index[1])==1))){
                    valid=1;
                }
                else{
                    printf("Invalid move.");
                    valid=0;
                    return;
                }
            }else{
                if((index[2]-index[0]==1) && (index[3]-index[1]==0) && (pieces[index[2]][index[3]]=='\0')){
                    valid = 1;
                }
                else if((pieces[index[2]][index[3]]!='\0')&& ((index[2]-index[0]==1)&& (abs(index[3]-index[1])==1))){
                    valid = 1;
                }
                else{
                    printf("Invalid move.");
                    valid=0;
                    return;
                }
            }
            if (index[2]==9){
                printf("Promote to: ");
                readLine(promote,2);
                while((promote[0] != 'R' && promote[0] != 'N' && promote[0] != 'B' && promote[0] != 'Q' && promote[0] != 'P')||(promote[1]!='\0')){
                    printf("Wrong promotion\n");
                    printf("Promote to: ");
                    readLine(promote,2);
                }
                if(promote[0] == 'R'){
                    pieces[index[0]][index[1]]='R';
                    valid=1;
                }
                else if(promote[0] == 'N'){
                    pieces[index[0]][index[1]]='N';
                    valid=1;
                }
                else if(promote[0] == 'B'){
                    pieces[index[0]][index[1]]='B';
                    valid=1;
                }
                else if(promote[0] == 'Q'){
                    pieces[index[0]][index[1]]='Q';
                    valid=1;
                }else if(promote[0] == 'P'){
                    pieces[index[0]][index[1]]='P';
                    valid=1;
                }
                promotedLast=1;
            }
        }
    }

void moveSet(){//checking which movement the code should do
    if ((pieces[index[0]][index[1]]=='p')||(pieces[index[0]][index[1]]=='P')){
        pawn();
    }
    else if ((pieces[index[0]][index[1]]=='r')||(pieces[index[0]][index[1]]=='R')){
        rook();
    }
    else if ((pieces[index[0]][index[1]]=='n')||(pieces[index[0]][index[1]]=='N')){
        knight();
    }
    else if ((pieces[index[0]][index[1]]=='b')||(pieces[index[0]][index[1]]=='B')){
        bishop();
    }
    else if ((pieces[index[0]][index[1]]=='q')||(pieces[index[0]][index[1]]=='Q')){
        queen();
    }
    else if ((pieces[index[0]][index[1]]=='k')||(pieces[index[0]][index[1]]=='K')){
        king();
    }
}

void kingWhiteCheckMate(){ //check if own king is in checkmate
    checkMateBlack=1;
    //check if all movements available for the king are in check
    if ((pieces[king1x+1][king1y]<'a')&&(king1x+1>1)&&(king1x+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x+1,king1y);
    }
    if ((checkMateWhite==1)&&(pieces[king1x][king1y+1]<'a')&&(king1y+1>1)&&(king1y+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x,king1y+1);
    }
    if ((checkMateWhite==1)&&(pieces[king1x+1][king1y+1]<'a')&&(king1y+1>1)&&(king1y+1<10)&&(king1x+1>1)&&(king1x+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x+1,king1y+1);
    }
    if ((checkMateWhite==1)&&(pieces[king1x-1][king1y]<'a')&&(king1x-1>1)&&(king1x-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x-1,king1y);
    }
    if ((checkMateWhite==1)&&(pieces[king1x][king1y-1]<'a')&&(king1y-1>1)&&(king1y-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x,king1y-1);
    }
    if ((checkMateWhite==1)&&(pieces[king1x-1][king1y-1]<'a')&&(king1y-1>1)&&(king1y-1<10)&&(king1x-1>1)&&(king1x-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x-1,king1y-1);
    }
    if ((checkMateWhite==1)&&(pieces[king1x+1][king1y-1]<'a')&&(king1y-1>1)&&(king1y-1<10)&&(king1x+1>1)&&(king1x+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x+1,king1y-1);
    }
    if ((checkMateWhite==1)&&(pieces[king1x-1][king1y+1]<'a')&&(king1y+1>1)&&(king1y+1<10)&&(king1x-1>1)&&(king1x-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x-1,king1y+1);
    }
    if (checkMateWhite==1){
        isKing=0;
        fullPieceCheckBlack(index[2],index[3]);//check if any owned piece can kill the opposite piece so own king becomes not in check
        if (checkMateBlack==0){
            checkMateWhitePath(king1x,king1y);//check if any owned piece can be in the path of opposite checking piece so own king becomes not in check
        }
    }
}

void kingBlackCheckMate(){
    checkMateWhite=1;
    if (((pieces[king2x+1][king2y]>='a')||(pieces[king2x+1][king2y]=='\0'))&&(king2x+1>1)&&(king2x+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x+1,king2y);
    }
    if ((checkMateBlack==1)&&((pieces[king2x][king2y+1]>='a')||(pieces[king2x][king2y+1]=='\0'))&&(king2y+1>1)&&(king2y+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x,king2y+1);
    }
    if ((checkMateBlack==1)&&((pieces[king2x+1][king2y+1]>='a')||(pieces[king2x+1][king2y+1]=='\0'))&&(king2y+1>1)&&(king2y+1<10)&&(king2x+1>1)&&(king2x+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x+1,king2y+1);
    }
    if ((checkMateBlack==1)&&((pieces[king2x-1][king2y]>='a')||(pieces[king2x-1][king2y]=='\0'))&&(king2x-1>1)&&(king2x-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x-1,king2y);
    }
    if ((checkMateBlack==1)&&((pieces[king2x][king2y-1]>='a')||(pieces[king2x][king2y-1]=='\0'))&&(king2y-1>1)&&(king2y-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x,king2y-1);
    }
    if ((checkMateBlack==1)&&((pieces[king2x-1][king2y-1]>='a')||(pieces[king2x-1][king2y-1]=='\0'))&&(king2y-1>1)&&(king2y-1<10)&&(king2x-1>1)&&(king2x-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x-1,king2y-1);
    }
    if ((checkMateBlack==1)&&((pieces[king2x+1][king2y-1]>='a')||(pieces[king2x+1][king2y-1]=='\0'))&&(king2y-1>1)&&(king2y-1<10)&&(king2x+1>1)&&(king2x+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x+1,king2y-1);
    }
    if ((checkMateBlack==1)&&((pieces[king2x-1][king2y+1]>='a')||(pieces[king2x-1][king2y+1]=='\0'))&&(king2y+1>1)&&(king2y+1<10)&&(king2x-1>1)&&(king2x-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x-1,king2y+1);
    }
    if (checkMateBlack==1){
        isKing=0;
        fullPieceCheckWhite(index[2],index[3]);
        if (checkMateWhite==0){
            checkMateBlackPath(king2x,king2y);
        }
    }
}

void checkMateWhitePath(int x,int y){ //check if any possible path for his own piece has opposite piece threatens his own king
    //vertical path
    if((index[3]-y)==0 && (index[2]-x>0)){
        for (k=1;k<index[2]-x;k++){
            isKing=0;
            fullPieceCheckBlack(x+k,y);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    else if ((index[3]-y)==0 && (x-index[2]>0)){
        for (k=1;k<x-index[2];k++){
            isKing=0;
            fullPieceCheckBlack(x-k,y);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    //horizontal path
    else if ((index[3]-y)>0 && (index[2]-x==0)){
        for (k=1;k<index[3]-y;k++){
            isKing=0;
            fullPieceCheckBlack(x,y+k);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    else if ((y-index[3])>0 && (index[2]-x==0)){
        for (k=1;k<y-index[3];k++){
            isKing=0;
            fullPieceCheckBlack(x,y-k);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    //diagonal path
    else if (((x-index[2])==(y-index[3]))&&((x-index[2])>0)){
        for (k=1;k<x-index[2];k++){
            isKing=0;
            fullPieceCheckBlack(x-k,y-k);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(y-index[3]))&&((index[2]-x)>0)){
        for (k=1;k<index[2]-x;k++){
            isKing=0;
            fullPieceCheckBlack(x+k,y+k);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(index[3]-y))&&((x-index[2])>0)){
        for (k=1;k<x-index[2];k++){
            isKing=0;
            fullPieceCheckBlack(x-k,y+k);
            if (checkMateBlack==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(index[3]-y))&&((index[2]-x)>0)){
        for (k=1;k<index[0]-index[2];k++){
            isKing=0;
            fullPieceCheckBlack(x+k,y-k);
            if (checkMateBlack==1){
                break;
            }
        }
    }
}

void checkMateBlackPath(int x,int y){
    if((index[3]-y)==0 && (index[2]-x>0)){
        for (k=1;k<index[2]-x;k++){
            isKing=0;
            fullPieceCheckWhite(x+k,y);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if ((index[3]-y)==0 && (x-index[2]>0)){
        for (k=1;k<x-index[2];k++){
            isKing=0;
            fullPieceCheckWhite(x-k,y);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if ((index[3]-y)>0 && (index[2]-x==0)){
        for (k=1;k<index[3]-y;k++){
            isKing=0;
            fullPieceCheckWhite(x,y+k);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if ((y-index[3])>0 && (index[2]-x==0)){
        for (k=1;k<y-index[3];k++){
            isKing=0;
            fullPieceCheckWhite(x,y-k);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(y-index[3]))&&((x-index[2])>0)){
        for (k=1;k<x-index[2];k++){
            isKing=0;
            fullPieceCheckWhite(x-k,y-k);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(y-index[3]))&&((index[2]-x)>0)){
        for (k=1;k<index[2]-x;k++){
            isKing=0;
            fullPieceCheckWhite(x+k,y+k);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(index[3]-y))&&((x-index[2])>0)){
        for (k=1;k<x-index[2];k++){
            isKing=0;
            fullPieceCheckWhite(x-k,y+k);
            if (checkMateWhite==1){
                break;
            }
        }
    }
    else if (((x-index[2])==(index[3]-y))&&((index[2]-x)>0)){
        for (k=1;k<index[0]-index[2];k++){
            isKing=0;
            fullPieceCheckWhite(x+k,y-k);
            if (checkMateWhite==1){
                break;
            }
        }
    }
}

void kingWhiteStaleMate(){ //check if his own king is in stalemate
    checkWhite=1;
    checkMateBlack=1;
    //check if all possible movements for the king will put him in check
    if ((pieces[king1x+1][king1y]<'a')&&(king1x+1>1)&&(king1x+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x+1,king1y);
    }
    if ((checkWhite==1)&&(pieces[king1x][king1y+1]<'a')&&(king1y+1>1)&&(king1y+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x,king1y+1);
    }
    if ((checkWhite==1)&&(pieces[king1x+1][king1y+1]<'a')&&(king1y+1>1)&&(king1y+1<10)&&(king1x+1>1)&&(king1x+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x+1,king1y+1);
    }
    if ((checkWhite==1)&&(pieces[king1x-1][king1y]<'a')&&(king1x-1>1)&&(king1x-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x-1,king1y);
    }
    if ((checkWhite==1)&&(pieces[king1x][king1y-1]<'a')&&(king1y-1>1)&&(king1y-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x,king1y-1);
    }
    if ((checkWhite==1)&&(pieces[king1x-1][king1y-1]<'a')&&(king1y-1>1)&&(king1y-1<10)&&(king1x-1>1)&&(king1x-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x-1,king1y-1);
    }
    if ((checkWhite==1)&&(pieces[king1x+1][king1y-1]<'a')&&(king1y-1>1)&&(king1y-1<10)&&(king1x+1>1)&&(king1x+1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x+1,king1y-1);
    }
    if ((checkWhite==1)&&(pieces[king1x-1][king1y+1]<'a')&&(king1y+1>1)&&(king1y+1<10)&&(king1x-1>1)&&(king1x-1<10)){
        isKing=1;
        fullPieceCheckWhite(king1x-1,king1y+1);
    }
    if (checkWhite==1){
        checkMateBlack=0;
        //check if any of his own pieces can move
        for (k=2;k<=9;k++){
            for (l=2;l<=9;l++){
                if (pieces[k][l]<'a'){
                    isKing=0;
                    fullPieceCheckBlack(k,l);
                    if (checkMateBlack==1){
                        checkValidWhiteStalemate();
                        if (checkWhite==0){
                            return;
                        }
                    }
                }
            }
        }
    }
}

void kingBlackStaleMate(){
    checkBlack=1;
    checkMateWhite=1;
    if (((pieces[king2x+1][king2y]>='a')||(pieces[king2x+1][king2y]=='\0'))&&(king2x+1>1)&&(king2x+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x+1,king2y);
    }
    if ((checkBlack==1)&&((pieces[king2x][king2y+1]>='a')||(pieces[king2x][king2y+1]=='\0'))&&(king2y+1>1)&&(king2y+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x,king2y+1);
    }
    if ((checkBlack==1)&&((pieces[king2x+1][king2y+1]>='a')||(pieces[king2x+1][king2y+1]=='\0'))&&(king2y+1>1)&&(king2y+1<10)&&(king2x+1>1)&&(king2x+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x+1,king2y+1);
    }
    if ((checkBlack==1)&&((pieces[king2x-1][king2y]>='a')||(pieces[king2x-1][king2y]=='\0'))&&(king2x-1>1)&&(king2x-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x-1,king2y);
    }
    if ((checkBlack==1)&&((pieces[king2x][king2y-1]>='a')||(pieces[king2x][king2y-1]=='\0'))&&(king2y-1>1)&&(king2y-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x,king2y-1);
    }
    if ((checkBlack==1)&&((pieces[king2x-1][king2y-1]>='a')||(pieces[king2x-1][king2y-1]=='\0'))&&(king2y-1>1)&&(king2y-1<10)&&(king2x-1>1)&&(king2x-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x-1,king2y-1);
    }
    if ((checkBlack==1)&&((pieces[king2x+1][king2y-1]>='a')||(pieces[king2x+1][king2y-1]=='\0'))&&(king2y-1>1)&&(king2y-1<10)&&(king2x+1>1)&&(king2x+1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x+1,king2y-1);
    }
    if ((checkBlack==1)&&((pieces[king2x-1][king2y+1]>='a')||(pieces[king2x-1][king2y+1]=='\0'))&&(king2y+1>1)&&(king2y+1<10)&&(king2x-1>1)&&(king2x-1<10)){
        isKing=1;
        fullPieceCheckBlack(king2x-1,king2y+1);
    }
    if (checkBlack==1){
        checkMateWhite=0;
        for (k=2;k<=9;k++){
            for (l=2;l<=9;l++){
                if ((pieces[k][l]=='\0')||(pieces[k][l]>='a')){
                    isKing=0;
                    fullPieceCheckWhite(k,l);
                    if (checkMateWhite==1){
                        checkValidBlackStalemate();
                        if (checkBlack==0){
                            return;
                        }
                    }
                }
            }
        }
    }
}

void fullPieceCheckBlack(int x,int y){//check if sent piece is in check or not
    //check if this piece is attacked by opposite knight
    if (pieces[x-1][y+2]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x-1;
        checkingY=y+2;
        return;
    }
    else if (pieces[x+1][y+2]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+1;
        checkingY=y+2;
        return;
    }
    else if (pieces[x-1][y-2]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x-1;
        checkingY=y-2;
        return;
    }
    else if (pieces[x+1][y-2]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+1;
        checkingY=y-2;
        return;
    }
    else if (pieces[x+2][y-1]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+2;
        checkingY=y-1;
        return;
    }
    else if (pieces[x+2][y+1]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+2;
        checkingY=y+1;
        return;
    }
    else if (pieces[x-2][y-1]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x-2;
        checkingY=y-1;
        return;
    }
    else if (pieces[x-2][y+1]=='n'){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x-2;
        checkingY=y+1;
        return;
    }
    else if (((pieces[x+1][y-1]=='p')||(pieces[x+1][y+1]=='p'))&&(isKing)){//check if king is attacked by opposite pawn
        checkBlack=1;
        checkMateBlack=1;
        return;
    }
    //check if king is attacked by opposite king
    else if ((isKing)&&((pieces[x+1][y]=='k')||(pieces[x-1][y]=='k')||(pieces[x][y+1]=='k')||(pieces[x][y-1]=='k')||(pieces[x+1][y+1]=='k')||(pieces[x-1][y-1]=='k')||(pieces[x+1][y-1]=='k')||(pieces[x-1][y+1]=='k'))){
        checkBlack=1;
        checkMateBlack=1;
        return;
    }
    //check if this piece is attacked by opposite pawn
    else if ((pieces[x+1][y-1]=='p')&&(pieces[x][y]>='A')&&(pieces[x][y]<='Z')&&(!isKing)){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+1;
        checkingY=y-1;
        return;
    }
    else if ((pieces[x+1][y+1]=='p')&&(pieces[x][y]>='A')&&(pieces[x][y]<='Z')&&(!isKing)){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+1;
        checkingY=y+1;
        return;
    }
    //check if opposite pawn can move to this empty place
    else if (((pieces[x+1][y]=='p')&&(pieces[x][y]=='\0')&&(!isKing))){
        checkBlack=1;
        checkMateBlack=1;
        checkingX=x+1;
        checkingY=y;
        return;
    }
    else{
        for(i=1; i<=7;i++){//check if this piece is attacked by opposite rook or queen
            if(x+i>9){
                break;
            }
            else if(pieces[x+i][y]!='\0' && (pieces[x+i][y]!='r' && pieces[x+i][y]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x+i][y]=='r'||pieces[x+i][y]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x+i;
                checkingY=y;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x-i<2){
                break;
            }
            else if(pieces[x-i][y]!='\0' && (pieces[x-i][y]!='r' && pieces[x-i][y]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x-i][y]=='r'||pieces[x-i][y]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x-i;
                checkingY=y;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(y+i>9){
                break;
            }
            else if(pieces[x][y+i]!='\0' && (pieces[x][y+i]!='r' && pieces[x][y+i]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x][y+i]=='r'||pieces[x][y+i]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x;
                checkingY=y+i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(y-i<2){
                break;
            }
            else if(pieces[x][y-i]!='\0' && (pieces[x][y-i]!='r' && pieces[x][y-i]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x][y-i]=='r'||pieces[x][y-i]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x;
                checkingY=y-i;
                return;
            }
        }
        for(i=1; i<=7;i++){//check if this piece is attacked by opposite bishop or queen
            if(x+i>9 || y+i>9){
                break;
            }else if(pieces[x+i][y+i]!='\0' && (pieces[x+i][y+i]!='b' && pieces[x+i][y+i]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x+i][y+i]=='b'||pieces[x+i][y+i]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x+i;
                checkingY=y+i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x-i<2 || y+i>9){
                break;
            }else if(pieces[x-i][y+i]!='\0' && (pieces[x-i][y+i]!='b' && pieces[x-i][y+i]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x-i][y+i]=='b'||pieces[x-i][y+i]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x-i;
                checkingY=y+i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x-i<2 || y-i<2){
                break;
            }else if(pieces[x-i][y-i]!='\0' && (pieces[x-i][y-i]!='b' && pieces[x-i][y-i]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x-i][y-i]=='b'||pieces[x-i][y-i]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x-i;
                checkingY=y-i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x+i>9 || y-i<2){
                break;
            }else if(pieces[x+i][y-i]!='\0' && (pieces[x+i][y-i]!='b' && pieces[x+i][y-i]!='q')){
                checkBlack=0;
                checkMateBlack=0;
                break;
            }
            else if(pieces[x+i][y-i]=='b'||pieces[x+i][y-i]=='q'){
                checkBlack=1;
                checkMateBlack=1;
                checkingX=x+i;
                checkingY=y-i;
                return;
            }
        }
    }
}

void fullPieceCheckWhite(int x,int y){
    if (pieces[x-1][y+2]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-1;
        checkingY=y+2;
        return;
    }
    else if (pieces[x+1][y+2]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x+1;
        checkingY=y+2;
        return;
    }
    else if (pieces[x-1][y-2]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-1;
        checkingY=y-2;
        return;
    }
    else if (pieces[x+1][y-2]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x+1;
        checkingY=y-2;
        return;
    }
    else if (pieces[x+2][y-1]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x+2;
        checkingY=y-1;
        return;
    }
    else if (pieces[x+2][y+1]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x+2;
        checkingY=y+1;
        return;
    }
    else if (pieces[x-2][y-1]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-2;
        checkingY=y-1;
        return;
    }
    else if (pieces[x-2][y+1]=='N'){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-2;
        checkingY=y+1;
        return;
    }
    else if (((pieces[x-1][y-1]=='P')||(pieces[x-1][y+1]=='P'))&&(isKing)){
        checkWhite=1;
        checkMateWhite=1;
        return;
    }
    else if ((isKing)&&((pieces[x+1][y]=='K')||(pieces[x-1][y]=='K')||(pieces[x][y+1]=='K')||(pieces[x][y-1]=='K')||(pieces[x+1][y+1]=='K')||(pieces[x-1][y-1]=='K')||(pieces[x+1][y-1]=='K')||(pieces[x-1][y+1]=='K'))){
        checkWhite=1;
        checkMateWhite=1;
        return;
    }
    else if ((pieces[x-1][y-1]=='P')&&(pieces[x][y]>='a')&&(pieces[x][y]<='z')&&(!isKing)){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-1;
        checkingY=y-1;
        return;
    }
    else if ((pieces[x-1][y+1]=='P')&&(pieces[x][y]>='a')&&(pieces[x][y]<='z')&&(!isKing)){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-1;
        checkingY=y+1;
        return;
    }
    else if (((pieces[x-1][y]=='P')&&(pieces[x][y]=='\0')&&(!isKing))){
        checkWhite=1;
        checkMateWhite=1;
        checkingX=x-1;
        checkingY=y;
        return;
    }
    else{
        for(i=1; i<=7;i++){
            if(x+i>9){
                break;
            }
            else if(pieces[x+i][y]!='\0' && (pieces[x+i][y]!='R' && pieces[x+i][y]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x+i][y]=='R'||pieces[x+i][y]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x+i;
                checkingY=y;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x-i<2){
                break;
            }
            else if(pieces[x-i][y]!='\0' && (pieces[x-i][y]!='R' && pieces[x-i][y]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x-i][y]=='R'||pieces[x-i][y]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x-i;
                checkingY=y;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(y+i>9){
                break;
            }
            else if(pieces[x][y+i]!='\0' && (pieces[x][y+i]!='R' && pieces[x][y+i]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x][y+i]=='R'||pieces[x][y+i]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x;
                checkingY=y+i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(y-i<2){
                break;
            }
            else if(pieces[x][y-i]!='\0' && (pieces[x][y-i]!='R' && pieces[x][y-i]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x][y-i]=='R'||pieces[x][y-i]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x;
                checkingY=y-i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x+i>9 || y+i>9){
                break;
            }else if(pieces[x+i][y+i]!='\0' && (pieces[x+i][y+i]!='B' && pieces[x+i][y+i]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x+i][y+i]=='B'||pieces[x+i][y+i]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x+i;
                checkingY=y+i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x-i<2 || y+i>9){
                break;
            }else if(pieces[x-i][y+i]!='\0' && (pieces[x-i][y+i]!='B' && pieces[x-i][y+i]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x-i][y+i]=='B'||pieces[x-i][y+i]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x-i;
                checkingY=y+i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x-i<2 || y-i<2){
                break;
            }else if(pieces[x-i][y-i]!='\0' && (pieces[x-i][y-i]!='B' && pieces[x-i][y-i]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x-i][y-i]=='B'||pieces[x-i][y-i]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x-i;
                checkingY=y-i;
                return;
            }
        }
        for(i=1; i<=7;i++){
            if(x+i>9 || y-i<2){
                break;
            }else if(pieces[x+i][y-i]!='\0' && (pieces[x+i][y-i]!='B' && pieces[x+i][y-i]!='Q')){
                checkWhite=0;
                checkMateWhite=0;
                break;
            }
            else if(pieces[x+i][y-i]=='B'||pieces[x+i][y-i]=='Q'){
                checkWhite=1;
                checkMateWhite=1;
                checkingX=x+i;
                checkingY=y-i;
                return;
            }
        }
    }
}

void gameOver(){
    printf("For new game press \"N\" / to load a game press \"L\" / to exit press \"E\".\n");
    readLine(play,5);
    while ((play[1]!='\0')||((play[0]!='N')&&(play[0]!='L')&&(play[0]!='E'))){//options after the game is over
        printf("Wrong input.\n");
        printf("For new game press \"N\" / to load a game press \"L\" / to exit press \"E\".\n");
        readLine(play,5);
    }
}

void saveGame(){
    printf("Enter the file name.\n");
    char saveName[100];
    readLine(saveName,100);
    fPointer = fopen(saveName,"w");
    //saving the current turn
    fputc(turn,fPointer);
    //saving pieces
    for (i=2;i<=9;i++){
        for (j=2;j<=9;j++){
            fputc(pieces[i][j],fPointer);
        }
    }
    //saving dead pieces
    for (i=0;i<37;i++){
    fputc(deadPieces[i],fPointer);
    }
    //saving undo
    for (i=0;i<500;i++){
        for (j=0;j<6;j++){
            fputc(undo[i][j],fPointer);
        }
    }
    //saving redo
    for (i=0;i<500;i++){
        for (j=0;j<5;j++){
            fputc(redo[i][j],fPointer);
        }
    }
    //saving deadPieces index
    fputc(died,fPointer);
    fputc(promotedLast,fPointer);
    fputc(diedLast,fPointer);
    fclose(fPointer);
    printf("Game successfully saved.\n");
}

void loadGame(){
    //loading turn
    turn=fgetc(fPointer);
    //load pieces
    for (i=2;i<=9;i++){
        for (j=2;j<=9;j++){
            pieces[i][j]=fgetc(fPointer);
        }
    }
    //load deadPieces
    for (i=0;i<37;i++){
        deadPieces[i]=fgetc(fPointer);
    }
    //load undo
    for (i=0;i<500;i++){
        for (j=0;j<6;j++){
            undo[i][j]=fgetc(fPointer);
        }
    }
    //load redo
    for (i=0;i<500;i++){
        for (j=0;j<5;j++){
            redo[i][j]=fgetc(fPointer);
        }
    }
    //load diedIndex
    died=fgetc(fPointer);
    promotedLast=fgetc(fPointer);
    diedLast=fgetc(fPointer);
    fclose(fPointer);
    printGame();
    kingsIndex();
    printf("Game loaded successfully.\n");
    isKing=1;//display check if the king is in check
    fullPieceCheckBlack(king2x,king2y);
    if (checkBlack==1){
        printf("Check black.\n");
    }
    isKing=1;
    fullPieceCheckWhite(king1x,king1y);
    if (checkWhite==1){
        printf("Check white.\n");
    }

}

void checkValidWhiteStalemate(){
    char tmp;
    int x=checkingX;
    int y=checkingY;
    tmp=pieces[x][y];
    pieces[x][y]='\0';
    fullPieceCheckWhite(king1x,king1y);
    pieces[x][y]=tmp;
}

void checkValidBlackStalemate(){
    char tmp;
    int x=checkingX;
    int y=checkingY;
    tmp=pieces[x][y];
    pieces[x][y]='\0';
    fullPieceCheckBlack(king2x,king2y);
    pieces[x][y]=tmp;
}
