#define MINMOVEDELAY 1
#define MAXMOVEDELAY 6

#define BOARD_X 79
#define BOARD_Y 63

#define BLOCKSIZE 4
#define DRAWSIZE (BLOCKSIZE-1)
#define BOARD_WIDTH (152 / BLOCKSIZE)
#define BOARD_HEIGHT (84 / BLOCKSIZE)

#define BOARDSIZE	(BOARD_WIDTH*BOARD_HEIGHT)
#define SNAKELENGTH (BOARDSIZE + 2)

typedef enum {bd_blank=0, bd_wall, bd_snake, bd_snake_x, bd_snake_y, bd_snake_xy, bd_apple};
typedef enum {snake_up, snake_down, snake_left, snake_right, numsnakedirs};

typedef struct
{
	unsigned char board[BOARDSIZE];

	unsigned char xcoords[SNAKELENGTH], ycoords[SNAKELENGTH];
	unsigned length, dir, grow, applex, appley, apple, tics;
	unsigned score;
	int index;
} SnakeGame;

SnakeGame _seg *snake;
unsigned bestscore = 0;

#define MyRandom(range) (US_RndT() % (range))
#define DrawRect VWB_Bar

int near GetBoard(int x, int y)
{
	if (x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT)
	{
		return snake->board[x+y*BOARD_WIDTH];
	}
	return bd_wall;
}

void near SetBoard(int x, int y, int val)
{
	if (x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT)
	{
		snake->board[x+y*BOARD_WIDTH] = val;
		switch (val)
		{
		case bd_wall:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, GREEN);
			break;
		case bd_snake:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, DRAWSIZE, DRAWSIZE, LIGHTGREEN);
			break;
		case bd_snake_x:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, BLOCKSIZE, DRAWSIZE, LIGHTGREEN);
			break;
		case bd_snake_y:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, DRAWSIZE, BLOCKSIZE, LIGHTGREEN);
			break;
		case bd_snake_xy:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, BLOCKSIZE, DRAWSIZE, LIGHTGREEN);
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+DRAWSIZE+y*BLOCKSIZE, DRAWSIZE, BLOCKSIZE-DRAWSIZE, LIGHTGREEN);
			break;
		case bd_apple:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, DRAWSIZE, DRAWSIZE, LIGHTRED);
			break;
		case bd_blank:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, BackColor);
			break;
		default:
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y+y*BLOCKSIZE, BLOCKSIZE, BLOCKSIZE, WHITE);
			break;
		}
	}
}

void near CheckScore(void)
{
	if (snake->score > bestscore)
		bestscore = snake->score;
}

void near DrawScore(void)
{
	fontcolor = HiliteColor;
	PrintY = py = CtlPanelSY + 4;
	PrintX = px = CtlPanelSX + 4;
	VWB_Bar(px,py,50,6,BackColor);
	//US_Print("SCORE:");
	SM_Print(S_NNSCORE);
	PrintX++;
	//PrintX = px;
	US_PrintUnsigned(snake->score);
	PrintX = px = CtlPanelSX + 108;
	VWB_Bar(px,py,50,6,BackColor);
	//US_Print("BEST:");
	SM_Print(S_NNBEST);
	PrintX++;
	//PrintX = px;
	US_PrintUnsigned(bestscore);
}

void near ClearBoard(void)
{
	DrawRect(BOARD_X-1, BOARD_Y-1, BOARD_WIDTH*BLOCKSIZE+1, BOARD_HEIGHT*BLOCKSIZE+1, BackColor);
	_fmemset(&snake->board, bd_blank, sizeof(snake->board));
	snake->apple = 0;

	// fill border with walls:
	/*
	{
		register int i;
		for (i=0; i<BOARD_WIDTH; i++)
		{
			SetBoard(i, 0, bd_wall);
			SetBoard(i, BOARD_HEIGHT-1, bd_wall);
		}
		for (i=0; i<BOARD_HEIGHT; i++)
		{
			SetBoard(0, i, bd_wall);
			SetBoard(BOARD_WIDTH-1, i, bd_wall);
		}
	}
	*/
}


