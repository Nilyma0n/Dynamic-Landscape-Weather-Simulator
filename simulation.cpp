#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

// Animation variables
int cloudX = -200;
int rainY = 0;
int snowY = 0;
float wavePhase = 0.0;
float leafOffset = 0.0;
float sunY = 220.0; 
bool isNight = false;
char weatherMode = 's'; 

// Birds animation variables
float birdX1 = 600, birdY1 = 80;
float birdX2 = 650, birdY2 = 60;
float birdWingPhase = 0.0;

// Boat animation variables
float boatX = 750.0; 
float boatY = 380.0;
bool isBoatStopped = false; 

// Man walking animation variables
float manX = 0;
float manY = 0;
bool isManWalking = false; 
bool isManInside = false;  

const int W_WIDTH = 800;
const int W_HEIGHT = 600;

struct Star {
    int x, y;
    bool visible;
};

Star stars[40]; 

void initStars() {
    srand(time(0));
    for (int i = 0; i < 40; i++) {
        stars[i].x = rand() % W_WIDTH;
        stars[i].y = rand() % 180; 
        stars[i].visible = rand() % 2;
    }
}

void drawBird(int x, int y, int wingState) {
    if (isNight) return; 
    setcolor(BLACK);
    setlinestyle(SOLID_LINE, 0, 2); 
    if (wingState == 0) {
        line(x, y, x - 15, y - 8);
        line(x, y, x + 15, y - 8);
    } else {
        line(x, y, x - 15, y + 4);
        line(x, y, x + 15, y + 4);
    }
    setlinestyle(SOLID_LINE, 0, 1); 
}

void drawBoatAndMan(int bx, int by) {
    if (bx < 480 || bx > W_WIDTH + 50) return;

    int tiltY = isBoatStopped ? 0 : 3 * sin(wavePhase * 1.2);
    int finalY = by + tiltY;

    // 1. Draw Boat Base
    setcolor(BLACK);
    if (isNight) setfillstyle(SOLID_FILL, DARKGRAY);
    else setfillstyle(SOLID_FILL, BROWN);

    int boatPoints[] = {
        bx, finalY,         
        bx + 50, finalY,    
        bx + 38, finalY + 12,
        bx + 12, finalY + 12,
        bx, finalY
    };
    fillpoly(5, boatPoints);

    if (!isNight) {
        setfillstyle(SOLID_FILL, BLACK);
        bar(bx + 18, finalY - 10, bx + 38, finalY);
    }

    // 2. Draw Man on Boat
    if (!isManWalking && !isManInside) {
        if (isNight) setfillstyle(SOLID_FILL, LIGHTGRAY);
        else setfillstyle(SOLID_FILL, RED); 
        
        int manBody[] = {bx + 8, finalY, bx + 15, finalY, bx + 11, finalY - 12, bx + 8, finalY};
        fillpoly(4, manBody);
        
        if (isNight) setfillstyle(SOLID_FILL, WHITE);
        else setfillstyle(SOLID_FILL, YELLOW); 
        fillellipse(bx + 11, finalY - 15, 3, 3);

        setcolor(BLACK);
        line(bx + 8, finalY - 3, bx + 1, finalY + 15);
    }
}

void drawWalkingMan() {
    if (!isManWalking || isManInside) return;

    if (isNight) setfillstyle(SOLID_FILL, LIGHTGRAY);
    else setfillstyle(SOLID_FILL, RED);

    int mx = (int)manX;
    int my = (int)manY;

    int manBody[] = {mx - 4, my, mx + 3, my, mx, my - 12, mx - 4, my};
    fillpoly(4, manBody);

    if (isNight) setfillstyle(SOLID_FILL, WHITE);
    else setfillstyle(SOLID_FILL, YELLOW);
    fillellipse(mx, my - 15, 3, 3);
}

void drawNightSky() {
    for (int i = 0; i < 40; i++) {
        if (rand() % 15 == 0) {
            stars[i].visible = !stars[i].visible;
        }
        if (stars[i].visible) {
            putpixel(stars[i].x, stars[i].y, WHITE);
            putpixel(stars[i].x + 1, stars[i].y, WHITE);
        }
    }
    setcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);
    fillellipse(W_WIDTH - 150, 60, 20, 20);
    setcolor(BLACK);
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(W_WIDTH - 140, 55, 20, 20);
}

