#include <SDL3/SDL.h>

#include <iostream>
using namespace std;




struct Command
{
    int x;
    int y;
    int direction;
};


struct SnakeSegment
{
    int posX;
    int posY;
    int direction;
    int type; //3 - tail, 2 - head, 1 - body 
};


struct Snake
{
    SnakeSegment* segments;
    int length;
    Command* commands;
    int command_length = 0;

    Snake(int length, int startX, int startY)
    {
        //This one should throw when provided with a length < 1
        this->length = length;
        segments = new SnakeSegment[length];
        
        segments[0] = {startX, startY, -1 ,2}; //SnakeSegment

        

    }


    void addSegment()
    {

        SnakeSegment* newsegments = new SnakeSegment[(this->length)+1];

        SnakeSegment penultimateSegment;
        SnakeSegment lastSegment = this->segments[this->length-1];
        //Copy old values
        for(int i = 0; i < this->length; i++)
        {
            newsegments[i] = this->segments[i];
            //cout << "Copying " << this->segments[i].type << " to newsegments at " << i << endl;  

        }

        newsegments[this->length] = {lastSegment.posX,
                                     lastSegment.posY,
                                     -1,
                                     3};
        
        //Add logic to check for head segment if length is less than 2 and to update the types accordingly
        if(this->length + 1 > 2)
        {
            newsegments[this->length - 1].type = 1;
           
        }

        this->length = this->length+1;                                     
        delete[] this->segments;

        this->segments = newsegments;
        

        
    }
    
    void addCommand(int x, int y, int direction)
    {
        
        Command* temp = new Command[command_length+1];

        //Copy old values
        for(int i = 0; i < command_length; i++)
        {
            temp[i] = commands[i];
        }
        
        temp[command_length] = {x,y,direction};

        this->commands = new Command[command_length+1];

        for(int i = 0; i < command_length + 1; i++)
        {
            this->commands[i] = temp[i];
            
        }
       
        delete[] temp;
        this->command_length++;

        
    }

    void updateField(int** m, int dimX, int dimY)
    {
        SnakeSegment currentSegment;
        int posX; int posY;
        for(int i = 0; i < length; i++)
        {
            currentSegment = this->segments[i];
            posX = currentSegment.posX;
            posY = currentSegment.posY;


            m[posX][posY] = currentSegment.type;


        }

    }

   


    void setHeadDirection(int dir, int** directionMatrix)
    {
        /*
        0 - UP
        1 - DOWN
        2 - LEFT
        3 - RIGHT
        */ 
        SnakeSegment* headPtr = &this->segments[0];

        
        int currentDir = (*headPtr).direction;

        int currentI = (*headPtr).posX;
        int currentJ = (*headPtr).posY;

        if(currentDir == dir)
        {
            return;
        }
        
        if(this->length == 1)
        {
            (*headPtr).direction = dir;
            this->addCommand(currentI, currentJ, dir);
                                

            return;
        }
        


        switch(currentDir)
        {
            case 0:
                if(dir != 1) (*headPtr).direction = dir; 
                this->addCommand(currentI, currentJ, dir);
                break;
            case 1:
                if(dir != 0) (*headPtr).direction = dir;
                this->addCommand(currentI, currentJ, dir);
                break;
            case 2:
                if(dir != 3) (*headPtr).direction = dir;
                this->addCommand(currentI, currentJ, dir);
                break;
            case 3:
                if(dir != 2) (*headPtr).direction = dir;
                this->addCommand(currentI, currentJ, dir);
                break;
            
            
        }
        
        if(this->length > 1 ) directionMatrix[currentI][currentJ] = (*headPtr).direction;


        return;
        
    }

    void getNextCell(int posX, int posY, int direction, int& nextX, int& nextY, int dimX, int dimY)
    {
        int newI = posX;
        int newJ = posY;

        if(direction == -1)
        {
            return;
        }

        switch(direction)
        {
            case 0:
                if(newI - 1 >= 0) newI--;
                break;
            case 1:
                if(newI + 1 < dimX) newI++;
                break;
            case 2:
                if(newJ - 1 >= 0) newJ--;
                break;
            case 3:
                if(newJ + 1 < dimY) newJ++;
                break;
                
        }

        nextX = newI;
        nextY = newJ;
    }