void near GetCoords(int i, int *x, int *y)
{
	i %= SNAKELENGTH;
	if (i < 0)
		i += SNAKELENGTH;
	*x = snake->xcoords[i];
	*y = snake->ycoords[i];
}

void near SetCoords(int i, int x, int y)
{
	i %= SNAKELENGTH;
	if (i < 0)
		i += SNAKELENGTH;
	snake->xcoords[i] = x;
	snake->ycoords[i] = y;
}

void near InitSnake(void)
{
	ClearBoard();
	snake->length = 0;
	snake->score = 0;
	snake->grow = 5;
	snake->tics = MAXMOVEDELAY;
	snake->dir = MyRandom(numsnakedirs);
	SetCoords(snake->index, BOARD_WIDTH/2, BOARD_HEIGHT/2);
	SetBoard(BOARD_WIDTH/2, BOARD_HEIGHT/2, bd_snake);
	DrawScore();
}

void near KillSnake(void)
{
	longword lasttime = TimeCount;
	VW_UpdateScreen();
	while ((TimeCount - lasttime) < 35);
	CheckScore();
	InitSnake();
}

void near MoveSnake(void)
{
	int x, y;
	unsigned newtype, oldtype;

	GetCoords(snake->index, &x, &y);
	newtype = bd_snake;
	oldtype = GetBoard(x, y);
	switch (snake->dir)
	{
	case snake_up:
		y--;
		newtype = bd_snake_y;
		break;
	case snake_down:
		switch (oldtype)
		{
		case bd_snake:
			SetBoard(x, y, bd_snake_y);
			break;
		case bd_snake_x:
			SetBoard(x, y, bd_snake_xy);
			break;
		}
		y++;
		break;
	case snake_left:
		x--;
		newtype = bd_snake_x;
		break;
	case snake_right:
		switch (oldtype)
		{
		case bd_snake:
			SetBoard(x, y, bd_snake_x);
			break;
		case bd_snake_y:
			SetBoard(x, y, bd_snake_xy);
			break;
		}
		x++;
		break;
	}
	snake->index++;
	if (snake->index == SNAKELENGTH)
		snake->index = 0;

	switch (GetBoard(x, y))
	{
	case bd_snake:
	case bd_snake_x:
	case bd_snake_y:
	case bd_snake_xy:
		SetBoard(x, y, newtype);
	case bd_wall:
		// small hacks to make the snake touch the top/left borders:
		if (x < 0)
			DrawRect(BOARD_X-1, BOARD_Y+y*BLOCKSIZE, 1, DRAWSIZE, LIGHTGREEN);
		else if (y < 0)
			DrawRect(BOARD_X+x*BLOCKSIZE, BOARD_Y-1, DRAWSIZE, 1, LIGHTGREEN);

		SD_PlaySound(COMPSCOREDSND);
		KillSnake();
		return;
	case bd_apple:
		snake->score++;
		snake->grow += 5;
		snake->apple = 0;
		DrawScore();
		//
		// increase speed every 10 points
		//
		if (snake->tics > MINMOVEDELAY && !(snake->score % 10))
		{
			SD_PlaySound(KEENSCOREDSND);
			snake->tics--;
		}
		else
		{
			SD_PlaySound(KEENPADDLESND);
		}
		break;
	}

	SetBoard(x, y, newtype);
	SetCoords(snake->index, x, y);

	if (snake->grow && snake->length < SNAKELENGTH-2)
	{
		snake->length++;
		snake->grow--;
	}
	else
	{
		GetCoords(snake->index-1-snake->length, &x, &y);
		SetBoard(x, y, bd_blank);
	}
}

void near SpawnApple(void)
{
	register unsigned count;

	if (snake->apple)
	{
		SetBoard(snake->applex, snake->appley, bd_blank);
		snake->apple = 0;
	}

	for (count=42; --count!=0;)	// fixed number of repeats to avoid endless loops
	{
		snake->applex = MyRandom(BOARD_WIDTH);
		snake->appley = MyRandom(BOARD_HEIGHT);
		if (GetBoard(snake->applex, snake->appley) == bd_blank)
		{
			SetBoard(snake->applex, snake->appley, bd_apple);
			snake->apple = 1;
			break;
		}
	}
}

