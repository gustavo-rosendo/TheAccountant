#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <math.h>

using namespace std;

float MAX_DISTANCE = sqrt(16000 * 16000 + 9000 * 9000);

float CalculateDistance(int xFirst, int yFirst, int xSecond, int ySecond) {
	int xSquare = (xSecond - xFirst) * (xSecond - xFirst);
	int ySquare = (ySecond - yFirst) * (ySecond - yFirst);
	return sqrt(xSquare + ySquare);
}

class DataPoint {
public:
	int id;
	int x;
	int y;
	float distToPlayer;

};

class Enemy {
public:
	int id;
	int x;
	int y;
	int life;

	DataPoint targetDP;
	float distToPlayer;
	float distToDataPoint;
	float distToPlayerAfterMove;
	int nextX, nextY;

	void CalculateNextXY(){
		//!! Call this function only when the targetDP and distToDataPoint are already calculated!!
		nextX = floor((500.0f * (targetDP.x - x) / distToDataPoint) - x);
		nextY = floor((500.0f * (targetDP.y - y) / distToDataPoint) - y);
	}

};

class Player {
public:
	int x;
	int y;

	void CheckPossibleDeath(list<Enemy> enemies, list<DataPoint> dataPoints) {

	}
};

Player myPlayer;
list<DataPoint> dataPoints;
list<Enemy> enemies;

void PrepareDataPoints() {
	for (list<DataPoint>::iterator it = dataPoints.begin(); it != dataPoints.end(); ++it){
		it->distToPlayer = CalculateDistance(it->x, it->y, myPlayer.x, myPlayer.y);
	}
}

void PrepareEnemies() {
	for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
		it->distToPlayer = CalculateDistance(it->x, it->y, myPlayer.x, myPlayer.y);

		it->distToDataPoint = MAX_DISTANCE;
		for(list<DataPoint>::iterator itDP = dataPoints.begin(); itDP != dataPoints.end(); ++itDP){
			int distToDP = CalculateDistance(it->x, it->y, itDP->x, itDP->y);
			if (distToDP < it->distToDataPoint) {
				it->distToDataPoint = distToDP;
				it->targetDP = *itDP;
			}
			else if (distToDP == it->distToDataPoint) {
				if (itDP->id < it->targetDP.id) {
					it->targetDP = *itDP;
				}
			}
		}

		//Calculate nextX, nextY
		it->CalculateNextXY();

		//Calculate distToPlayerAfterMove
		it->distToPlayerAfterMove = CalculateDistance(myPlayer.x, myPlayer.y, it->nextX, it->nextY);
	}

}

/**
* Shoot enemies before they collect all the incriminating data!
* The closer you are to an enemy, the more damage you do but don't get too close or you'll get killed.
**/
int main()
{
	// game loop
	while (1) {
		//the coordinates of your character
		cin >> myPlayer.x >> myPlayer.y; cin.ignore();

		//clear all lists first to make sure dead enemies
		//and collected data points are not accounted for
		dataPoints.clear();
		enemies.clear();

		//the amount of data points.
		int dataCount;
		cin >> dataCount; cin.ignore();
		for (int i = 0; i < dataCount; i++) {
			//the unique id and coordinates of a data point
			DataPoint dp;
			cin >> dp.id >> dp.x >> dp.y; cin.ignore();
			dataPoints.push_back(dp);
		}

		//the amount of enemies left to take out
		int enemyCount;
		cin >> enemyCount; cin.ignore();
		for (int i = 0; i < enemyCount; i++) {
			//the unique id, current coordinates and life points of an enemy
			Enemy e;
			cin >> e.id >> e.x >> e.y >> e.life; cin.ignore();
			enemies.push_back(e);
		}

		//Gather important info about data points and enemies
		PrepareDataPoints();
		PrepareEnemies();
		
		cout << "MOVE 8000 4500" << endl; // MOVE x y or SHOOT id
	}
}