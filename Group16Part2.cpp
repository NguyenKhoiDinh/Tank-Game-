#include "Nhom16.h"
#include <list>

using namespace _Nhom16;
ITank* moved = NULL; //biến moved dùng để lưu lại địa chỉ xe tăng đã đi ở bước trước

Nhom16::Nhom16()
: _playerInfo(NULL), _gameInfo(NULL) {}

Nhom16::~Nhom16() {}

void Nhom16::onBindPlayerInfo(IPlayerInfo* playerInfo) {
	_playerInfo = playerInfo;
}
void Nhom16::onBindGameInfo(IGameInfo* gameInfo) {
	_gameInfo = gameInfo;
}

IPlayerInfo* Nhom16::getPlayerInfo() const{

	return _playerInfo;
}

bool Nhom16::onStart() {
	return true;
}

bool Nhom16::onFinish(){
	return true;
}

// hàm này trả về ID của địch, đồng thời lấy danh sách địch còn sống vào list enemyTank
IPlayerInfo* Nhom16::getEnemies()
{
	// lấy danh sách xe tăng còn sống.
	list<ITank*> alives = _playerInfo->getAliveTanks();
	int numOfTank = alives.size();

	if (numOfTank <= 0) // không còn xe tăng nào.
		return 0;

	//lấy ID địch
	vector<IPlayerInfo*> listPlayer = _gameInfo->getPlayersInfo();
	IPlayerInfo *enemy = (listPlayer[0] == _playerInfo) ? listPlayer[1] : listPlayer[0];

	//láy danh sách địch
	enemyTank = enemy->getAliveTanks();
	return enemy;

}

bool Nhom16::isHeadquarterRow(int row, std::pair<int, int> headquarter) const {

	int getHeight = _gameInfo->getMap()->getHeight();
	IMap* map = _gameInfo->getMap();

	for (int i = 0; i < getHeight; i++)
	if (map->isHeadquarter((*map)(i, row)) && (i != headquarter.first || row != headquarter.second))
		return true;
	return false;
}

bool Nhom16::isHeadquarterCol(int col, std::pair<int, int> headquarter) const {

	int getWidth = _gameInfo->getMap()->getWidth();
	IMap* map = _gameInfo->getMap();

	for (int i = 0; i < getWidth; i++)
	if (map->isHeadquarter((*map)(col, i)) && (col != headquarter.first || i != headquarter.second))
		return true;
	return false;
}

std::pair<int, int> Nhom16::getOpponetnHeadquarter() const{

	int getWidth = _gameInfo->getMap()->getWidth();
	int getHeight = _gameInfo->getMap()->getHeight();
	IMap* map = _gameInfo->getMap();

	//lay vi tri headquarter cua ta
	std::pair<int, int> headquarter = _playerInfo->getHeadquarterPosition();

	//kiem tra tren map vi tri cua cac headquarter, tra ve vi tri headquarter khac vi tri headquarter cua ta
	for (int i = 0; i < getHeight; i++)
	for (int j = 0; j < getWidth; j++)
	if (map->isHeadquarter((*map)(i, j)) && (i != headquarter.first) && (j != headquarter.second))
		return std::pair<int, int>(i, j);
}

int findDistance(std::pair<int, int> curPos, std::pair<int, int> nextPos){

	return (abs(curPos.first - nextPos.first) + abs(curPos.second - nextPos.second));
}


