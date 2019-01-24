#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include "Board.h"
#include "Dictionary.h"

using namespace std;
void option();

//==============================================================================================//
//==============================================================================================//

/*
Sends the board to be saved on a file
@param board - board, to be saved, to be resumed later
@param dic_name - name of the dictionary associated to the board
@param moves - map with the positions and the respective words of the board
*/
void saveToResume(const Board& board, const string& dic_name, map<string, string> &moves, int &num_board)
{
	ostringstream ss;
	ss << setw(3) << setfill('0') << num_board;
	string outfile_name(ss.str());
	outfile_name = "b" + outfile_name + ".txt";
	ifstream infile;
	infile.open(outfile_name);
	string name = outfile_name;
	while (!infile.fail()) //if the file already exists, increase the number of the board
	{
		infile.close();
		num_board++;
		ostringstream nameoss;
		nameoss << setw(3) << setfill('0') << num_board;
		name = nameoss.str();
		name = "b" + name + ".txt";
		infile.open(name);
	}
	infile.close();
	ofstream outfile;
	outfile.open(name);
	outfile << dic_name << endl;
	board.sendtofile(outfile, moves);
	outfile << endl;
	outfile.close();
	cout << "Crossword puzzle saved in " << name << " ...\n";
	cout << endl;
	option();
}

//==============================================================================================//
//==============================================================================================//

/*
Ensures that the board is completly valid before being saved
@param board - board that the user wants to save
@param dic - dictionary associated to the board
@param lines - number of lines of the board
@param columns - number of colums of the board
@function return valid - true if the board is ready to be saved
*/
bool valid_board(const Board &board, const Dictionary& dic, size_t lines, size_t columns)
{
	for (size_t i = 0; i < lines; i++) //loop that reads all the words in a line and checks if they are valid;
	{
		string s = board.get_line((char)(65 + i));
		string::iterator begin = s.begin();

		for (string::iterator it = begin; it != s.end(); ++it)
		{
			if ((it == begin && *it != '.' && *it != '#') || (*it != '.' && *it != '#' && (*(it - 1) == '.' || *(it - 1) == '#')))
				begin = it;
			if ((it == (s.end() - 1) && *it != '.' && *it != '#') || (*it != '.' && *it != '#' && (*(it + 1) == '.' || *(it + 1) == '#')))
			{
				string word(begin, it + 1);
				if (!word.empty())
				{
					if (word.length() != 1 && !dic.valid_word(word))
					{
						int index = static_cast<int>(i);
						cout << "Invalid word at " << char(index + 65) << char(distance(s.begin(), begin) + 97) << "H" << endl;
						return false;
					}
				}
			}
		}
	}

	cout << endl;

	for (size_t i = 0; i < columns; i++) // loop that checks all the words in a column and checks if they are valid
	{
		string s = board.get_column((char)(97 + i));
		string::iterator begin = s.begin();

		for (string::iterator it = begin; it != s.end(); ++it)
		{
			if ((it == begin && *it != '.' && *it != '#') || (*it != '.' && *it != '#' && (*(it - 1) == '.' || *(it - 1) == '#')))
				begin = it;
			if ((it == (s.end() - 1) && *it != '.' && *it != '#') || (*it != '.' && *it != '#' && (*(it + 1) == '.' || *(it + 1) == '#')))
			{
				string word(begin, it + 1);
				if (!word.empty())
				{
					if (word.length() != 1 && !dic.valid_word(word)) //we do not consider a word with only one letter invalid
					{
						int index = static_cast<int>(i);
						cout << "Invalid word at " << char(distance(s.begin(), begin) + 65) << char(index + 97) << "V" << endl;
						return false;
					}
				}
			}
		}
	}

	return true;
}

//==============================================================================================//
//==============================================================================================//

/*
Sends the board to be saved on a file
@param board - finished board, to be saved
@param dic_name - name of the dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param dic - dic associated to the board
@param num_board - number of the board to be saved
*/
void saveFinished(const Board& board, const string& dic_name, map<string, string> &moves, Dictionary &dic, int &num_board)
{
	size_t lines = board.get_line('A').length();
	size_t columns = board.get_column('a').length();
	if (!valid_board(board, dic, lines, columns))
	{
		cout << "You need to change some things..." << endl;
		board.showBoard();
	}
	else
	{
		ostringstream ss;
		ss << setw(3) << setfill('0') << num_board;
		string outfile_name(ss.str());
		outfile_name = "b" + outfile_name + ".txt";
		ifstream infile;
		infile.open(outfile_name);
		string name = outfile_name;
		while (!infile.fail())  //if the file already exists, increase the number of the board
		{
			infile.close();
			num_board++;
			ostringstream nameoss;
			nameoss << setw(3) << setfill('0') << num_board;
			name = nameoss.str();
			name = "b" + name + ".txt";
			infile.open(name);
		}
		infile.close();
		ofstream outfile;
		outfile.open(name);
		outfile << dic_name << endl;
		board.sendtofile_finished(outfile, moves);
		outfile << endl;
		outfile.close();
		cout << "Crossword puzzle saved in " << name << " ...\n";
		cout << endl;
		option();
	}
}

