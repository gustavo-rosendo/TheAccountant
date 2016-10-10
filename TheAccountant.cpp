#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <list>
#include <math.h>

using namespace std;

#define ARENA_WIDTH 16000
#define ARENA_HEIGHT 9000

float MAX_DISTANCE = sqrt(16000 * 16000 + 9000 * 9000);

int ENEMY_STEP = 500;
int DEATH_RANGE = 2500;
int MULTIPLYING_FACTOR = 1002000;

int getMax(int a, int b) {
	return (a > b) ? a : b;
}

int getMin(int a, int b) {
	return (a < b) ? a : b;
}

//Vector2
class Vector2
{
public:
	//Constructors
	Vector2() {
		x = 0.0f;
		y = 0.0f;
	}
	Vector2(float _x, float _y){
		x = _x;
		y = _y;
	}
	Vector2(float * pArg){
		x = pArg[0];
		y = pArg[1];
	}
	Vector2(Vector2 & vector) {
		x = vector.x;
		y = vector.y;
	}

	//Vector's operations
	float Length()
	{
		return sqrt(x*x + y*y);
	}

	Vector2 & Normalize()
	{
		float lenInv = 1.0f / Length();
		x *= lenInv;
		y *= lenInv;

		return *this;
	}

	Vector2 operator + (Vector2 & vector)
	{
		Vector2 temp(x + vector.x, y + vector.y);
		return temp;
	}

	Vector2 & operator += (Vector2 & vector)
	{
		x += vector.x;
		y += vector.y;

		return *this;
	}

	Vector2 operator - ()
	{
		Vector2 temp(-x, -y);
		return temp;
	}

	Vector2 operator - (Vector2 & vector)
	{
		Vector2 temp(x - vector.x, y - vector.y);
		return temp;
	}

	Vector2 & operator -= (Vector2 & vector)
	{
		x -= vector.x;
		y -= vector.y;

		return *this;
	}

	Vector2 operator * (float k)
	{
		Vector2 temp(x * k, y * k);
		return temp;
	}

	Vector2 & operator *= (float k)
	{
		x *= k;
		y *= k;

		return *this;
	}

	Vector2 operator / (float k)
	{
		float kInv = 1.0f / k;
		Vector2 temp(x * kInv, y * kInv);
		return temp;
	}

	Vector2 & operator /= (float k)
	{
		return operator *= (1.0f / k);
	}

	Vector2 & operator = (Vector2 & vector)
	{
		x = vector.x;
		y = vector.y;

		return *this;
	}

	float operator [] (unsigned int idx)
	{
		return (&x)[idx];
	}

	Vector2 Modulate(Vector2 & vector)
	{
		Vector2 temp(x * vector.x, y * vector.y);
		return temp;
	}

	float Dot(Vector2 & vector)
	{
		return x * vector.x + y * vector.y;
	}

	//data members
	float x;
	float y;
};


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
	Vector2 directionToPlayer;
	Vector2 directionToDP;

	Enemy() {}

	Enemy(const Enemy& e) {
		id = e.id;
		x = e.x;
		y = e.y;
		life = e.life;

		targetDP = e.targetDP;
		distToPlayer = e.distToPlayer;
		distToDataPoint = e.distToDataPoint;
		distToPlayerAfterMove = e.distToPlayerAfterMove;
		nextX = e.nextX;
		nextY = e.nextY;
		directionToPlayer.x = e.directionToPlayer.x;
		directionToPlayer.y = e.directionToPlayer.y;
	}

	void CalculateNextXY(){
		//!! Call this function only when the targetDP and distToDataPoint are already calculated!!
		if (distToDataPoint <= ENEMY_STEP) {
			nextX = targetDP.x;
			nextY = targetDP.y;
		}
		else {
			nextX = floor((ENEMY_STEP * (targetDP.x - x) / distToDataPoint) + x);
			nextY = floor((ENEMY_STEP * (targetDP.y - y) / distToDataPoint) + y);
		}
	}

	bool CanBeKilled() {
		bool canBeKilled = false;
		int damage = round(125000.0f / (pow(distToPlayer, 1.2f)));
		int remainingLife = life - damage;

		if (remainingLife <= 0) {
			canBeKilled = true;
		}
		return canBeKilled;
	}

};

list<DataPoint> dataPoints;
list<Enemy> enemies;

class Player {
public:
	int x;
	int y;
	int nextX, nextY;
	string action;
	Vector2 directionNextMove;

