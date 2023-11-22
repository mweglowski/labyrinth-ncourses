#include <iostream>
#include <ncursesw/curses.h>
#include <fstream>
#include <vector>
#include <random>

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
char wall_char = '#';
char player_char = '@';
char ghost_char = '?';
    
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
int get_random_int(int min, int max) {
	static std::random_device random_device;
	static std::mt19937 generator(random_device());
	
	std::uniform_int_distribution<> distribution(min, max);
	return distribution(generator);
}

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

bool check_for(int row_index, int col_index, std::string object_to_check) {
	char character = ' ';
	if (object_to_check == "coin") {
		character = coin_char;
	} else if (object_to_check == "wall") {
		character = wall_char;
	} else if (object_to_check == "ghost") {
		if (ghost_coords[0] == row_index && ghost_coords[1] == col_index) {
			return true;
		}
		return false;
	} else {
		character = player_char;
	}
	
	if (maze[row_index][col_index] == character) {
		return true;
	}
	return false;
}

void remove_coin_from_maze(int row_index, int col_index) {
	maze[row_index][col_index] = ' ';
}

void init_ghost() {
	int ghost_row_index = get_random_int(1, 25);
	int ghost_col_index = get_random_int(1, 55);
	
	while (maze[ghost_row_index][ghost_col_index] == '#') {
		ghost_row_index = get_random_int(1, 25);
		ghost_col_index = get_random_int(1, 50);
	}
	
	ghost_coords = {ghost_row_index, ghost_col_index};
}

bool is_direction_available(int direction, std::vector<int> available_directions) {
	for (int i = 0; i < available_directions.size(); i++) {
		if (available_directions[i] == direction) {
			return true;
		}
	}
	return false;
}

void move_ghost() {
	int direction = get_random_int(0, 3);
	int ghost_row_index = ghost_coords[0];
	int ghost_col_index = ghost_coords[1];
	
	// CHECKING ALL AVAILABLE DIRECTIONS
	std::vector<int> available_directions = {};
	
	if (check_for(ghost_row_index - 1, ghost_col_index, "wall") == false) {
		// UP
		available_directions.push_back(0);
	} else if (!check_for(ghost_row_index, ghost_col_index + 1, "wall")) {
		// RIGHT
		available_directions.push_back(1);
	} else if (check_for(ghost_row_index + 1, ghost_col_index, "wall") == false) {
		// DOWN
		available_directions.push_back(2);
	} else if (!check_for(ghost_row_index, ghost_col_index - 1, "wall")) {
		// LEFT
		available_directions.push_back(3);
	} else {
		// GHOST IN INESCAPABLE PLACE
		return;
	}
	
	// IF NOT AVAILABLE, GET NEW AND SO ON
	while (is_direction_available(direction, available_directions) == false) {
		direction = get_random_int(0, 3);
	}
	
	switch (direction) {
		case 0:
			ghost_coords = {ghost_coords[0] + 1, ghost_coords[1]};
			break;
		case 1:
			ghost_coords = {ghost_coords[0], ghost_coords[1] + 1};
			break;
		case 2:
			ghost_coords = {ghost_coords[0] - 1, ghost_coords[1]};
			break;
		case 3:
			ghost_coords = {ghost_coords[0], ghost_coords[1] - 1};
			break;
		default:
			return;	
	}
}

void ncurses_init_colors() {
	start_color();
	
	// WALLS
	init_pair(1, COLOR_BLUE, COLOR_BLUE);
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
	
	// MOVE GHOST
	move_ghost();
	
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].length(); j++) {
			move(i, j);
			
			// PRINT GHOST CHARACTER
			if (check_for(i, j, "ghost")) {
				attron(COLOR_PAIR(4));
				addch('?');
				attroff(COLOR_PAIR(4));
				
			// CHECK IF CHARACTER FROM MAZE IS NOT A COIN THAT HAS BEEN FOUND
			} else if (!check_for(i, j, "coin") && is_coin_found(i, j)) {
				attron(COLOR_PAIR(3));
				addch(' ');
				attroff(COLOR_PAIR(3));
			} else {
				if (check_for(i, j, "coin")) {
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
			if (maze[i][j] == ' ') {
//				coin_initial_positions.push_back({i, j});
				maze[i][j] = coin_char;
				initial_coin_quantity += 1;
			}
		}
	}

	int width = 0, height = 0;
	
	while (true) {
		// pobiera wymiary terminala
		getmaxyx(stdscr, height, width); 
		
//		// MOVE GHOST
//		move_ghost();
		
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
		if (check_for(player_row_index, player_col_index, "ghost") == true) {
			delwin(mainwin);
			endwin();
			refresh();
			return EXIT_SUCCESS;
		}
		
		// CHECK IF COINS HAS BEEN FOUND
		if (check_for(player_row_index, player_col_index, "coin")) {
			coin_found_positions.push_back({player_row_index, player_col_index});
			coin_found_counter += 1;
			
			// REMOVE COIN FROM MAZE
			remove_coin_from_maze(player_row_index, player_col_index);
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
