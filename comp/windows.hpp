#ifndef WINDOWS_HPP
#define WINDOWS_HPP

#include "curses.hpp"
#include <memory>
#include <lc3_all.hpp>

class MemoryWindow
{
public:
    MemoryWindow(int x, int y, int columns, int rows) :
        width(columns), height(rows), window(new CursesWindow(x, y, columns, rows)) {InitColors(); Update();}
    void Update(void);
    void Display(unsigned short start, unsigned short end, int level);
    void WriteInstruction(unsigned short data);
private:
    int width, height;
    void InitColors();
    void WriteRow(int row, unsigned short addr, int level = LC3_NORMAL_DISASSEMBLE);
    std::unique_ptr<CursesWindow> window;
};

class RegisterWindow
{
public:
    RegisterWindow(int x, int y, int width, int height) :
        window(new CursesWindow(x, y, width, height)) {Update();}
    void Update(void);
private:
    std::unique_ptr<CursesWindow> window;
};


#endif
