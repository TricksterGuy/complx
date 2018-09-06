import cmd
import inspect
import shlex
import pylc3

def toShort(value):
    """Converts a value into a 16 bit unsigned short.

    Args:
        value: Integer - Value to convert.

    Returns:
        The value as a 16 bit unsigned short.
    """
    return value & 0xFFFF

def address(param):
    """Converter function for lc3 addresses and symbols

    If the string param begins with x it will interpret it as a hexadecimal address.
    Otherwise it is treated as a label.

    Args:
        param: String - Value to try to convert.

    Returns:
        An Integer if the param is an address,
        A string if the param is a label.

    Raises:
        ValueError if the string is not parseable as an address or label.
    """
    if param[0] == 'x':
        try:
            return toShort(int(param[1:], 16))
        except ValueError:
            pass
    if param[0] >= '0' and param[0] <= '9':
        raise ValueError('%s is not parseable as an address or label' % param)
    return param

def watch_point_target(param):
    """Converter function for a watch point target param.

    The target of a watchpoint can be a label, a register, or a memory address.

    Args:
        param: String - Value to try to convert.

    Returns:
        (True, reg_num) if param is a register.
        (False, address) if param was an address.
        (label,) A tuple with a string if param was a label.

    Raises:
        ValueError if the string is not parseable as an address or label.
    """
    if len(param) == 2 and (param[0] == 'R' or param[0] == 'r') and (param[1] >= '0' and param[1] <= '7'):
        return (True, int(param[1]))
    value = address(param)
    if isinstance(value, int):
        return (False, value)
    return value

