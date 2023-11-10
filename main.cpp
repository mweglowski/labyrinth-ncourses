#include <iostream>
#include <ncursesw/curses.h>
#include <fstream>
#include <vector>

std::vector<std::string> maze = {
        "   ##########################################################",
        "            #o #          o                  #     #     # o#",
        "#  #  ####  #  ####  #  #######  ####  ##########  ####  #  #",
        "#  #  #  #  #     #  #        #  #              #        #  #",
        "#  ####  #  ####  ####  #  #######  #  ####  ##########  #  #",
        "#  #o    #     #     #  #  #    o#  #  #     #  #           #",
        "#  ####  ####  #  ####  #  #  #############  #  ####  ####  #",
        "#     #                 #        #  #  #        #    o#     #",
        "#  #######  #######  ####  #  ####  #  ####  ##########  #  #",
        "#     #     #  # o#  #  #  #  # o#                    #  #  #",
        "####  #######  #  #  #  #######  ####  #  ####  #######  #  #",
        "#o       #           #     # o#        #  #        #  #  #  #",
        "##########  ####  ####  #  #  ####  ##########  #  #  ####  #",
        "#  #     #     #        #  #     #  #     #  #  # o#        #",
        "#  #  ####  ##########  ####  #  #  #  #  #  ##########  ####",
        "#     #     #  #o             #  #     #  #  #     #        #",
        "####  ####  #  #######  #######  #  #######  #  #######  ####",
        "#     #     #    o#        #     #       o#     #     #     #",
        "##########  #  ####  ####  ####  #  ####  #  ####  #######  #",
        "#       o#        #  #  #  #  #  #     #     #o             #",
        "#  #######  #  #  #  #  ####  #  ####  #  #  #  #  ####  #  #",
        "#              #  #  #    o#     #  #  #  #     #  #  #     #",
        "#  #############  #  ####  #  #######  ####  #  #  #  ####  #",
        "#  #  #     #     #  #     #              #  #        #     #",
        "#  #  ####  ####  #  #  ##########  #############  ####  #  #",
        "#          o#                             #        #o    #   ",
        "##########################################################   ",
    };
    
// POSTIIONS OF COINS THAT HAVE BEEN FOUND
std::vector<std::vector<int>> coin_found_positions = {};

    
// USEFUL FUNCTIONS

bool can_player_move(int row_index, int col_index) {
	if (maze[row_index][col_index] == '#') {
		return false;
	}
	return true;
}

bool check_for_coin(int row_index, int col_index) {
	return maze[row_index][col_index] == 'o';
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

void ncurses_init_colors() {
	// więcej o kolorach tu https://www.linuxjournal.com/content/programming-color-ncurses

	// musimy ustawić jeśli będziemy używać kolorowania konsoli
	start_color();
	// i zdefiniować pary kolorów które będziemy używać
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
}

void ncurses_config(){
	// definiujemy kolory tekstu które użyjemy do kolorowania planszy
	ncurses_init_colors();
	// getch ma interpretować znaki specjalne takie jak KEY_UP
	keypad(stdscr, TRUE);
	// ustawiamy timeout dla getch (milisekundy)
	// po tym czasie program przejdzie dalej nawet jeśli nie podasz klawisza
	timeout(50);
	// podczas wpisywania z klawiatury nie powinna się drukować literka
	noecho();
}

void print_board(int x, int y, int character) {
	clear();
	attron(COLOR_PAIR(1));
	
	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[0].length(); j++) {
			move(i, j);
			
			// CHECK IF CHARACTER FROM MAZE IS NOT A COIN THAT HAS BEEN FOUND
			if (check_for_coin(i, j) && is_coin_found(i, j)) {
				addch(' ');
			} else {
				addch(maze[i][j]);
			}
		}
	}
	
	move(y, x);
	addch(character);
	move(0, 0);
	attroff(COLOR_PAIR(1));
	
	refresh();
}

int main(void) {
	WINDOW * mainwin = initscr();
	ncurses_config();

	int last_character = '!';
	int last_position_x = 60, last_position_y = 26;
	int width = 0, height = 0;
	
	while (true) {
		// pobiera wymiary terminala
		getmaxyx(stdscr, height, width); 
		
		int input = getch();
		if (input != ERR) {
			switch(input) {
				case KEY_UP:
					// sprawdzamy czy nie wychodzimy poza ekran
					if (last_position_y > 0) {
						if (can_player_move(last_position_y - 1, last_position_x)) {
							last_position_y -= 1;
						}
					}
					break;
				case KEY_DOWN:
					if (last_position_y < height - 1) {
						if (can_player_move(last_position_y + 1, last_position_x)) {
							last_position_y += 1;
						}
					}
					break;
				case KEY_LEFT:
					if (last_position_x > 0) {
						if (can_player_move(last_position_y, last_position_x - 1)) {
							last_position_x -= 1;
						}
					}
					break;
				case KEY_RIGHT:
					if (last_position_x < width - 1) {
						if (can_player_move(last_position_y, last_position_x + 1)) {
							last_position_x += 1;
						}
					}
					break;
				case 'q':
				case 'Q':
					return 0;
				default:
					last_character = input;
			}
		}
		
		// CHECK IF COINS HAS BEEN FOUND
		if (check_for_coin(last_position_y, last_position_x)) {
			coin_found_positions.push_back({last_position_y, last_position_x});
		}		
		
		print_board(last_position_x, last_position_y, last_character);
	}
	
	// zakańczamy prace ncurses
	delwin(mainwin);
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
