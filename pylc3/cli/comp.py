from terminaltables import SingleTable
import cmd
import enum
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

class SetTargetType(enum.Enum):
    register = 0
    pc = 1
    cc = 2
    address = 3
    label = 4

def set_target(param):
    """Converter function for the target of the set command.

    The target can be any register, a label, the pc, the cc, or a memory address."""
    if len(param) == 2 and (param[0] == 'R' or param[0] == 'r') and (param[1] >= '0' and param[1] <= '7'):
        return (SetTargetType.register, int(param[1]))
    if param.lower() == 'pc':
        return (SetTargetType.pc, None)
    if param.lower() == 'cc':
        return (SetTargetType.cc, None)
    value = address(param)
    if isinstance(value, int):
        return (SetTargetType.address, value)
    return (SetTargetType.label, value)

class Comp(cmd.Cmd):

    def __init__(self, lc3_state=None, show_help=True, disable_message=False):
        cmd.Cmd.__init__(self)
        self.state = pylc3.LC3State() if not lc3_state else lc3_state
        self.show_help = show_help
        self.disable_message = disable_message
        self.prompt = '(comp) '
        self.intro = 'Welcome to the comp text interface.\n\n' + self.status_str()
        # Display status after command completion
        self.display_status = True
        self.file = None

    def parse(self, arg, *types, **kwargs):
        def error():
            if self.show_help:
                print('Error in command. Usage below.\n')
                caller_func = inspect.stack()[2][3]
                caller = caller_func[3:]
                self.do_help(caller)
            self.display_status = False

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
        if self.display_status:
            print(self.status_str())
        self.display_status = True

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

    def message(self, string):
        if not self.disable_message:
            print(string)
        self.display_status = False

    def do_run(self, arg):
        """run - Runs until the lc3 halts."""
        params = self.parse(arg)
        if params is None:
            return

        self.state.run()

    # Execute
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

        self.state.next_line(*params)

    def do_previous(self, arg):
        """previous [x=1] - If x is given goes back x lines (not stepping into any subroutines / traps) else back steps 1 line."""
        params = self.parse(arg, int, num_required=0, defaults=[1])
        if params is None:
            return

        self.state.previous_line(*params)

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
            self.message('Successfully set breakpoint.')
        else:
            self.message('Failed to set breakpoint.')

    def do_breakpoint(self, arg):
        """breakpoint addr[ cond[ times[ name]]] - Puts breakpoint at addr with condition cond, times number of hits before becoming inactive, and name name"""
        params = self.parse(arg, address, str, int, str, num_required=1, defaults=["1", -1, ""])
        if params is None:
            return

        if not self.state.add_breakpoint(*params):
            self.message('Successfully set breakpoint.')
        else:
            self.message('Failed to set breakpoint.')

    def do_watchpoint(self, arg):
        "watchpoint addr/label/reg condition[ times[ name]] - Sets a watchpoint on target with condition cond, times number of hits before becoming inactive, and name name."""
        params = self.parse(arg, watch_point_target, str, int, str, num_required=2, defaults=[-1, ""])
        if params is None:
            return

        target, condition, times, name = params

        if isinstance(target, tuple):
            is_reg, data = target
            failure = self.state.add_watchpoint(is_reg, data, condition, times, name)
        else:
            failure = self.state.add_watchpoint(target, condition, times, name)


        if not failure:
            self.message('Successfully set watchpoint.')
        else:
            self.message('Failed to set watchpoint.')

    def do_blackbox(self, arg):
        """blackbox addr[ condition[ name]] - Marks addr as a blackbox."""
        params = self.parse(arg, address, str, str, num_required=1, defaults=["1", ""])
        if params is None:
            return

        if not self.state.add_blackbox(*params):
            self.message('Successfully set blackbox.')
        else:
            self.message('Failed to set blackbox.')

    def do_undo_stack_size(self, arg):
        """undo_stack_size [num=-1] - Sets the undo stack length. Default is maximum int."""
        params = self.parse(arg, int, num_required=0, defaults=[0xFFFFFFFF])
        if params is None:
            return

        self.state.max_undo_stack_size, = params
        self.message('Set max undo stack stack size to %d.' % self.state.max_undo_stack_size)

    def do_call_stack_size(self, arg):
        """call_stack_size [num=1] - Sets the call stack length. Default is maximum int."""
        params = self.parse(arg, int)
        if params is None:
            return

        self.state.max_call_stack_size, = params
        self.message('Set max undo stack stack size to %d.' % self.state.max_undo_stack_size)

    def do_delete_breakpoint(self, arg):
        """delete_breakpoint addr - Deletes the breakpoint with label or address."""
        params = self.parse(arg, address)
        if params is None:
            return

        if not self.state.remove_breakpoint(*params):
            self.message('Successfully deleted breakpoint.')
        else:
            self.message('Failed to delete breakpoint.')

    def do_delete_watchpoint(self, arg):
        """delete_watchpoint addr/symbol/reg - Deletes the watchpoint with address/symbol/register."""
        params = self.parse(arg, watch_point_target)
        if params is None:
            return

        target, = params

        if len(target) == 1:
            symbol, = target
            target_str = symbol
            failure = self.state.remove_watchpoint(symbol)
        else:
            is_reg, data = target
            target_str = 'R%d' % data if is_reg else 'x%04x' % data
            failure = self.state.remove_watchpoint(is_reg, data)

        if not failure:
            self.message('Successfully deleted watchpoint.')
        else:
            self.message('Failed to delete watchpoint.')

    def do_delete_blackbox(self, arg):
        """delete_blackbox addr - Deletes the blackbox with label or address."""
        params = self.parse(arg, address)
        if params is None:
            return

        if not self.state.remove_blackbox(*params):
            self.message('Successfully deleted blackbox.')
        else:
            self.message('Failed to delete blackbox.')

    # Manipulation
    def do_set(self, arg):
        """set thing value - Sets register/cc/pc/address to value."""
        params = self.parse(arg, set_target, int)
        if params is None:
            return

        (set_type, set_data), value = params

        if (set_type == SetTargetType.register):
            self.state.set_register(set_data, value)
        elif (set_type == SetTargetType.pc):
            self.state.set_pc(value)
        elif (set_type == SetTargetType.cc):
            self.state.n = value < 0
            self.state.z = value == 0
            self.state.p = value > 0
        elif (set_type == SetTargetType.address):
            self.state.set_memory(set_data, value)
        elif (set_type == SetTargetType.label):
            addr = self.state.lookup(set_data)
            if (addr == -1):
                self.message('Label %s not found.' % set_data)
                return
            self.state.set_memory(addr, value)

    def do_fill_memory(self, arg):
        """fill_memory value - Fills entire lc3 memory and registers with value."""
        params = self.parse(arg, int)
        if params is None:
            return

        self.state.init(False, *params)

    def do_reset(self, arg):
        """reset - Resets the lc3 state."""
        params = self.parse(arg)
        if params is None:
            return

        self.state.init()

    def do_randomize(self, arg):
        """randomize seed - Randomizes the lc3 state."""
        params = self.parse(arg, int, num_required=0, defaults=[time.now()])
        if params is None:
            return

        self.state.seed(*params)
        self.state.init()

    def do_true_traps(self, arg):
        """true_traps [bool] - Toggles true_traps if no param given else true sets true_traps false unsets true_traps."""
        params = self.parse(arg, int, num_required=0, defaults=[not self.state.true_traps])
        if params is None:
            return

        value, = params
        self.state.true_traps, = bool(value)
        self.message('%s true traps.' % ('Set' if value else 'Reset'))

    def do_interrupts(self, arg):
        """interrupts [bool] - Toggles interrupts if no param given else true sets interrupts false unsets interrupts."""
        params = self.parse(arg, int, num_required=0, defaults=[not self.state.interrupts])
        if params is None:
            return

        value, = params
        self.state.interrupts, = bool(value)
        self.message('%s interrupts.' % ('Set' if value else 'Reset'))

    def do_strict_execution(self, arg):
        """strict_execution [bool] - Toggles strict_execution if no param given else true sets strict_execution false unsets strict_execution."""
        params = self.parse(arg, int, num_required=0, defaults=[not self.state.strict_execution])
        if params is None:
            return

        value, = params
        self.state.interrupts, = bool(value)
        self.message('%s strict execution.' % ('Set' if value else 'Reset'))

    # Display
    def do_list(self, arg):
        """list [addr[ end]] - Displays memory addresses starting from start to end inclusive. If end is not given then displays start. If neither given displays instructions around the PC."""
        def binary(data):
            base = bin(data)[2:]
            return '0' * (16 - len(base)) + base

        params = self.parse(arg, address, address, num_required=0, defaults=[None, None])
        if params is None:
            return

        start, end = params
        if start is None and end is None:
            start = self.state.pc - 5
            end = self.state.pc + 5
        elif end is None:
            end = start
        
        table_data = [['Addr', 'Hex', 'Dec', 'Binary', 'Label', 'Instruction', 'Comment']]

        for addr in xrange(start, end+1):
            data = self.state.get_memory(addr)
            symbol = self.state.reverse_lookup(addr)
            instruction = self.state.disassemble(addr, 1)
            comment = self.state.comment(addr).strip()
            table_data.append(['x%04x' % addr, 'x%04x' % toShort(data), data, binary(toShort(data)), symbol, instruction, comment])

        table = SingleTable(table_data)
        self.message(table.table)

    def do_list_breakpoints(self, arg):
        """list_breakpoints - Lists all breakpoints with detailed information."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.state.breakpoints:
            self.message('There are no breakpoints.')
            return

        table_data = [["Name", "Target", "Enabled", "Condition", "Max Hits", "Hits"]]

        for entry in self.state.breakpoints:
            addr, breakpoint = (entry.key(), entry.data())
            
            name = breakpoint.label
            symbol = self.state.reverse_lookup(addr)
            target = symbol if symbol else 'x%04x' % addr
            enabled = str(breakpoint.enabled)
            condition = breakpoint.condition
            times = str(breakpoint.max_hits)
            hits = str(breakpoint.hit_count)
            table_data.append([name, target, enabled, condition, times, hits])
        
        table = SingleTable(table_data)
        self.message(table.table)

    def do_list_watchpoints(self, arg):
        """list_watchpoints - Lists all watchpoints with detailed information."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.state.memory_watchpoints and not self.state.register_watchpoints:
            self.message('There are no watchpoints.')
            return

        table_data = [["Name", "Target", "Enabled", "Condition", "Max Hits", "Hits"]]

        for entry in self.state.register_watchpoints:
            reg, watchpoint = (entry.key(), entry.data())
            
            name = watchpoint.label
            target = 'R%d' % reg
            enabled = str(watchpoint.enabled)
            condition = watchpoint.condition
            times = str(watchpoint.max_hits)
            hits = str(watchpoint.hit_count)
            table_data.append([name, target, enabled, condition, times, hits])

        for entry in self.state.memory_watchpoints:
            addr, watchpoint = (entry.key(), entry.data())
            
            name = watchpoint.label
            symbol = self.state.reverse_lookup(addr)
            target = symbol if symbol else 'x%04x' % addr
            enabled = str(watchpoint.enabled)
            condition = watchpoint.condition
            times = str(watchpoint.max_hits)
            hits = str(watchpoint.hit_count)
            table_data.append([name, target, enabled, condition, times, hits])
        
        table = SingleTable(table_data)
        self.message(table.table)

    def do_list_blackboxes(self, arg):
        """list_blackboxes - Lists all blackboxes with detailed information."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.state.blackboxes:
            self.message('There are no blackboxes.')
            return

        table_data = [["Name", "Target", "Enabled", "Condition", "Hits"]]

        for entry in self.state.blackboxes:
            addr, blackbox = (entry.key(), entry.data())
            
            name = blackbox.label
            symbol = self.state.reverse_lookup(addr)
            target = symbol if symbol else 'x%04x' % addr
            enabled = str(blackbox.enabled)
            condition = blackbox.condition
            hits = str(blackbox.hit_count)
            table_data.append([name, target, enabled, condition, hits])
        
        table = SingleTable(table_data)
        self.message(table.table)

    # File
    def do_load(self, arg):
        """load filename - Reinitializes and loads filename."""
        params = self.parse(arg, str)
        if params is None:
            return

        self.state.init()
        if not self.state.load(*params):
            self.message('Unable to load file %s.' % params)
            return
        self.file, = params
        self.message('Successfully loaded file %s.' % params)

    def do_reload(self, arg):
        """reload - Reinitializes and Reloads last file if there was a file already loaded."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.file:
            return

        self.state.init()
        if not self.state.load(*params):
            self.message('Unable to reload file %s.' % params)
            return
        self.message('Successfully reloaded file %s.' % params)

    def do_load_over(self, arg):
        """loadover filename - Loads filename over current state."""
        params = self.parse(arg, str)
        if params is None:
            return

        if not self.state.load(*params):
            self.message('Unable to load file %s.' % params)
            return
        self.message('Successfully loaded file %s.' % params)
        self.file, = params

    def do_reload_over(self, arg):
        """reloadover - Reloads last filename over current state."""
        params = self.parse(arg)
        if params is None:
            return

        if not self.file:
            return

        if not self.state.load(*params):
            self.message('Unable to load file %s.' % params)
            return
        self.message('Successfully loaded file %s.' % params)

    def do_quit(self, arg):
        """quit - Exits the simulator."""
        self.message('Goodbye.')
        return True

    def do_exit(self, arg):
        """exit - Exits the simulator."""
        self.message('Goodbye.')
        return True

    def do_EOF(self, arg):
        self.message('Goodbye.')
        return True

if __name__ == '__main__':
    Comp().cmdloop()