//ham tra ve vi tri moi sao cho tien toi gan so chi huy cua dich nhat
std::pair<int, int> Nhom16::findNextPos(std::pair<int, int> curPos, std::pair<int, int> opHeadquarter) const {


	pair<int, int> delta[4] = {
		pair<int, int>(0, -1), // north
		pair<int, int>(0, 1), // south
		pair<int, int>(1, 0), // east
		pair<int, int>(-1, 0) // west
	};

	IMap* map = _gameInfo->getMap();
	int minDistance = INT_MAX;
	pair<int, int> nextPos;
	pair<int, int> tmpPos;
	//Nếu có cầu thì tìm đường ngắn nhất tới cầu
	pair<int, int> nearestBridge = this->findNearestBridge(curPos);
	if (nearestBridge.first != -1)
	{
		for (int i = 0; i < 4; i++)
		{
			tmpPos.first = curPos.first + delta[i].first;
			tmpPos.second = curPos.second + delta[i].second;
			int distance = findDistance(tmpPos, nearestBridge);
			//kiem tra vi tri tiep theo thoa: nam tren map, (khong co chuong ngai vat hoac la cau) va khoang cach ngan nhat
			if ((*map).isOnMap(tmpPos) && minDistance>distance && (!(*map).isWater(tmpPos.first, tmpPos.second))){

				minDistance = distance;
				nextPos = tmpPos;
			}
		}
	}
	//tiếp tục đi
	for (int i = 0; i < 4; i++)
	{
		tmpPos.first = curPos.first + delta[i].first;
		tmpPos.second = curPos.second + delta[i].second;
		int distance = findDistance(tmpPos, opHeadquarter);
		//kiem tra vi tri tiep theo thoa: nam tren map, (khong co chuong ngai vat hoac la cau) va khoang cach ngan nhat
		if ((*map).isOnMap(tmpPos) && minDistance>distance && (!(*map).isWater(tmpPos.first, tmpPos.second))){

			minDistance = distance;
			nextPos = tmpPos;
		}
	}
	return nextPos;
}
std::pair<int, int> Nhom16::findNearestBridge(std::pair<int, int> curPos) const
{
	int getWidth = _gameInfo->getMap()->getWidth();
	int getHeight = _gameInfo->getMap()->getHeight();
	IMap* map = _gameInfo->getMap();
	int min = INT_MAX;
	int distance;
	pair<int, int> nearestBridge(-1, -1);
	//kiem tra tren map vi tri cua cac cầu, tra ve vi tri cau gan nhat
	for (int i = 0; i < getHeight; i++)
	for (int j = 0; j < getWidth; j++)
	{
		distance = findDistance(curPos, pair<int, int>(i, j));
		if (_gameInfo->getMap()->isBridge(i, j) && distance < min)
		{
			min = distance;
			nearestBridge = pair<int, int>(i, j);
		}
	}
	return nearestBridge;
}

//Hàm kiểm tra xem 4 phía có là vật cản không
bool Nhom16::isCoverd(std::pair<int, int> curPos)
{
	pair<int, int> delta[4] = {
		pair<int, int>(0, -1), // north
		pair<int, int>(0, 1), // south
		pair<int, int>(1, 0), // east
		pair<int, int>(-1, 0) // west
	};
	pair<int, int> tmpPos;
	for (int i = 0; i < 4; i++)
	{
		tmpPos.first = curPos.first + delta[i].first;
		tmpPos.second = curPos.second + delta[i].second;
		if (_gameInfo->getMap()->isEmptySpace(tmpPos.first, tmpPos.second) || _gameInfo->getMap()->isBridge(tmpPos.first, tmpPos.second))
			return false;
	}
	return true;
}