    void move(int** matrix, int** directionMatrix, int dimX, int dimY)
    {
        SnakeSegment currentSegment;

        //2 - head
        //1 - body
        //3 - tail

        int newI, newJ, oldValue;
        SnakeSegment* segPtr;

        int currentI;
        int currentJ;
        int currentMatrixDirection;
        int lastRecordedLength = this->length;
        for(int i = 0; i < lastRecordedLength; i++)
        {
            currentSegment = segments[i];
            segPtr = &segments[i];
            currentI = currentSegment.posX;
            currentJ = currentSegment.posY;

            //To decide the direction, body and tail segments will look at the directions matrix and decide what direction it must have at a certain point of simulation
            //If a segment(-head) has no direction it will check the head direction. After that at each step each segment will check the directions matrix. If a segment will encounter
            //a non negative direction, it will use that direction for next steps of the simulation.
            
            //A non negative direction will be reversed to -1 when a tail segment will cross it.
            switch(currentSegment.type)
            {
                /*
                    0 - UP
                    1 - DOWN
                    2 - LEFT
                    3 - RIGHT
                */ 

                case 1: //body
                    currentMatrixDirection = directionMatrix[currentI][currentJ];
                    if(currentMatrixDirection == -1)
                    {
                        (*segPtr).direction = this->segments[0].direction;
                    }else
                    {
                        (*segPtr).direction = currentMatrixDirection;
                    }


                    getNextCell(currentSegment.posX, currentSegment.posY, currentSegment.direction, newI, newJ, dimX, dimY);
                    
                    (*segPtr).posX = newI;
                    (*segPtr).posY = newJ;
                    
                    oldValue = matrix[currentI][currentJ];
                    matrix[currentI][currentJ] = 0;
                    matrix[newI][newJ] = oldValue;

                    break;
                case 2: //head

                    newI = currentSegment.posX;
                    newJ = currentSegment.posY;

                    if(currentSegment.direction == -1)
                    {
                        return;
                    }

                    switch(currentSegment.direction)
                    {
                        case 0:
                            if(newI - 1 >= 0) newI--;
                            break;
                        case 1:
                            if(newI + 1 < dimX) newI++;
                            break;
                        case 2:
                            if(newJ - 1 >= 0) newJ--;
                            break;
                        case 3:
                            if(newJ + 1 < dimY) newJ++;
                            break;
                            
                    }

                    if(matrix[newI][newJ] == 4)
                    {
                        cout << "I ate apple" << endl;
                        this->addSegment();

                    }

                    oldValue = matrix[currentI][currentJ];
                    matrix[currentI][currentJ] = 0;
                    matrix[newI][newJ] = oldValue;

                    
                    
                    (*segPtr).posX = newI;
                    (*segPtr).posY = newJ;

                    break;
                case 3: //tail
                    
                    currentMatrixDirection = directionMatrix[currentI][currentJ];
                    if(currentMatrixDirection == -1)
                    {
                        (*segPtr).direction = this->segments[0].direction;
                    }else
                    {
                        (*segPtr).direction = currentMatrixDirection;
                    }

                    directionMatrix[currentI][currentJ] = -1;

                    getNextCell(currentSegment.posX, currentSegment.posY, currentSegment.direction, newI, newJ, dimX, dimY);

                    (*segPtr).posX = newI;
                    (*segPtr).posY = newJ;
                    
                    oldValue = matrix[currentI][currentJ];
                    matrix[currentI][currentJ] = 0;
                    matrix[newI][newJ] = oldValue;

                    break;
            }

            


            cout<<lastRecordedLength << " vs " << this->length << endl;




        }
        updateField(matrix, dimX, dimY);


    }