	list<Enemy> findEnemyWhichComeCloser(){
		list<Enemy> result;
		for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
			Vector2 dummyDirToPlayer;
			dummyDirToPlayer.x = x - it->x;
			dummyDirToPlayer.y = y - it->y;
			Vector2 dummyDirToTargetDP;
			dummyDirToTargetDP.x = it->targetDP.x - it->x;
			dummyDirToTargetDP.y = it->targetDP.y - it->y;

			float dotProd = dummyDirToPlayer.Dot(dummyDirToTargetDP);
			cerr << "DOT : " << dotProd << " distToDataPoint : " << it->distToDataPoint << endl;
			if(dotProd > (it->distToDataPoint/2) && dotProd <= (it->distToDataPoint)){
				result.push_back(*it);
			}
		}
		return result;
	}

	void CalculateNextMove() {
		nextX = x;
		nextY = y;
		int wallRangeThreshold = 1200;

		directionNextMove *= MULTIPLYING_FACTOR;
		cerr << "Current X : " << x << " , Current Y : " << y << endl;
		cerr << "directionNextMove.x : " << directionNextMove.x << " directionNextMove.y : " << directionNextMove.y << endl;
		/*dummyNextX += directionNextMove.x;
		dummyNextY += directionNextMove.y;*/
		bool isFollow = true;
		Enemy closestEnemy = findClosestEnemyToPlayer();
		list<Enemy> incomingEnemy = findEnemyWhichComeCloser();
		int xGreaterThanPlayer = 0;
		int xLessThanPlayer = 0;
		int yGreaterThanPlayer = 0;
		int yLessThanPlayer = 0;

		for (list<Enemy>::iterator it = incomingEnemy.begin(); it != incomingEnemy.end(); ++it){
			if(it->x > x)
				xGreaterThanPlayer++;
			else
				xLessThanPlayer++;

			if(it->y > y)
				yGreaterThanPlayer++;
			else
				yLessThanPlayer++;
		}

		cerr << xGreaterThanPlayer << " " << xLessThanPlayer << " " << yGreaterThanPlayer << " " << yLessThanPlayer << endl;

		if(pow(directionNextMove.x,2) > pow(directionNextMove.y,2)){
			if(x >= ARENA_WIDTH/2){
				if(directionNextMove.x >= 0){
					if(xLessThanPlayer >= xGreaterThanPlayer && (ARENA_WIDTH - x) >= wallRangeThreshold){
						isFollow = true;
					}else{
						isFollow = false;
					}
				}else{
					if(xLessThanPlayer >= xGreaterThanPlayer && (ARENA_WIDTH - x) >= wallRangeThreshold){
						isFollow = false;
					}else{
						isFollow = true;
					}
				}
			}else{
				if(directionNextMove.x >= 0){
					if(xGreaterThanPlayer <= xLessThanPlayer && (x) >= wallRangeThreshold){
						isFollow = false;
					}else{
						isFollow = true;
					}
				}else{
					if(xGreaterThanPlayer <= xLessThanPlayer && (x) >= wallRangeThreshold){
						isFollow = true;
					}else{
						isFollow = false;
					}
				}
			}
		}else if(pow(directionNextMove.x,2) < pow(directionNextMove.y,2)){
			if(y >= ARENA_HEIGHT/2){
				cerr << "A" << endl;
				if(directionNextMove.y >= 0){
					cerr << "A1" << endl;
					if(yLessThanPlayer >= yGreaterThanPlayer && (ARENA_HEIGHT - y) >= wallRangeThreshold){
						cerr << "A1-1" << endl;
						isFollow = true;
					}else{
						cerr << "A1-2" << endl;
						isFollow = false;
					}
				}else{
					cerr << "A2" << endl;
					if(yLessThanPlayer >= yGreaterThanPlayer && (ARENA_HEIGHT - y) >= wallRangeThreshold){
						cerr << "A2-1" << endl;
						isFollow = false;
					}else{
						cerr << "A2-2" << endl;
						isFollow = true;
					}
				}
			}else{
				cerr << "B" << endl;
				if(directionNextMove.y >= 0){
					if(yGreaterThanPlayer >= yLessThanPlayer && (y) >= wallRangeThreshold){
						isFollow = false;
					}else{
						isFollow = true;
					}
				}else{
					if(yGreaterThanPlayer >= yLessThanPlayer && (y) >= wallRangeThreshold){
						isFollow = true;
					}else{
						isFollow = false;
					}
				}
			}
		}
		
		if(isFollow){
			nextX += directionNextMove.x;
			nextY += directionNextMove.y;
		}else{
			nextX -= directionNextMove.x;
			nextY -= directionNextMove.y;
		}
		
		cerr << "After X : " << nextX << " , After Y : " << nextY << endl;

		/*if(dummyNextX > ARENA_WIDTH || dummyNextX < 0 || dummyNextY > ARENA_HEIGHT || dummyNextY < 0){
			nextX -= directionNextMove.x;
			nextY -= directionNextMove.y;
		}else{
			nextX = dummyNextX;
			nextY = dummyNextY;
		}*/
	}

	int getDirectionVectorType(Vector2 input){
		if(input.x <= 0){
			if(input.y <= 0){
				return 1;
			}else if(input.y > 0){
				return 2;
			}
		}else if(input.x > 0){
			if(input.y <= 0){
				return 4;
			}else if(input.y > 0){
				return 3;
			}
		}
	}

	list<Enemy> IsGoingToDie() {
		directionNextMove.x = 0.0f;
		directionNextMove.y = 0.0f;
		list<Enemy> killers;
		for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
			if (it->distToPlayerAfterMove <= DEATH_RANGE) {
				killers.push_back(*it);
				
				//Calculate Vector Direction to DP
				Vector2 temp1(it->targetDP.x, it->targetDP.y);
				Vector2 temp2(it->x, it->y);
				it->directionToDP.x = temp1.x - temp2.x;
				it->directionToDP.y = temp1.y - temp2.y;
				it->directionToDP.Normalize();
				it->directionToDP /= it->distToPlayer;

				int enemyMovementType = -1;
				getDirectionVectorType(it->directionToDP);

				Vector2 nextBestDirection;

				bool isEvadedClockWise = true;
				if(isEvadedClockWise){
					nextBestDirection.x= (it->directionToDP.y);
					nextBestDirection.y= -(it->directionToDP.x);
				}else{
					nextBestDirection.x= -(it->directionToDP.y);
					nextBestDirection.y= (it->directionToDP.x);
				}

				directionNextMove += nextBestDirection;
			}
		}
		return killers;
	}

	int findClosestEnemyDP() {
		Enemy closestOne = enemies.front();
		for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
			//if (it->distToDataPoint < closestOne.distToDataPoint) {
			if (it->distToPlayer < closestOne.distToPlayer) {
				closestOne = *it;
			}
		}
		return closestOne.id;
	}

	Enemy findClosestEnemyToPlayer() {
		Enemy closestOne = enemies.front();
		for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
			if (it->distToPlayer < closestOne.distToPlayer) {
				closestOne = *it;
			}
		}
		return closestOne;
	}

	void DecideAction() {
		action = "";
		//1- Check if we are going to be killed by an enemy in the next round
		//   YES: Check if we can kill it from our current position
		//			YES: then, kill it!
		//			NO : run, Forest, run! But calculate where we should run to first based on all enemies' moving directions and distance from player
		list<Enemy> killers = IsGoingToDie();
		if (!killers.empty()) {
			//Check if we can kill it from our current position
			if (killers.size() == 1) {
				Enemy killer = killers.front();
				if (killer.CanBeKilled()) {
					//kill it!
					action = "SHOOT " + to_string(killer.id);

					cerr << "killer.id = " << killer.id << endl;
					cerr << action << endl;
				}
				else{
					//move your ass!
					CalculateNextMove();
					action = "MOVE ";
					action.append(to_string(nextX));
					action.append(" ");
					action.append(to_string(nextY));
				}
			}
			else {
				//move your ass!
				CalculateNextMove();
				action = "MOVE ";
				action.append(to_string(nextX));
				action.append(" ");
				action.append(to_string(nextY));
			}
		}
		//   NO : proceed with the algorithm 
		else {
			//Enemy enemyToShoot = findClosestEnemyDP();
			int enemyID = findClosestEnemyDP();
			action = "SHOOT " + to_string(enemyID);
		}

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
	//reset the direction Vector
	myPlayer.directionNextMove;

	for (list<Enemy>::iterator it = enemies.begin(); it != enemies.end(); ++it){
		it->distToPlayer = CalculateDistance(it->x, it->y, myPlayer.x, myPlayer.y);

		it->distToDataPoint = MAX_DISTANCE;
		for (list<DataPoint>::iterator itDP = dataPoints.begin(); itDP != dataPoints.end(); ++itDP){
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

		//Calculate Vector Direction to Player
		Vector2 temp1(myPlayer.x, myPlayer.y);
		Vector2 temp2(it->x, it->y);
		it->directionToPlayer.x = temp1.x - temp2.x;
		it->directionToPlayer.y = temp1.y - temp2.y;
		it->directionToPlayer.Normalize();
		it->directionToPlayer /= it->distToPlayer;

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