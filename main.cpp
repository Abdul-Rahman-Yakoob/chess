/*

    TO DO:
        1) Pawn Promotion - DONE
        2) Castling - DONE
        3) En Passant
        3) Optimising and restructuring the code
        4) Computer Moves

*/

/*
    FUNCTIONS:
        1) getPos(int piecVal) : returns the position of a piece
                Arguments: int pieceVal (The piece whose position is to be found)
                Return type: std::pair<int,int> (it returns the row and column in which the piece is placed)
                Time Complexity: O(1)
                Space Complexity: O(1)
        
        2) promotePawn(int pieceVal,int newX,int newY): Takes user input for new piece after
                                                        pawn promotion and replaces pawn with it
                Return type: void
                Time Complexity: Infinite while loop to select piece
                Space Complexity: O(1)
        3) castle(int pieceVal,int oldX,int oldY,int newX,int newY): This changes the position of
                                                                     the rook and the king
                Return type: void
        4) isCheck(int turn,int pieceVal,int newX,int newY,int oldX,int oldY)
*/

#include<iostream>
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>

const int max_depth = 5;
bool canCastleWhite = true,canCastleBlack = true;
sf::Sprite f[32];
int size = 333;
sf::Sound captureSound;
bool checkmate = false;
std::string winner = "";
int board[8][8] = {
     5, 4, 3, 1, 2, 3, 4, 5,
     6, 6, 6, 6, 6, 6, 6, 6,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
    -6,-6,-6,-6,-6,-6,-6,-6,
    -5,-4,-3,-1,-2,-3,-4,-5
};

bool useless = false;

std::pair<int,int> getPos(int pieceVal){
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            std::cout<<board[i][j]<<" ";
            if(board[i][j] == pieceVal){
                std::cout<<"returning "<<i<<" "<<j;
                return std::make_pair(i,j);
            }
        }
        std::cout<<"\n";
    }
    std::cout<<"\n"<<pieceVal<<" has not been detected!!!\n";
    return std::make_pair(-1,-1);
}

void promotePawn(int pieceVal,int newX,int newY){
    int newPiece = 2*pieceVal/6; // assuming only queen is made
    // std::cin>>newPiece;
    // newPiece *= pieceVal/6;
    sf::RenderWindow windowForPieceSelection(sf::VideoMode(500,100),"Select Piece");
    sf::Sprite pieces;
    sf::Texture piecesTexture;
    piecesTexture.loadFromFile("textures/basics.png");
    pieces.setTexture(piecesTexture);
    pieces.setScale(sf::Vector2f(0.26f,0.26f));
    bool pieceSelected = false;
    while(windowForPieceSelection.isOpen()){
        std::cout<<"Window is open\n";
        sf::Event event;
        while(windowForPieceSelection.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                windowForPieceSelection.close();
            }
            if(event.type == sf::Event::MouseButtonPressed){
                int index = int(sf::Mouse::getPosition(windowForPieceSelection).x*6/500);
                std::cout<<"INDEX: "<<index<<"\n";
                if(index == 0 || index == 5)
                continue;
                else{
                    newPiece = (index+1) * newPiece/2;
                    windowForPieceSelection.close();
                    pieceSelected = true;
                    break;
                }
            }
        }
        if(pieceSelected)
        break;
        windowForPieceSelection.clear();
        windowForPieceSelection.draw(pieces);
        windowForPieceSelection.display();
    }
    board[newX][newY] = newPiece;
    for(int i=0;i<32;i++){
        if(int(floor(f[i].getPosition().y/100)) == newX && int(floor(f[i].getPosition().x/100)) == newY){
            f[i].setScale(sf::Vector2f(0,0));
            int xpos = size * (abs(newPiece)-1);
            int ypos = newPiece<0?size:0;
            f[i].setTextureRect(sf::IntRect(xpos,ypos,size,size));
            f[i].setScale(sf::Vector2f(0.3f,0.3f));
            f[i].setPosition(sf::Vector2f(100*newY,100*newX));
            return;
        }
    }
    std::cout<<"\n\nQUEEN HAS BEEN MADE\n\n";
}