class Comp(cmd.Cmd):

    def __init__(self, show_help=True):
        cmd.Cmd.__init__(self)
        self.state = pylc3.LC3State()
        self.show_help=show_help
        self.prompt = '(comp) '
        self.intro = 'Welcome to the comp text interface.\n\n' + self.status_str()
        self.last_command_error = False
        self.file = None

    def parse(self, arg, *types, **kwargs):
        def error():
            if self.show_help:
                print 'Error in command. Usage below.\n'
                caller_func = inspect.stack()[2][3]
                caller = caller_func[3:]
                self.do_help(caller)
            self.last_command_error = True

        num_required = kwargs.get('num_required', len(types))
        defaults = kwargs.get('defaults', [])


        assert num_required <= len(types), 'Number of required arguments > argument types'
        assert num_required + len(defaults) == len(types) or len(defaults) == 0, 'Too many default values given'

            
        args = shlex.split(arg)

        if len(args) > len(types) or len(args) < num_required:
            error()
            return None

        params = []
        for i, arg in enumerate(args):
            try:
                params.append(types[i](arg))
            except (TypeError, ValueError):
                error()
                return None

        for i in xrange(len(args), len(types)):
            index = i - num_required
            params.append(defaults[index] if index < len(defaults) else None)

        return tuple(params)

    def postcmd(self, stop, line):
        if stop:
            return stop
        if not self.last_command_error:
            print self.status_str()
        self.last_command_error = False

    def status_str(self):
        r0 = self.state.r0
        r1 = self.state.r1
        r2 = self.state.r2
        r3 = self.state.r3
        r4 = self.state.r4
        r5 = self.state.r5
        r6 = self.state.r6
        r7 = self.state.r7
        cc = "n" if self.state.n else ("z" if self.state.z else "p")
        pc = self.state.pc
        instr = self.state.disassemble(self.state.pc, 1)
        return ("r0 x%04x %5d\tr1 x%04x %5d\tr2 x%04x %5d\tr3 x%04x %5d\n"
                "r4 x%04x %5d\tr5 x%04x %5d\tr6 x%04x %5d\tr7 x%04x %5d\n"
                "cc %s\n"
                "pc x%04x instr: %s") % (
                 toShort(r0), r0, toShort(r1), r1, toShort(r2), r2, toShort(r3), r3,
                 toShort(r4), r4, toShort(r5), r5, toShort(r6), r6, toShort(r7), r7,
                 cc,
                 pc, instr)

    def do_run(self, arg):
        """run - Runs until the lc3 halts or 1000000 instructions were executed."""
        params = self.parse(arg)
        if params is None:
            return

        self.state.run()

    def do_step(self, arg):
        """step [x=1] - If x is given runs for that many instructions else steps 1 instruction."""
        params = self.parse(arg, int, num_required=0, defaults=[1])
        if params is None:
            return

        self.state.run(*params)

    def do_back(self, arg):
        """back [x=1] - If x is given back steps for that many instructions else backsteps 1 instruction"""
        params = self.parse(arg, int, num_required=0, defaults=[1])
        if params is None:
            return

        self.state.rewind(*params)

    def do_next(self, arg):
        """next [x=1] - If x is given steps x lines (not stepping into any subroutines / traps) else steps 1 line."""
        params = self.parse(arg, int, num_required=0, defaults=[1])
        if params is None:
            return

        for _ in xrange(*params):
            self.state.next_line()

    def do_prev(self, arg):
        """prev [x=1] - If x is given goes back x lines (not stepping into any subroutines / traps) else back steps 1 line."""
        params = self.parse(arg, int, num_required=0, defaults=[1])
        if params is None:
            return

        for _ in xrange(*params):
            self.state.previous_line()

    def do_finish(self, arg):
        """finish - Steps out of the current subroutine/trap/interrupt."""
        params = self.parse(arg)
        if params is None:
            return

        self.state.finish()

    def do_rewind(self, arg):
        """rewind - Backsteps until undo stack empty."""
        params = self.parse(arg)
        if params is None:
            return

        self.state.rewind()

    # Debugging
    def do_tempbreak(self, arg):
        """tempbreak addr - Puts a temporary breakpoint at addr (this is equivalent to break addr "1" -1 name)."""
        params = self.parse(arg, address)
        if params is None:
            return

        if not self.state.add_breakpoint(*params):
            print 'Successfully set breakpoint at %s.' % params

    def do_break(self, arg):
        """break addr[, cond[, times[, name]]] - Puts breakpoint at addr with condition cond, times number of hits before becoming inactive, and name name"""
        params = self.parse(arg, address, str, int, str, num_required=1, defaults=["1", -1, ""])
        if params is None:
            return

        if not self.state.add_breakpoint(*params):
            print 'Successfully set breakpoint with params %s.' % str(params)

    def do_watch(self, arg):
        "watch target, condition[, times[, name]] - Sets a watchpoint on target with condition cond, times number of hits before becoming inactive, and name name."""
        params = self.parse(arg, watch_point_target, str, int, str, num_required=2, defaults=[-1, ""])
        if params is None:
            return

        target, condition, times, name = params

        if len(target) == 1:
            symbol, = target
            target_str = symbol
            failure = self.state.add_watchpoint(symbol, condition, times, name)
        else:
            is_reg, data = target
            target_str = 'R%d' % data if is_reg else 'x%04x' % data
            failure = self.state.add_watchpoint(is_reg, data, condition, times, name)

        if not failure:
            print 'Successfully set watchpoint on %s with condition %s and times %d with name %s' % (target_str, condition, times, name)

    def do_blackbox(self, arg):
        """blackbox addr[, name[, condition]] - Marks addr as a blackbox."""
        pass

    def do_undostack(self, arg):
        """undostack num - Sets the undo stack length."""
        pass

    def do_callstack(self, arg):
        """callstack num - Sets the call stack length."""
        pass

    def do_delete(self, arg):
        """delete name/addr - Deletes the breakpoint/watchpoint/blackbox with name or address."""
        pass

    # Manipulation
    def do_set(self, arg):
        """set thing value - Sets register/cc/pc/address to value."""
        pass

    def do_fillmem(self, arg):
        """fillmem value - Fills entire lc3 memory with value."""
        pass

    def do_input(self, arg):
        """input fname/cin - Sets source for console input."""
        pass

    def do_output(self, arg):
        """output fname/cout - Sets source for console output."""
        pass

    def do_reset(self, arg):
        """reset - Resets the lc3 state."""
        pass

    def do_randomize(self, arg):
        """randomize - Randomizes the lc3 state."""
        pass

    def do_truetraps(self, arg):
        """truetraps bool - Toggles true_traps if no param given else true sets true_traps false unsets true_traps."""
        pass

    def do_interrupt(self, arg):
        """interrupt bool - Toggles interrupts if no param given else true sets interrupts false unsets interrupts."""
        pass

    # Display
    def do_list(self, arg):
        """list addr[, level=1] - Displays memory addresses starting from start. If level is given affects the disassemble level (basic=0, normal=1, highlevel=2)"""
        pass


    # File
    def do_load(self, arg):
        """load filename - Reinitializes and loads filename."""
        params = self.parse(arg, str)
        if params is None:
            return

        self.state.init()
        self.state.load(*params)
        self.file, = params

    def do_reload(self, arg):
        """reload - Reinitializes and Reloads last file if there was a file already loaded."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.file:
            return

        self.state.init()
        self.state.load(self.file)

    def do_loadover(self, arg):
        """loadover filename - Loads filename over current state."""
        params = self.parse(arg, str)
        if params is None:
            return

        self.state.load(*params)
        self.file, = params

    def do_reloadover(self, arg):
        """reloadover - Reloads last filename over current state."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.file:
            return

        self.state.load(self.file)

    def do_quit(self, arg):
        """quit - Exits the simulator."""
        return True

    def do_exit(self, arg):
        """exit - Exits the simulator."""
        return True

    def do_EOF(self, arg):
        return True

if __name__ == '__main__':
    Comp().cmdloop()
