#include "Player.h"

//==============================================================================================//
//==============================================================================================//

/*
@param name - player name
@constructor
*/
Player::Player(string& name)
{
	player_name = name;
	start = time(NULL); //starts counting the player time
	num_helps = 0;
}

//==============================================================================================//
//==============================================================================================//

/*
Saves the player stats on a file
@param outfile - file where the player stats shall be saved
*/
void Player::sendToFile(ofstream &outfile)
{
	end = time(NULL);
	diff = (time_t)difftime(end, start); //player time
	outfile << player_name << ": " << diff << " segundos" << "( " << num_helps << " cluses added)"; //send to file the player stats(time and number of hepls)
}