void castle(int pieceVal,int oldX,int oldY,int newX,int newY){
    for(int i=0;i<32;i++){
        if(int(floor(f[i].getPosition().y/100)) == oldX && int(floor(f[i].getPosition().x/100)) == oldY){
            f[i].setPosition(sf::Vector2f(100*newY,100*newX));
            return;
        }
    }
}

bool isCheck(int turn,int pieceVal,int newX,int newY,int oldX,int oldY){
    int temp = board[newX][newY];
    board[oldX][oldY] = 0;
    board[newX][newY] = pieceVal;
    
    std::cout<<"Checking the Check condition!!\n";
    std::pair<int,int> kingPos = getPos(turn);
    int r = kingPos.first;
    int c = kingPos.second;
    std::cout<<"King POS "<<r<<" "<<c<<"\n";
    int r1 = r + turn;
    int c1 = c + turn;
    
    //  DID OPP KING CHECK??
    for(int i = r-1;i<=r+1;i++){
        for(int j = c-1;j<=c+1;j++){
            if(i>=0&&i<8&&j>=0&&j<8&&board[i][j] == turn*-1){
                std::cout<<"Opposite King causing check\n";
                board[newX][newY] = temp;
                board[oldX][oldY] = pieceVal;
                return true;
            }
        }
    }

    //DID OPP KNIGHT CHECK??
    if(r-1 >=0 && c-2 >= 0 && board[r-1][c-2] == turn*-1*4){
        std::cout<<board[r-1][c-2]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r-1 >=0 && c+2 < 8 && board[r-1][c+2] == turn*-1*4){
        std::cout<<board[r-1][c+2]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r+1 < 8 && c-2 >= 0 && board[r+1][c-2] == turn*-1*4){
        std::cout<<board[r+1][c-2]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r+1 < 8 && c+2 >= 0 && board[r+1][c+2] == turn*-1*4){
        std::cout<<board[r+1][c+2]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r+2 < 8 && c+1 < 8 && board[r+2][c+1] == turn*-1*4){
        std::cout<<board[r+2][c+1]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r+2 < 8 && c-1 >= 0 && board[r+2][c-1] == turn*-1*4){
        std::cout<<board[r+2][c-1]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r-2 >= 0 && c+1 < 8 && board[r-2][c+1] == turn*-1*4){
        std::cout<<board[r-2][c+1]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    if(r-2 >= 0 && c-1 >= 0 && board[r-2][c-1] == turn*-1*4){
        std::cout<<board[r-2][c-1]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }

    if(r1 < 8 && r1 >= 0 && c1 < 8 && c1 >= 0 && (board[r1][c1] == 6*turn*-1 )){
        std::cout<<board[r1][c1]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    c1 -= 2*turn;
    if(r1 < 8 && r1 >= 0 && c1 < 8 && c1 >= 0 && (board[r1][c1] == 6*turn*-1 )){
        std::cout<<board[r1][c1]<<": caused Check\n";
        board[newX][newY] = temp;
        board[oldX][oldY] = pieceVal;
        return true;
    }
    // c
    // if(r1 < 8 && r1 >= 0 && c1 < 8 && c1 >= 0 && (board[r1][c1] == 6*turn*-1 || (board[r1][c1] == 1*turn*-1 ))){
    //     std::cout<<board[r1][c1]<<": caused Check\n";
    //     board[newX][newY] = temp;
    //     return true;
    // }
    // if(r1 < 8 && r1 >= 0 && c1 < 8 && c1 >= 0 && (board[r1][c1] == 6*turn*-1 || (board[r1][c1] == 1*turn*-1 ))){
    //     std::cout<<board[r1][c1]<<": caused Check\n";
    //     board[newX][newY] = temp;
    //     return true;
    // }
    
    for(int i=r+1;i<8;i++){
        if(board[i][c] == 5*turn*-1||board[i][c] == 2*turn*-1){
            std::cout<<board[i][c]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[i][c])
        break;
    }
    for(int i=r-1;i>=0;i--){
        if(board[i][c] == 5*turn*-1||board[i][c] == 2*turn*-1){
            std::cout<<board[i][c]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[i][c])
        break;
    }
    for(int i=c+1;i<8;i++){
        if(board[r][i] == 5*turn*-1||board[r][i] == 2*turn*-1){
            std::cout<<board[r][i]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[r][i])
        break;
    }
    for(int i=c-1;i>=0;i--){
        if(board[r][i] == 5*turn*-1||board[r][i] == 2*turn*-1){
            std::cout<<board[r][i]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[r][i])
        break;
    }
    for(int i=r+1,j=c+1;i<8,j<8;i++,j++){
        if(board[i][j] == 2*turn*-1||board[i][j] == 3*turn*-1){
            std::cout<<board[i][j]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[i][j])
        break;
    }
    for(int i=r-1,j=c-1;i>=0,j>=0;i--,j--){
        if(board[i][j] == 2*turn*-1||board[i][j] == 3*turn*-1){
            std::cout<<board[i][j]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[i][j])
        break;
    }
    for(int i=r-1,j=c+1;i>=0,j<8;i--,j++){
        if(board[i][j] == 2*turn*-1||board[i][j] == 3*turn*-1){
            std::cout<<board[i][j]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[i][j])
        break;
    }
    for(int i=r+1,j=c-1;i<8,j>=0;i++,j--){
        if(board[i][j] == 2*turn*-1||board[i][j] == 3*turn*-1){
            std::cout<<board[i][j]<<": caused Check\n";
            board[newX][newY] = temp;
            board[oldX][oldY] = pieceVal;
            return true;
        }
        else if(board[i][j])
        break;
    }
    board[newX][newY] = temp;
    board[oldX][oldY] = pieceVal;
    return false;
}

bool isValid(int pieceVal,int oldX,int oldY,int newX,int newY,bool& captured){
    // return false;
    bool valid = true;
    int diffX = abs(newX - oldX);
    int diffY = abs(newY - oldY);
    int startX = oldX;
    int startY = oldY;
    int dirX = (newX - oldX)/abs(newX - oldX);
    int dirY = (newY - oldY)/abs(newY - oldY);
    std::cout<<"#####################################################&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n";
    std::cout<<"Differences \n";
    std::cout<<diffX<<" "<<diffY<<"\n";
    if(abs(board[newX][newY]) == 1){
        return false;
    }
    if(board[oldX][oldY]*board[newX][newY] > 0){
        return false;
    }
    board[oldX][oldY] = 0;
    switch (abs(pieceVal)){
                    case 1:
                        if(diffY == 2 && diffX == 0 && oldY == 3  && ((pieceVal < 0 && oldX == 7) || (pieceVal > 0 && oldX == 0))){
                            //trying to castle
                            if(oldY > newY && abs(board[oldX][0]) != 5){
                                std::cout<<"King trying to castle without rook on left\n";
                                valid = false;
                                break;
                            }
                            if(oldY < newY && abs(board[oldX][7]) != 5){
                                std::cout<<"King trying to castle without rook on right\n";
                                valid = false;
                                break;
                            }
                            if(oldY > newY){
                                for(int i = oldY-1;i>=1;i--){
                                    if(board[oldX][i] != 0){
                                        std::cout<<"King trying to castle to left through other pieces\n";
                                        valid = false;
                                        break;
                                    }
                                }
                            }
                            else{
                                for(int i = oldY+1;i<=6;i++){
                                    if(board[oldX][i] != 0){
                                        std::cout<<"King trying to castle to right through other pieces\n";
                                        valid = false;
                                        break;
                                    }
                                }
                            }
                            return true;
                        }
                        if(pieceVal*board[newX][newY] > 0){
                            std::cout<<"King trying to kill friends\n";
                            valid = false;
                            break;
                        }
                        if(diffX > 1 || diffY > 1){
                            std::cout<<"king trying to move to a square too far\n";
                            valid = false;
                            break;
                        }
                        if(board[newX][newY]){
                            captured = true;
                            // capture(pieceMoving,newX,newY);
                        }
                        break;
                    
                    case 2:
                        if(pieceVal*board[newX][newY] > 0){
                            valid = false;
                            break;
                        }
                        if(diffX != diffY && diffX != 0 && diffY != 0){
                            valid = false;
                            break;
                        }
                        startX += dirX;
                        startY += dirY;
                        std::cout<<"Dir "<<dirX<<" "<<dirY<<"\n";
                        while(startX != newX && startY != newY){
                            if(board[startX][startY]){
                                valid = false;
                                break;
                            }
                            startX += dirX;
                            startY += dirY;
                        }
                        while(startX != newX){
                            if(board[startX][startY]){
                                valid = false;
                                break;
                            }
                            startX += dirX;
                        }
                        while(startY != newY){
                            if(board[startX][startY]){
                                valid = false;
                                break;
                            }
                            startY += dirY;
                        }
                        if(!valid)
                        break;
                        if(board[newX][newY]){
                            captured = true;
                            // capture(pieceMoving,newX,newY);
                        }
                        break;
                    
                    case 3:
                        if(pieceVal*board[newX][newY] > 0){
                            valid = false;
                            break;
                        }
                        if(diffX != diffY){
                            valid = false;
                            break;
                        }
                        startX += dirX;
                        startY += dirY;
                        while(startX != newX && startY != newY){
                            if(board[startX][startY]){
                                valid = false;
                                break;
                            }
                            startX += dirX;
                            startY += dirY;
                        }
                        if(!valid)
                        break;
                        if(board[newX][newY]){
                            captured = true;
                            // capture(pieceMoving,newX,newY);
                        }
                        break;

                    case 4:
                        if(pieceVal*board[newX][newY] > 0){
                            valid = false;
                            break;
                        }
                        if((diffX == 2 && diffY == 1)||(diffY == 2 && diffX == 1)){
                            if(board[newX][newY]){
                                captured = true;
                                // capture(pieceMoving,newX,newY);
                            }
                        }
                        else{
                            valid = false;
                        }
                        break;
                    
                    case 5:
                        if(diffX >= 1 && diffY >= 1){
                            std::cout<<"Rook trying to move in non-linear format\n";
                            valid = false;
                            break;
                        }
                        if(diffX > 0){
                            int start = oldX>newX?newX:oldX;
                            int end = oldX>newX?oldX:newX;
                            for(int i = start + 1; i < end; i++){
                                if(board[i][oldY] != 0){
                                    valid = false;
                                    std::cout<<"Rook trying to go through other peices\n";
                                    break;
                                }
                            }
                            if(!valid)
                            break;
                            if(board[newX][newY] != 0){
                                if(pieceVal*board[newX][newY] < 0){
                                    captured = true;
                                    // capture(pieceMoving,newX,newY);
                                }
                                else{
                                    valid = false;
                                    std::cout<<"Rook trying to kill friends\n";
                                }
                            }
                        }
                        if(diffY > 0){
                            int start = oldY>newY?newY:oldY;
                            int end = oldY>newY?oldY:newY;
                            for(int i = start + 1; i < end; i++){
                                if(board[oldX][i] != 0){
                                    valid = false;
                                    std::cout<<"Rook trying to go through other peices\n";
                                    break;
                                }
                            }
                            if(!valid)
                            break;
                            if(board[newX][newY] != 0){
                                if(pieceVal*board[newX][newY] < 0){
                                    captured = true;
                                    // capture(pieceMoving,newX,newY);
                                }
                                else{
                                    valid = false;
                                    std::cout<<"Rook trying to kill friends\n";
                                }
                            }
                        }
                        break;
                    
                    case 6:// PAWN 
                        if((diffY > 1 || diffX > 2)){
                            std::cout<<"Pawn trying to move too far\n";
                            valid = false;
                            break;
                        }
                        if((diffX == 2) && diffY == 0){
                            if(pieceVal < 0 && oldX != 6){
                                std::cout<<"Black pawn trying to move 2 squares not from start\n";
                                valid = false;
                                break;
                            }
                            if(pieceVal > 0 && oldX != 1){
                                std::cout<<"White pawn trying to move 2 squares not from start\n";
                                valid = false;
                                break;
                            }
                        }
                        else if(diffX == 2 && diffY > 0){
                            valid = false;
                            break;
                        }
                        if(pieceVal < 0 && oldX < newX){
                            std::cout<<"pawn trying to move backwards(1)\n";
                            valid = false;
                            break;
                        }
                        if(pieceVal > 0 && oldX > newX){
                            std::cout<<"pawn trying to move backwards(2)\n";
                            valid = false;
                            break;
                        }
                        if(diffY){
                            if(diffX){
                                if(board[newX][newY] && board[newX][newY]*pieceVal < 0){
                                    captured = true;
                                    // capture(pieceMoving,newX,newY);
                                }
                                else{
                                    if(!board[newX][newY]){
                                        std::cout<<"Pawn trying to move to diagonal without enemy\n";
                                    }
                                    else{
                                        std::cout<<"Pawn trying to kill friends\n";
                                    }
                                    valid = false;
                                    break;
                            }
                            }
                            else{
                                std::cout<<"Pawn trying to move to adjacent square\n";
                                valid = false;
                                break;
                            }
                        }
                        else if(diffX){
                            if(board[newX][newY]){
                                std::cout<<"Pawn trying to move into another piece in front\n";
                                valid = false;
                            }
                        }
                        break;
                }
    board[oldX][oldY] = pieceVal;
    return valid;
}

bool isCheckMate(int king){
    std::cout<<king<<" ";
    int r = getPos(king).first;
    int c = getPos(king).second;
    std::cout<<"is Check mate "<<r<<" "<<c<<"\n";
    if(!isCheck(king,king,r,c,r,c)){
        std::cout<<"No cm detected as no check\n";
        return false;
    }
    std::cout<<"Currently in check\n";
    for(int i = 0;i<8;i++){
        for(int j = 0;j<8;j++){
            if(king*board[i][j] > 0){
                bool f;
                for(int k1=0;k1<8;k1++){
                    for(int k2=0;k2<8;k2++){
                        if(isValid(board[i][j],i,j,k1,k2,useless) && !isCheck(king,board[i][j],k1,k2,i,j)){
                            std::cout<<"can move "<<board[i][j]<<" from "<<i<<" "<<j<<" to "<<k1<<" "<<k2<<"\n";
                            return false;
                        }
                    }
                }
            }
        }
    }
    std::cout<<"CHECK mate DETECTED\n";
    return true;
}


void loadTextures(){
    int k = 0;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            int val = board[i][j];
            if(val == 0)
            continue;
            int xpos = size * (abs(val)-1);
            int ypos = val<0?size:0;
            f[k].setTextureRect(sf::IntRect(xpos,ypos,size,size));
            f[k].setScale(sf::Vector2f(0.3f,0.3f));
            f[k].setPosition(sf::Vector2f(100*j,100*i));
            // f[k].setOrigin(sf::Vector2f(size/6,size/6));
            k++;
        }
    }
}

