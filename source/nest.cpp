#include <mutex>
#include <condition_variable>
#include <vector>
#include <unordered_map>
#include <thread>

struct NestState {
    int eggs;
    bool occByChicken = false;
};

class Coop {
    std::mutex mtx; 
    std::condition_variable nestFull; //false means space for eggs
                                      //true means farmer can collect
    std::condition_variable chickenCanLay; //false means nest is empty
                                          //true means chicken is present
    std::condition_variable farmerCanCollect;

    std::vector<NestState> nests;

    struct Pos {int x; int y;};
    std::unordered_map<int, Pos> chickenPositions;

public:
    Coop(int numNests):
        nests(numNests)
    {}

    std::pair<int, int> moveChicken(int chickenId, int xToMove, int yToMove) {
        std::lock_guard<std::mutex> lock(mtx);

        int finalX = xToMove;
        int finalY = yToMove;
        bool conflict = true; 
        int attempts = 0; 
        while (conflict && attempts < 10) {
            conflict = false; 
            for (auto& [otherId, pos] : chickenPositions) {
                if (otherId != chickenId) continue;

                int dx = std::abs(pos.x - finalX);
                int dy = std::abs(pos.y - finalY);

                if (dx < 15 && dy < 15) {
                    finalX += 10;
                    finalY += 10; 
                    conflict = true;
                }
            }
            attempts++;
        } 

        chickenPositions[chickenId] = {finalX, finalY};
        return {finalX, finalY};
    }

    void chickenOccupyNest(int nestId) {
        std::unique_lock<std::mutex> lock(mtx);
        chickenCanLay.wait(lock, [&](){
            return nests[nestId].occByChicken == false && nests[nestId].eggs < 3;
        });
        nests[nestId].occByChicken = true;
    }

    int layEggs(int nestId, int maxLay){
        std::lock_guard<std::mutex> lock(mtx);
        int space = 3 - nests[nestId].eggs;
        int laid = std::min(space, maxLay);
        nests[nestId].eggs += laid;

        farmerCanCollect.notify_all();
        nestFull.notify_all();
        return laid;
    }

    void chickenLeaveNest(int nestId) {
        std::lock_guard<std::mutex> lock(mtx);
        nests[nestId].occByChicken = false;
        chickenCanLay.notify_all();
        farmerCanCollect.notify_all();
    }

    int farmerCollectNest(int nestId){
        std::unique_lock<std::mutex> lock(mtx);
        farmerCanCollect.wait(lock, [&](){
            return nests[nestId].eggs > 0 && nests[nestId].occByChicken == false;
        });
        int taken = nests[nestId].eggs; 
        nests[nestId].eggs = 0;
        chickenCanLay.notify_all();
        nestFull.notify_all();
        return taken;
    }  
}; 

class Intersection {

private: 
    std::mutex mtx;
    std::condition_variable var;
    bool occupied = false; 
    int lastTruckId = -1;

public: 

    void enterIntersection(int truckId){
        std::unique_lock<std::mutex> lock(mtx); 
        var.wait(lock, [&](){
            return occupied == false && truckId != lastTruckId;
        });
        occupied = true; 
    };

    void leaveIntersection(int truckId){
        std::lock_guard<std::mutex> lock(mtx); 
        occupied = false;
        lastTruckId = truckId; 
        var.notify_all(); 
    };
};

class Bakery {

private: 
    std::mutex mtx;
    int eggs = 0; 
    int flour = 0; 
    int butter = 0; 
    int sugar = 0; 

    int cakes = 0; 

    bool ovenBusy = false;
    bool childInside = false;

    std::condition_variable spaceForDelivery; 
    std::condition_variable ovenReady; 
    std::condition_variable cakesAvailable;
    std::condition_variable childInBakery;
    
    struct Pos {int x,y; };
    std::unordered_map<int, Pos> childPos; 

public: 

    bool addEggsFromFarmer(int n){
        std::unique_lock<std::mutex> lock(mtx);
        spaceForDelivery.wait(lock, [&](){
            return eggs + n <= 6; 
        });
        eggs += n;
        ovenReady.notify_all();
        return true; 
    }

    void unloadTruckFullBatch(int eggsD, int butterD, int flourD, int sugarD){
        std::unique_lock<std::mutex> lock(mtx);
        spaceForDelivery.wait(lock, [&](){
            return eggs + eggsD <= 6 && 
                butter + butterD <= 6 && 
                flour + flourD <= 6 && 
                sugar + sugarD <= 6;
        });
        eggs += eggsD;
        butter += butterD;
        flour += flourD;
        sugar += sugarD;
        ovenReady.notify_all();
    }

    void bakeOneBatch() {
        std::unique_lock<std::mutex> lock(mtx);
        ovenReady.wait(lock, [&](){
            return !ovenBusy &&
                eggs >= 2 &&
                butter >= 2 &&
                flour >= 2 &&
                sugar >= 2 &&
                cakes + 3 <= 6;
        });
        ovenBusy = true;

        eggs -= 2;
        butter -= 2;
        flour -= 2;
        sugar -= 2; 

        lock.unlock(); 
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        lock.lock();

        cakes += 3;
        if (cakes > 6) cakes = 6;

        ovenBusy = false;
        cakesAvailable.notify_all();
        ovenReady.notify_all(); 
        spaceForDelivery.notify_all(); 
    }

    void childEnterShop(){};

};