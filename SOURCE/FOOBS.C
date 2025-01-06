// FOOBS IN SPACE! prototype by K1n9_Duk3

// #include this file at the end of CK_INTER.C

#define MAXANGLE 0x100
#define SINTABLESIZE (MAXANGLE/4)
#define COS_MUL(angle, factor)	SIN_MUL((angle)+MAXANGLE/4, factor)

// sine lookup table ("fixed point", 7 bits):
unsigned char sin_table[SINTABLESIZE+1] =
{
   0,   3,   6,   9,  12,  15,  18,  21,  24,  28,  31,  34,  37,  40,  43,  46,
  48,  51,  54,  57,  60,  63,  65,  68,  71,  73,  76,  78,  81,  83,  85,  88,
  90,  92,  94,  96,  98, 100, 102, 104, 106, 108, 109, 111, 112, 114, 115, 117,
 118, 119, 120, 121, 122, 123, 124, 124, 125, 126, 126, 127, 127, 127, 127, 127,
 128
};

int near pascal SIN_MUL(unsigned char angle, int factor)
{
#if 0
	// This works, but it's slightly less efficient because it uses long (32-bit)
	// multiply and shift operations at the end
	int sinval;
	
	if (angle <= SINTABLESIZE)
		sinval = sin_table[angle];
	else if (angle <= 2*SINTABLESIZE)
		sinval = sin_table[2*SINTABLESIZE - angle];
	else if (angle <= 3*SINTABLESIZE)
		sinval = -sin_table[angle - 2*SINTABLESIZE];
	else
		sinval = -sin_table[MAXANGLE - angle];

	return ((long)factor * sinval) >> 7;
#else
	// This inline assembly version is smaller and more efficient
	asm {
		xor	bx, bx;
		xor	ax, ax;
		mov	bl, angle;
		cmp	bl, SINTABLESIZE
		ja 	q2;
		mov	al, BYTE PTR sin_table[bx];
		jmp	multiply;
	}
q2:
	asm {
		cmp	bl, 2*SINTABLESIZE;
		ja 	q3;
		neg	bx;
		mov	al, BYTE PTR sin_table[bx+2*SINTABLESIZE];
		jmp	multiply;
	}
q3:
	asm {
		cmp	bl, 3*SINTABLESIZE;
		ja 	q4;
		mov	al, BYTE PTR sin_table[bx-2*SINTABLESIZE];
		jmp	neg_multiply;
	}
q4:
	asm {
		neg	bx;
		mov	al, BYTE PTR sin_table[bx+MAXANGLE];
	}
neg_multiply:
	asm	neg	ax;
multiply:
	asm {
		imul	factor;
		shl	ax, 1;
		rcl	dx, 1;
		mov	al, ah;
		mov	ah, dl;
	}
	return _AX;
#endif
}

//===========================================================================

#define PIXELCHECK	// screen has parts that should not get overwritten

#define SCREENPIXWIDTH 320
#define SCREENPIXHEIGHT 200

#ifdef PIXELCHECK
unsigned char near pascal ReadPixel(int x, int y)
{
	asm {
		mov	es, screenseg;
		
		mov	bx, y;
		shl	bx, 1;
		mov	bx, WORD PTR ylookup[bx];
		add	bx, displayofs;
		
		mov	ax, x;
		mov	cl, al;
		shr	ax, 3;
		add	bx, ax;
		
		// create bit mask for current x position
		mov	ah, 80h;
		and	cl, 7;
		shr	ah, cl;
		
		xor	cx, cx;	// pixel is black by default
		
		mov	dx, 3CEh;
		mov	al, 4;
		out	dx, al;
		inc	dx;
		mov	al, 0;
		out	dx, al;	// set read map: plane 0 (blue)
		test	es:[bx], ah;
		jz		green;
		or		cl, 1;
	}
green:
	asm {
		inc	al;
		out	dx, al;	// set read map: plane 1 (green)
		test	es:[bx], ah;
		jz		red;
		or		cl, 2;
	}
red:
	asm {
		inc	al;
		out	dx, al;	// set read map: plane 2 (red)
		test	es:[bx], ah;
		jz		intensity;
		or		cl, 4;
	}
intensity:
	asm {
		inc	al;
		out	dx, al;	// set read map: plane 3 (intensity)
		test	es:[bx], ah;
		jz		done;
		or		cl, 8;
	}
done:
	return _CX;
}
#endif

//===========================================================================

#define MyRand() ((unsigned)(rand()))

//===========================================================================

