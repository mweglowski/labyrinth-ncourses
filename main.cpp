#include <iostream>
#include <ncursesw/curses.h>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>


// ======== MAZE/LEVELS ========
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

// ======== COINS ========
// TOTAL COINS AT GAME START
int initial_coin_quantity = 0;
// POSTIIONS OF COINS THAT HAVE BEEN FOUND
std::vector<std::vector<int>> coin_found_positions = {};
// COUNTER OF COINS FOUND
int coin_found_counter = 0;

// ======== GHOSTS ========
// CURRENT NUMBER OF GHOSTS ON THE MAP
int ghost_quantity = 0;
// POSITION OF GHOST
std::vector<int> ghost_coords = {};
// POSITION OF GHOSTS
std::vector<std::vector<int>> ghosts_coords = {};
// IF PLAYER ENCOUNTERED GHOST
bool ghost_encountered = false;
// LAST MOVE OF EACH GHOST
std::vector<int> ghost_last_moves = {0, 0, 0, 0};
    
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
		for (int ghost_index = 0; ghost_index < ghost_quantity; ghost_index++) {
			std::vector<int> current_ghost_coords = ghosts_coords[ghost_index];
		
			if (current_ghost_coords[0] == row_index && current_ghost_coords[1] == col_index) {
				return true;
			}
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

void init_ghosts(int maze_height, int maze_width, int ghost_quantity) {
	int ghost_row_index = 0;
	int ghost_col_index = 0;
	
	for (int ghost_index = 0; ghost_index < ghost_quantity; ghost_index++) {
		ghost_row_index = get_random_int(2, maze_height - 2);
		ghost_col_index = get_random_int(2, maze_width - 2);
		
		while (maze[ghost_row_index][ghost_col_index] == '#') {
			ghost_row_index = get_random_int(2, maze_height - 2);
			ghost_col_index = get_random_int(2, maze_width - 2);
		}
		
		std::vector<int> current_ghost_coords = {ghost_row_index, ghost_col_index};
		ghosts_coords.push_back(current_ghost_coords);
	}
}

bool is_direction_available(int direction, std::vector<int> available_directions) {
	for (int i = 0; i < available_directions.size(); i++) {
		if (available_directions[i] == direction) {
			return true;
		}
	}
	return false;
}

void move_ghosts() {
	for (int ghost_index = 0; ghost_index < ghost_quantity; ghost_index++) {
		bool top_available = false;
		bool right_available = false;
		bool bottom_available = false;
		bool left_available = false;
		
    std::vector<int> current_ghost_coords = ghosts_coords[ghost_index];
		
		std::vector<std::vector<int>> possible_moves;
		// 0, 1, 2, 3 = top, right, bottom, left
		std::vector<int> possible_directions = {};
    
    // Current ghost coordinates
    int ghost_row = current_ghost_coords[0];
    int ghost_col = current_ghost_coords[1];

    // Check each direction for a possible move
    // UP
    if (ghost_row > 0 && maze[ghost_row - 1][ghost_col] != wall_char) {
    	top_available = true;
    	possible_directions.push_back(0);
      possible_moves.push_back({ghost_row - 1, ghost_col});
    }
    // RIGHT
    if (ghost_col < maze[0].size() - 1 && maze[ghost_row][ghost_col + 1] != wall_char) {
    	right_available = true;
    	possible_directions.push_back(1);
      possible_moves.push_back({ghost_row, ghost_col + 1});
    }
    // DOWN
    if (ghost_row < maze.size() - 1 && maze[ghost_row + 1][ghost_col] != wall_char) {
    	bottom_available = true;
    	possible_directions.push_back(2);
      possible_moves.push_back({ghost_row + 1, ghost_col});
    }
    // LEFT
    if (ghost_col > 0 && maze[ghost_row][ghost_col - 1] != wall_char) {
    	left_available = true;
    	possible_directions.push_back(3);
    	possible_moves.push_back({ghost_row, ghost_col - 1});
    }

    // If there are possible moves, select one at random
    if (!possible_moves.empty()) {
			int last_move = ghost_last_moves[ghost_index];
			
			// IF THERE ARE 2 POSSIBLE MOVES, WE DO NOT CHOOSE THE LAST MOVE
			if (possible_moves.size() == 2) {
				int move_index = get_random_int(0, possible_directions.size() - 1);
				
				// IF GHOST CAN REPEAT LAST MOVE (GO IN THE SAME DIRECTION)
				if (possible_directions[0] == last_move || possible_directions[1] == last_move) {
					if (possible_directions[0] == last_move) {
						ghosts_coords[ghost_index] = possible_moves[0];
					} else {
						ghosts_coords[ghost_index] = possible_moves[1];
					}
				} else {
					// IF IT CANNOT GO IN THE SAME DIRECTION
	      	int direction = possible_directions[move_index];
					
					if (last_move == 0 || last_move == 2) {
						if (possible_directions[0] == 0 || possible_directions[0] == 2) {
							ghosts_coords[ghost_index] = possible_moves[1];
	      			ghost_last_moves[ghost_index] = possible_directions[1];
						} else {
							ghosts_coords[ghost_index] = possible_moves[0];
	      			ghost_last_moves[ghost_index] = possible_directions[0];
						}
					} else {
						if (possible_directions[0] == 1 || possible_directions[0] == 3) {
							ghosts_coords[ghost_index] = possible_moves[1];
	      			ghost_last_moves[ghost_index] = possible_directions[1];
						} else {
							ghosts_coords[ghost_index] = possible_moves[0];
	      			ghost_last_moves[ghost_index] = possible_directions[0];
						}
					}
					
				}
			} else {
				int move_index = get_random_int(0, possible_moves.size() - 1);
      	ghosts_coords[ghost_index] = possible_moves[move_index];
      	
      	int direction = possible_directions[move_index];
      	ghost_last_moves[ghost_index] = direction;
			}
    }
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

void fill_maze_with_coins() {
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].length(); j++) {
			if (maze[i][j] == ' ') {
				maze[i][j] = coin_char;
				initial_coin_quantity += 1;
			}
		}
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
	
	// INITIALIZE GHOSTS
	ghost_quantity = 2;
	init_ghosts(maze_height, maze_width, ghost_quantity);
	
	// FILL BLANKS WITH COINS
	fill_maze_with_coins();

	int width = 100, height = 100;
	
	// GHOST MOVE INTERVALS
	std::chrono::milliseconds ghost_move_interval(250);
	auto last_ghost_move_time = std::chrono::steady_clock::now();
	
	while (true) {
		getmaxyx(stdscr, height, width); 
		
		auto current_time = std::chrono::steady_clock::now();
		// MOVE GHOST
		if (current_time - last_ghost_move_time >= ghost_move_interval) {
			move_ghosts();
			last_ghost_move_time = current_time;
		}
		
		int input = getch();
		if (input != ERR) {
			switch(input) {
				case KEY_UP:
					if (player_row_index > 1) {
						if (can_player_move(player_row_index - 1, player_col_index)) {
							player_row_index -= 1;
						}
					}
					break;
				case KEY_DOWN:
					if (player_row_index < maze_height - 1) {
						if (can_player_move(player_row_index + 1, player_col_index)) {
							player_row_index += 1;
						}
					}
					break;
				case KEY_LEFT:
					if (player_col_index > 1) {
						if (can_player_move(player_row_index, player_col_index - 1)) {
							player_col_index -= 1;
						}
					}
					break;
				case KEY_RIGHT:
					if (player_col_index < maze_width - 1) {
						if (can_player_move(player_row_index, player_col_index + 1)) {
							player_col_index += 1;
						}
					}
					break;
				case 'w':
					if (player_row_index > 2) {
						if (can_player_move(player_row_index - 2, player_col_index)) {
							player_row_index -= 2;
						}
					}
					break;
				case 'a':
					if (player_col_index > 2) {
						if (can_player_move(player_row_index, player_col_index - 2)) {
							player_col_index -= 2;
						}
					}
					break;
				case 's':
					if (player_row_index < maze_height - 2) {
						if (can_player_move(player_row_index + 2, player_col_index)) {
							player_row_index += 2;
						}
					}
					break;
				case 'd':
					if (player_col_index < maze_width - 2) {
						if (can_player_move(player_row_index, player_col_index + 2)) {
							player_col_index += 2;
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
		if (coin_found_counter == initial_coin_quantity) {
//		if (coin_found_counter == 5) {
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
			std::vector<char> random_chars = {'$', '%', '&', '!'};
			int random_char_index = get_random_int(0, random_chars.size());
			player_character = random_chars[random_char_index];
			player_row_index = get_random_int(1, maze_height - 2);
			player_col_index = get_random_int(1, maze_width - 2);
			
			while (check_for(player_row_index, player_col_index, "wall")) {
				player_row_index = get_random_int(1, maze_height - 2);
				player_col_index = get_random_int(1, maze_width - 2);
			}
			
			// INITIALIZE GHOSTS 
			// INCREASING NUMBER OF GHOSTS
			ghosts_coords = {};
			ghost_quantity += 2;
			init_ghosts(maze_height, maze_width, ghost_quantity);
			
			// FILL MAZE WITH COINS
			initial_coin_quantity = 0;
			fill_maze_with_coins();
		}
		
		print_board(player_col_index, player_row_index, player_character);
	}
	
	// END PROCESS
	delwin(mainwin);
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
