#ifndef CURSES_HPP
#define CURSES_HPP

#include <ncurses.h>

class CursesWindow
{
public:
    CursesWindow(int x, int y, int width, int height);
    ~CursesWindow();
    void box(void);
    void clear(void);
    void refresh(void);
    void move(int x, int y);
    void advance(int x, int y);
    void write(const char* format, ...);
    void write(int x, int y, const char* format, ...);
    void write(char ch);
    void write(int x, int y, char ch);
    void add_attr(int attr);
    void remove_attr(int attr);
    void set_attr(int attr);
    WINDOW* window;
private:
    CursesWindow(const CursesWindow&) = delete;
    CursesWindow(CursesWindow&&) = delete;
    CursesWindow& operator=(const CursesWindow&) = delete;
    CursesWindow& operator=(CursesWindow&&) = delete;
};

#endif
