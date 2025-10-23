#include "displayobject.hpp"
#include <atomic>

std::unordered_map<int, DisplayObject> DisplayObject::theFarm{};
std::shared_ptr<std::unordered_map<int, DisplayObject>> DisplayObject::buffedFarmPointer{std::make_shared<decltype(theFarm)>()};
BakeryStats DisplayObject::stats{};

DisplayObject::DisplayObject(const std::string& str, const int w, const int h, const int l, const int i)
{
	x = 0;
	y = 0;
	texture = str;
	layer = l;
	width = w;
	height = h;
	id = i;
}

DisplayObject::~DisplayObject()
{
}

std::mutex DisplayObject::farm_mtx;
void DisplayObject::updateFarm()
{
	std::lock_guard<std::mutex> lock(farm_mtx);
	auto res = theFarm.insert({id, *this});
	if (!res.second) {
		res.first->second = *this;
	}
}
void DisplayObject::erase()
{
	// auto it = theFarm.find(id);
	// if (it != theFarm.end()) {
	// 	theFarm.erase(it);
	// }
	std::lock_guard<std::mutex> lk(farm_mtx);                
	theFarm.erase(id);
}
void DisplayObject::setPos(int x, int y)
{
	this->x = x;
	this->y = y;
}
void DisplayObject::setTexture(const std::string& str)
{
	texture = str;
}

void DisplayObject::redisplay(BakeryStats& _stats)
{
	std::shared_ptr<std::unordered_map<int,DisplayObject>> snapshot;
	{
		std::lock_guard<std::mutex> lock(farm_mtx);
		snapshot = std::make_shared<std::unordered_map<int,DisplayObject>>(theFarm);
	}
	std::atomic_store_explicit(
		&buffedFarmPointer,
		snapshot,
		std::memory_order_release);
	_stats.print();
}
