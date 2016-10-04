#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <math.h>

using namespace std;

float MAX_DISTANCE = sqrt(16000 * 16000 + 9000 * 9000);

int ENEMY_STEP = 500;
int DEATH_RANGE = 2000;

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
		if (distToDataPoint <= ENEMY_STEP) {
			nextX = targetDP.x;
			nextY = targetDP.y;
		}
		else {
			nextX = floor((ENEMY_STEP * (targetDP.x - x) / distToDataPoint) - x);
			nextY = floor((ENEMY_STEP * (targetDP.y - y) / distToDataPoint) - y);
		}
	}

	bool CanBeKilled() {
		bool canBeKilled = false;
		int damage = round(125000.0f / (pow(distToPlayer, 1.2f)));
		int remainingLife = life - damage;

		if (remainingLife <= 0) {
			canBeKilled = true;
		}
	}

};

list<DataPoint> dataPoints;
list<Enemy> enemies;

class Player {
public:
	int x;
	int y;
	string action;

	list<Enemy> IsGoingToDie() {
		list<Enemy> killers;
		for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
			if (it->distToPlayerAfterMove <= DEATH_RANGE) {
				killers.push_back(*it);
			}
		}
	}

	void DecideAction() {
		action = "";
		//1- Check if we are going to be killed by an enemy in the next round
		//   YES: Check if we can kill it from our current position
		list<Enemy> killers = IsGoingToDie();
		if (!killers.empty() && killers.size() == 1) {
			//Check if we can kill it from our current position
			Enemy killer = killers.front();
			if (killer.CanBeKilled()) {
				//kill it!
				action = "SHOOT " + killer.id;
			}
			else {
				//move your ass!
			}
		}
		//			YES: then, kill it!
		//			NO : run, Forest, run! But calculate where we should run to first based on all enemies' moving directions and distance from player
		//   NO : proceed with the algorithm 

		//2- Check what enemy is the closest to a data point

		//3- Check if we can kill it from our current position
		//	 YES: then, kill it!
		//	 NO : Calculate where we should move to be able to kill it or how many shots are needed to kill it before it gets the data point
	}
};

Player myPlayer;

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

		//start decision-making algorithm for myPlayer
		myPlayer.DecideAction();
		
		cout << myPlayer.action << endl; // MOVE x y or SHOOT id
	}
}