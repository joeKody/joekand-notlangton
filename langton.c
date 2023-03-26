#include <ncurses.h>
#include <string.h>
#include <time.h>

int title_screen(void);

int run_ant(int limit);

int main(int argc, char * * argv)
{
// ---- INIT ----
	initscr();
	cbreak();
	noecho();
// ---- MAIN ----
	{
		int generations = title_screen();
		int err = run_ant(generations);
		if (err != 0)
			mvprintw(1, 1, "Something went wrong: errcode(%d)", err);
		else
			mvprintw(1, 1, "Press any key to exit...");
		getch();
	}
// ---- DNIT ----
	endwin();
	return 0;
}

int title_screen(void)
{
	int rows, cols;
	getmaxyx(stdscr, rows, cols);
	
	char * big_title[5] = {
		"#####       ##### #  #       ###  #   # #####",
		"  #    ###  #     # #       #   # ##  #   #",
		"  #   #   # ####  ## #      ##### # # #   #",
		"# #   #   # #     #  #      #   # #  ##   #",
		"###    ###  ##### #   #     #   # #   #   #"
	};
	
	// MENU BOX
	WINDOW * menubox = newwin(rows/1.5, cols/1.5, rows/2 - rows/3, cols/2 - cols/3);
	box(menubox, 0, 0);

	wattron(menubox, A_BOLD); // TITLE
	for (int i = 0; i < 5; i++)
		mvwprintw(menubox, 2+i, 20, "%s", big_title[i]);
	wattroff(menubox, A_BOLD);

	mvwprintw(menubox, 8, 20, "\"Definitely not a Langton's ant ripoff.\"");
	mvwprintw(menubox, 9, 53, "- me(joe)");
	mvwprintw(menubox, 12, 20, "How many generations do you want to run?");
	
	// MENU
	int result;

	mvwprintw(menubox, 13, 20, "Answer in positive integers: ");
	echo();
	wscanw(menubox, "%d", &result);	
	noecho();

	wrefresh(menubox);

	//wgetch(menubox);

	return result;
}

int run_ant(int limit)
{
	int rows, cols;
	getmaxyx(stdscr, rows, cols);

	int ant_grid[rows][cols];
	int ant_state = 0; 	// 0, 1, 2, 3
				// up right down left 
	int ant_y = (rows-2)/2;
	int ant_x = (cols-2)/2;

	memset(ant_grid, 0, sizeof(ant_grid));
	ant_grid[ant_y][ant_x] = 1;
	
	WINDOW * antscreen = newwin(rows, cols, 0, 0);
	box(antscreen, 0, 0);

	wtimeout(antscreen, 1);
	for (int current = 0; current < limit; current++)
	{
		for (int y = 0; y < rows-2; y++)
		{
			for (int x = 0; x < cols-2; x++)
			{
				if (ant_grid[y][x] == 1)
					wattrset(antscreen, A_REVERSE);
				else if (ant_grid[y][x] == 0)
					wattrset(antscreen, A_NORMAL);
				else
					return -1;
				mvwaddch(antscreen, y + 1, x + 1, ' ');
			}
		}

		if (ant_grid[ant_y][ant_x] == 0)
		{
			ant_grid[ant_y][ant_x] = 1;
			ant_state++;
			if (ant_state > 3)
				ant_state = 0;
		}
		else if (ant_grid[ant_y][ant_x] == 1)
		{
			ant_grid[ant_y][ant_x] = 0;
			ant_state--;
			if (ant_state < 0)
				ant_state = 3;
		}


		if (ant_grid[ant_y][ant_x] == 1)
			wattrset(antscreen, A_REVERSE);
		else if (ant_grid[ant_y][ant_x] == 0)
			wattrset(antscreen, A_NORMAL);
		else
		{
			mvprintw(5, 6, "y, x = %d, %d", ant_y, ant_x);
			mvprintw(6, 6, "the grid = %d", ant_grid[ant_y][ant_x]);
			getch();
			return -1;
		}
				
		switch (ant_state)
		{
			case 0:
				mvwaddch(antscreen, ant_y + 1, ant_x + 1, '^');
				ant_y++;
				break;
			case 1:
				mvwaddch(antscreen, ant_y + 1, ant_x + 1, '>');
				ant_x++;
				break;
			case 2:
				mvwaddch(antscreen, ant_y + 1, ant_x + 1, 'V');
				ant_y--;
				break;
			case 3:
				mvwaddch(antscreen, ant_y + 1, ant_x + 1, '<');
				ant_x--;
				break;
			default:
				return -2;
		}

		if (ant_y >= rows-3)
			ant_y = 0;
		else if (ant_y <= 0)
			ant_y = rows-3;

		if (ant_x >= cols-3)
			ant_x = 0;
		else if (ant_x <= 0)
			ant_x = cols-3;
		
		switch (wgetch(antscreen))
		{
			case 'x':
				return 0;
				break;
			case 'X':
				return 0;
				break;
			case 's':
				break;
			default:
				break;
		}
	}

	cbreak();
	return 0;
}
