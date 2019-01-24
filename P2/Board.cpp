#include "Board.h"
#include <iostream>
#include <iomanip> 
#include <ctime>
#include <cstdlib>
#include <windows.h>
#include <cctype>

using namespace std;

//==============================================================================================//
//==============================================================================================//

//COLOR CODES: (alternative: use symbolic const’s)
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15
//==========================================================================================
void clrscr(void)
{
	COORD coordScreen = { 0, 0 }; // upper left corner
	DWORD cCharsWritten;
	DWORD dwConSize;
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hCon, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	// fill with spaces
	FillConsoleOutputCharacter(hCon, TEXT(' '), dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hCon, &csbi);
	FillConsoleOutputAttribute(hCon, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	// cursor to upper left corner
	SetConsoleCursorPosition(hCon, coordScreen);
}
//==========================================================================================
// Position the cursor at column 'x', line 'y'
// The coodinates of upper left corner of the screen are (x,y)=(0,0)
void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
//==========================================================================================
// Set text color
void setcolor(unsigned int color)
{
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hcon, color);
}
//==========================================================================================
// Set text color & background
void setcolor(unsigned int color, unsigned int background_color)
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	if (background_color == BLACK)
		SetConsoleTextAttribute(hCon, color);
	else
		SetConsoleTextAttribute(hCon, color | BACKGROUND_BLUE | BACKGROUND_GREEN |
			BACKGROUND_RED);
}

//==============================================================================================//
//==============================================================================================//