    ~Snake()
    {
        delete[] segments;
        delete[] commands;
    }

    
};


void go(int dir, int i, int j, int** m, int dimX, int dimY)
{
    /*
    0 - UP
    1 - DOWN
    2 - LEFT
    3 - RIGHT
    */ 

    int newI = i;
    int newJ = j;

    if(dir == -1)
    {
        return;
    }

    switch(dir)
    {
        case 0:
            if(newI - 1 >= 0) newI--;
            break;
        case 1:
            if(newI + 1 < dimX) newI++;
            break;
        case 2:
            if(newJ - 1 >= 0) newJ--;
            break;
        case 3:
            if(newJ + 1 < dimY) newJ++;
            break;
            
    }

    int oldValue = m[i][j];
    m[i][j] = 0;
    m[newI][newJ] = oldValue;

}


void allocateMemory(int*** m, int dimX, int dimY)
{
    *m = new int* [dimX];
    for(int i = 0; i < dimX; i++)
    {
        (*m)[i] = new int[dimY];
    }

    //return;
}

void deallocateMemory(int** m, int dimX)
{
    for (int i=0;i<dimX;i++){
        delete [] m[i];
    }
    delete [] m;
}



int getSquareCoord(float mouseX, float mouseY, int& squareX, int& squareY, int size, int offset)
{
    //Returns 1 when the function succeeded
    //Returns 0 when the function fails
    
    if(&mouseX!= NULL && &mouseY != NULL)
    {
        squareX = (int(mouseX)/size);
        squareY = (int(mouseY)/size);
        return 1;
    }
    return 0;
    

}


void instantiateMatrix(int*** matrix, int posX, int posY, int specialCell, int normalCell, int dimX, int dimY)
{

    for(int i = 0; i < dimX; i++)
    {
        for(int j = 0; j < dimY; j++)
        {
            if(j == posX && i == posY)
            {
                (*matrix)[i][j] = specialCell;
            }else
            {
                (*matrix)[i][j] = normalCell;
            }
        }
    }



}