void drawRealisticRiver() {
    int riverPoints[] = {
        520, 310,  
        W_WIDTH, 310,
        W_WIDTH, W_HEIGHT,
        380, W_HEIGHT,
        550, 480,
        480, 420,
        580, 360,
        520, 310   
    };
    if (isNight) setfillstyle(SOLID_FILL, BLUE);
    else setfillstyle(SOLID_FILL, CYAN);
    fillpoly(8, riverPoints);

    int xPoints[] = {550, 600, 650, 700, 750, 480, 510, 580, 620, 680};
    int yPoints[] = {340, 380, 420, 460, 520, 550, 580, 450, 490, 530};
    setcolor(WHITE);
    for (int i = 0; i < 10; i++) {
        int cx = xPoints[i];
        int cy = yPoints[i];
        if (cx < W_WIDTH) { 
            int offsetX = isBoatStopped ? 0 : 8 * sin(wavePhase + i);
            line(cx + offsetX, cy, cx + offsetX + 20, cy);
        }
    }
}

// FIXED: Coconut tree leaves remain GREEN in both day and night modes
void drawCoconutTree(int tx, int ty, int height) {
    setcolor(BROWN);
    setfillstyle(SOLID_FILL, BROWN);
    int trunk[] = {tx, ty, tx + 10, ty, tx + 7, ty - height, tx - 2, ty - height};
    fillpoly(4, trunk);
    
    // Always GREEN leaves (Day & Night)
    setcolor(DARKGRAY);
    setfillstyle(SOLID_FILL, GREEN);
    
    int ox = (int)leafOffset;
    fillellipse(tx + ox, ty - height - 10, 40, 10);
    fillellipse(tx - 15 + ox, ty - height, 35, 8);
    fillellipse(tx + 15 + ox, ty - height, 35, 8);
    fillellipse(tx - 5, ty - height - 15, 12, 30);
}

void drawScene() {
    // 1. Sky
    if (isNight) {
        setfillstyle(SOLID_FILL, BLACK);
        bar(0, 0, W_WIDTH, 310);
        drawNightSky(); 
    } else if (weatherMode == 'c' || weatherMode == 'r') {
        setfillstyle(SOLID_FILL, DARKGRAY);
        bar(0, 0, W_WIDTH, 310);
    } else {
        for (int y = 0; y <= 310; y++) {
            if (y < 120) setcolor(LIGHTBLUE);
            else if (y < 220) setcolor(LIGHTCYAN);
            else setcolor(YELLOW);
            line(0, y, W_WIDTH, y);
        }
        setcolor(RED);
        setfillstyle(SOLID_FILL, RED);
        fillellipse(350, (int)sunY, 40, 40);
    }

    // 2. Distant Greenery
    if (isNight) setfillstyle(SOLID_FILL, BLACK);
    else setfillstyle(SOLID_FILL, GREEN);
    for (int x = 0; x <= W_WIDTH; x += 40) {
        fillellipse(x, 310, 25, 15);
    }

    // 3. Ground Terrain
    if (isNight) setfillstyle(SOLID_FILL, DARKGRAY); 
    else setfillstyle(SOLID_FILL, LIGHTGREEN);
    int groundPoints[] = {0, 310, 520, 310, 580, 360, 480, 420, 550, 480, 380, W_HEIGHT, 0, W_HEIGHT};
    fillpoly(7, groundPoints);

    // 4. Soil Pathway
    if (isNight) setfillstyle(SOLID_FILL, BLACK); 
    else setfillstyle(SOLID_FILL, YELLOW); 
    int pathPoints[] = {150, W_HEIGHT, 240, W_HEIGHT, 220, 480, 320, 410, 220, 400, 130, 430};
    fillpoly(6, pathPoints);

    // 5. River and Boat Elements
    drawRealisticRiver();
    drawBoatAndMan((int)boatX, (int)boatY);
    drawWalkingMan();

    // 6. Flying Birds
    int wingState = ((int)birdWingPhase % 2);
    drawBird((int)birdX1, (int)birdY1, wingState);
    drawBird((int)birdX2, (int)birdY2, 1 - wingState); 

    // 7. Background Tree (FIXED: Always GREEN in both Day and Night modes)
    setfillstyle(SOLID_FILL, BROWN);
    bar(150, 220, 175, 380); 
    
    setfillstyle(SOLID_FILL, GREEN); // Always GREEN
    int ox = (int)leafOffset;
    fillellipse(120 + ox, 210, 60, 60);
    fillellipse(200 + ox, 210, 60, 60);
    fillellipse(160, 150 + (int)(leafOffset/2), 70, 70);

    // 8. Dual Houses Section
    // Background House
    if (isNight) {
        setfillstyle(SOLID_FILL, LIGHTGRAY); 
        bar(220, 320, 350, 410);
        int roof1[] = {200, 320, 370, 320, 340, 260, 230, 260}; 
        setfillstyle(SOLID_FILL, RED); 
        fillpoly(4, roof1);
    } else {
        setfillstyle(SOLID_FILL, LIGHTGRAY); 
        bar(220, 320, 350, 410);
        int roof1[] = {200, 320, 370, 320, 340, 260, 230, 260}; 
        setfillstyle(SOLID_FILL, BROWN);
        fillpoly(4, roof1);
    }

    // Foreground Main House
    if (isNight) {
        setfillstyle(SOLID_FILL, WHITE); 
        bar(110, 360, 250, 450);
        int roof2[] = {90, 360, 270, 360, 230, 300, 120, 300}; 
        setfillstyle(SOLID_FILL, RED); 
        fillpoly(4, roof2);
    } else {
        setfillstyle(SOLID_FILL, WHITE); 
        bar(110, 360, 250, 450);
        int roof2[] = {90, 360, 270, 360, 230, 300, 120, 300}; 
        setfillstyle(SOLID_FILL, BROWN);
        fillpoly(4, roof2);
    }
    
    setfillstyle(SOLID_FILL, BLACK);
    bar(150, 400, 180, 450); 
    bar(280, 370, 305, 410); 
    bar(200, 380, 220, 395);

    // FIXED: Haystack remains BROWN in night mode instead of turning black
    setfillstyle(SOLID_FILL, BROWN);
    fillellipse(370, 400, 20, 30);
    bar(350, 400, 390, 435);

    // 9. Palm Trees (Will execute drawCoconutTree with persistent green rules)
    drawCoconutTree(400, 380, 100); 
    drawCoconutTree(580, 310, 80);  
    drawCoconutTree(680, 310, 90); 

    // 10. Floating Clouds
    if (weatherMode == 'c' || weatherMode == 'r') {
        if (isNight) setfillstyle(SOLID_FILL, DARKGRAY);
        else setfillstyle(SOLID_FILL, LIGHTGRAY);
        fillellipse(cloudX, 50, 35, 25);
        fillellipse(cloudX + 40, 45, 50, 35);
        fillellipse(cloudX + 80, 50, 35, 25);
    }

    // 11. Falling Rain
    if (weatherMode == 'r') {
        setcolor(WHITE);
        for (int i = 0; i < W_WIDTH; i += 35) {
            line(i, (rainY + i) % 310, i - 4, ((rainY + i) % 310) + 12);
        }
    }

    // 12. Snowfall
    if (weatherMode == 'w') {
        setcolor(WHITE);
        setfillstyle(SOLID_FILL, WHITE);
        for (int i = 0; i < W_WIDTH; i += 50) {
            int driftX = 10 * sin(wavePhase + i);
            fillellipse(i + driftX, (snowY + i) % 310, 3, 3);
        }
    }
}