typedef struct
{
	unsigned char angle, y;
	unsigned x;
	unsigned speed, moved;
} startype;

#if 0

#define NUM_STARS 100
startype stars[NUM_STARS];

#else
	
// I'll be re-using the ck_SWDrawFuncs array for storing the stars array to
// reduce the memory taken up by this code. The Star Wars code is going to
// initialize the array while generating the scaling code, so storing other data
// in that memory region while the Star Wars code is not running should not
// cause any problems.
#define NUM_STARS (sizeof(ck_SWDrawFuncs)/sizeof(startype))
#define stars ((startype*)&ck_SWDrawFuncs)

#endif

void near pascal CalcStarPos(startype *star)
{
	star->x = SIN_MUL(star->angle, star->moved/2) + SCREENPIXWIDTH/2;
	star->y = COS_MUL(star->angle, star->moved/2) + SCREENPIXHEIGHT/2;
}

void near pascal InitStar(startype *star)
{
	do
	{
		star->angle = MyRand() % MAXANGLE;
		star->moved = MyRand() % 128;
		CalcStarPos(star);
	} while (star->x >= SCREENPIXWIDTH || star->y >= SCREENPIXHEIGHT);
	star->speed = (MyRand() % 4) + 1;
}

void near InitStarfield(void)
{
	startype *star;
	
	for (star=stars; star<stars+NUM_STARS; star++)
	{
		InitStar(star);
	}
}

void near UpdateStarfield(void)
{
	unsigned char color;
	startype *star;
	
	for (star=stars; star<stars+NUM_STARS; star++)
	{
		// erase the star at the old position:
#ifdef PIXELCHECK
		color = ReadPixel(star->x, star->y);
		if (color == DARKGRAY || color == LIGHTGRAY || color == WHITE)
			VW_Plot(star->x, star->y, BLACK);
#else
		VW_Plot(star->x, star->y, BLACK);
#endif
		
		// move the star:
		star->moved += star->speed;
		CalcStarPos(star);
		
		// reset the star when it leaves the visible screen area:
		if (star->x >= SCREENPIXWIDTH || star->y >= SCREENPIXHEIGHT)
			InitStar(star);
		
		// stars start dark in the center and get brighter as they move away 
		if (star->moved < 75)
			color = DARKGRAY;
		else if (star->moved < 150)
			color = LIGHTGRAY;
		else
			color = WHITE;
		
		// draw the star at the new position:
#ifdef PIXELCHECK
		if (ReadPixel(star->x, star->y) == BLACK)
			VW_Plot(star->x, star->y, color);
#else
		VW_Plot(star->x, star->y, star->color);
#endif
	}
}

//===========================================================================

#define DrawFoobImage(x, y, n) VW_ScreenToScreen(masterofs+(n)*2, displayofs+ylookup[(y)]+((unsigned)(x)/8), 2, 18)

static signed char amplitude = 10;
static signed char ampdelta = 1;

void near pascal DrawLetter(int x, int y, unsigned char *pattern, int size, int frame, int counter)
{
	while (size)
	{
		DrawFoobImage(x+pattern[0], y+pattern[1]+SIN_MUL(counter+x+pattern[0], amplitude), ((counter+frame) >> 4) & 1);
		pattern += 2;
		size -= 2;
		
		// try to make the Foobs animate separately instead of all at once:
		frame += 8;
	}
}

void near DrawFoobs(int counter)
{
#define LETTERWIDTH (3*16)
#define SPACEWIDTH 8
	static unsigned char F[] = {0,16, 16,16, 32,16, 0,33, 0,50, 16,50, 0,67, 0,84};
	static unsigned char O[] = {0,24, 0,41, 0,58, 0,75, 16,16, 16,84, 32,24, 32,41, 32,58, 32,75};
	static unsigned char B[] = {0,16, 16,16, 0,33, 0,50, 16,50, 0,67, 0,84, 16,84, 32,21, 32,38, 32,62, 32,79};
	static unsigned char S[] = {32,16, 16,16, 0,24, 0,41, 16,50, 32,58, 32,75, 16,84, 0,84};
	
	int frame = 0;
	int x = (SCREENPIXWIDTH-(5*LETTERWIDTH+4*SPACEWIDTH))/2;
	int y = 0;
	
	DrawLetter(x, y, F, sizeof(F), frame++, counter); x+=LETTERWIDTH+SPACEWIDTH;
	DrawLetter(x, y, O, sizeof(O), frame++, counter); x+=LETTERWIDTH+SPACEWIDTH;
	DrawLetter(x, y, O, sizeof(O), frame++, counter); x+=LETTERWIDTH+SPACEWIDTH;
	DrawLetter(x, y, B, sizeof(B), frame++, counter); x+=LETTERWIDTH+SPACEWIDTH;
	DrawLetter(x, y, S, sizeof(S), frame++, counter);
	
#undef LETTERWIDTH
#undef SPACEWIDTH
}

