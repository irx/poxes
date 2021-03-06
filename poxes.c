/* Copyright (c) 2021 Maksymilian Mruszczak <u at one u x dot o r g>
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
*/

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>

unsigned int		pox_stdscr_cols, pox_stdscr_lines;
static struct termios	orig_conf;
static void		(*winch_custom_handler)(void);

static void		update_win_size(void);
static void		signal_handler(int);

void
pox_bind_resize_handler(void (*handler)(void))
{
	winch_custom_handler = handler;
}

void
pox_clear(void)
{
	printf("\33[2J");
	fflush(stdout);
}

void
pox_cooked(void)
{
	struct termios cooked;
	tcgetattr(STDIN_FILENO, &cooked);
	cooked.c_lflag |= (ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &cooked);
}

void
pox_exit(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_conf);
	printf("\33[?1049l");
	fflush(stdout);
}

static void
update_win_size(void)
{
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	pox_stdscr_cols = w.ws_col;
	pox_stdscr_lines = w.ws_row;
	printf("%u %u\n", pox_stdscr_cols, pox_stdscr_cols);
}

int
pox_init(void)
{
	struct sigaction sa;
	printf("\33[?1049h"); /* save screen */
	fflush(stdout);
	tcgetattr(STDIN_FILENO, &orig_conf);
	/* place for initialisations and stuff */
	memset(&sa, 0, sizeof(sa));
	sigaddset(&sa.sa_mask, SIGWINCH);
	sa.sa_handler = signal_handler;
	if (sigaction(SIGWINCH, &sa, NULL) == -1)
		return 1;
	update_win_size();
	winch_custom_handler = NULL;
	return 0;
}

void
pox_move(unsigned int x, unsigned int y)
{
	printf("\33[%u;%uH", y, x);
	fflush(stdout);
}

void
pox_raw(void)
{
	struct termios raw;
	tcgetattr(STDIN_FILENO, &raw);
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static void
signal_handler(int signo)
{
	update_win_size();
	if (signo == SIGWINCH && winch_custom_handler)
		(*winch_custom_handler)();
}