/*
@param lines - number of lines of the board to be created
@param columns - number of columns of the board to be created
@constructor
*/
Board::Board(int lines, int columns)
{
	this->lines = lines;
	this->columns = columns;
	for (size_t i = 0; i < this->lines; i++)
	{
		string word;
		for (size_t j = 0; j < this->columns; j++)
			word.push_back('.');
		board.push_back(word);
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Tests if line from the board file belongs to the actual board
@param s - the line (string)
@function return value: true if the line is from the actual board
*/
bool valid_line(string s)
{
	for (size_t i = 0; i < s.length(); i++)
		if ((s[i] <'A' || s[i] > 'Z') && s[i] != '.' && s[i] != '#' && s[i] != ' ')
			return false;
	return true;
}

/*
Tests if the line from the board file represents a position and a word
@param s - the line (string)
@function return value: true if the line representes a position and a word
*/
bool valid_move(string s)
{
	if ((s[0] >= 'A' && s[0] <= 'Z') && (s[1] >= 'a' && s[1] <= 'z') && s.find('.') == string::npos)
	{
		return true;
	}
	return false;
}

//==============================================================================================//
//==============================================================================================//

/*
@param infile - file that contains the information about the board we want to create
@param moves - map where we insert the position the respective word that shall be writtend on the board
@constructor
*/
Board::Board(ifstream &infile, map<string, string>& moves)
{
	size_t num_lines = 0;
	size_t num_columns = 0;
	string line;
	string pos;
	string word;
	while (getline(infile, line))
	{
		if (!line.empty() && valid_line(line))
		{
			line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());
			num_columns = line.length();
			board.push_back(line);
			num_lines++;
		}

		if (!line.empty() && valid_move(line))
		{
			istringstream iss(line);
			iss >> pos >> word;
			moves.insert(pair<string, string>(pos, word));
		}

	}

	lines = num_lines;
	columns = num_columns;
}

//==============================================================================================//
//==============================================================================================//

/*
Checks if a word can be placed in a specific position
@param line - line where the user is trying to place the word
@param column - line where the user is trying to place the word
@param word - word that the user is trying to place on the board
@function return value:
1 if the word does not fit on the board
2 if it happens an invalid interseption of words
3 if the word was already introduced on the board
4 if the word can be placed
*/
int Board::can_be_placed(char line, char column, char direction, string &word)
{
	size_t i = line - 65, j = column - 97;
	ostringstream oss;
	oss << line << column << direction;
	string pos = oss.str();

	if (moves.find(pos) != moves.end())
	{
		removeword(line, column, direction);
		return 4; //word can be placed and substitute another
	}

	for (auto& x : moves)
	{
		if (x.second == word) //if the word was already placed
			return 3;
	}

	if (direction == 'H')
	{
		int space = columns - (j + word.length());

		if (space < 0) //if the word does not fit on the board
			return 1;

		for (size_t m = j; m < j + word.size(); m++)
		{
			if (board[i][m] != '#' && board[i][m] != '.' && board[i][m] != word[m - j]) //if this word intercepts another and the letter of the new word is different that the letter that was already placed
				return 2;
		}
	}

	else if (direction == 'V')
	{
		int space = lines - (i + word.length());

		if (space < 0) //if the word does not fit on the board
			return 1;

		for (size_t m = i; m < i + word.size(); m++)
		{
			if (board[m][j] != '#' && board[m][j] != '.' && board[m][j] != word[m - i]) //if this word intercepts another and the letter of the new word is different that the letter that was already placed
				return 2;
		}
	}

	return 4; //word can be placed on the board
}

//==============================================================================================//
//==============================================================================================//

/*
Inserts a word on the board
@param line -line where the user is trying to place the word
@param column - line where the user is trying to place the word
@param direction - direction of the word to be placed on the board
@param word - word to be placed on the board
*/
void Board::insertword(char line, char column, char direction, string &word)
{
	size_t i = line - 65, j = column - 97;

	if (direction == 'V')
	{
		for (size_t m = i; m < lines; m++)
		{
			if ((m - i) == word.size())
				break;
			board[m][j] = word[m - i];
		}

		if (i > 0 && board[i - 1][j] == '.')
			board[i - 1][j] = '#';
		if ((i + word.length()) < lines)
			board[i + word.length()][j] = '#';
	}

	if (direction == 'H')
	{
		for (size_t m = j; m < columns; m++)
		{
			if ((m - j) == word.size())
				break;
			board[i][m] = word[m - j];
		}

		if (j > 0 && board[i][j - 1] == '.')
			board[i][j - 1] = '#';
		if ((j + word.length()) < columns)
			board[i][j + word.length()] = '#';
	}

	ostringstream oss;
	oss << line << column << direction;
	string pos = oss.str();
	moves[pos] = word;
}

//==============================================================================================//
//==============================================================================================//

/*
Removes a word on the board
@param line -line where the user is trying to remove the word
@param column - column where the user is trying to remove the word
@param direction - direction of the word to be removed from the board
*/
void Board::removeword(char line, char column, char direction)
{
	size_t i = line - 65, j = column - 97;
	ostringstream oss;
	oss << line << column << direction;
	string pos = oss.str();

	map<string, string>::iterator it = moves.find(pos);
	if (it != moves.end()) //erase word from moves
		moves.erase(pos);

	if (direction == 'H')
	{
		size_t begin = 0, end = columns - 1;
		for (int m = j; m >= 0; m--) //loop that defines where the word starts
		{
			if (m == 0)
			{
				begin = m;
				break;
			}

			else if (board[i][m] == '#' && (size_t)(m + 1)<columns)
			{
				begin = m + 1;
				break;
			}

			else
			{
				begin = m;
				break;
			}
		}

		for (size_t m = j; m < columns; m++) //loop that defines where the word ends
		{
			if (board[i][m] == '#' && m != 0)
			{
				end = m - 1;
				break;
			}
		}

		for (size_t t = begin; t <= end; t++) //loop that decides wich letters are to eliminate
		{
			if (lines == 1)
				board[t][j] = '.';

			if (i == (lines - 1) && (board[i - 1][t] == '.' || board[i - 1][t] == '#'))
				board[i][t] = '.';

			else if (i == 0 && (board[i + 1][t] == '.' || board[i + 1][t] == '#'))
				board[i][t] = '.';

			else if (i != (lines - 1) && i != 0 && (board[i - 1][t] == '.' || board[i - 1][t] == '#') && (board[i + 1][t] == '.' || board[i + 1][t] == '#'))
				board[i][t] = '.';
		}
	}

	if (direction == 'V')
	{
		size_t begin = 0, end = lines - 1;
		for (int m = i; m >= 0; m--) //loop that defines the beginning of the word
		{
			if (m == 0)
			{
				begin = m;
				break;
			}

			else if (board[m][j] == '#' && (size_t)(m + 1)<lines)
			{
				begin = m + 1;
				break;
			}

			else
			{
				begin = m;
				break;
			}
		}

		for (size_t m = i; m < lines; m++) //loop that defines the end of the word
		{
			if (board[m][j] == '#' && m != 0)
			{
				end = m - 1;
				break;
			}
		}

		for (size_t t = begin; t <= end; t++) //loop that decides wich letters are to eliminate

		{
			if (columns == 1)
				board[t][j] = '.';

			if (j == (columns - 1) && (board[t][j - 1] == '.' || board[t][j - 1] == '#'))
				board[t][j] = '.';

			else if (j == 0 && (board[t][j + 1] == '.' || board[t][j + 1] == '#'))
				board[t][j] = '.';

			else if (j != (columns - 1) && j != 0 && (board[t][j + 1] == '.' || board[t][j + 1] == '#') && (board[t][j - 1] == '.' || board[t][j - 1] == '#'))
				board[t][j] = '.';
		}
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Replaces the letter by '.' to start solving the board
*/
void Board::boardToSolve()
{

	for (size_t i = 0; i < lines; i++)
	{
		for (size_t j = 0; j < columns; j++)
		{
			if (board[i][j] >= 'A' && board[i][j] <= 'Z')
				board[i][j] = '.';
		}
	}
}

//==============================================================================================//
//==============================================================================================//

/*
@param line - line to be returned
@function return value - returns a line (string) from the board
*/
string Board::get_line(char line) const
{
	size_t i = line - 65;
	string lineStr;

	for (size_t t = 0; t < this->columns; t++)
		lineStr.push_back(board[i][t]);

	remove_if(lineStr.begin(), lineStr.end(), isspace);
	return lineStr;
}

//==============================================================================================//
//==============================================================================================//

/*
@param column - column to be returned
@function return value - returns a column (string) from the board
*/
string Board::get_column(char column) const
{
	size_t j = column - 97;
	string columnStr;

	for (size_t t = 0; t < lines; t++)
		columnStr.push_back(board[t][j]);

	return columnStr;
}


//==============================================================================================//
//==============================================================================================//

/*
Checks if the board is full (finished)
@function return value: True if board is full
*/
bool Board::finished() const
{
	for (size_t i = 0; i < lines; i++)
		if (board[i].find('.') != string::npos)
			return false;
	return true;
}

//==============================================================================================//
//==============================================================================================//

/*
Saves a board, to be resumed later, on a file
@param outfile - file where the board shall be saved
@param moves - map that contains the positions and the respective words
*/
void Board::sendtofile(ofstream &outfile, map<string, string> &moves) const
{
	for (size_t m = 0; m < lines; m++)
	{
		outfile << endl;
		for (size_t j = 0; j < columns; j++)
			outfile << setw(2) << board[m][j];
	}

	outfile << endl << endl;

	for (auto& x : moves)
		outfile << x.first << " " << x.second << endl;
	outfile.close();
}

//==============================================================================================//
//==============================================================================================//

/*
Saves a board on a file
@param outfile - file where the board shall be saved
@param moves - map that contains the positions and the respective words
*/
void Board::sendtofile_finished(ofstream &outfile, map<string, string> &moves) const
{
	for (size_t m = 0; m < lines; m++)
	{
		outfile << endl;
		for (size_t j = 0; j < columns; j++)
		{
			if (board[m][j] != '.')
				outfile << setw(2) << board[m][j];
			else outfile << setw(2) << '#';
		}
	}

	outfile << endl << endl;

	for (auto& x : moves)
		outfile << x.first << " " << x.second << endl;
	outfile.close();
}

//==============================================================================================//
//==============================================================================================//

/*
@param line - line where the word is placed on the board
@param column - column where the word is placed on the board
@param direction - direction of the word
@function return value: returns the word on a specific postition
*/
string Board::get_word(size_t line, size_t column, char direction) const
{
	string word;
	size_t i = line - 65, j = column - 97;
	if (direction == 'H')
	{
		string s = get_line((char)line);
		size_t begin;
		size_t length = 0;

		for (j; j >= 0; j--)
		{
			if (board[i][j] == '#' || board[i][j] == '.')
			{
				begin = j + 1;
				break;
			}
			else if (j == 0)
			{
				begin = j;
				length++;
				break;
			}
			length++;
		}

		j = column - 97;
		for (j; j < columns; j++)
		{
			if (board[i][j] == '#' || board[i][j] == '.')
			{
				break;
			}
			else if (j == columns - 1)
			{
				length++;
				break;
			}
			length++;
		}

		word = s.substr(begin, length - 1);
	}

	else if (direction == 'V')
	{
		string s = get_column((char)column);
		size_t begin;
		size_t length = 0;

		for (i; i >= 0; i--)
		{
			if (board[i][j] == '#' || board[i][j] == '.')
			{
				begin = i + 1;
				break;
			}
			else if (i == 0)
			{
				begin = i;
				length++;
				break;
			}
			length++;
		}

		i = line - 65;
		for (i; i < columns; i++)
		{
			if (board[i][j] == '#' || board[i][j] == '.')
			{
				break;
			}
			else if (i == (lines - 1))
			{
				length++;
				break;
			}
			length++;
		}

		word = s.substr(begin, length - 1);
	}
	return word;
}

//==============================================================================================//
//==============================================================================================//

/*
Displays board
*/
void Board::showBoard() const
{
	setcolor(4);
	char column = 'a';
	char line = 'A';
	cout << "\n  ";
	for (size_t i = 0; i < columns; i++)
		cout << setw(2) << (char)(column + i);
	for (size_t m = 0; m < lines; m++)
	{
		setcolor(4);
		cout << endl << (char)(line + m) << " ";
		for (size_t j = 0; j < columns; j++)
		{
			if (board[m][j] == '#')
			{
				setcolor(15, 0);
				cout << setw(2) << board[m][j];
			}
			else
			{
				setcolor(0, 15);
				cout << setw(2) << board[m][j];
			}
		}
		setcolor(0, 15);
		cout << " ";
	}
	setcolor(15, 0);
	cout << endl;
}

//==============================================================================================//
//==============================================================================================//

/*
Displays board to be solved, i.e the function doesn't display '.' that corresponds to the positions of letters before solving
*/
void Board::showBoard_toSolve() const
{
	setcolor(4);
	char column = 'a';
	char line = 'A';
	cout << "\n  ";
	for (size_t i = 0; i < columns; i++)
		cout << setw(2) << (char)(column + i);
	for (size_t m = 0; m < lines; m++)
	{
		setcolor(4);
		cout << endl << (char)(line + m) << " ";
		for (size_t j = 0; j < columns; j++)
		{
			if (board[m][j] == '#')
			{
				setcolor(15, 0);
				cout << setw(2) << "#";
			}
			else if (board[m][j] == '.')
			{
				setcolor(0, 15);
				cout << setw(2) << " ";
			}
			else
			{
				setcolor(0, 15);
				cout << setw(2) << board[m][j];
			}
		}
		setcolor(15, 0);
		cout << " ";
	}
	setcolor(15, 0);
	cout << endl;
}
