#include "curses.hpp"

CursesWindow::CursesWindow(int x, int y, int width, int height)
{
    window = newwin(height, width, y, x);
    ::box(window, 0, 0);
    wrefresh(window);
}

CursesWindow::~CursesWindow()
{
    wborder(window, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(window);
    delwin(window);
    window = NULL;
}

void CursesWindow::box(void)
{
    ::box(window, 0, 0);
}

void CursesWindow::clear(void)
{
    wclear(window);
}

void CursesWindow::refresh(void)
{
    wrefresh(window);
}

void CursesWindow::move(int x, int y)
{
    wmove(window, y, x);
}

void CursesWindow::advance(int x, int y)
{
    int cx, cy;
    getyx(window, cy, cx);
    wmove(window, cy + y, cx + x);
}

void CursesWindow::write(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    vwprintw(window, format, argptr);
    va_end(argptr);
}

void CursesWindow::write(int x, int y, const char* format, ...)
{
    wmove(window, y, x);
    va_list argptr;
    va_start(argptr, format);
    vwprintw(window, format, argptr);
    va_end(argptr);
}

void CursesWindow::write(char ch)
{
    waddch(window, ch);
}

void CursesWindow::write(int x, int y, char ch)
{
    wmove(window, y, x);
    waddch(window, ch);
}

void CursesWindow::add_attr(int attr)
{
    wattron(window, attr);
}

void CursesWindow::remove_attr(int attr)
{
    wattroff(window, attr);
}

void CursesWindow::set_attr(int attr)
{
    wattrset(window, attr);
}
