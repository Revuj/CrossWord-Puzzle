#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream> 

using namespace std;

class Board
{
public:
	Board(int lines, int columns);
	Board(ifstream &infile, map<string, string>& moves);
	void insertword(char line, char column, char direction, string &word);
	void removeword(char line, char column, char direction);
	void boardToSolve();
	bool finished() const;
	void showBoard() const;
	void showBoard_toSolve() const;
	int can_be_placed(char line, char column, char direction, string &word);
	void sendtofile(ofstream &outfile, map<string, string> &moves) const;
	void sendtofile_finished(ofstream &outfile, map<string, string> &moves) const;
	string get_word(size_t line, size_t column, char direction) const;
	string get_line(char line) const;
	string get_column(char column) const;
private:
	size_t lines, columns;
	vector<string> board;
	map<string, string> moves;
};

#endif

