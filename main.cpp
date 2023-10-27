#include <iostream>
#ifdef _WIN32
#include <ncursesw/curses.h>
#else
#include <curses.h>
#endif


void ncurses_init_colors() {
	// wiÄcej o kolorach tu https://www.linuxjournal.com/content/programming-color-ncurses

	// musimy ustawiÄ jeĹli bÄdziemy uĹźywaÄ kolorowania konsoli
	start_color();
	// i zdefiniowaÄ pary kolorĂłw ktĂłre bÄdziemy uĹźywaÄ
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
}

void ncurses_config() {
	// definiujemy kolory tekstu ktĂłre uĹźyjemy do kolorowania planszy
	ncurses_init_colors();
	// getch ma interpretowaÄ znaki specjalne takie jak KEY_UP
	keypad(stdscr, TRUE);
	
	// ustawiamy timeout dla getch (milisekundy)
	// po tym czasie program przejdzie dalej nawet jeĹli nie podasz klawisza
	timeout(500);
	// podczas wpisywania z klawiatury nie powinna siÄ drukowaÄ literka
	noecho();
}

void print_board(int x, int y, int character) {
	// operujemy na wirtualnym ekranie bÄdÄcym buforem ekranu
	// a nastÄpnie wyĹwietlamy bufor w terminalu funkcjÄ refresh
	
	clear(); // czyĹci wirtualny ekran (lepiej byĹo by czyĹciÄ jeden znak albo jedna linie)
	
	attron(COLOR_PAIR(1)); // ustawiamy wczeĹniej zdefiniowanÄ perÄ kolorĂłw
	// (moĹźna uĹźyÄ mvaddch zamiast dwĂłch funcji)
	move(y, x); // skaczemy kursorem do danej pozycji
	addch(character); // drukujemy podany znak
	move(0, 0); // aby migajÄcy kursor nam nie przeszkadzaĹ
	attroff(COLOR_PAIR(1)); // przywracamy domyĹlny kolor
	
	refresh(); // wyĹwietlamy zawartoĹÄ wirtualnego ekranu dopiero po refresh
}

int main(void) {
	// inicjalizacja ncurses
	initscr();
	
	win = newwin(30, 60, 0, 0);
	
	ncurses_config();

	int last_character = '!';
	int last_position_x = 30;
	int last_position_y = 10;
	
	while(true) {
		// sczytujemy literkÄ z klawiatury
		// (jeĹli sÄ tu znaki specjalne musi byÄ int bo nie zmieszczÄ siÄ w char)
		int input = getch();
		
		if(input != ERR) {	
			switch(input) {
				case KEY_UP:
					last_position_y -= 1;
					break;
				case KEY_RIGHT:
					last_position_x += 1;
					break;
				case KEY_DOWN:
					last_position_y += 1;
					break;
				case KEY_LEFT:
					last_position_x -= 1;
					break;
				default:
					delwin(mainwin);
					endwin();
					refresh();
					return EXIT_SUCCESS;
			}
		}
		
		print_board(last_position_x, last_position_y, last_character);
	}
	
	
	
	endwin();
	refresh();
	return EXIT_SUCCESS;
}