//===========================================================================

// Note: caller is responsible for cache level adjustments!

void RunFoobsInSpace(void)
{
	unsigned oldtimecount;
	int counter, frame, x;
	
	VW_ClearVideo(BLACK);
	RF_FixOfs();	// clear panning variables
	CK_DefaultPalette();
	
	// replace EGA LIGHTMAGENTA with EGA WHITE to allow the Foob image to show
	// white pixels that won't get overwritten by the flying stars:
	asm	mov	ax, 1000h;
	asm	mov	bx, 1F0Dh;
	asm	int	10h;
	// replace EGA LIGHTRED with EGA BLACK to allow the Foob image to show
	// black pixels that won't get overwritten by the flying stars:
	asm	mov	ax, 1000h;
	asm	mov	bx, 000Ch;
	asm	int	10h;
	
	InitStarfield();
	
	// draw Foob images to master page
	CA_CacheGrChunk(FOOBSPIC);
#if FOOBSPIC >= STARTEXTERNS
	// assuming Foobs pic is in an EXTERN chunk (32 pixels wide, 16 pixels high)
	VW_MemToScreen(grsegs[FOOBSPIC], masterofs+ylookup[1], 32/SCREENXDIV, 16);
#else
	// assuming Foobs pic is a regular pic chunk
	bufferofs = masterofs;
	VW_DrawPic(0, 1, FOOBSPIC);
#endif
	
	// draw everything else directly to the display page
	bufferofs = displayofs;
	
	// set window size so we can use CPrint to center the text
	WindowX = 0;
	WindowW = SCREENPIXWIDTH;
	
	// let's use the Star Wars font for "IN SPACE!" -- just because we can
	CA_CacheGrChunk(STARTFONT+2);
	fontnumber = 2;
	
	fontcolor = YELLOW;
	PrintY = 128;
	SM_CPrint(S_INSPACE);
	//US_CPrint("IN SPACE!");
	
	fontnumber = 0;	// back to default font
	fontcolor = RED;
	PrintY = 184;
	SM_CPrint(S_INSERTCOIN);
	//US_CPrint("*** INSERT COIN ***");
	fontcolor = WHITE;
	
	IN_ClearKeysDown();
	
	counter = 0;
	amplitude = 10;
	ampdelta = 1;
	
	//oldtimecount = TimeCount;	// not really necessary
	// Note: Initializing oldtimecount would be good practice, but we don't
	// really need to do it here. The worst thing that will happen is that the
	// code may or may not skip waiting for 1 tic after the first frame.
	
	for (;;)
	{
		counter++;
#if 0
		for (frame=0,x = (SCREENPIXWIDTH-16)/2 - 128; x <= (SCREENPIXWIDTH-16)/2 + 128; x += 32, frame+=4)
			DrawFoobImage(x, (SCREENPIXHEIGHT-16)/2+SIN_MUL(counter+x, 16), ((counter+frame) >> 4) & 1);
#else
		if ((counter & 0x1FF) == 0)
		{
			if (amplitude == 10 || amplitude == 0)
				ampdelta = -ampdelta;
			amplitude += ampdelta;
		}
		
		DrawFoobs(counter);
#endif
		
		UpdateStarfield();
		
		// wait 1 VBL:
		//VW_WaitVBL(1);
		// Note: VW_WaitVBL() can miss the vertical retrace when the game is
		// running on a slow system and music is enabled, which can lead to
		// VERY long delays before the screen is updated. It's probably better
		// not to wait for a retrace here and instead wait for a timer tic.
		
		// wait until at least 1 tic has elapsed:
		while (oldtimecount == (unsigned)TimeCount)
			;
		oldtimecount = TimeCount;
		
		// make red pixels flash:
		asm	mov	bh, 4;
		if (counter & 8)
			asm	or 	bh, 18h;
		asm	mov	bl, 04h;
		asm	mov	ax, 1000h;
		asm	int	10h;
		
		// check if a key was hit:
		if (IN_IsUserInput())
			break;
	}
	
	VW_ClearVideo(BLACK);	// can't fade out the FOOBS stuff in EGA (palette changes)
	RF_FixOfs();	// reset bufferofs
}