int main()
{

    static SDL_Renderer *renderer = NULL;
    static SDL_Window *window = NULL;



    char title[10] = "My Window";
    bool kill = false;

    int width = 1600;
    int height = 900;


    int squareI = 0;
    int squareJ = 0;
    

    int squareSize = 100;
    int borderSize = 5;

    int dimY = width / squareSize;
    int dimX = height / squareSize;

    //0 - field
    //2 - snake head
    //1 - snake body
    //3 - snake tail
    //4 - apple
    int** playingField;
    int** directions;
    allocateMemory(&playingField, dimX, dimY);
    allocateMemory(&directions, dimX, dimY);



    //Should be random
    int snakeX = 5;
    int snakeY = 5;


    float mouseX;
    float mouseY;


    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if(!SDL_CreateWindowAndRenderer(title, width, height, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Problem with the creation of window and renderer: %s", SDL_GetError());
        return 1;
    }
    SDL_SetRenderLogicalPresentation(renderer, width, height, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    instantiateMatrix(&playingField, snakeX, snakeY, 2, 0, dimX, dimY);
    instantiateMatrix(&directions, 0, 0, -1, -1, dimX, dimY);
    for(int i = 0; i < dimX; i++)
    {
        for(int j = 0; j < dimY; j++)
        {
            cout << playingField[i][j] << " ";
        }
        cout << endl;
    }

    for(int i = 0; i < dimX; i++)
    {
        for(int j = 0; j < dimY; j++)
        {
            cout << directions[i][j] << " ";
        }
        cout << endl;
    }
    
    //Testing purposes; spawns an apple
    playingField[6][3] = 4;
    playingField[4][6] = 4;
    playingField[7][7] = 4;
   
    SDL_FRect fieldSquare;
    
    Snake snake = {1,snakeX, snakeY};

    uint64_t currentTime = 0;
    uint64_t lastTime = 0;

    int direction = -1;

    while(!kill)
    {

        //Clear the previous frame
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  
        SDL_RenderClear(renderer);

        currentTime = SDL_GetTicks();

        SDL_Event event;

    
        while(SDL_PollEvent(&event))
        {   
            
            // poll until all events are handled!
            if(event.type == SDL_EVENT_KEY_DOWN)
            {
                /* the pressed key was Escape? */
                if(event.key.key == SDLK_ESCAPE)
                {
                    kill = true;
                }

                //Get the direction for the snake
                if(event.key.key == SDLK_W)
                {
                    //cout << "Up" << endl;
                    direction = 0;

                }else if(event.key.key == SDLK_S)
                {
                    //cout << "Down" << endl;
                    direction = 1;

                }else if(event.key.key == SDLK_A)
                {
                    //cout << "Left" << endl;
                    direction = 2;

                }else if(event.key.key == SDLK_D)
                {
                    //cout << "Right" << endl;
                    direction = 3;
                }

        
            }
            if(event.button.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                
                cout << "Mouse clicked" << endl;
                SDL_GetMouseState(&mouseX, &mouseY);

                cout << "X: " << mouseX << " " << "Y: " << mouseY << endl;

            }
 
        }
        SDL_GetMouseState(&mouseX, &mouseY);


        if(getSquareCoord(mouseX, mouseY, squareI, squareJ, squareSize, 0) == 1)
        {
            //this function calls just gets coordinates of currently selected square
            //cout << *mouseX << " " << *mouseY << " " << squareI << " " << squareJ << endl;
        }else
        {
            cout << "getSquare failed" << endl;
        }
        
        //Update playing field logic
        //Based on the key pressed the matrix's next state will be calculated
        //Then based on the recalculated state the field will be drawn
        if(currentTime > lastTime + 500)
        {
            
        
            snake.setHeadDirection(direction, directions);
            snake.move(playingField, directions, dimX, dimY);

            cout << "Segments from main()" << endl;
            for(int i = 0; i < snake.length; i++)
            {
                cout << "POS:{" << snake.segments[i].posX << " , " << snake.segments[i].posY << "} " << snake.segments[i].type << ":" << snake.segments[i].direction << endl;
            }

            
            cout << "Matrix" << endl;
            for(int i = 0; i < dimX; i++)
            {
                for(int j = 0; j < dimY; j++)
                {
                    cout << playingField[i][j] << " ";
                }
                cout << endl;
            }

            cout << endl << endl;
            cout << "Directions" << endl;
            for(int i = 0; i < dimX; i++)
            {
                for(int j = 0; j < dimY; j++)
                {
                    cout << directions[i][j] << " ";
                }
                cout << endl;
            }
            


            lastTime = currentTime;
        }



        //Draw the field
        for(int i = 0; i < dimX; i++)
        {
            for(int j = 0; j < dimY; j++)
            {
                //Select drawing color
                if(squareI == j && squareJ == i)
                {
                    //Set render color to gray
                    SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
                }else
                {
                    switch(playingField[i][j])
                    {
                        case 0:
                            SDL_SetRenderDrawColor(renderer, 255,255,255,255);
                            break;
                        case 1: case 2: case 3:
                            SDL_SetRenderDrawColor(renderer, 50, 190, 25, 255);
                            break;
                        case 4:
                            SDL_SetRenderDrawColor(renderer, 190, 50, 25, 255);
                            break;
                    }
                }
                  
                
                                
                //Update the coordinates of the square
                fieldSquare.x = (j*squareSize) + borderSize;
                fieldSquare.y = (i*squareSize) + borderSize;
                fieldSquare.w = (squareSize) - borderSize * 2;
                fieldSquare.h = (squareSize) - borderSize * 2;
                
        
                SDL_RenderFillRect(renderer, &fieldSquare);
                
            }


        }
        SDL_RenderPresent(renderer);

    }

    deallocateMemory(playingField, dimX);
    deallocateMemory(directions, dimX);
    cout << "I exit here" << endl;

    return 0;
}