void near PlaySnake(void)
{
	CursorInfo      cursorinfo;
	longword lasttime;
	unsigned ticcount;

	boolean leftkeywas, rightkeywas, upkeywas, downkeywas;
	boolean leftkeydown, rightkeydown, upkeydown, downkeydown;
	boolean leftkeyhit, rightkeyhit, upkeyhit, downkeyhit;

	//DrawRect(BOARD_X-2, BOARD_Y-2, BOARD_WIDTH*BLOCKSIZE+4, BOARD_HEIGHT*BLOCKSIZE+4, LIGHTGREEN);
	DrawRect(BOARD_X-3, BOARD_Y-3, BOARD_WIDTH*BLOCKSIZE+5, BOARD_HEIGHT*BLOCKSIZE+5, GREEN);

	InitSnake();
	lasttime = TimeCount;

	leftkeywas=rightkeywas=upkeywas=downkeywas=
		leftkeyhit=rightkeyhit=upkeyhit=downkeyhit=0;

	while (LastScan != sc_Escape)	// loop until user hits escape
	{
		//
		// always parse inputs, so we don't "miss" key strokes
		//
		IN_ReadCursor(&cursorinfo);

		leftkeywas = leftkeydown;
		rightkeywas = rightkeydown;
		upkeywas = upkeydown;
		downkeywas = downkeydown;

		leftkeydown = ((cursorinfo.x < 0) || IN_KeyDown(sc_LeftArrow));
		rightkeydown = ((cursorinfo.x > 0) || IN_KeyDown(sc_RightArrow));
		upkeydown  = ((cursorinfo.y < 0) || IN_KeyDown(sc_UpArrow));
		downkeydown = ((cursorinfo.y > 0) || IN_KeyDown(sc_DownArrow));

		if (leftkeydown && !leftkeywas)
			leftkeyhit = true;
		if (rightkeydown && !rightkeywas)
			rightkeyhit = true;
		if (upkeydown && !upkeywas)
			upkeyhit = true;
		if (downkeydown && !downkeywas)
			downkeyhit = true;

		//
		// check if enough time has passed to move the snake
		//
		ticcount = TimeCount-lasttime;
		if (ticcount >= snake->tics)
		{
			lasttime = TimeCount;

			if (!snake->apple)
				SpawnApple();

			leftkeyhit |= leftkeydown;
			rightkeyhit |= rightkeydown;
			upkeyhit |= upkeydown;
			downkeyhit |= downkeydown;

			switch (snake->dir)
			{
			case snake_up:
				if (leftkeyhit && !rightkeyhit)
				{
					snake->dir = snake_left;
				}
				else if (rightkeyhit && !leftkeyhit)
				{
					snake->dir = snake_right;
				}
				break;
			case snake_down:
				if (leftkeyhit && !rightkeyhit)
				{
					snake->dir = snake_left;
				}
				else if (rightkeyhit && !leftkeyhit)
				{
					snake->dir = snake_right;
				}
				break;
			case snake_left:
				if (upkeyhit && !downkeyhit)
				{
					snake->dir = snake_up;
				}
				else if (downkeyhit && !upkeyhit)
				{
					snake->dir = snake_down;
				}
				break;
			case snake_right:
				if (upkeyhit && !downkeyhit)
				{
					snake->dir = snake_up;
				}
				else if (downkeyhit && !upkeyhit)
				{
					snake->dir = snake_down;
				}
				break;
			}

			leftkeyhit=rightkeyhit=upkeyhit=downkeyhit=0;

			MoveSnake();

			VW_UpdateScreen();
		}
	}

	CheckScore();	// in case the player quits while having a high score
	IN_ClearKeysDown();
}

void PlayMinigame(void)
{
	MM_GetPtr(&(memptr)snake, sizeof(*snake));
	PlaySnake();
	MM_FreePtr(&(memptr)snake);
}
