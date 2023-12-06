#include <iostream>
#include <ncursesw/curses.h>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>

std::vector<std::vector<std::string>> mazes = {};
std::vector<std::string> maze = {};
    
// CONSTANT VALUE OF INITIAL COIN CHARACTER
char coin_char = '.';
char wall_char = '#';
char player_char = '@';
char ghost_char = '?';

// MAZE PARAMETERS
int maze_width = 0;
int maze_height = 0;

// CURRENT LEVEL
int current_level = 0;

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

void init_ghost(int maze_height, int maze_width) {
	int ghost_row_index = get_random_int(2, maze_height - 2);
	int ghost_col_index = get_random_int(2, maze_width - 2);
	
	while (maze[ghost_row_index][ghost_col_index] == '#') {
		ghost_row_index = get_random_int(2, maze_height - 2);
		ghost_col_index = get_random_int(2, maze_width - 2);
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
    std::vector<std::vector<int>> possible_moves;
    
    // Current ghost coordinates
    int ghost_row = ghost_coords[0];
    int ghost_col = ghost_coords[1];

    // Check each direction for a possible move
    // UP
    if (ghost_row > 0 && maze[ghost_row - 1][ghost_col] != wall_char) {
      possible_moves.push_back({ghost_row - 1, ghost_col});
    }
    // DOWN
    if (ghost_row < maze.size() - 1 && maze[ghost_row + 1][ghost_col] != wall_char) {
      possible_moves.push_back({ghost_row + 1, ghost_col});
    }
    // LEFT
    if (ghost_col > 0 && maze[ghost_row][ghost_col - 1] != wall_char) {
    	possible_moves.push_back({ghost_row, ghost_col - 1});
    }
    // RIGHT
    if (ghost_col < maze[0].size() - 1 && maze[ghost_row][ghost_col + 1] != wall_char) {
      possible_moves.push_back({ghost_row, ghost_col + 1});
    }

    // If there are possible moves, select one at random
    if (!possible_moves.empty()) {
      int move_index = get_random_int(0, possible_moves.size() - 1);
      ghost_coords = possible_moves[move_index];
    }
}

void get_levels() {
	std::ifstream file("levels.txt");
	
	if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
    	if (line == "") {
    		mazes.push_back(maze);
    		maze = {};
			} else {
      	maze.push_back(line);
			}
    }
    file.close();
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
	timeout(250);
	noecho();
}

void print_board(int x, int y, int character) {
	clear();
	
	// MOVE GHOST
//	move_ghost();
	
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
	
	// READING LEVELS FROM FILE
	get_levels();	
	
	current_level = 0;
	maze = mazes[current_level];
	
	// CREATE MAZE
	maze_width = maze[0].size();
	maze_height = maze.size();
	int levels_quantity = mazes.size();
	
	// INITIALIZE PLAYER
	int player_character = '@';
	int player_row_index = get_random_int(1, maze_height - 2);
	int player_col_index = get_random_int(1, maze_width - 2);
	
	while (check_for(player_row_index, player_col_index, "wall")) {
		player_row_index = get_random_int(1, maze_height - 2);
		player_col_index = get_random_int(1, maze_width - 2);
	}
	
	// INITIALIZE GHOST
	init_ghost(maze_height, maze_width);
	
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

	int width = 100, height = 100;
	
	// GHOST MOVE INTERVALS
	std::chrono::milliseconds ghost_move_interval(250);
	auto last_ghost_move_time = std::chrono::steady_clock::now();
	
	while (true) {
		getmaxyx(stdscr, height, width); 
		
		auto current_time = std::chrono::steady_clock::now();
		// MOVE GHOST
		if (current_time - last_ghost_move_time >= ghost_move_interval) {
			move_ghost();
			last_ghost_move_time = current_time;
		}
		
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
		
		// CHECK IF COIN HAS BEEN FOUND
		if (check_for(player_row_index, player_col_index, "coin")) {
			coin_found_positions.push_back({player_row_index, player_col_index});
			coin_found_counter += 1;
			
			// REMOVE COIN FROM MAZE
			remove_coin_from_maze(player_row_index, player_col_index);
		}
		
		// CHECK FOR WIN AND SET NEXT LEVEL
//		if (coin_found_counter == initial_coin_quantity) {
		if (coin_found_counter == 5) {
			// CHECK FOR WIN (WHETHER ALL LEVELS HAVE BEEN COMPLETED
			if (current_level + 1 == mazes.size()) {
				delwin(mainwin);
				endwin();
				refresh();
				return EXIT_SUCCESS;
			}
			
			coin_found_counter = 0;
			coin_found_positions = {};
			
			current_level += 1;
			maze = mazes[current_level];
			maze_height = maze.size();
			maze_width = maze[0].length();
			
			// INITIALIZE PLAYER
			player_character = '$';
			player_row_index = get_random_int(1, maze_height - 2);
			player_col_index = get_random_int(1, maze_width - 2);
			
			while (check_for(player_row_index, player_col_index, "wall")) {
				player_row_index = get_random_int(1, maze_height - 2);
				player_col_index = get_random_int(1, maze_width - 2);
			}
			
			// INITIALIZE GHOST
			init_ghost(maze_height, maze_width);
			
			// FILL MAZE WITH COINS
			initial_coin_quantity = 0;
			for (int i = 0; i < maze.size(); i++) {
				for (int j = 0; j < maze[0].length(); j++) {
					if (maze[i][j] == ' ') {
						maze[i][j] = coin_char;
						initial_coin_quantity += 1;
					}
				}
			}
		}
		
		print_board(player_col_index, player_row_index, player_character);
	}
	
	// END PROCESS
	delwin(mainwin);
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
