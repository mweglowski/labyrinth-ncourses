#include <iostream>
#include <ncursesw/curses.h>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <string>

class Coords {
	public:
		int row_index, col_index;
		Coords(int row_index = 0, int col_index = 0) : row_index(row_index), col_index(col_index) {};
};

class Tools {
	public:
		int get_random_int(int min, int max) {
			static std::random_device random_device;
			static std::mt19937 generator(random_device());
			
			std::uniform_int_distribution<> distribution(min, max);
			return distribution(generator);
		}
};

class Entity {
	public:
		char symbol;
		Coords coords;
		
		Entity(char symbol, Coords coords) : symbol(symbol), coords(coords) {};

		void move(int row_offset, int col_offset) {
			coords.row_index += row_offset;
			coords.col_index += col_offset;
		}
};

class Player : public Entity {
	public:
		Player(Coords initial_coords) : Entity('@', initial_coords) {};
		
		void set_row_index(int row_index) {
			coords.row_index = row_index;
		}
		
		void set_col_index(int col_index) {
			coords.col_index = col_index;
		}
};

class Ghost : public Entity {
	public:
		Ghost(Coords initial_coords) : Entity('?', initial_coords) {};
};


class Game {
	public:
		std::vector<std::vector<std::string>> mazes;
		std::vector<std::string> maze;
		int maze_width;
		int maze_height;
		int current_level;
		
		char COIN_CHAR = '.';
		char WALL_CHAR = '#';
		char PLAYER_CHAR = '@';
		char GHOST_CHAR = '?';
		
		int initial_coin_quantity = 0;
		int coin_found_counter = 0;
		std::vector<std::vector<int>> coin_found_positions = {};
		
		int ghost_quantity = 2;
		bool ghost_encountered = false;
		std::vector<Ghost> ghosts = {};
		std::vector<int> ghost_last_moves = {0, 0, 0, 0};
		
		Tools tools;
		
		Game() : current_level(0), COIN_CHAR('.'), WALL_CHAR('#'), PLAYER_CHAR('@'), GHOST_CHAR('?'), initial_coin_quantity(0), coin_found_counter(0), ghost_quantity(2), tools() {};
		
   	void get_levels() {
			std::ifstream file("levels.txt");
			
			std::vector<std::string> current_maze = {};
			
			if (file.is_open()) {
		    std::string line;
		    while (std::getline(file, line)) {
		    	if (line == "") {
		    		mazes.push_back(current_maze);
		    		current_maze = {};
					} else {
		      	current_maze.push_back(line);
					}
		    }
		    file.close();
			}
		}
		
		void set_maze() {
			maze = mazes[current_level];
		}
		
		bool can_player_move(int row_index, int col_index) {
			if (maze[row_index][col_index] == '#') {
				return false;
			}
			return true;
		}

