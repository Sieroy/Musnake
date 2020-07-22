#pragma once

namespace musnake {
	class Grid;
}

// 格子内物品种类
enum GridObjectType {
	empty,  // 空格
	snake,  // 蛇体
	block,  // 砖块
	food,  // 食物
};

// 地图格
class musnake::Grid {
public:
	Grid();
	~Grid();

};
