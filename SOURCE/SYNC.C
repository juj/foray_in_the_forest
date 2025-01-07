/* SYNC.C: Implements timing synchronization using CRT Terminator.
 * Copyright (C) 2025 Jukka Jyl„nki
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Implements vertical refresh synchronization and smooth hardware scrolling
// in a manner that minimizes the waiting time with interrupts disabled, and
// avoids various (S)VGA hardware adapter glitches in DS and HS register
// latching semantics. Uses CRT Terminator support if available.

// Also provides an optimized version of Adlib register synchronization for
// a reduced time spent in Keen music interrupt.

#include <dos.h>

// Waits until vblank start using CRT Terminator hardware support.
// N.b. this function will disable interrupts and when this function returns,
// interrupts will still be disabled, so caller is responsible for calling
// "asm sti" or enable() afterwards.
static void wait_vblank_start_crt_terminator(void)
{
	asm mov dx, 127h // Port 127h: CRT Terminator "Scanlines until vblank"

wait_vblank_end: // If we happen to already be in vblank, skip this one.
	asm in al, dx
	asm test al, al
	asm jz wait_vblank_end

leisurely_wait_scanlines: // With interrupts enabled, skip visible scanlines.
	asm in al, dx
	asm test al, 0E0h // al >= 32?
	asm jnz leisurely_wait_scanlines

	asm cli // we are close to start of vblank, better disable interrupts
	asm dec dl // Port 126h: CRT Terminator "Frame counter" port
	asm in al, dx
	asm mov ah, al // ah: the old frame counter, wait until we see a new frame

wait_until_vblank_start: // tight wait the last few scanlines until vblank
	asm in al, dx
	asm xor al, ah
	asm jz wait_until_vblank_start
}

// Waits until vblank start using self-calibrating code when CRT Terminator
// is not available.
// N.b. this function will disable interrupts and when this function returns,
// interrupts will still be disabled, so caller is responsible for calling
// "asm sti" or enable() afterwards.
static void wait_vblank_start_self_calibrating(void)
{
	// We want to sync to the start of a vblank (not vsync) below, so that
	// DS and HS registers can be safely updated. But the problem is that the
	// EGA/VGA adapters do not provide a mechanism to wait until start of vblank,
	// i.e. it does not enable distinguishing between hblank and vblank.

	// To overcome this problem, we first measure the length of a hblank, and then
	// use the measured value to search when we arrive at start of vblank instead.

	// This way the latching behavior of all EGA/VGA/SVGA cards is appeased, and
	// since the hblank length is re-benchmarked each frame, the following code
	// also works on emulators like DOSBox when one messes with the cycles
	// setting in the middle of running the game.

	asm mov dx, 3DAh

restart_wait:
	asm cli        // Stop interrupts. The code below is timing critical.
	asm mov ch, 0  // CH will accumulate the hblank length

wait_blank_end:  // 1. if we are in hblank or vblank, wait for it to end
	asm in al, dx  // Read 3DAh - Status Register
	asm test al, 1 // Bit 0: Display Blank
	asm jnz wait_blank_end

wait_active_end: // 2. wait for the end of the active scanline
	asm in al, dx  // Read 3DAh again
	asm test al, 1
	asm jz wait_active_end

	// We are now right at the very start of a blank, either hblank or vblank.
	// Calibrate calculation of how many cycles this blank lasts.
calc_blank_length: // 3. measure how many I/O port read cmds we can do in blank
	asm in al, dx    // Read status port.

	asm test al, 8 // Test if we are in fact in vsync, and not in hblank?
	asm jnz restart_wait // We did reach vsync, must restart search from scratch.

	asm inc ch       // Accumulate count of I/Os we can do within a blank.
	asm test al, 1   // Still in blank period? Then repeat
	asm jnz calc_blank_length

	// We are now in visible picture area with interrupts disabled.

	// Search for a blank length that is longer than (blank+1)*2 the one that we
	// found. That should be enough to ascertain we have a vblank and not just a
	// hblank.
	asm inc ch
	asm shl ch, 1

wait_for_active_picture: // 4. Wait until we are in visible picture area.
	asm in al, dx          // Read 3DAh - Status Register
	asm test al, 1         // Bit 0: set if we are in Display Blank.
	asm jnz wait_for_active_picture

	// We are now in visible picture area (so can't be in vsync, or right headed
	// into it)

wait_for_hblank:
	asm sti        // Let interrupts breathe, or audio and keyboard may stall.
	asm mov cl, ch // Reset wait counter to # of I/Os in hblank
	asm cli        // Restore interrupts to enter time critical stage.

loop_hblank_length_times: // 5. Estimate the length of this blank.
	asm in al, dx    // Read status port

	asm test al, 1   // Are we in display blank?
	asm jz wait_for_hblank // If 0, we are still in visible picture area. Restart.

	asm test al, 8 // We are in blank, but have we slipped over to vsync?
	asm jnz restart_wait // If 1, we are in vsync so blew it, restart the wait.

in_blank_not_vsync: // We are in blank, either hblank or vblank
	asm dec cl        // Decrement search counter
	asm jnz loop_hblank_length_times // And loop back if we still need to.

	// When we get here, we have entered a blank period that is longer than
	// a hblank interval, so we conclude we must have just now entered a vblank.
	// (but we aren't yet at start of vsync)
	// Interrupts are disabled at this point, so we can safely update
	// Display Start (DS) and Horizontal Shift Count (HS) registers so all
	// adapters will latch it properly, with all their varying quirky behaviors.
}

// Updates Display Start and Horizontal Shift registers. Must be in start of
// vblank so that this will be done synchronously in a safe manner. Enables
// interrupts after done.
static void write_scroll_regs(unsigned int display_start, unsigned char hshift)
{
	// Pedantically, it is tiny bit better better to write DS register before HS,
	// because IBM EGA and VGA latch the DS register before the HS register.

	asm mov dx, 3D4h
	asm mov ax, [display_start]

	asm mov bh, al // save low part of address temporarily to bh
	asm mov al, 0Ch

	asm out dx, ax // Write 3D4h:0Ch Display Start Address high part
	asm inc al
	asm mov ah, bh
	asm out dx, ax // Write 3D4h:0Dh Display Start Address low part

	asm mov dx, 3DAh
	asm in al, dx // Do a dummy I/O read to reset 3C0h flip flop to index mode

	asm mov al, 33h // 20h (display visible) | 13h (Pixel Panning register)
	asm mov dx, 3C0h
	asm out dx, al
	asm mov al, [hshift]
	asm out dx, al // Write Horizontal Pixel Shift Count

	asm sti // We are all done, so re-enable interrupts and exit
}

// Returns 1 if CRT Terminator is detected on the system, 0 otherwise.
int crtt_detect(void)
{
	static int crtt_found = 0;
	int i;
	if (!crtt_found)
	{
		crtt_found = -1;
		for(i = 4; i--;) // one of 4 consecutive reads must be a 'C'
		{
			char id = inportb(0x120);
			if (id == 'C')
			{
				if (inportb(0x120) != 'R') break;
				if (inportb(0x120) != 'T') break;
				if (inportb(0x120) != 'T') break;
				crtt_found = 1;
				break;
			}
			if (id != 'R' && id != 'T') break;
		}
	}
	return crtt_found == 1;
}

void detect_vsync_method(void);

void (*wait_vblank)(void) = detect_vsync_method;

// First run will calibrate whether CRT Terminator is present and choose which
// synchronization method to use.
static void detect_vsync_method(void)
{
	wait_vblank = crtt_detect() ? wait_vblank_start_crt_terminator : wait_vblank_start_self_calibrating;
}

//////////////////////////////////////
// Commander Keen specific code below:

extern unsigned long TimeCount;

// Fetches the current frame number that the display is on using CRT Terminator.
// This function must be called while interrupts are disabled, because the
// function is also called from within the Keen audio timer interrupt. Otherwise
// if this function is re-entered, TimeCount could get the wrong value.
void update_time_count_interrupts_disabled(void)
{
	if (crtt_detect())
	{
		static unsigned char prev_frame;
		unsigned char frame = inp(0x126), delta = frame - prev_frame;
		TimeCount += delta;
		prev_frame = frame;
	}
}

void update_time_count(void)
{
	disable();
	update_time_count_interrupts_disabled();
	enable();
}

static unsigned int next_crtc, next_pel;

// Records the new DS and HS register values to program when synchronizing to
// vblank start the next time.
void VW_SetScreenDelayed(unsigned int crtc, unsigned int pel)
{
	next_crtc = crtc;
	next_pel = pel;
}

// Override original Keen VW_SetScreen() function to perform a vblank wait using
// CRT Terminator, and reprogram DS and HS registers in a safe area.
void VW_SetScreen(unsigned int crtc, unsigned int pel)
{
	wait_vblank();
	write_scroll_regs(crtc, pel);

	next_crtc = crtc;
	next_pel = pel;
}

// Override original Keen VWL_WaitVBL() function to perform a vblank wait using
// CRT Terminator, and program any pending delayed DS and HS registers onto the
// adapter.
void VWL_WaitVBL(void)
{
	wait_vblank();
	write_scroll_regs(next_crtc, next_pel);
	update_time_count(); // Make sure Keen has the latest game time in TimeCount.
}

// Override original Keen VW_WaitVBL() function to wait N vblanks. TODO: this
// function could be greatly improved to first measure how many of these frames
// might have already been taken up, to implement bomb proof 35fps scrolling as
// well (which is problematic without a frame counter)
void VW_WaitVBL(unsigned int number)
{
	while(number--) VWL_WaitVBL();
}

extern int alPort;

// A replacement of the original Keen alOut() function that uses CRT Terminator
// high precision wallclock timer for more performant Adlib programming
// synchronization. This helps reduce game FPS microstuttering caused by audio
// interrupt overhead.
// Outputs data b to Adlib address n using CRT Terminator synchronization.
static void adlib_out_crt_terminator(unsigned char n, unsigned char b)
{
	// Activate that Adlib address [n] to write to.
	asm pushf
	asm mov dx, alPort
	asm mov al, [n]
	asm out dx, al

	// After writing to Adlib Address port, Adlib programming manual states that
	// a wait of 3.3 usecs is needed. Traditionally this is done by reading Adlib
	// address register six times. However on my 80 MHz 486 PC at least, this ends
	// up overshooting the wait to 9.105 usecs (2.75x longer)

	// CRT Terminator provides a 32-bit 27_000_000 Hz high precision clock
	// on ports 128h, 129h, 12Ah, 12Bh (little endian)
	// In 3.3 usecs this port advances by 27clk/us*3.3us = 89.1clk.
	// This code was benchmarked on the same system to take about 6.195 usecs.
	asm mov dx, 128h
	asm in al, dx
	asm mov bl, al
again:
	asm in al, dx
	asm sub al, bl
	asm cmp al, 89
	asm jbe again

	// Write the data [b] to the Adlib Data port.
	asm mov dx, alPort
	asm inc dx
	asm mov al, [b]
	asm out dx, al
	asm popf

	// After writing to Adlib Data port, Adlib programming manual states that a
	// wait of 23 usecs is needed. Traditionally this is done by performing 36
	// hardware port reads. However on my 80 MHz 486 PC at least, this ends
	// up overshooting the wait to 52.94 usecs (2.30x longer)
	// In 23 usecs CRT Terminator high precision time counter advances by
	// 27clk/us*23us = 621clk. This code was benchmarked to take only about
	// 26.38 usecs, and helps game FPS stability and micro-stuttering.
	asm mov dx, 128h
	asm in ax, dx
	asm mov bx, ax

again2:
	asm in ax, dx
	asm sub ax, bx
	asm cmp ax, 621
	asm jbe again2
}

void alOut_original(unsigned char n, unsigned char b);
void alOut_probe_on_first_run(unsigned char n, unsigned char b);

void (*alOut)(unsigned char n, unsigned char b) = alOut_probe_on_first_run;

void alOut_probe_on_first_run(unsigned char n, unsigned char b)
{
	alOut = crtt_detect() ? adlib_out_crt_terminator : alOut_original;
	alOut(n, b);
}