		bool check_for(int row_index, int col_index, std::string object_to_check) {
			char character = ' ';
			if (object_to_check == "coin") {
				character = COIN_CHAR;
			} else if (object_to_check == "wall") {
				character = WALL_CHAR;
			} else if (object_to_check == "ghost") {
				for (int ghost_index = 0; ghost_index < ghost_quantity; ghost_index++) {
					int ghost_row_index = ghosts[ghost_index].coords.row_index;
					int ghost_col_index = ghosts[ghost_index].coords.col_index;
				
					if (ghost_row_index == row_index && ghost_col_index == col_index) {
						return true;
					}
				}
				return false;
			} else {
				character = PLAYER_CHAR;
			}
			
			if (maze[row_index][col_index] == character) {
				return true;
			}
			return false;
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
		
		void remove_coin_from_maze(int row_index, int col_index) {
			maze[row_index][col_index] = ' ';
		}
		
		void init_ghosts() {
			int ghost_row_index = 0;
			int ghost_col_index = 0;
			
			for (int ghost_index = 0; ghost_index < ghost_quantity; ghost_index++) {
				ghost_row_index = tools.get_random_int(2, maze_height - 2);
				ghost_col_index = tools.get_random_int(2, maze_width - 2);
				
				while (maze[ghost_row_index][ghost_col_index] == '#') {
					ghost_row_index = tools.get_random_int(2, maze_height - 2);
					ghost_col_index = tools.get_random_int(2, maze_width - 2);
				}
				
				Coords new_ghost_coords = Coords(ghost_row_index, ghost_col_index);
				Ghost new_ghost = Ghost(new_ghost_coords);
				ghosts.push_back(new_ghost);
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
				
		    Ghost current_ghost = ghosts[ghost_index];
				
				std::vector<std::vector<int>> possible_moves;
				// 0, 1, 2, 3 = top, right, bottom, left
				std::vector<int> possible_directions = {};
		    
		    // Current ghost coordinates
		    int ghost_row = current_ghost.coords.row_index;
		    int ghost_col = current_ghost.coords.col_index;
		
		    // Check each direction for a possible move
		    // UP
		    if (ghost_row > 0 && maze[ghost_row - 1][ghost_col] != WALL_CHAR) {
		    	top_available = true;
		    	possible_directions.push_back(0);
		      possible_moves.push_back({ghost_row - 1, ghost_col});
		    }
		    // RIGHT
		    if (ghost_col < maze[0].size() - 1 && maze[ghost_row][ghost_col + 1] != WALL_CHAR) {
		    	right_available = true;
		    	possible_directions.push_back(1);
		      possible_moves.push_back({ghost_row, ghost_col + 1});
		    }
		    // DOWN
		    if (ghost_row < maze.size() - 1 && maze[ghost_row + 1][ghost_col] != WALL_CHAR) {
		    	bottom_available = true;
		    	possible_directions.push_back(2);
		      possible_moves.push_back({ghost_row + 1, ghost_col});
		    }
		    // LEFT
		    if (ghost_col > 0 && maze[ghost_row][ghost_col - 1] != WALL_CHAR) {
		    	left_available = true;
		    	possible_directions.push_back(3);
		    	possible_moves.push_back({ghost_row, ghost_col - 1});
		    }
		
		    // If there are possible moves, select one at random
		    if (!possible_moves.empty()) {
					int last_move = ghost_last_moves[ghost_index];
					
					// IF THERE ARE 2 POSSIBLE MOVES, WE DO NOT CHOOSE THE LAST MOVE
					if (possible_moves.size() == 2) {
						int move_index = tools.get_random_int(0, possible_directions.size() - 1);
						
						// IF GHOST CAN REPEAT LAST MOVE (GO IN THE SAME DIRECTION)
						if (possible_directions[0] == last_move || possible_directions[1] == last_move) {
							if (possible_directions[0] == last_move) {
								ghosts[ghost_index].coords.row_index = possible_moves[0][0];
								ghosts[ghost_index].coords.col_index = possible_moves[0][1];
								if (check_for(ghost_row, ghost_col, "player")) {
									ghost_encountered = true;
								}
							} else {
								ghosts[ghost_index].coords.row_index = possible_moves[1][0];
								ghosts[ghost_index].coords.col_index = possible_moves[1][1];
								if (check_for(ghost_row, ghost_col, "player")) {
									ghost_encountered = true;
								}
							}
						} else {
							// IF IT CANNOT GO IN THE SAME DIRECTION
			      	int direction = possible_directions[move_index];
							
							if (last_move == 0 || last_move == 2) {
								if (possible_directions[0] == 0 || possible_directions[0] == 2) {
									ghosts[ghost_index].coords.row_index = possible_moves[1][0];
									ghosts[ghost_index].coords.col_index = possible_moves[1][1];
									if (check_for(ghost_row, ghost_col, "player")) {
										ghost_encountered = true;
									}
			      			ghost_last_moves[ghost_index] = possible_directions[1];
								} else {
									ghosts[ghost_index].coords.row_index = possible_moves[0][0];
									ghosts[ghost_index].coords.col_index = possible_moves[0][1];
									if (check_for(ghost_row, ghost_col, "player")) {
										ghost_encountered = true;
									}
			      			ghost_last_moves[ghost_index] = possible_directions[0];
								}
							} else {
								if (possible_directions[0] == 1 || possible_directions[0] == 3) {
									ghosts[ghost_index].coords.row_index = possible_moves[1][0];
									ghosts[ghost_index].coords.col_index = possible_moves[1][1];
									if (check_for(ghost_row, ghost_col, "player")) {
										ghost_encountered = true;
									}
			      			ghost_last_moves[ghost_index] = possible_directions[1];
								} else {
									ghosts[ghost_index].coords.row_index = possible_moves[0][0];
									ghosts[ghost_index].coords.col_index = possible_moves[0][1];
									if (check_for(ghost_row, ghost_col, "player")) {
										ghost_encountered = true;
									}
			      			ghost_last_moves[ghost_index] = possible_directions[0];
								}
							}
							
						}
					} else {
						int move_index = tools.get_random_int(0, possible_moves.size() - 1);
		      	ghosts[ghost_index].coords.row_index = possible_moves[move_index][0];
		      	ghosts[ghost_index].coords.col_index = possible_moves[move_index][1];
		      	if (check_for(ghost_row, ghost_col, "player")) {
							ghost_encountered = true;
						}
		      	
		      	int direction = possible_directions[move_index];
		      	ghost_last_moves[ghost_index] = direction;
					}
		    }
			}
		}
		
		void fill_maze_with_coins() {
			for (int i = 0; i < maze.size(); i++) {
				for (int j = 0; j < maze[0].length(); j++) {
					if (maze[i][j] == ' ') {
						maze[i][j] = COIN_CHAR;
						initial_coin_quantity += 1;
					}
				}
			}
		}
		
		void print_board(Player player) {
			clear();
			
			int player_row_index = player.coords.row_index;
			int player_col_index = player.coords.col_index;
			
			for (int i = 0; i < maze_height; i++) {
				for (int j = 0; j < maze_width; j++) {
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
							addch(COIN_CHAR);
							attroff(COLOR_PAIR(2));
						} else {
							attron(COLOR_PAIR(1));
							addch(maze[i][j]);
							attroff(COLOR_PAIR(1));
						}
					}
				}
			}
			
