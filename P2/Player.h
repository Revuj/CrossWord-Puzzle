#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <ctime>
#include <fstream>

using namespace std;

class Player
{
public:
	Player(string& name);
	void sendToFile(ofstream &outfile);
	int num_helps;
private:
	string player_name;
	time_t start, end, diff;
};

#endif


