#pragma once

namespace musnake {
	class Grid;
}

// ��������Ʒ����
enum GridObjectType {
	empty,  // �ո�
	snake,  // ����
	block,  // ש��
	food,  // ʳ��
};

// ��ͼ��
class musnake::Grid {
public:
	Grid();
	~Grid();

};
