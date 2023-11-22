#include <iostream>
#include <ncursesw/curses.h>
#include <fstream>
#include <vector>

std::vector<std::string> maze = {
        "#############################################################",
        "#  #           #                             #     #     #  #",
        "#  #  ####  #  ####  #  #######  ####  ##########  ####  #  #",
        "#  #  #     #     #  #        #  #              #        #  #",
        "#  #  #  #######  ####  #  #######  #  ####  ##########  #  #",
        "#     #  #           #  #  #     #  #  #     #  #           #",
        "#############  #  ####  #  #  #############  #  ####  ####  #",
        "#  #           #        #        #  #  #        #     #     #",
        "#  #######  #######  ####  #  ####  #  ####  ##########  #  #",
        "#     #     #  #  #  #  #  #  #  #                    #  #  #",
        "####  #  ####  #  #  #  #######  ####  #  ####  #######  #  #",
        "#        #           #     #  #        #  #        #  #  #  #",
        "#  #######  ####  ####  #  #  ####  ##########  #  #  ####  #",
        "#  #           #        #  #     #  #     #  #  #  #        #",
        "#  #  ####  ##########  ####  #  #  #  #  #  ##########  ####",
        "#     #     #  #              #  #     #  #  #     #        #",
        "####  ####  #  #######  #######  #  #######  #  #######  ####",
        "#     #     #     #        #     #        #     #     #     #",
        "##########  #  ####  ####  ####  #  ####  #  ####  #######  #",
        "#        #        #  #  #  #  #  #     #     #              #",
        "#  #######  #  #  #  #  ####  #  ####  #  #  #  #  ####  #  #",
        "#              #  #  #     #     #  #  #  #     #  #  #     #",
        "#  #############  #  ####  #  #######  ####  #  #  #  ####  #",
        "#  #  #     #     #  #     #              #  #        #     #",
        "#  #  ####  ####  #  #  ##########  #############  ####  #  #",
        "#           #                             #        #     #  #",
        "#############################################################",
    };
    
// CONSTANT VALUE OF INITIAL COIN CHARACTER
char coin_char = '.';
    
//std::vector<std::vector<int>> coin_initial_positions = {};

// TOTAL COINS AT GAME START
int initial_coin_quantity = 0;
    
// POSTIIONS OF COINS THAT HAVE BEEN FOUND
std::vector<std::vector<int>> coin_found_positions = {};

// COUNTER OF COINS FOUND
int coin_found_counter = 0;

// POSITION OF GHOST
std::vector<int> ghost_coords = {};

// IF PLAYER ENCOUNTERED GHOST
bool ghost_encountered = false;

    
// USEFUL FUNCTIONS
bool can_player_move(int row_index, int col_index) {
	
	if (maze[row_index][col_index] == '#') {
		return false;
	}
	return true;
}

bool is_coin_found(int coin_row_index, int coin_col_index) {
	for (int i = 0; i < coin_found_positions.size(); i++) {
		std::vector<int> cords = coin_found_positions[i];
		
		if (cords[0] == coin_row_index && cords[1] == coin_col_index) {
			return true;
		}
	}
	return false;
}

bool check_for_coin(int row_index, int col_index) {
	if (maze[row_index][col_index] == coin_char) {
		return true;
	}
	return false;
}

bool check_for_ghost(int row_index, int col_index) {
	if (ghost_coords[0] == row_index && ghost_coords[1] == col_index) {
		return true;
	}
	return false;
}

void remove_coin_from_maze(int row_index, int col_index) {
	maze[row_index][col_index] = ' ';
}

void init_ghost() {
	ghost_coords = {5, 5};
}


void ncurses_init_colors() {
	start_color();
	
	// WALLS
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	// COINS
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	// PLAYER
	init_pair(3, COLOR_RED, COLOR_BLACK);
	// GHOST
	init_pair(4, COLOR_WHITE, COLOR_MAGENTA);
}

void ncurses_config() {
	ncurses_init_colors();
	keypad(stdscr, TRUE);
	timeout(500);
	noecho();
}