			attron(COLOR_PAIR(3));
			move(player_row_index, player_col_index);
			addch(PLAYER_CHAR);
			move(0, 0);
			attroff(COLOR_PAIR(3));
			
			refresh();
		}

		void change_player_char() {
			std::vector<char> random_chars = {'$', '%', '&', '!'};
			int random_char_index = tools.get_random_int(0, random_chars.size() - 1);
			PLAYER_CHAR = random_chars[random_char_index];
		}

		void set_maze_parameters(int level) {
			maze_height = mazes[0].size();
			maze_width = maze[0].length();
		}
		
		void next_level() {
			current_level += 1;
			maze = mazes[current_level];
			maze_height = maze.size();
			maze_width = maze[0].length();
			coin_found_counter = 0;
			coin_found_positions = {};
		}
};

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

int main(void) {
	WINDOW * mainwin = initscr();
	
	ncurses_config();
	
	Game game = Game();
	
	Tools tools = Tools();
	
	// CREATE MAZES
	// READING LEVELS FROM FILE
	game.get_levels();
	game.set_maze();	
	game.set_maze_parameters(0);
	int levels_quantity = game.mazes.size();
	
	// INITIALIZE PLAYER
	int initial_player_row_index = tools.get_random_int(1, game.maze_height - 2);
	int initial_player_col_index = tools.get_random_int(1, game.maze_width - 2);
	game.change_player_char();
	
	while (game.check_for(initial_player_row_index, initial_player_col_index, "wall")) {
		initial_player_row_index = tools.get_random_int(1, game.maze_height - 2);
		initial_player_col_index = tools.get_random_int(1, game.maze_width - 2);
	}
	
	Coords initial_player_coords = Coords(initial_player_row_index, initial_player_col_index);
	Player player = Player(initial_player_coords);
	
	// INITIALIZE GHOSTS
	game.init_ghosts();
	
	// FILL BLANKS WITH COINS
	game.fill_maze_with_coins();

	int width = 50, height = 50;
	
	// GHOST MOVE INTERVALS
	std::chrono::milliseconds ghost_move_interval(250);
	auto last_ghost_move_time = std::chrono::steady_clock::now();
	
	while (true) {
		getmaxyx(stdscr, height, width); 
		
		int player_current_row_index = player.coords.row_index;
		int player_current_col_index = player.coords.col_index;
		
		auto current_time = std::chrono::steady_clock::now();
		// MOVE GHOST
		if (current_time - last_ghost_move_time >= ghost_move_interval) {
			game.move_ghosts();
			last_ghost_move_time = current_time;
		}
		
		int input = getch();
		if (input != ERR) {
			switch(input) {
				case KEY_UP:
					if (player_current_row_index > 1) {
						if (game.can_player_move(player_current_row_index - 1, player_current_col_index)) {
							player.move(-1, 0);
						}
					}
					break;
				case KEY_DOWN:
					if (player_current_row_index < game.maze_height - 1) {
						if (game.can_player_move(player_current_row_index + 1, player_current_col_index)) {
							player.move(1, 0);
						}
					}
					break;
				case KEY_LEFT:
					if (player_current_col_index > 1) {
						if (game.can_player_move(player_current_row_index, player_current_col_index - 1)) {
							player.move(0, -1);
						}
					}
					break;
				case KEY_RIGHT:
					if (player_current_col_index < game.maze_width - 1) {
						if (game.can_player_move(player_current_row_index, player_current_col_index + 1)) {
							player.move(0, 1);
						}
					}
					break;
				case 'w':
					if (player_current_row_index > 2) {
						if (game.can_player_move(player_current_row_index - 2, player_current_col_index)) {
							player.move(-2, 0);
						}
					}
					break;
				case 'a':
					if (player_current_col_index > 2) {
						if (game.can_player_move(player_current_row_index, player_current_col_index - 2)) {
							player.move(0, -2);
						}
					}
					break;
				case 's':
					if (player_current_row_index < game.maze_height - 2) {
						if (game.can_player_move(player_current_row_index + 2, player_current_col_index)) {
							player.move(2, 0);
						}
					}
					break;
				case 'd':
					if (player_current_col_index < game.maze_width - 2) {
						if (game.can_player_move(player_current_row_index, player_current_col_index + 2)) {
							player.move(0, 2);
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
		if (game.check_for(player_current_row_index, player_current_col_index, "ghost")) {
			delwin(mainwin);
			endwin();
			refresh();
			return EXIT_SUCCESS;
		}
		
		// CHECK IF COIN HAS BEEN FOUND
		if (game.check_for(player_current_row_index, player_current_col_index, "coin")) {
			game.coin_found_positions.push_back({player_current_row_index, player_current_col_index});
			game.coin_found_counter += 1;
			
			// REMOVE COIN FROM MAZE
			game.remove_coin_from_maze(player_current_row_index, player_current_col_index);
		}
		
		// CHECK FOR WIN AND SET NEXT LEVEL
		if (game.coin_found_counter == game.initial_coin_quantity) {
			// CHECK FOR WIN (WHETHER ALL LEVELS HAVE BEEN COMPLETED)
			if (game.current_level + 1 == game.mazes.size()) {
				delwin(mainwin);
				endwin();
				refresh();
				return EXIT_SUCCESS;
			}
			
			game.next_level();
			
			// INITIALIZE PLAYER
			player_current_row_index = tools.get_random_int(1, game.maze_height - 2);
			player_current_col_index = tools.get_random_int(1, game.maze_width - 2);
			game.change_player_char();
			
			while (game.check_for(player_current_row_index, player_current_col_index, "wall")) {
				player_current_row_index = tools.get_random_int(1, game.maze_height - 2);
				player_current_col_index = tools.get_random_int(1, game.maze_width - 2);
			}
			
			// INITIALIZE GHOSTS 
			// INCREASING NUMBER OF GHOSTS
			game.ghosts = {};
			game.ghost_quantity += 2;
			game.init_ghosts();
			
			// FILL MAZE WITH COINS
			game.initial_coin_quantity = 0;
			game.fill_maze_with_coins();
		}
		
		game.print_board(player);
	}
	
	// END PROCESS
	delwin(mainwin);
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
