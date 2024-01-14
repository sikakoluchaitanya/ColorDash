// author- Chaitanya sikakolu
// date - 14-01-2024

#include <iostream>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <random>

using namespace std;
#define SCREEN_HEIGHT 800
#define SCREEN_WIDTH 1200
#define FPS 60 
#define MAX_X 2400

#define MAX_BUILDINGS 20000
#define MAX_BUILDING_HEIGHT 400
#define MIN_BUILDING_HEIGHT 200
#define MIN_BUILDING_WIDTH 40
#define MAX_BUILDING_WIDTH 100

#define BALL_RADIUS 10
#define MIN_DISTANCE_OBS 200
#define MAX_DISTANCE_OBS 800
#define MAX_OBS 1000
#define MIN_OBS_HEIGHT 50
#define MAX_OBS_HEIGHT 100

//#define BALL_INIT_SPEED 10
#define JUMP_HEIGHT 120
#define JUMP_FRAMES 20


// void setupBuildings(Rectangle *buildings, Color *colors){
//     int spacing = 0;
//     for(int i = 0; i<MAX_BUILDINGS; i++){
//         buildings[i].width = GetRandomValue(MIN_BUILDING_WIDTH, MAX_BUILDING_WIDTH - MIN_BUILDING_WIDTH);
//         buildings[i].height = GetRandomValue(MIN_BUILDING_HEIGHT, MAX_BUILDING_HEIGHT - MIN_BUILDING_HEIGHT);
//         buildings[i].y = SCREEN_HEIGHT/2.0f - buildings[i].height;
//         buildings[i].x = spacing;
//         colors[i] = (Color){GetRandomValue(200, 255),GetRandomValue(200, 255),GetRandomValue(200, 255),255};

//         printf("Building %d: width=%f, height=%f, x=%f, y=%f\n", i, buildings[i].width, buildings[i].height, buildings[i].x, buildings[i].y);
//         printf("Color %d: r=%d, g=%d, b=%d\n", i, colors[i].r, colors[i].g, colors[i].b);

//         spacing += buildings[i].width;
    
//     }
// }  // not able to find the reson behind the inbuilt function of raylib GetRandomValue not working 


void setupBuildings(Rectangle *buildings, Color *colors) {
    int spacing = 0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> widthDist(MIN_BUILDING_WIDTH, MAX_BUILDING_WIDTH );
    std::uniform_int_distribution<int> heightDist(MIN_BUILDING_HEIGHT, MAX_BUILDING_HEIGHT );
    std::uniform_int_distribution<int> colorDist(100, 255);

    for (int i = 0; i < MAX_BUILDINGS; i++) {
        buildings[i].width = widthDist(gen);
        buildings[i].height = heightDist(gen);
        buildings[i].y = SCREEN_HEIGHT / 2.0f - buildings[i].height;
        buildings[i].x = spacing;

        colors[i] = (Color){
            (unsigned char)colorDist(gen),
            (unsigned char)colorDist(gen),
            (unsigned char)colorDist(gen),
            255};

        spacing += buildings[i].width; // Adjust the gap as needed
    }
}


void drawBuildings(Rectangle *buildings, Color *colors){
    for(int i = 0; i<MAX_BUILDINGS; i++){
        DrawRectangleRec(buildings[i], colors[i]);
    }
}


void drawRoad(){
    DrawLine(0.0f, SCREEN_HEIGHT/2.0f, MAX_X, SCREEN_HEIGHT/2.0f, BLACK); // startposx,startposy,endposx,endposy,color
}

void drawBall(Vector2 ballPosition){
    DrawCircle(ballPosition.x,ballPosition.y - BALL_RADIUS, BALL_RADIUS,RED);//centerX,CenterY,radius,color
}

void setupObstacles(Rectangle *obstacles, Vector2 ballPosition){
    std::random_device rd1;
    std::mt19937 gen(rd1());

    std::uniform_int_distribution<int> OBSdis(MIN_DISTANCE_OBS, MAX_DISTANCE_OBS);
    std::uniform_int_distribution<int> OBSHT(MIN_OBS_HEIGHT, MAX_OBS_HEIGHT);
    
    int spacing = OBSdis(gen) + ballPosition.x + MIN_DISTANCE_OBS; // Ensure initial spacing
    for(int i = 0;i< MAX_OBS; i++){
        spacing += OBSdis(gen);
        obstacles[i].height = OBSHT(gen);
        obstacles[i].width = 20;
        obstacles[i].y = SCREEN_HEIGHT/2.0f - obstacles[i].height;
        obstacles[i].x = spacing;
    }
}

void drawObstacles(Rectangle *obstacles){
    for(int i = 0; i<MAX_OBS; i++){
        DrawRectangleRec(obstacles[i], BLACK);

    }
}