void capture(int thisPiece ,int xpos ,int ypos){
    if(abs(board[xpos][ypos]) == 1){
        std::cout<<"#######      OMG KING IS BEING TAKEN  #################\n";
    }
    for(int i=0;i<32;i++){
        if(i != thisPiece && int(floor(f[i].getPosition().y/100)) == xpos && int(floor(f[i].getPosition().x/100)) == ypos){
            f[i].setScale(sf::Vector2f(0,0));
        }
    }
    captureSound.play();
}

int main(){
    sf::Texture t1; // texture for pieces
    sf::Texture t2; // texture for board
    t1.loadFromFile("textures/basics.png");
    t2.loadFromFile("textures/chess-board.png");
    sf::Sprite chessBoard;
    for(int i=0;i<32;i++){
        f[i].setTexture(t1);
    }
    chessBoard.setTexture(t2);
    chessBoard.setScale(sf::Vector2f(0.95f,0.95f));
    
    loadTextures();

    //      AUDIO 
    sf::SoundBuffer captureBuf;
    sf::SoundBuffer moveBuf;
    sf::SoundBuffer illegalBuf;
    captureBuf.loadFromFile("audio/capture.mp3");
    moveBuf.loadFromFile("audio/move.mp3");
    illegalBuf.loadFromFile("audio/illegal.mp3");

    sf::Sound moveSound;
    sf::Sound illegalSound;

    captureSound.setBuffer(captureBuf);
    moveSound.setBuffer(moveBuf);
    illegalSound.setBuffer(illegalBuf);

    sf::RenderWindow window(sf::VideoMode(810,810),"Chess");

    bool Moving = false;
    int pieceMoving = -1;
    int pieceVal;

    int oldX,oldY,newX,newY;
    int turn = 1;
    while(window.isOpen()/*&&!checkmate*/){
        // if(checkmate){
        //     window.close();
        //     break;
        // }
        // if(isCheckMate(turn)){
        //     std::cout<<"@@@@@@@@@   CHECK-MATE    @@@@@@@@@@@\n";
        //     window.close();
        //     break;
        // }
        // else{
        //     std::cout<<"###########   NO_CHECK_MATE  ##########\n";
        // }
        bool skipTurn = false;
        float dx,dy;
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                window.close();
            }
            if(event.type == sf::Event::MouseButtonPressed){
                std::cout<<"Mouse click detected\n";
                std::cout<<pos.x<<" "<<pos.y<<window.getPosition().x<<"\n";
                for(int i=0;i<32;i++){ 
                    // std::cout<<"Piece "<<i<<" "<<f[i].getGlobalBounds().contains(sf::Vector2f(pos.x,pos.y))<<"\n";
                    if(f[i].getGlobalBounds().contains(sf::Vector2f(pos.x,pos.y))){
                        pieceVal = board[int(floor(f[i].getPosition().y/100))][int(floor(f[i].getPosition().x/100))];
                        if(pieceVal*turn < 0)
                        continue;
                        // std::cout<<pieceVal<<" tried to move when turn is "<<turn<<"\n";
                        oldX = int(floor(f[i].getPosition().y/100));
                        oldY = int(floor(f[i].getPosition().x/100));
                        // board[oldX][oldY] = 0;
                        // std::cout<<"trying to moving piece "<<pieceVal<<"\n";
                        // f[pieceMoving].setPosition((sf::Vector2f)pos-sf::Vector2f(50,50));
                        Moving = true;
                        pieceMoving = i;
                        // if(pieceVal * turn < 0){
                        //     skipTurn = true;
                        //     std::cout<<pieceVal<<" not allowed to move\n";
                        //     break;
                        // }
                    }
                }
            }
            // if(skipTurn)
            // continue;
            if(event.type == sf::Event::MouseButtonReleased){
                if(!pieceVal || pieceVal*turn<0){
                    // std::cout<<pieceVal<<" "<<turn<<" ,so rejected turn\n";
                    continue;
                }
                bool captured = false;
                f[pieceMoving].setPosition(sf::Vector2f(floor((f[pieceMoving].getPosition().x+50)/100)*100,floor((f[pieceMoving].getPosition().y+50)/100)*100));
                newX = int(floor(f[pieceMoving].getPosition().y/100));
                newY = int(floor(f[pieceMoving].getPosition().x/100));
                // std::cout<<diffX<<" "<<diffY<<"\n";
                bool valid = isValid(pieceVal,oldX,oldY,newX,newY,captured);
                bool castling = false;
                if(valid && abs(pieceVal) == 1 && abs(oldY - newY) == 2){
                    //Castling
                    if(oldY > newY){
                        for(int i = oldY;i>=newY;i--){
                            valid = !isCheck(turn,pieceVal,oldX,i,oldX,oldY);
                            if(!valid)
                            {
                                std::cout<<"changing king position to "<<oldX<<" "<<i<<"causes check\n";
                                break;
                            }
                        }
                    }
                    else{
                        for(int i = oldY;i<=newY;i++){
                            valid = !isCheck(turn,pieceVal,oldX,i,oldX,oldY);
                            if(!valid)
                            {
                                std::cout<<"changing king position to "<<oldX<<" "<<i<<"causes check\n";
                                break;
                            }
                        }
                    }
                    if((turn>0 && !canCastleWhite) || (turn<0 && !canCastleBlack)){
                        std::cout<<"Player has already castled once\n";
                        valid = false;
                    }
                    else{
                        if(turn > 0){
                            canCastleWhite = false;
                        }
                        else{
                            canCastleBlack = false;
                        }
                    }
                    castling = true;
                }
                // if(turn*pieceVal < 0){
                //     Moving = false;
                //     continue;
                // }
                if(valid){
                    valid = !isCheck(turn,pieceVal,newX,newY,oldX,oldY);
                    if(!valid){
                        std::cout<<"@@@@@@@ Check @@@@@@@";
                        if(isCheckMate(turn)){
                            checkmate = true;
                            // std::cout<<"NO HERE";
                            std::cout<<"@@@@@@@@@   CHECK-MATE    @@@@@@@@@@@\n";
                            std::cout<<"########-----   ";
                            if(turn == 1){
                                std::cout<<"BLACK";
                                winner = "BLACK";
                            }
                            else{
                                winner = "WHITE";
                                std::cout<<"WHITE";
                            }
                            std::cout<<"-WINS   ########";
                            // window.close();
                            // break;
                        }
                    }
                }
                if(valid && captured){
                    if(abs(board[newX][newY]) == 1){
                        std::cout<<pieceVal<<" is trying to kill king "<<board[newX][newY]<<"\n";
                        break;
                    }
                    capture(pieceMoving,newX,newY);
                }
                if(!valid){
                    illegalSound.play();
                    newX = oldX;
                    newY = oldY;
                    f[pieceMoving].setPosition(sf::Vector2f(newY*100,newX*100));
                    std::cout<<"Invalid Move\n";
                }
                if(valid && castling){
                    if(oldY > newY){
                        board[oldX][0] = 0;
                        board[oldX][newY+1] = pieceVal * 5;
                        castle(pieceVal,oldX,0,newX,newY+1);
                    }
                    else{
                        board[oldX][7] = 0;
                        board[oldX][newY-1] = pieceVal * 5;
                        castle(pieceVal,oldX,7,newX,newY-1);
                    }
                }
                if(valid && (oldX != newX || oldY != newY)){
                    moveSound.play();
                    board[oldX][oldY] = 0;
                    turn *= -1;
                }
                board[newX][newY] = pieceVal;
                if(abs(pieceVal) == 6){
                    if((pieceVal < 0 && newX == 0) || (pieceVal > 0 && newX == 7)){
                        promotePawn(pieceVal,newX,newY);
                    }
                }
                if(isCheckMate(turn*-1)){
                    // std::cout<<"HERE";
                        // std::cout<<"@@@@@@@@@   CHECK-MATE    @@@@@@@@@@@\n";
                        // std::cout<<"########-----   ";
                        checkmate = true;
                        if(turn == 1)
                        winner = "WHITE";
                        else
                        winner = "BLACK";
                        // std::cout<<"-WINS   ########";
                                // window.close();
                        // break;
                    }
                    else{
                        std::cout<<"###########   NO_CHECK_MATE  ##########\n";
                    }
                std::cout<<"\n";
                for(int i=0;i<8;i++){
                    for(int j=0;j<8;j++){
                        std::cout<<board[i][j]<<" ";
                    }
                    std::cout<<"\n";
                }
                Moving = false;
            }
        }
        if(Moving){
            // std::cout<<"Moving piece: "<<pieceMoving<<"\n";
            f[pieceMoving].setPosition((sf::Vector2f)pos-sf::Vector2f(50,50));
        }
        if(checkmate == true && winner == "WHITE"){
            window.clear(sf::Color::White);
        }
        else if(checkmate == true && winner == "BLACK"){
            window.clear(sf::Color::Black);
        }
        else if(checkmate){
            window.clear(sf::Color::Red);
        }
        else{
            window.clear();
        window.draw(chessBoard);
        for(int i=0;i<32;i++){
            window.draw(f[i]);
        }
        }
        window.display();
    }
    if(!checkmate){
        std::cout<<"************* DRAW ****************\n";
        return 0;
    }
    std::cout<<"@@@@@@@@@   CHECK-MATE    @@@@@@@@@@@\n";
    std::cout<<"########-----   ";
    std::cout<<winner;
    std::cout<<"-WINS   ########";
    return 0;
}