//Command Nhom16::nextMove(){
//
//
//	pair<int, int> opponentHeadquarter = getOpponetnHeadquarter();
//	list<ITank*> alives = _playerInfo->getAliveTanks();
//	int numOfTank = alives.size();
//	int next = 0;
//	IPlayerInfo* enemy = getEnemies();
//
//	if (numOfTank > 0) {
//
//		//chon tank dang dung gan so chi huy nhat
//		int minDistance = INT_MAX;
//		int distance;
//		int j = 0;
//		for (std::list<ITank*>::iterator it = alives.begin(); it != alives.end(); ++it, ++j)
//		{
//			distance = findDistance((*it)->getPosition(), opponentHeadquarter);
//			if (minDistance > distance)
//			{
//				minDistance = distance;
//				next = j;
//			}
//		}
//
//		list<ITank*>::iterator it;
//		for (it = alives.begin(), j = 0; it != alives.end() && j < next; ++it, ++j)
//			;
//
//		ITank* nextTank = *it;
//		int direction;
//		pair<int, int> curPos = nextTank->getPosition();
//		pair<int, int> nextPos = findNextPos(curPos, opponentHeadquarter);
//		pair<int, int> deltaPos, enemyPos;
//		list<ITank*>::iterator it1 = enemyTank.begin();
//		list<ITank*>::iterator it2 = alives.begin();
//
//		//neu o vi tri hien tai co the ban duoc so chi huy cua dich thi ban
//
//		if (findDistance(curPos,opponentHeadquarter) <= nextTank->getRange())
//			return Command(nextTank, Command::FIRE, opponentHeadquarter.first, opponentHeadquarter.second);
//
//		//neu khong,
//		for (; it1 != enemyTank.end(); ++it1)
//			for (; it2 != alives.end(); ++it2){
//
//				enemyPos = (*it1)->getPosition();
//				curPos = (*it2)->getPosition();
//
//				//neu tank cua dich co the ban ta, di chuyen den vi tri an toan
//				if (deltaPos.first*deltaPos.second == 0 && findDistance(curPos, enemyPos) <= (*it1)->getRange()){
//
//					if (deltaPos.first == 0)//Nếu cùng cot
//					{
//						if (curPos.first + 1 < _gameInfo->getMap()->getHeight() && (_gameInfo->getMap()->isEmptySpace(curPos.first + 1, curPos.second) || _gameInfo->getMap()->isBridge(curPos.first + 1, curPos.second)))
//							return Command(nextTank, Command::MOVE, curPos.first + 1, curPos.second);
//						if (curPos.first - 1 >= 0 && (_gameInfo->getMap()->isEmptySpace(curPos.first - 1, curPos.second) || _gameInfo->getMap()->isEmptySpace(curPos.first - 1, curPos.second)))
//							return Command(nextTank, Command::MOVE, curPos.first - 1, curPos.second);
//							
//					}	
//					if (deltaPos.second == 0)//Neu cung dong
//					{
//						if (curPos.second + 1 < _gameInfo->getMap()->getWidth() && (_gameInfo->getMap()->isEmptySpace(curPos.first, curPos.second + 1) || _gameInfo->getMap()->isBridge(curPos.first, curPos.second + 1)))
//							return Command(nextTank, Command::MOVE, curPos.first, curPos.second + 1);
//						if (curPos.second - 1 >= 0 && (_gameInfo->getMap()->isEmptySpace(curPos.first, curPos.second - 1) || _gameInfo->getMap()->isBridge(curPos.first, curPos.second - 1)))
//							return Command(nextTank, Command::MOVE, curPos.first, curPos.second - 1);
//					}
//					//gap phai tank cua dich tren duong di thi ban
//					if (findDistance(curPos,enemyPos) <= (*it2)->getRange())
//						return Command(nextTank, Command::FIRE, enemyPos.first, enemyPos.second);
//				}
//			}
//			//nguoc lai di toi vi tri gan so chi huy cua dich nhat
//			return Command(nextTank, Command::MOVE, nextPos.first, nextPos.second);
//	}
//}

