#include "comp.tab.hpp"
#include "comp.hpp"
#include "windows.hpp"
#include <lc3_all.hpp>
#include <ncurses.h>
#include <cstdlib>
#include <csignal>
#include <memory>
#include <sstream>

lc3_state state;
std::unique_ptr<MemoryWindow> memory;
std::unique_ptr<RegisterWindow> registers;
std::vector<std::string> cmd_history;
unsigned int cmd_history_index = 0;
std::string current_command;

void resize_handler(int)
{
    endwin();
    memory.reset(NULL);
    registers.reset(NULL);
    refresh();
    clear();

    if (LINES < 10 || COLS < 70)
    {
        mvprintw(0, 0, "Window too small");
        refresh();
        return;
    }

    refresh();
    memory.reset(new MemoryWindow(0, 0, COLS, LINES - 8));
    registers.reset(new RegisterWindow(0, LINES - 8, COLS, 6));
    mvprintw(LINES - 2, 0, "> ");
    refresh();
}


void initialize_colors(void)
{
    start_color();
    for (int i = 1; i < 8; i++)
        init_pair(i, COLOR_WHITE, i);
}

void warning(const char* s, char* t)
{
    mvprintw(LINES - 1, 0, "comp: %s", s);
    if (t)
        printw(" %s", t);
    move(LINES - 2, 2);
}

void yyerror(const char* s)
{
    warning(s, NULL);
}

std::string read_command(void)
{
    int x, y;
    getyx(stdscr, y, x);
    clrtoeol();
    refresh();

    noecho();
    std::unique_ptr<std::stringstream> cmd(new std::stringstream);
    int ch;
    int curpos = 0;
    int size = 0;
    current_command = "";
    while ((ch = getch()) != ERR)
    {
        if (ch == '\n' || ch == '\r' || ch == KEY_ENTER)
        {
            break;
        }
        else if (ch == KEY_UP)
        {
            if (cmd_history_index == 0)
            {
                beep();
            }
            else
            {
                cmd_history_index--;
                cmd->str("");
                (*cmd) << cmd_history[cmd_history_index];
                size = curpos = cmd_history[cmd_history_index].size();
            }
        }
        else if (ch == KEY_DOWN)
        {
            if (cmd_history_index == cmd_history.size())
            {
                beep();
            }
            else
            {
                cmd_history_index++;
                cmd->str("");
                if (cmd_history_index == cmd_history.size())
                {
                    (*cmd) << current_command;
                    curpos = current_command.size();
                }
                else
                {
                    (*cmd) << cmd_history[cmd_history_index];
                    curpos = cmd_history[cmd_history_index].size();
                }
                size = curpos;
            }
        }
        else if (ch == KEY_LEFT)
        {
            if (curpos <= 0)
            {
                beep();
            }
            else
            {
                cmd->seekp(-1, std::ios_base::cur);
                curpos--;
            }
        }
        else if (ch == KEY_RIGHT)
        {
            if (curpos >= size)
            {
                beep();
            }
            else
            {
                cmd->seekp(1, std::ios_base::cur);
                curpos++;
            }
        }
        else if (ch == KEY_BACKSPACE)
        {
            if (curpos <= 0)
            {
                beep();
            }
            else
            {
                std::stringstream* new_cmd = new std::stringstream;
                std::string cur = cmd->str();
                cur.erase(curpos - 1, 1);
                (*new_cmd) << cur;
                cmd.reset(new_cmd);
                curpos--;
                size--;
            }
        }
        else if (ch == KEY_DC)
        {
            if (curpos >= size)
            {
                beep();
            }
            else
            {
                std::stringstream* new_cmd = new std::stringstream;
                std::string cur = cmd->str();
                cur.erase(curpos, 1);
                (*new_cmd) << cur;
                cmd.reset(new_cmd);
                size--;
            }
        }
        else
        {
            cmd->put(ch);
            curpos++;
            size++;
        }
        move(y, x);
        clrtoeol();
        std::string command = cmd->str();
        printw("%s", command.c_str());
        //mvprintw(LINES - 1, 0, "%d %d %d", curpos, size, cmd_history_index);
        move(y, curpos + x);
        refresh();
        if (cmd_history_index == cmd_history.size())
            current_command = std::move(command);
    }
    echo();
    if (ch == ERR)
        return "";

    return cmd->str();
}

void init(void)
{
    //yydebug = 1;
    lc3_init(state);
    initscr();
    keypad(stdscr, 1);
    raw();
    initialize_colors();
    refresh();

    resize_handler(0);

}

void cmd_loop(void)
{
    while(1)
    {
        std::string command = read_command();
        if (command.empty() && !cmd_history.empty())
            command = cmd_history.back();
        else
            cmd_history.push_back(command);
        cmd_history_index = cmd_history.size();
        move(LINES - 2, 0);
        clrtoeol();
        move(LINES - 1, 0);
        clrtoeol();
        mvprintw(LINES - 2, 0, "> ");

        yy_scan_string(command.c_str());
        yyparse();

        refresh();
    }
}

int main(int argc, char** argv)
{
    //putenv("TERM=xterm-256color");

    signal(SIGWINCH, resize_handler);

    init();
    cmd_loop();
    getch();

    endwin();
    return 0;
}