void print_board(int x, int y, int character) {
	clear();
	
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].length(); j++) {
			move(i, j);
			
			// PRINT GHOST CHARACTER
			if (check_for_ghost(i, j)) {
				attron(COLOR_PAIR(4));
				addch('?');
				attroff(COLOR_PAIR(4));
				
			// CHECK IF CHARACTER FROM MAZE IS NOT A COIN THAT HAS BEEN FOUND
			} else if (check_for_coin(i, j) && is_coin_found(i, j)) {
//				attron(COLOR_PAIR(3));
				addch(' ');
//				attroff(COLOR_PAIR(3));
			} else {
				if (check_for_coin(i, j)) {
					attron(COLOR_PAIR(2));
					addch(coin_char);
					attron(COLOR_PAIR(2));
				} else {
					attron(COLOR_PAIR(1));
					addch(maze[i][j]);
					attroff(COLOR_PAIR(1));
				}
			}
		}
	}
	
	attron(COLOR_PAIR(3));
	move(y, x);
	addch(character);
	move(0, 0);
	attroff(COLOR_PAIR(3));
	
	refresh();
}

int main(void) {
	WINDOW * mainwin = initscr();
	ncurses_config();
	
	int player_character = '@';
	int player_col_index = 59, player_row_index = 25;
	
	// CREATE GHOST
	init_ghost();
	
	
	// FILL BLANKS WITH COINS
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].length(); j++) {
			if (maze[i][j] == ' ' && (i != player_row_index && j != player_col_index)) {
//				coin_initial_positions.push_back({i, j});
				maze[i][j] = coin_char;
				initial_coin_quantity += 1;
			}
		}
	}

//	int player_col_index = 1, player_row_index = 1;
	int width = 100, height = 100;
	
	while (true) {
		// pobiera wymiary terminala
		getmaxyx(stdscr, height, width); 
		
		int input = getch();
		if (input != ERR) {
			switch(input) {
				case KEY_UP:
					// sprawdzamy czy nie wychodzimy poza ekran
					if (player_row_index > 0) {
						if (can_player_move(player_row_index - 1, player_col_index)) {
							player_row_index -= 1;
						}
					}
					break;
				case KEY_DOWN:
					if (player_row_index < height - 1) {
						if (can_player_move(player_row_index + 1, player_col_index)) {
							player_row_index += 1;
						}
					}
					break;
				case KEY_LEFT:
					if (player_col_index > 0) {
						if (can_player_move(player_row_index, player_col_index - 1)) {
							player_col_index -= 1;
						}
					}
					break;
				case KEY_RIGHT:
					if (player_col_index < width - 1) {
						if (can_player_move(player_row_index, player_col_index + 1)) {
							player_col_index += 1;
						}
					}
					break;
				case 'q':
				case 'Q':
					return 0;
				default:
//					player_character = input;
					continue;
			}
		}
		
		// CHECK IF PLAYER ENCOUNTERED GHOST
		if (check_for_ghost(player_row_index, player_col_index) == true) {
			delwin(mainwin);
			endwin();
			refresh();
			return EXIT_SUCCESS;
		}
		
		// CHECK IF COINS HAS BEEN FOUND
		if (check_for_coin(player_row_index, player_col_index)) {
			coin_found_positions.push_back({player_row_index, player_col_index});
			coin_found_counter += 1;
			// REMOVE COIN ('o') FROM MAZE
			// TRANSFORM TO A FUNCTION
			remove_coin_from_maze(player_row_index, player_col_index);
//			maze[player_row_index][player_col_index] = ' ';
		}
		
		// CHECK FOR WIN
		if (coin_found_counter == initial_coin_quantity) {
			delwin(mainwin);
			endwin();
			refresh();
			return EXIT_SUCCESS;
		}		
		
		print_board(player_col_index, player_row_index, player_character);
	}
	
	// zakańczamy prace ncurses
	delwin(mainwin);
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
