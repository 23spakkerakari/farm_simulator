#include "FarmLogic.h"
#include "displayobject.hpp"
#include <unistd.h>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>


//gonna use this to randomly move a given object
void moveRandomly(DisplayObject& obj, int minX, int maxX, int minY, int maxY, int speed) {
    int dx = (std::rand() % (2 * speed + 1)) - speed;
    int dy = (std::rand() % (2 * speed + 1)) - speed;
    
    int newX = obj.x + dx;
    int newY = obj.y + dy;
    
    if (newX < minX) newX = minX;
    if (newX > maxX) newX = maxX;
    if (newY < minY) newY = minY;
    if (newY > maxY) newY = maxY;
    
    obj.setPos(newX, newY);
}


/*
While it would be cleaner to put all the thread functions in one while loop and
call moveRandomly from there, to prep us better for part 2, and the innate logic 
that would accompany specific elements, we take the longer approach of having
each element have its own thread function.
*/


void chickenThread(DisplayObject* chicken) {
    while (true) {
        moveRandomly(*chicken, 50, 750, 50, 550, 3);
        {
            chicken->updateFarm();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void cowThread(DisplayObject* cow) {
    while (true) {
        if (std::rand() % 10 == 0) {
            moveRandomly(*cow, 50, 300, 50, 300, 2);
            cow->updateFarm();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

// Thread function for trucks
void truckThread(DisplayObject* truck, bool isTruck1) {
    int barnX = 50;
    int barnY = isTruck1 ? 50 : 150;
    int bakeryX = 550;
    int bakeryY = 150;
    
    bool goingToBarn = true;
    while (true) {
        int targetX = goingToBarn ? barnX : bakeryX;
        int targetY = goingToBarn ? barnY : bakeryY;
        
        int dx = (targetX - truck->x);
        int dy = (targetY - truck->y);
        
        if (abs(dx) > 5) dx = (dx > 0) ? 5 : -5;
        if (abs(dy) > 5) dy = (dy > 0) ? 5 : -5;
        
        truck->setPos(truck->x + dx, truck->y + dy);
        
        {
            truck->updateFarm();
        }
        
        if (abs(truck->x - targetX) < 10 && abs(truck->y - targetY) < 10) {
            goingToBarn = !goingToBarn;
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void farmerThread(DisplayObject* farmer) {
    std::vector<std::pair<int, int>> nestPositions = {{100, 500}, {700, 500}};
    int currentNest = 0;
    
    while (true) {
        int targetX = nestPositions[currentNest].first;
        int targetY = nestPositions[currentNest].second;
        
        int dx = (targetX - farmer->x);
        int dy = (targetY - farmer->y);
        
        if (abs(dx) > 3) dx = (dx > 0) ? 3 : -3;
        if (abs(dy) > 3) dy = (dy > 0) ? 3 : -3;
        
        farmer->setPos(farmer->x + dx, farmer->y + dy);
        {
            farmer->updateFarm();
        }
        if (abs(farmer->x - targetX) < 10 && abs(farmer->y - targetY) < 10) {
            currentNest = (currentNest + 1) % nestPositions.size();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void childThread(DisplayObject* child, int childId) {
    int bakeryX = 550;
    int bakeryY = 150;
    int startX = 600 + (childId * 40);
    int startY = 100 + (childId * 20);
    
    bool goingToBakery = true;
    
    while (true) {
        int targetX = goingToBakery ? bakeryX : startX;
        int targetY = goingToBakery ? bakeryY : startY;
        
        int dx = (targetX - child->x);
        int dy = (targetY - child->y);
        
        if (abs(dx) > 2) dx = (dx > 0) ? 2 : -2;
        if (abs(dy) > 2) dy = (dy > 0) ? 2 : -2;
        
        child->setPos(child->x + dx, child->y + dy);
        
        {
            child->updateFarm();
        }
        if (abs(child->x - targetX) < 10 && abs(child->y - targetY) < 10) {
            goingToBakery = !goingToBakery;
            std::this_thread::sleep_for(std::chrono::milliseconds(800)); 
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void redisplayThread(BakeryStats* stats) {
    while (true) {
        {
            DisplayObject::redisplay(*stats);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10 FPS
    }
}

void FarmLogic::run() {
    BakeryStats stats;

    std::srand(std::time(0));
    DisplayObject chicken("chicken", 60, 60, 2, 0);
    DisplayObject chicken2("chicken", 60, 60, 2, 1);
    DisplayObject chicken3("chicken",60,60,2,29);
    DisplayObject nest("nest", 80, 60, 0, 2);
    DisplayObject nest2("nest", 80, 60, 0, 3);
    DisplayObject nest1eggs[3] = {
        DisplayObject("egg", 10, 20, 1, 4),
        DisplayObject("egg", 10, 20, 1, 5),
        DisplayObject("egg", 10, 20, 1, 6)
    };

    DisplayObject cow("cow", 60, 60, 2, 7);
    DisplayObject cow2("cow", 60, 60, 2, 8);
    DisplayObject truck("truck", 80, 60, 2, 8);
    DisplayObject truck2("truck", 80, 60, 2, 9);
    DisplayObject farmer("farmer", 30, 60, 2, 9);
    DisplayObject child("child", 30, 60, 2, 10);
    DisplayObject child2("child",30,60,2,32);
    DisplayObject child3("child",30,60,2,33);
    DisplayObject child4("child",30,60,2,34);
    DisplayObject child5("child",30,60,2,35);
    DisplayObject barn1("barn", 100, 100, 0, 11);
    DisplayObject barn2("barn", 100, 100, 0, 12);
    DisplayObject bakery("bakery", 250, 250, 0, 13);
    DisplayObject bakeryeggs[3] = {
        DisplayObject("egg", 10, 20, 1, 14),
        DisplayObject("egg", 10, 20, 1, 15),
        DisplayObject("egg", 10, 20, 1, 16)
    };
    DisplayObject bakeryflour[3] = {
        DisplayObject("flour", 20, 20, 1, 17),
        DisplayObject("flour", 20, 20, 1, 18),
        DisplayObject("flour", 20, 20, 1, 19)
    };
    DisplayObject bakerybutter[3] = {
        DisplayObject("butter", 20, 20, 1, 20),
        DisplayObject("butter", 20, 20, 1, 21),
        DisplayObject("butter", 20, 20, 1, 22)
    };
    DisplayObject bakerysugar[3] = {
        DisplayObject("sugar", 20, 20, 1, 23),
        DisplayObject("sugar", 20, 20, 1, 24),
        DisplayObject("sugar", 20, 20, 1, 25)
    };


    DisplayObject bakerycake[3] = {
        DisplayObject("cake", 20, 20, 1, 26),
        DisplayObject("cake", 20, 20, 1, 27),
        DisplayObject("cake", 20, 20, 1, 28)
    };


    chicken.setPos(400, 300);
    chicken2.setPos(300, 300);
    nest.setPos(100, 500);
    nest2.setPos(700, 500);
    nest1eggs[0].setPos(90, 507);
    nest1eggs[1].setPos(100, 507);
    nest1eggs[2].setPos(110, 507);
    cow.setPos(200, 200);
    truck.setPos(300, 200);
    farmer.setPos(600, 400);
    child.setPos(620, 100);
    barn1.setPos(50, 50);
    barn2.setPos(50, 150);
    bakery.setPos(550, 150);
    bakeryeggs[0].setPos(510, 130);
    bakeryeggs[1].setPos(520, 130);
    bakeryeggs[2].setPos(530, 130);

    bakeryflour[0].setPos(500, 110);
    bakeryflour[1].setPos(520, 110);
    bakeryflour[2].setPos(540, 110);

    bakerysugar[0].setPos(500, 90);
    bakerysugar[1].setPos(520, 90);
    bakerysugar[2].setPos(540, 90);

    bakerybutter[0].setPos(500, 70);
    bakerybutter[1].setPos(520, 70);
    bakerybutter[2].setPos(540, 70);

    bakerycake[0].setPos(600, 200);
    bakerycake[1].setPos(620, 200);
    bakerycake[2].setPos(640, 200);

    chicken.updateFarm();
    chicken2.updateFarm();
    nest.updateFarm();
    nest2.updateFarm();
    nest1eggs[0].updateFarm();
    nest1eggs[1].updateFarm();
    nest1eggs[2].updateFarm();
    cow.updateFarm();
    truck.updateFarm();
    farmer.updateFarm();
    child.updateFarm();
    barn1.updateFarm();
    barn2.updateFarm();
    bakery.updateFarm();
    bakeryeggs[0].updateFarm();
    bakeryeggs[1].updateFarm();
    bakeryeggs[2].updateFarm();

    bakeryflour[0].updateFarm();
    bakeryflour[1].updateFarm();
    bakeryflour[2].updateFarm();

    bakerybutter[0].updateFarm();
    bakerybutter[1].updateFarm();
    bakerybutter[2].updateFarm();

    bakerysugar[0].updateFarm();
    bakerysugar[1].updateFarm();
    bakerysugar[2].updateFarm();

    bakerycake[0].updateFarm();
    bakerycake[1].updateFarm();
    bakerycake[2].updateFarm();

    DisplayObject::redisplay(stats);
    
    // int frame = 0;
    // int randomNumberX = (std::rand() % 11) - 5;
    // int randomNumberY = (std::rand() % 11) - 5;

    std::thread t_chicken1(chickenThread, &chicken);
    std::thread t_chicken2(chickenThread, &chicken2);
    std::thread t_chicken3(chickenThread, &chicken3);
    
    std::thread t_cow1(cowThread, &cow);
    std::thread t_cow2(cowThread, &cow2);
    
    std::thread t_truck1(truckThread, &truck, true);
    std::thread t_truck2(truckThread, &truck2, false);
    
    std::thread t_farmer(farmerThread, &farmer);
    
    std::thread t_child1(childThread, &child, 0);
    std::thread t_child2(childThread, &child2, 1);
    std::thread t_child3(childThread, &child3, 2);
    std::thread t_child4(childThread, &child4, 3);
    std::thread t_child5(childThread, &child5, 4);
    
    // Start redisplay thread
    std::thread t_redisplay(redisplayThread, &stats);
    
    // Wait for all threads (they run forever)
    t_chicken1.join();
    t_chicken2.join();
    t_chicken3.join();
    t_cow1.join();
    t_cow2.join();
    t_truck1.join();
    t_truck2.join();
    t_farmer.join();
    t_child1.join();
    t_child2.join();
    t_child3.join();
    t_child4.join();
    t_child5.join();
    t_redisplay.join();
    
    // while (true) {
    //     frame++;
    //     if(frame % 5 == 0) {
    //         randomNumberX = (std::rand() % 11) - 5; // Generate a random number between -5 and 5
    //         randomNumberY = (std::rand() % 11) - 5; // Generate a random number between -5 and 5
    //     }
    //     if(frame % 10 == 0) {
    //         int randEggs = (std::rand() % 3);
    //         for(int i = 0; i < 3; i++) {
    //             if (i <= randEggs) {
    //                 nest1eggs[i].updateFarm();
    //             } else {
    //                 nest1eggs[i].erase();
    //             }
    //         }
    //     }

    //     chicken.setPos(chicken.x + randomNumberX*3, chicken.y  + randomNumberY);
    //     chicken2.setPos(chicken2.x + randomNumberX, chicken2.y + randomNumberY*3);
    //     chicken.updateFarm();
    //     chicken2.updateFarm();
    //     DisplayObject::redisplay(stats);
    //     // sleep for 100 ms
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
}


void FarmLogic::start() {
    std::thread([]() {
    FarmLogic::run();
    })
    .detach();
}   