//==============================================================================================//
//==============================================================================================//

/*
Displays a menu where the user can chose how he wants to save the board
@param board - board to be saved
@param dic_name - name of the dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param dic - dic associated to the board
@param num_board - number of the board to be saved
*/
void saveMenu(Board &board, string &dic_name, Dictionary &dic, map<string, string> &moves, int &num_board)
{
	cin.clear();
	char s;
	while (cout << "Do you want to save the board and resume later ( S ) or to finish ( F ) ? " && !(cin >> s))
	{
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "Invalid input, please re-enter\n";
	}

	toupper(s);

	if (s == 'S')
		saveToResume(board, dic_name, moves, num_board);

	else if (s == 'F')
		saveFinished(board, dic_name, moves, dic, num_board);
}

//==============================================================================================//
//==============================================================================================//

/*
Helps the user by displaying the words that can be placed in a specified position
@param board - board where the word is to be placed
@param dic - dictionary associated to the board
@param line - line where the user wants to place a word
@param column - column where the user wants to place a word
@param direction - direction of the word to be placed
*/
void userNeedsHelp(Board &board, Dictionary &dic, char line, char column, char direction)
{
	vector<string> possible_words;
	if (direction == 'H')
	{
		string token = board.get_line(line);
		token.erase(0, column - 97);
		dic.help(possible_words, token, token.size());
	}
	if (direction == 'V')
	{
		string token = board.get_column(column);
		token.erase(0, line - 65);
		dic.help(possible_words, token, token.size());
	}

	if (possible_words.size() != 0)
	{
		for (size_t m = 0; m < possible_words.size(); m++)
		{
			if (board.can_be_placed(line, column, direction, possible_words[m]) == 4)
				cout << possible_words[m] << endl;
		}
	}
	else cout << "None of the dictionary words can be placed\n";
	board.showBoard();
}

//==============================================================================================//
//==============================================================================================//

/*
itsAWord is called if the input was not "-" , "+", or "?" and calls the function that checks if the word can be placed, displaying a message depending of the situation
@param board - board where the word is to be placed
@param dic - dictionary associated to the board
@param line_column_direction - string that contains the position of the word to be placed
@param word - word to be placed
*/
void itsAWord(Board &board, Dictionary &dic, map<string, string> &moves, string &line_column_direction, string &word)
{
	char line, column, direction;
	istringstream iss(line_column_direction);
	iss >> line >> column >> direction;

	transform(word.begin(), word.end(), word.begin(), toupper);
	if (dic.valid_word(word) && board.can_be_placed(line, column, direction, word) == 4)
	{
		board.insertword(line, column, direction, word);
		moves.insert(pair<string, string>(line_column_direction, word));
	}
	else if (board.can_be_placed(line, column, direction, word) == 5)
		cout << "Word already introduced\n";
	else if (board.can_be_placed(line, column, direction, word) == 1)
		cout << "This word doesn't fit on the board\n";
	else if (board.can_be_placed(line, column, direction, word) == 2)
		cout << "Invalid interseption of words\n";
	else if (!dic.valid_word(word))
		cout << "Invalid word\n";
	board.showBoard();
}



//==============================================================================================//
//==============================================================================================//

/*
Depending of the user input, edit_board calls the respective functions that are needed to edit the bord
@param board - board where the word is to be placed
@param dic - dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param word - input word that can be "+", "-", "?" or a word to be placed on the board
@param line_column_direction - string that contains the position of the word to be placed
*/
void edit_board(Board &board, Dictionary &dic, map<string, string> &moves, string &word, string &line_column_direction)
{
	char line, column, direction;
	istringstream iss(line_column_direction);
	iss >> line >> column >> direction;


	if (word == "+")
	{
		string s = board.get_word(line, column, direction);

		if (!dic.valid_word(s))
			cout << "The word is invalid." << endl;
		else
			cout << "The word is valid!" << endl;
	}

	else if (word == "-")
	{
		board.removeword(line, column, direction);
		map<string, string>::iterator it = moves.find(line_column_direction);
		if (it != moves.end())
			moves.erase(line_column_direction);
		board.showBoard();
	}

	else if (word == "?")
		userNeedsHelp(board, dic, line, column, direction);


	else
		itsAWord(board, dic, moves, line_column_direction, word);
}

//==============================================================================================//
//==============================================================================================//