int main() {
    initwindow(W_WIDTH, W_HEIGHT, "Village Weather Simulation");
    initStars(); 

    int page = 0;

    while (1) {
        setactivepage(page);
        cleardevice();

        drawScene();

        wavePhase += 0.08; 
        leafOffset = 2.0 * sin(wavePhase * 0.4); 
        birdWingPhase += 0.15; 

        if (!isNight) {
            birdX1 -= 1.5; birdX2 -= 1.5;
            if (birdX1 < -50) birdX1 = W_WIDTH + 50;
            if (birdX2 < -50) birdX2 = W_WIDTH + 100;
        }

        if (!isBoatStopped) {
            boatX -= 0.4f; 
            if (boatX >= 480) {
                boatY = 320.0 + (100.0 * (W_WIDTH - boatX) / (W_WIDTH - 480));
            }
            if (boatX <= 485) {
                isBoatStopped = true;
                isManWalking = true;
                manX = boatX + 11;
                manY = boatY;
            }
        }

        if (isManWalking && !isManInside) {
            if (manX > 165) manX -= 1.0f; 
            if (manY > 430) manY -= 0.3f; 
            else if (manY < 430) manY += 0.2f;

            if (manX <= 166) {
                isManInside = true;
                isManWalking = false;
                isBoatStopped = false; 
                boatX = 479; 
            }
        }

        if (boatX < 350) { 
            boatX = W_WIDTH + 50; 
            boatY = 320.0;
            isManInside = false; 
            isManWalking = false;
            isBoatStopped = false;
        }

        if (weatherMode == 'c' || weatherMode == 'r') {
            cloudX += 2;
            if (cloudX > W_WIDTH + 50) cloudX = -150;
        }

        if (weatherMode == 'r') {
            rainY += 10; 
            if (rainY > 310) rainY = 0;
        }

        if (weatherMode == 'w') {
            snowY += 3;
            if (snowY > 310) snowY = 0;
        }

        if (kbhit()) {
            char key = getch();
            if (key == 's') { weatherMode = 's'; isNight = false; } 
            if (key == 'c') { weatherMode = 'c'; isNight = false; } 
            if (key == 'r') { weatherMode = 'r'; isNight = false; } 
            if (key == 'w') { weatherMode = 'w'; isNight = false; } 
            if (key == 'n') { isNight = true; }                    
            if (key == 'd') { isNight = false; }                    
            if (key == 27) break; 
        }

        setvisualpage(page);
        page = 1 - page;

        delay(25); 
    }

    closegraph();
    return 0;
}

