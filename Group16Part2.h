#ifndef __TANKGAME_Nhom16__
#define __TANKGAME_Nhom16__

#include "..\IPlayer.h"
#include "..\IPlayerInfo.h"
#include "..\IGameInfo.h"
#include "..\Command.h"
#include "..\ITank.h"
#include <vector>
#include <queue>

using namespace std;

namespace _Nhom16
{
	class Nhom16 : public IPlayer {
	public:
		Nhom16();
		~Nhom16();

		IPlayerInfo* getPlayerInfo() const;
		void onBindPlayerInfo(IPlayerInfo* playerInfo);
		void onBindGameInfo(IGameInfo* gameInfo);
		bool onStart();
		bool onFinish();
		IPlayerInfo* getEnemies();
		Command nextMove();

	protected:
		IPlayerInfo* _playerInfo;
		IGameInfo* _gameInfo;
		int _currentTank;
		list<ITank*> enemyTank;
		// chiến thuật nhóm
		//Command getNextMove_no1(vector<ITank*>& posibleTanks, ITank* nextTank);
	private:
		//return if the row has a headquarter (opponent's)
		bool isHeadquarterRow(int row, std::pair<int, int> headquarter) const;
		//return if the col has a headquarter (opponent's)
		bool isHeadquarterCol(int col, std::pair<int, int> headquarter) const;
		//get opponent's headquarter
		std::pair<int, int> getOpponetnHeadquarter() const;
		//determine the tank's direction to lead to opponent's headquarter
		std::pair<int, int> findNextPos(std::pair<int, int> curPos, std::pair<int, int> opHeadquarter) const;
		bool isCoverd(std::pair<int, int> curPos);
		std::pair<int, int> findNearestBridge(std::pair<int, int> curPos) const;
	};
}
#endif