void jumpBall(Vector2 *ballPosition, int *currentJumpFrame){
    if(*currentJumpFrame > 0){
        ballPosition->y -= ((float)JUMP_HEIGHT / JUMP_FRAMES);
       *currentJumpFrame = *currentJumpFrame + 1;
       if(*currentJumpFrame >= JUMP_FRAMES){
        *currentJumpFrame = 0;
       }
    } else if (ballPosition->y <= SCREEN_HEIGHT / 2.0f){
        ballPosition->y += ((float)JUMP_HEIGHT / JUMP_FRAMES);
    }
}

void moveball(Vector2 *ballPosition, int *currentJumpFrame,float ballSpeed){
    //float deltaTime = GetFrameTime();
    ballPosition->x += ballSpeed;
    jumpBall(ballPosition, currentJumpFrame);

}



int checkCollision(Vector2 *ballPosition, Rectangle *obstacles){
    for(int i = 0;i < MAX_OBS; i++){
        if(CheckCollisionCircleRec(*ballPosition, BALL_RADIUS, obstacles[i])){
            return 1;
        }
    }
    return 0;
}

void printGameEndedBanner(Vector2 *ballPosition, int score){
    DrawText("Game Over", ballPosition->x - 100, SCREEN_HEIGHT / 2 - 400, 30, RED);

    // display the score and obstacles crossed after the game is over
    char scoreText[256];
    sprintf(scoreText, "Total Score: %d",score);
    DrawText(scoreText , ballPosition->x - 60, SCREEN_HEIGHT / 2 - 360, 20, RED);
}


int main () {

    double lastTime = GetTime();
    double speedIncreaseInterval = 15.0; // speed increase interval in seconds
    double timeSinceLastSpeedIncrease = 0.0;

    float ballSpeed = 10.0; // initial speed
    float speedIncreaseRate = 1.0; // speed increase rate per second

    Rectangle buildings[MAX_BUILDINGS] = {0};
    Color buildingColors[MAX_BUILDINGS] = {0};
    Rectangle obstacles[MAX_OBS] = {0};
    

    bool gameStarted = false;
    bool gameEnded = false;

    Vector2 ballPosition = {SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f};

    Camera2D camera = {0};
    camera.offset = (Vector2){SCREEN_WIDTH/2.0f , SCREEN_HEIGHT/2.0f};
    camera.target = ballPosition;
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    setupObstacles(obstacles, ballPosition);
    setupBuildings(buildings, buildingColors);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT,"ColorDash");
    SetTargetFPS(FPS);

    camera.target = ballPosition;

    int currentjumpFrame = 0;
    float score = 0;

    while(!WindowShouldClose()){
        double currentTime = GetTime();
        double deltaTime = currentTime - lastTime;
        timeSinceLastSpeedIncrease += deltaTime;
        if (timeSinceLastSpeedIncrease >= speedIncreaseInterval) {
            // Gradual speed increase
            ballSpeed += speedIncreaseRate * deltaTime;
            // Reset time since last speed increase
            timeSinceLastSpeedIncrease = 0.0;
            // Print or handle the speed increase
            printf("Speed increased to: %.2f\n", ballSpeed);
        }
        lastTime = currentTime;
    
        if (IsKeyPressed(KEY_R)) {
            ballPosition = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
            currentjumpFrame = 0;
            gameStarted = false;
            gameEnded = false;
            score = 0;
            Rectangle obstacles[MAX_OBS] = {0};
            setupObstacles(obstacles, ballPosition);
        }
        
        ClearBackground(RAYWHITE);
        BeginDrawing();
        BeginMode2D(camera);
        drawBuildings(buildings, buildingColors);
        drawObstacles(obstacles);
        drawRoad();


        gameEnded = checkCollision(&ballPosition, obstacles);


        if (IsKeyDown(KEY_SPACE) && currentjumpFrame == 0){
            currentjumpFrame++;
            score += 0.5;
            gameStarted = true;
        };

        if (IsKeyDown(KEY_DOWN)) {
            ballPosition.y += ballSpeed; // Adjust the speed as needed
            // Ensure the ball does not go below the road
            if (ballPosition.y > SCREEN_HEIGHT / 2.0f - BALL_RADIUS) {
                score += 0.5;
                ballPosition.y = SCREEN_HEIGHT / 2.0f - BALL_RADIUS;
            }
        }
        
        if(gameEnded){
            printGameEndedBanner(&ballPosition,score);
        }
        else if(gameStarted){
            moveball(&ballPosition, &currentjumpFrame, ballSpeed);
        }

        drawBall(ballPosition);
        camera.target = ballPosition;
        //moveball(&ballPosition, &currentjumpFrame);

        EndMode2D();
        EndDrawing();
    }
CloseWindow();
return 0;

}