/*
Allows the user to input
@param board - board where the word is to be placed
@param dic - dictionary associated to the board
@param moves - map with the positions and the respective words of the board
@param dictionary_name - name of the dictionary associated with the board
@param num_board - number of the board
*/
void userInputs(Board &board, Dictionary &dic, map<string, string> &moves, string &dictionary_name, int& num_board)
{
	string line_column_direction;
	while (cout << "\nPosition ( LCD / CTRL-Z = stop ) ? " && !(cin >> line_column_direction) && !cin.eof())
	{
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "Invalid input, please re-enter\n";
	}

	if (cin.eof())
	{
		saveMenu(board, dictionary_name, dic, moves, num_board);
		return;
	}

	char line, column, direction;
	istringstream iss(line_column_direction);
	iss >> line >> column >> direction;
	while (line_column_direction.length() != 3 || (direction != 'H' && direction != 'V') || islower(line) || isupper(column))
	{
		cout << "LCD ( L-uppercase , C-lowercase , D-H or V ) ? ";
		cin >> line_column_direction;
		istringstream iss(line_column_direction);
		iss >> line >> column >> direction;
	}

	string word;
	while (cout << "Word ( - = remove / + = check if word is valid / ? = help ) ? " && !(cin >> word))
	{
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "Invalid input, please re-enter\n";
	}

	edit_board(board, dic, moves, word, line_column_direction);
}

//==============================================================================================//
//==============================================================================================//

/*
Displays a menu that allows the user to creat a Puzzle
*/
void createPuzzle()
{
	string dictionary_name;
	cout << "----------------------------------------\n";
	cout << "CREATE PUZZLE\n";
	cout << "----------------------------------------\n";
	cout << "Dictionary file name ? ";
	cin >> dictionary_name;
	ifstream dictionaryFile;
	dictionaryFile.open(dictionary_name);
	while (dictionaryFile.fail())
	{
		cout << "Dictionary not found!\n";
		cout << endl << "Dictionary file name ? ";
		cin >> dictionary_name;
		dictionaryFile.open(dictionary_name);
	}
	cout << "Loading Dictionary..." << endl;
	Dictionary dic(dictionary_name);
	int lines, columns;

	while (cout << "Board size (lines columns) ? " && !(cin >> lines >> columns))
	{
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "Invalid input, please re-enter\n";
	}
	Board board(lines, columns);
	int num_board = 1;
	cout << endl;
	board.showBoard();
	string line_column_direction;
	map<string, string> moves;

	while (!board.finished())
		userInputs(board, dic, moves, dictionary_name, num_board);

	ostringstream ss;
	ss << setw(3) << setfill('0') << num_board;
	string outfile_name(ss.str());
	outfile_name = "b" + outfile_name + ".txt";
	ofstream outfile;
	outfile.open(outfile_name);
	outfile << dictionary_name << endl;
	board.sendtofile_finished(outfile, moves);
	outfile << endl;
	outfile.close();
	cout << "Crossword puzzle saved in " << outfile_name << " ...\n";
	cout << endl;
	option();
}

//==============================================================================================//
//==============================================================================================//

/*
Allows the user to write the name of the board that he wants to resume
*/
void resumePuzzle()
{
	cout << "----------------------------------------\n";
	cout << "RESUME PUZZLE\n";
	cout << "----------------------------------------\n";
	string board_name;
	int num_board = 1;
	cout << "Board file name ? ";
	cin >> board_name;
	ifstream boardFile;
	boardFile.open(board_name);
	if (boardFile.fail())
	{
		cerr << "Board not found!\n";
		exit(1);
	}
	string dictionary_name;
	getline(boardFile, dictionary_name);
	cout << "Loading dictionary..." << endl;
	Dictionary dic(dictionary_name);
	map<string, string> moves;
	Board board(boardFile, moves);
	board.showBoard();
	string line_column_direction;

	while (!board.finished())
		userInputs(board, dic, moves, dictionary_name, num_board);

	ostringstream ss;
	ss << setw(3) << setfill('0') << num_board;
	string outfile_name(ss.str());
	outfile_name = "b" + outfile_name + ".txt";
	ofstream outfile;
	outfile.open(outfile_name);
	outfile << dictionary_name << endl;
	board.sendtofile_finished(outfile, moves);
	outfile << endl;
	outfile.close();
	cout << "Crossword puzzle saved in " << outfile_name << " ...\n";
	cout << endl;
	option();
}

//==============================================================================================//
//==============================================================================================//

/*
Allows the user to chose between create,resume a puzzle or to exit
*/
void option()
{
	cin.clear();
	int option;

	cout << "----------------------------------------\n";
	cout << "Options:\n";
	cout << "1 - Create puzzle\n";
	cout << "2 - Resume puzzle\n";
	cout << "0 - Exit\n";
	cout << "----------------------------------------\n";

	while (cout << "Option ? " && (!(cin >> option) || option < 0 || option > 2))
	{
		cin.clear();
		cin.ignore(100000, '\n');
		cout << "Invalid input, please re-enter\n";
	}

	switch (option)
	{
	case 0:
		cout << "Bye!!\n";
		exit(0);
		break;
	case 1:
		createPuzzle();
		break;
	case 2:
		resumePuzzle();
		break;
	}
}

//==============================================================================================//
//==============================================================================================//

int main()
{
	cout << "CROSSWORDS PUZZLE CREATOR\n";
	cout << "========================================\n";
	cout << "\n" << "INSTRUCTIONS:\n";
	cout << "Position ( LCD / CTRL-Z = stop to save)\n";
	cout << "LCD ( L - uppercase , C - lowercase , D- H or V )\n\n";
	option();
	return 0;
}
