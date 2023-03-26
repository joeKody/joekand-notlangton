#include <ncurses.h>
#include <string.h>
#include <time.h>

long long title_screen(void);

int run_ant(long long limit);

int main(int argc, char * * argv)
{
// ---- INIT ----
	initscr();
	cbreak();
	noecho();
// ---- MAIN ----
	{
		long generations = title_screen();
		int err = run_ant(generations);

		erase();
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

long long title_screen(void)
{
	int rows, cols;
	getmaxyx(stdscr, rows, cols);
	
	char * big_title[] = {
		"#####       ##### #  #      ###  #   # #####",
		"  #    ###  #     # #      #   # ##  #   #",
		"  #   #   # ####  ## #     ##### # # #   #",
		"# #   #   # #     #  #     #   # #  ##   #",
		"###    ###  ##### #   #    #   # #   #   #"
	};	// pretty scuffed lmao, 42 in length btw 

	char * mid_title[] = {
		"###    ## # #   ##  ##  # ###",
		" #  ## #= ##   #__# # # #  #",
		"##  ## ## # #  #  # #  ##  #"
	};	// 28 in length
	
	// TITLE
	if (rows >= 13 && cols >= 44){
		attrset(A_BOLD);
		for (int i = 0; i < 5; i++)
			mvprintw(i + 1, 1, "%s", big_title[i]);
	} else if (rows >= 11 && cols >= 30){
		attrset(A_BOLD);
		for (int i = 0; i < 3; i++)
			mvprintw(i + 1, 1, "%s", mid_title[i]);
	} else {
		attrset(A_STANDOUT);
		mvprintw(1, 1, "JoeK ANT");
	}

	attrset(A_NORMAL);
	int y, x;
	getyx(stdscr, y, x);
	y++;
	
	mvprintw(++y, 1, "Controls: ");
	attrset(A_STANDOUT);
	mvprintw(++y, 1, "(ctrl + c), c, q, x = quit");
	mvprintw(++y, 1,"g = slowdown");
	mvprintw(++y, 1,"h = speedup");
	attrset(A_NORMAL);
	y++;
	mvprintw(++y, 1, "How many generations to run(int): ");

	long long result;
	char newline;
	echo();
	scanw("%lld%c", &result, newline);
	if (newline == '\n' || result < 1)
		return -1;
	noecho();
	refresh();
	return result;
}

int run_ant(long long limit)
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

	int speed = 100;
	int gen_speed = 1;
	curs_set(0);
	wtimeout(antscreen, speed);
	for (int current = 0; current < limit; current+=gen_speed)
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
		for (int j = 0; j < gen_speed; j++)
		{	
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

			switch (ant_state)
			{
				case 0:
					ant_y++;
					break;
				case 1:
					ant_x++;
					break;
				case 2:
					ant_y--;
					break;
				case 3:
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
		
		}

		wattrset(antscreen, A_STANDOUT);
		mvwprintw(antscreen, 3, 1, "Generation: %d", current+1);
		mvwprintw(antscreen, 1, 1, "Speed = %d", speed);
		mvwprintw(antscreen, 2, 1, "GenSpeed = %d", gen_speed);
		wattrset(antscreen, A_NORMAL);
		
		switch (wgetch(antscreen))
		{
			case 'x':
				return 0;
				break;
			case 'q':
				return 0;
				break;
			case 'c':
				return 0;
				break;
			case 'g':
				if (gen_speed != 1)
					gen_speed /= 2;
				else if (speed <= 4000)
					speed *= 2;
				wtimeout(antscreen, speed);
				break;
			case 'h':
				if (speed >= 3)
					speed /= 2;
				else if (gen_speed <= 400096) 
					gen_speed *= 2;
				wtimeout(antscreen, speed);
				break;
			default:
				break;
		}
	}

	cbreak();
	return 0;
}
