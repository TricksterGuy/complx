#include "commands.hpp"
#include <lc3_all.hpp>

extern lc3_state state;

void do_run(void)
{

}

void do_step(int times)
{

}

void do_back(int times)
{

}

void do_next(int times)
{

}

void do_prev(int times)
{

}

void do_finish(void)
{

}

void do_rewind(void)
{

}

void do_tempbreak(unsigned short address)
{

}

void do_break(unsigned short address, const std::string& condition, int times, const std::string& name)
{

}

void do_watch(const std::string& target, const std::string& condition, int times, const std::string& name)
{

}

void do_watch(unsigned char reg, const std::string& condition, int times, const std::string& name)
{

}

void do_blackbox(unsigned short address, const std::string& name, const std::string& condition)
{

}

void do_undo_stack(unsigned short num)
{

}

void do_delete(const std::string& name)
{

}

void do_delete(unsigned short address)
{

}

void do_set(const std::string& thing, short value)
{

}

void do_set(const std::string& thing, const std::string& value)
{

}

void do_input(const std::string& source)
{

}

void do_output(const std::string& sink)
{

}

void do_reset(void)
{

}

void do_randomize(void)
{

}

void do_true_traps(bool set)
{

}

void do_interrupt(bool set)
{

}

void do_print(const std::string& thing)
{

}

void do_list(void)
{

}

void do_list(unsigned short start)
{

}

void do_list(unsigned short start, unsigned short end, int level)
{

}

void do_dump(unsigned short start)
{

}

void do_dump(unsigned short start, unsigned short end)
{

}

void do_debug_info(void)
{

}

void do_debug_info(const std::string& name)
{

}

void do_debug_info(unsigned short address)
{

}

void do_info(void)
{

}

void do_load(const std::string& filename)
{

}

void do_reload(const std::string& filename)
{

}

void do_loadover(const std::string& filename)
{

}

void do_reloadover(const std::string& filename)
{

}

void do_quit(void)
{

}

void do_exit(void)
{

}