/*Command Nhom16::getNextMove_no1(vector<ITank*>& posibleTanks, ITank* nextTank){


}*/
//================================================================================================================================================================
//Thuật toán cho 3 xe tăng
ITank * pickTank(list<ITank *> alives, ITank* moved, pair<int, int> opponentHeadquarter)
{
	int numOfTank = alives.size();
	int minDistance = INT_MAX;
	int distance;
	int j = 0, next = 0;
	ITank* nextTank;
	if (numOfTank > 1)
	{
		//Chọn tank đứng gần sở chỉ huy nhất và khác với tank đã di chuyển ở lần trước
		minDistance = INT_MAX;
		distance;
		j = 0;

		for (std::list<ITank*>::iterator it = alives.begin(); it != alives.end(); ++it, ++j)
		{
			distance = findDistance((*it)->getPosition(), opponentHeadquarter);
			if (distance <= (*it)->getRange())
			{
				minDistance = distance;
				next = j;
			}
			else
			{
				if (minDistance > distance && *it != moved)
				{
					minDistance = distance;
					next = j;
				}
			}

		}

		list<ITank*>::iterator it;
		for (it = alives.begin(), j = 0; it != alives.end() && j < next; ++it, ++j)
			;

		nextTank = *it;
	}
	else
	{
		if (numOfTank > 0)
		{
			minDistance = INT_MAX;
			distance;
			j = 0;

			for (std::list<ITank*>::iterator it = alives.begin(); it != alives.end(); ++it, ++j)
			{
				distance = findDistance((*it)->getPosition(), opponentHeadquarter);
				if (minDistance > distance && *it != moved)
				{
					minDistance = distance;
					next = j;
				}
			}

			list<ITank*>::iterator it;
			for (it = alives.begin(), j = 0; it != alives.end() && j < next; ++it, ++j)
				;

			ITank* nextTank = *it;
		}
	}
	return nextTank;
}
Command Nhom16::nextMove()
{

	pair<int, int> opponentHeadquarter = getOpponetnHeadquarter();
	list<ITank*> alives = _playerInfo->getAliveTanks();
	int numOfTank = alives.size();
	int next = 0;
	IPlayerInfo* enemy = getEnemies();

	if (numOfTank == 0)
		return Command();
	ITank* nextTank = pickTank(alives, moved, opponentHeadquarter);

	moved = nextTank;


	int direction;
	pair<int, int> curPos = nextTank->getPosition();
	pair<int, int> nextPos = findNextPos(curPos, opponentHeadquarter);
	pair<int, int> deltaPos, enemyPos;
	list<ITank*>::iterator it1 = enemyTank.begin();
	list<ITank*>::iterator it2 = alives.begin();

	//neu o vi tri hien tai co the ban duoc so chi huy cua dich thi ban

	if (findDistance(curPos, opponentHeadquarter) <= nextTank->getRange())
		return Command(nextTank, Command::FIRE, opponentHeadquarter.first, opponentHeadquarter.second);

	//neu khong,
	for (; it1 != enemyTank.end(); ++it1)
	for (; it2 != alives.end(); ++it2){

		enemyPos = (*it1)->getPosition();
		curPos = (*it2)->getPosition();

		//bắn chướng ngại vật nếu bị bao vây 4 phía
		if (this->isCoverd(curPos) || _gameInfo->getMap()->isBlock(nextPos.first, nextPos.second))
		{
			return Command(nextTank, Command::FIRE, nextPos.first, nextPos.second);
		}
		//nếu thấy địch trên đường đi, bắn
		if (findDistance(curPos, enemyPos) <= (*it2)->getRange()) // && enemyPos.first == nextPos.first && enemyPos.second == nextPos.second
		{
			if ((*it2)->getAmmoNumber() > 0)
			{
				return Command(nextTank, Command::FIRE, enemyPos.first, enemyPos.second);
			}
			return Command(nextTank, Command::MOVE, nextPos.first, nextPos.second);
		}
		//nguoc lai di toi vi tri gan so chi huy cua dich nhat
		return Command(nextTank, Command::MOVE, nextPos.first, nextPos.second);

		//if (deltaPos.first*deltaPos.second == 0 && findDistance(curPos, enemyPos) <= (*it1)->getRange()){

		//	if (deltaPos.first == 0)//Nếu cùng cot
		//	{
		//		if (curPos.first + 1 < _gameInfo->getMap()->getHeight() && (_gameInfo->getMap()->isEmptySpace(curPos.first + 1, curPos.second) || _gameInfo->getMap()->isBridge(curPos.first + 1, curPos.second)))
		//			return Command(nextTank, Command::MOVE, curPos.first + 1, curPos.second);
		//		if (curPos.first - 1 >= 0 && (_gameInfo->getMap()->isEmptySpace(curPos.first - 1, curPos.second) || _gameInfo->getMap()->isEmptySpace(curPos.first - 1, curPos.second)))
		//			return Command(nextTank, Command::MOVE, curPos.first - 1, curPos.second);

		//	}

		//	if (deltaPos.second == 0)//Neu cung dong
		//	{
		//		if (curPos.second + 1 < _gameInfo->getMap()->getWidth() && (_gameInfo->getMap()->isEmptySpace(curPos.first, curPos.second + 1) || _gameInfo->getMap()->isBridge(curPos.first, curPos.second + 1)))
		//			return Command(nextTank, Command::MOVE, curPos.first, curPos.second + 1);
		//		if (curPos.second - 1 >= 0 && (_gameInfo->getMap()->isEmptySpace(curPos.first, curPos.second - 1) || _gameInfo->getMap()->isBridge(curPos.first, curPos.second - 1)))
		//			return Command(nextTank, Command::MOVE, curPos.first, curPos.second - 1);
		//	}
		//	
		//	//gap phai tank cua dich tren duong di thi ban

	}
}
