"""Module defining the lc3 unit test case class and utilities.


"""
import base64
import enum
import pylc3
import six
import struct
import unittest


DEFAULT_MAX_EXECUTIONS = 1000000

def toShort(value):
    """Converts a value into a 16 bit unsigned short.

    Args:
        value: Integer - Value to convert.

    Returns:
        The value as a 16 bit unsigned short.
    """
    return value & 0xFFFF

class PreconditionFlag(enum.Enum):
    invalid = 0
    # True Traps Setting Flag. Default OFF
    true_traps = 1
    # Interrupts Setting Flag. Default OFF
    interrupts = 2
    # Plugins Setting Flag. Default ON
    plugins = 3
    # Strict Execution Setting Flag. Default ON
    strict_execution = 4
    # Strategy for Initialization of Memory (0: Fill with Value 1: Seeded Fill 2: Seeded Full Randomization).
    memory_strategy = 5
    # Parameter for Memory Strategy.
    memory_strategy_value = 6
    # Breakpoint address for Subroutine Testing.
    break_address = 7

    # 8-15 reserved.
    end_of_environment = 16

    # Set a Register.
    register = 17
    # Set the PC.
    pc = 18
    # Set a value at a label.
    value = 19
    # Set a value at an address pointed to by label.
    pointer = 20
    # Sets a region of memory starting at the address pointed to by label.
    array = 21
    # Sets a string starting at the address pointed to by label.
    string = 22
    # Sets console input
    input = 23
    # Simulate a call to a subroutine (following lc-3 calling convention).
    subroutine = 24

    end_of_preconditions = 0xff

class MemoryFillStrategy(enum.Enum):
    fill_with_value = 0
    random_fill_with_seed = 1
    completely_random_with_seed = 2

class Preconditions(object):
    """Represents the setup environment for a test case for replay.

    Generates base64 strings to replay the test in complx.
    """
    def __init__(self):
        # Dict of PreconditionFlag to data.
        self._environment_data = dict()
        # List of Tuple (PreconditionFlag, string label, num_params, params)
        self._precondition_data = []

    def addPrecondition(self, type_id, label, data):
        assert type_id.value > 15, 'Internal error, Precondition Flags are of id > 15'
        if isinstance(data, int):
            self._precondition_data.append((type_id.value, label, 1, [data]))
        else:
            self._precondition_data.append((type_id.value, label, len(data), data))

    def addEnvironment(self, type_id, data):
        assert type_id.value <= 15, 'Internal error, Environment Precondition Flags are of id <= 15'
        self._environment_data[type_id.value] = data

    def _formBlob(self):
        file = six.BytesIO()

        for id, value in self._environment_data.items():
            file.write(struct.pack('=B', id))
            file.write(struct.pack('=i', value))
        file.write(struct.pack('=B', 16))

        for id, label, num_params, params in self._precondition_data:
            label = six.b(label)

            file.write(struct.pack('=B', id))
            file.write(struct.pack('=I', len(label)))
            file.write(struct.pack('=%ds' % len(label), label))
            file.write(struct.pack('=I', num_params))
            params = [param & 0xFFFF for param in params]
            file.write(struct.pack('=%dH' % num_params, *params))

        file.write(struct.pack('=B', 0xff))

        blob = file.getvalue()
        file.close()

        return blob

    def encode(self):
        return base64.b64encode(self._formBlob())


class LC3UnitTestCase(unittest.TestCase):
    """LC3UnitTestCase class eases testing of LC3 code from within python.

    The methods init, setXXX and callSubroutine will automatically log the state set
    as preconditions. Later when the test runs and a assertion fails the state logged
    by the preconditions will output a base64 encoded string to ease setting up the
    test case in complx.

    It is important to not touch the state attribute and set things on it since these
    changes are not logged automatically and will result in the test environment being
    different from here and running in complx.
    """
    def setUp(self):
        self.state = pylc3.LC3State(testing_mode=True)
        self.break_address = None
        self.preconditions = Preconditions()
        self.enable_plugins = False
        self.true_traps = False
        # Registers values at the start of the test.
        self.registers = dict()
        # Expected Subroutine calls made.  Set of Tuple of (subroutine, Tuple of params)
        self.expected_subroutines = set()
        # Optional Subroutine calls.
        self.optional_subroutines = set()
        # Expected Trap calls made.  Set of Tuple of (vector, (Tuples of key value pairs of (register, param)) )
        self.expected_traps = set()
        # Optional Subroutine calls.
        self.optional_traps = set()
        # Trap specifications.  Dict of Vector to List of interested registers
        self.trap_specifications = dict()


    def init(self, strategy=MemoryFillStrategy.fill_with_value, value=0):
        """Initializes LC3 state memory.

        Args:
            strategy: a MemoryFillStrategy telling how memory should be initialized.
            value: Param for Memory Fill Strategy, either a fill value or random seed.
        """
        if strategy == MemoryFillStrategy.fill_with_value:
            self.state.init(False, value)
        elif strategy == MemoryFillStrategy.random_fill_with_seed:
            self.state.seed(value)
            self.state.init(False, self.state.random())
        elif strategy == MemoryFillStrategy.completely_random_with_seed:
            self.state.seed(value)
            self.state.init(True)

        self.preconditions.addEnvironment(PreconditionFlag.memory_strategy, strategy.value)
        self.preconditions.addEnvironment(PreconditionFlag.memory_strategy_value, value)

    def loadAsmFile(self, file):
        """Loads an assembly file.

        Args:
            file: String - Full path to the assembly file to load.
        """
        assert self.state.load(file, disable_plugins=not self.enable_plugins, process_debug_comments=False), ('Unable to load file %s' % file)

    def lookup(self, label):
        """Gets the address of a label by looking it up in the symbol table.

        This will assert if the label is not found.

        Args:
            label: String - Label to find.

        Returns:
            The address where the label is located.
        """
        addr = self.state.lookup(label)
        assert addr != -1, "Label %s was not found in the assembly code." % label
        return toShort(addr)

    def reverse_lookup(self, address):
        """Gets the label associated with address.

        This will assert if no label found at address.

        Args:
            address: Integer - Memory address to search for label.

        Returns:
            The label associated with the address.
        """
        label = self.state.reverse_lookup(toShort(address))
        assert label, "Address %x is not associated with a label." % address
        return label

    def readMem(self, addr, unsigned=False):
        """Reads a value at a memory address.

        Args:
            addr: Integer - Address to load from.
            unsigned: Boolean - Convert the value to be an unsigned short

        Returns:
            The value stored at that address.
        """
        value = self.state.get_memory(toShort(addr))
        return value if not unsigned else toShort(value)

    def writeMem(self, addr, value):
        """Write a value to a memory address.

        Args:
            addr: Integer - Address to write to.
            value: Integer - Value to write.
        """
        self.state.set_memory(toShort(addr), value)

    def readReg(self, reg, unsigned=False):
        """Reads a value in a register.

        Args:
            addr: Integer - Address to load from.
            unsigned: Boolean - Convert the value to be an unsigned short

        Returns:
            The value stored at that address.
        """
        assert reg >= 0 and reg < 8, 'Invalid register number'
        value = self.state.get_register(reg)
        return value if not unsigned else toShort(value)

    def writeReg(self, addr, value):
        """Write a value to a register.

        Args:
            addr: Integer - Address to write to.
            value: Integer - Value to write.
        """
        assert reg >= 0 and reg < 8, 'Invalid register number'
        self.state.set_register(toShort(addr), value)

    def setTrueTraps(self, setting):
        """Enables or disables True Traps.

        This should be called before load.

        Args:
            setting: True to enable.
        """
        self.true_traps = setting
        self.state.true_traps = setting

        self.preconditions.addEnvironment(PreconditionFlag.true_traps, setting)

    def setInterrupts(self, setting):
        """Enables or disables Interrupts.

        Args:
            setting: True to enable.
        """
        self.state.interupts = setting

        self.preconditions.addEnvironment(PreconditionFlag.interrupts, setting)

    def setPluginsEnabled(self, setting):
        """Enables or disables plugins.

        This should be called before load.

        Args:
            setting: True to enable.
        """
        self.enable_plugins = setting

        self.preconditions.addEnvironment(PreconditionFlag.plugins, setting)

    def setStrictExecution(self, setting):
        """Enables or disables strict execution mode.

        Executing invalid instructions will trigger an exception and halt the lc3state causing the test to fail.

        Args:
            setting: True to enable.
        """
        self.state.strict_execution = setting

        self.preconditions.addEnvironment(PreconditionFlag.strict_execution, setting)

    def setRegister(self, register_number, value):
        """Sets a Register.

        This exactly performs state.regs[register_number] = value

        Args:
            register_number: Index of the register to set.
            value: Integer - Value to write to that register.
        """
        assert register_number >= 0 and register_number < 8, 'Invalid register number'
        self.state.set_register(register_number, value)

        self.preconditions.addPrecondition(PreconditionFlag.register, str(register_number), value)

    def setPc(self, value):
        """Sets the PC.

        This exactly performs state.pc = value

        Args:
            value: Integer - Value to write to that register.
        """
        self.state.pc = toShort(value)

        self.preconditions.addPrecondition(PreconditionFlag.pc, "", value)

    def setValue(self, label, value):
        """Sets a value at a label.

        This exactly performs state.memory[label] = value.

        Args:
            label: String - Label pointing at the address to set.
            value: Integer - Value to write at that address
        """
        self.writeMem(self.lookup(label), value)

        self.preconditions.addPrecondition(PreconditionFlag.value, label, value)

    def setPointer(self, label, value):
        """Sets a value at an address pointed to by label

        This exactly performs state.memory[state.memory[label]] = value

        Args:
            label: String - Label pointing at the address which in turn contains the address to set.
            value: Integer - Value to write at that address.
        """
        self.writeMem(self.readMem(self.lookup(label)), value)

        self.preconditions.addPrecondition(PreconditionFlag.pointer, label, value)

    def setArray(self, label, arr):
        """Sets a sequence of values starting at the address pointed to by label.

        This exactly performs:
            state.memory[state.memory[label]] = arr[0]
            state.memory[state.memory[label] + 1] = arr[1]
            [...]
            state.memory[state.memory[label] + len(arr) - 1] = arr[len(arr) - 1]

        Args:
            label: String - Label pointing at the address which in turn contains the first address to set.
            arr: Iterable of Integers - Values to write sequentially in memory.
        """
        start_addr = self.readMem(self.lookup(label))
        for addr, elem in enumerate(arr, start_addr):
            self.writeMem(addr, elem)

        self.preconditions.addPrecondition(PreconditionFlag.array, label, arr)

    def setString(self, label, text):
        """Sets a sequence of characters followed by a NUL terminator starting at the address pointed to by label.

        This exactly performs:
            state.memory[state.memory[label]] = text[0]
            state.memory[state.memory[label] + 1] = text[1]
            [...]
            state.memory[state.memory[label] + len(text) - 1] = text[len(text) - 1]
            state.memory[state.memory[label] + len(text)] = 0

        Args:
            label: String - Label pointing at the address which in turn contains the address to set.
            value: String - String to write in memory.
        """
        start_addr = self.readMem(self.lookup(label))
        for addr, elem in enumerate(text, start_addr):
            self.writeMem(addr, ord(elem))
        self.writeMem(start_addr + len(text), 0)

        self.preconditions.addPrecondition(PreconditionFlag.string, label, [ord(char) for char in text])

    def setConsoleInput(self, input):
        """Sets console input for the test.

        Args:
            input: String - Input to set to the console.
        """
        self.state.input = input

        self.preconditions.addPrecondition(PreconditionFlag.input, "", [ord(char) for char in input])

    def callSubroutine(self, subroutine, params, r5=0xCAFE, r6=0xF000, r7=0x8000):
        """Sets the state to start executing a subroutine for the test.

        Note that this follows the LC3 Calling Convention.

        Args:
            subroutine: String - Label pointing at the start of the subroutine.
            params: List of Integer - Paramters to the subroutine.
            r5: Integer - Dummy value to store in r5 (frame pointer) for the test.
            r6: Integer - Dummy value to store in r6 (stack pointer) for the test.
            r7: Integer - Dummy value to store in r7 (return address) for the test.
        """
        self.state.pc = self.lookup(subroutine)
        self.state.r5 = r5
        self.state.r6 = r6 - len(params)
        self.state.r7 = r7
        self.break_address = r7
        self.state.add_breakpoint(r7)
        for addr, param in enumerate(params, self.state.r6):
            self.writeMem(addr, param)

        self.state.add_subroutine_info(subroutine, len(params))

        self.preconditions.addEnvironment(PreconditionFlag.break_address, r7)
        self.preconditions.addPrecondition(PreconditionFlag.subroutine, subroutine, [r5, r6, r7] + params)

    def expectSubroutineCall(self, subroutine, params, optional=False):
        """Expects that a subroutine call was made with parameters.

        Note that passing in the same (subroutine, params, optional) triplet is not supported. Nor is
        passing in the same (subroutine, params) with both optional set and not set.

        Args:
            subroutine: String - Label pointing at the start of the subroutine.
            params: List of Integer - Parameters to the subroutine.
            optional: Mark this as a optional subroutine call, if this subroutine call is found it is not checked.
        """
        self.addSubroutineInfo(subroutine, len(params))

        set_to_add = self.expected_subroutines
        value = (subroutine, tuple(params))
        if optional:
            set_to_add = self.optional_subroutines

        assert value not in set_to_add, 'Duplicate subroutine found %s, multiple copies of [subroutine, params] is not supported.' % value

        set_to_add.add(value)

        assert not (value in self.expected_subroutines and value in self.optional_subroutines), 'Subroutine %s found in both expected and optional subroutine calls.' % value

    def expectTrapCall(self, vector, params, optional=False):
        """Expects that a trap was made with parameters in registers.

        Note that passing in the same (vector, params, optional) triplet is not supported. Nor is
        passing in the same (vector, params) with both optional set and not set.

        It is undefined to call this with the same trap vector, but a different set of register parameters.

        Do not call this function with vector = x25. It is assumed that HALT will be called and is checked for via assertHalted.

        Args:
            vector: Integer - Label pointing at the start of the subroutine.
            params: Dict of Integer to Integer - Map of Register number to value.
            optional: Mark this as a optional trap call, if this trap call is found it is not checked.
        """
        assert vector != 0x25, 'Method expectTrapCall called with vector=0x25, use assertHalted instead.'

        if vector not in self.trap_specifications:
            self.trap_specifications[vector] = params.keys()

        set_to_add = self.expected_traps
        value = (vector, tuple([(k, v) for k, v in params.items()]))

        if optional:
            set_to_add = self.optional_traps

        assert value not in set_to_add, 'Duplicate trap found %s, multiple copies of [subroutine, params] is not supported.' % value

        set_to_add.add(value)

        assert not (value in self.expected_subroutines and value in self.optional_subroutines), 'Subroutine %s found in both expected and optional subroutine calls.' % value

    def addSubroutineInfo(self, subroutine, num_params):
        """Adds metadata for a subroutine.

        You shouldn't have to call this directly as it is called via callSubroutine and expectSubroutineCall.
        Calling this is required if you want to check a call made to a subroutine that follows the lc3 calling convention.
        The results are undefined if you call this multiple times with the same subroutine and different number of parameters.

        Args:
            subroutine: String - Label pointing at the start of the subroutine.
            num_params: Integer - Number of paramters to the subroutine.
        """
        self.state.add_subroutine_info(subroutine, num_params)

    def runCode(self, max_executions=DEFAULT_MAX_EXECUTIONS):
        """Runs the program until it halts or max_executions instructions has been executed.

        Args:
            max_executions: Integer - Maximum number of instructions to execute.
        """
        for id in range(8):
            self.registers[id] = self.state.get_register(id)
        self.state.run(max_executions)
        self.replay_msg = self._generateReplay()

    def assertShortEqual(self, expected, actual, msg=None):
        """Helper to assert if two 16 bit values are equal."""
        self.assertEqual(toShort(expected), toShort(actual), msg=msg)

    def assertReturned(self):
        """Assert that the code successfully returned from a subroutine call.

        This is achieved by hitting the breakpoint set at r7 when callSubroutine was called.
        """
        assert self.break_address is not None, "self.assertReturned() should only be used when a previous call to self.callSubroutine() was made."
        instruction = self.state.disassemble(self.state.pc, 1)
        malformed = '*' in instruction
        failure_msg = 'Code did not return from subroutine correctly.\n'
        failure_msg += 'This was due to executing data which was interpreted to a malformed instruction.\n' if malformed else ''
        failure_msg += 'This was probably due to an infinite loop in the code.\n' if not malformed else ''
        failure_msg += 'This may indicate that your handling of the stack is incorrect or that R7 was clobbered.\n'
        failure_msg += 'PC: x%04x\nInstruction last on: %s\n%s' % (self.state.pc, instruction, self.replay_msg)
        self.assertShortEqual(self.state.pc, self.break_address, failure_msg)

    def assertHalted(self):
        """Asserts that the LC3 has been halted normally.

        This is achieved by reaching a HALT statement or if true_traps is set and the MCR register's 15 bit is set.
        """
        assert self.break_address is None, "self.callSubroutine was previously used in this test, a call to assertReturned() should be made instead of assertHalted()."
        # Don't use self.state.halted here as that is set if a Malformed instruction is executed.
        instruction = self.state.disassemble(self.state.pc, 1)
        malformed = '*' in instruction
        failure_msg = 'Code did not halt normally.\n'
        failure_msg += 'This was due to executing data which was interpreted to a malformed instruction.\n' if malformed else ''
        failure_msg += 'This was probably due to an infinite loop in the code.\n' if not malformed else ''
        failure_msg += 'PC: x%04x\nInstruction last on: %s\n%s' % (self.state.pc, instruction, self.replay_msg)
        if self.true_traps:
            self.assertEqual(state.get_memory(0xFFFE) >> 15 & 1, 0, failure_msg)
        else:
            self.assertShortEqual(self.state.get_memory(self.state.pc), 0xF025, failure_msg)

    def assertNoWarnings(self):
        """Asserts that no warnings were reported during execution of the code."""
        self.assertFalse(self.state.warnings, 'Code generated warnings shown below:\n----\n%s%s' % (self.state.warnings, self.replay_msg))

    def assertRegister(self, register_number, value):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
        """
        assert register_number >= 0 and register_number < 8, 'Invalid register number'
        actual = self.readReg(register_number)
        expected = value
        self.assertShortEqual(expected, actual, 'R%d was expected to be (%d x%04x) but code produced (%d x%04x)\n%s' % (register_number, expected, toShort(expected), actual, toShort(actual), self.replay_msg))

    def assertPc(self, value):
        """Asserts that the PC is a certain value.

        This exactly checks if state.pc == value

        Args:
            value: Integer - Expected value.
        """
        actual = self.state.pc
        expected = value
        self.assertShortEqual(expected, actual, 'PC was expected to be x%04x but code produced x%04x\n%s' % (expected, actual, self.replay_msg))

    def assertValue(self, label, value):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
        """
        actual = self.readMem(self.lookup(label))
        expected = value
        self.assertShortEqual(expected, actual, 'MEM[%s] was expected to be (%d x%04x) but code produced (%d x%04x)\n%s' % (label, expected, toShort(expected), actual, toShort(actual), self.replay_msg))

    def assertPointer(self, label, value):
        """Asserts that a value at an address pointed to by label is a certain value.

        This exactly checks if state.memory[state.memory[label]] == value

        Args:
            label: String - Label pointing at the address which in turn contains the address to check.
            value: Integer - Expected value.
        """
        actual = self.readMem(self.readMem(self.lookup(label)))
        expected = value
        self.assertShortEqual(expected, actual, 'MEM[MEM[%s]] was expected to be (%d x%04x) but code produced (%d x%04x)\n%s' % (label, expected, toShort(expected), actual, toShort(actual), self.replay_msg))

    def assertArray(self, label, arr):
        """Asserts that a sequence of values starting at the address pointed to by label are certain values.

        This exactly checks if:
            state.memory[state.memory[label]] == arr[0]
            state.memory[state.memory[label] + 1] == arr[1]
            [...]
            state.memory[state.memory[label] + len(arr) - 1] == arr[len(arr) - 1]

        Args:
            label: String - Label pointing at the address which in turn contains the first address to start checking.
            arr: Iterable of Integers - Expected values to check sequentially.
        """
        start_addr = self.readMem(self.lookup(label))
        actual_arr = []
        for addr, _ in enumerate(arr, start_addr):
            actual_arr.append(self.readMem(addr))
        self.assertEqual(arr, actual_arr, 'Sequence of values starting at MEM[%s] was expected to be %s but code produced %s\n%s' % (label, arr, actual_arr, self.replay_msg))

    def assertString(self, label, text):
        """Asserts that sequence of characters followed by a NUL terminator starting at the address pointed to by label are certain values.

        This exactly checks if:
            state.memory[state.memory[label]] == text[0]
            state.memory[state.memory[label] + 1] == text[1]
            [...]
            state.memory[state.memory[label] + len(text) - 1] == text[len(text) - 1]
            state.memory[state.memory[label] + len(text)] == 0

        Args:
            label: String - Label pointing at the address which in turn contains the first address to start checking.
            text: String - Expected characters to check sequentially.
        """
        start_addr = self.readMem(self.lookup(label))
        actual_str = []
        for addr, _ in enumerate(text, start_addr):
            actual_str.append(six.unichr(self.readMem(addr, unsigned=True)))
        actual_str.append(six.unichr(self.readMem(start_addr + len(text), unsigned=True)))
        expected_str = list(six.u(text))
        expected_str.append(u'\0')
        self.assertEqual(expected_str, actual_str, 'String of characters starting at MEM[%s] was expected to be %s but code produced %s\n%s' % (label, repr(''.join(expected_str)), repr(''.join(actual_str)), self.replay_msg))

    def assertConsoleOutput(self, output):
        """Asserts that console output is a certain string.

        Args:
            input: String - Expected console output.
        """
        expected = output
        actual = self.state.output
        self.assertEqual(expected, actual, 'Console output was expected to be %s but code produced %s\n%s' % (repr(expected), repr(actual), self.replay_msg))

    def assertReturnValue(self, answer):
        """Asserts that the correct answer was returned.

        This is for verifying that the lc3 calling convention postcondition that memory[r6] == answer

        Args:
            answer: Integer - Expected answer.
        """
        expected = answer
        actual = self.readMem(self.state.r6)
        self.assertShortEqual(expected, actual, 'Return value was expected to be (%d x%04x) but code produced (%d x%04x)\n%s' % (expected, toShort(expected), actual, toShort(actual), self.replay_msg))

    def assertRegistersUnchanged(self, registers):
        """Asserts that registers value are the same as the beginning of execution.

        This is for verifying that caller saved registers are not clobbered as part of the lc3 calling convention.

        Args:
            registers: List(Integer) - Register numbers's to check.
        """
        original_values = {num: self.registers[num] for num in registers}
        current_values = {num: self.readReg(num) for num in registers}

        changed_registers = ['R%d' % reg for reg in registers if original_values[reg] != current_values[reg]]
        all_registers = ['R%d' % reg for reg in registers]
        self.assertFalse(changed_registers, 'Expected %s to be unchanged after program/subroutine execution.\nThese registers have changed %s\n%s' % (all_registers, changed_registers, self.replay_msg))

    def assertStackManaged(self, stack, return_address, old_frame_pointer):
        """Asserts that the stack was managed correctly.

        This means that:
            The stack_pointer (r6) was decremented by 1.
            The stack contents are [return_address, old_frame_pointer].
            Answer location is checked via assertReturnValue.

        Args:
            stack: Integer - Expected stack location.
            return_address: Integer - Expected Return Address.
            old_frame_pointer: Integer - Expected Old Frame Pointer.
        """
        actual_return_address = self.readMem(self.state.r6 - 1, unsigned=True)
        actual_old_frame_ptr = self.readMem(self.state.r6 - 2, unsigned=True)

        self.assertShortEqual(self.state.r6, stack, 'Calling convention not followed.\nExpected R6 to be decremented by exactly 1 after returning from subroutine it was decremented by: %d\n%s' % (toShort(self.state.r6) - (stack + 1), self.replay_msg))
        self.assertEqual(return_address, actual_return_address, 'Expected return address x%04x not found on stack in correct location code produced x%04x\n%s' % (return_address, actual_return_address, self.replay_msg))
        self.assertEqual(old_frame_pointer, actual_old_frame_ptr, 'Expected old frame pointer x%04x not found on stack in correct location code produced x%04x\n%s' % (old_frame_pointer, actual_old_frame_ptr, self.replay_msg))

    def assertSubroutineCallsMade(self):
        """Asserts that the expected subroutine calls were made with no unexpected ones made.

        It is required to call expectSubroutineCall in order for this function to work. If it is
        not then it will check for no subroutines to be called.
        """
        def subroutine_list(subrs):
            return ' '.join(['%s(%s)' % (name, ','.join(map(str, params))) for name, params in subrs])

        actual_subroutines = set()
        for call in self.state.first_level_calls:
            actual_subroutines.add((self.reverse_lookup(call.address), tuple(call.params)))

        made_calls = self.expected_subroutines & actual_subroutines
        missing_calls = self.expected_subroutines - actual_subroutines
        optional_calls = self.optional_subroutines & actual_subroutines
        unknown_calls = actual_subroutines - (self.expected_subroutines | self.optional_subroutines)

        status_message = ''
        status_message += 'Expected the following subroutine calls to be made: %s\n' % subroutine_list(self.expected_subroutines) if self.expected_subroutines else 'Expected no subroutines to have been called.\n'
        status_message += 'Calls made correctly: %s\n' % (subroutine_list(made_calls) if made_calls else 'none')
        status_message += 'Required calls missing: %s\n' % (subroutine_list(missing_calls) if missing_calls else 'none')
        status_message += 'Accepted optional calls made: %s\n' % subroutine_list(optional_calls) if optional_calls else ''
        status_message += 'Unknown subroutine calls made: %s\n' % subroutine_list(unknown_calls) if unknown_calls else ''
        status_message += self.replay_msg

        self.assertEqual(self.expected_subroutines, made_calls, status_message)
        self.assertFalse(missing_calls, status_message)
        self.assertFalse(unknown_calls, status_message)

    def assertTrapCallsMade(self):
        """Asserts that the expected traps were called with no unexpected ones made.

        It is required to call expectTrapCall for this function to work. If it is
        not then it will check for no traps called.
        """
        # (vector, (Tuples of key value pairs of (register, param)) )
        def trap_list(traps):
            trap_strs = []
            for trap in traps:
                vector, params = trap
                params_pairs = list(params)
                trap_name = self.state.disassemble_data(0xF000 | vector, 1)
                params = ', '.join(['R%d=(%d x%04x)' % (reg, value, toShort(value)) for reg, value in params_pairs])
                trap_strs.append('%s(%s)' % (trap_name, params) if params else trap_name)
            return ' '.join(trap_strs)

        actual_traps = set()
        for call in self.state.first_level_traps:
            params = tuple([(reg, param) for reg, param in enumerate(call.regs) if reg in self.trap_specifications[call.vector]])
            actual_traps.add((call.vector, params))

        made_calls = self.expected_traps & actual_traps
        missing_calls = self.expected_traps - actual_traps
        optional_calls = self.optional_traps & actual_traps
        unknown_calls = actual_traps - (self.expected_traps | self.optional_traps)

        status_message = ''
        status_message += 'Expected the following traps to have been made: %s\n' % trap_list(self.expected_traps) if self.expected_traps else 'Expected no traps to have been called.\n'
        status_message += 'Traps made correctly: %s\n' % (trap_list(made_calls) if made_calls else 'none')
        status_message += 'Required traps missing: %s\n' % (trap_list(missing_calls) if missing_calls else 'none')
        status_message += 'Accepted optional traps made: %s\n' % trap_list(optional_calls) if optional_calls else ''
        status_message += 'Unknown traps made: %s\n' % trap_list(unknown_calls) if unknown_calls else ''
        status_message += self.replay_msg

        self.assertEqual(self.expected_traps, made_calls, status_message)
        self.assertFalse(missing_calls, status_message)
        self.assertFalse(unknown_calls, status_message)

    def _generateReplay(self):
        return "\nString to set up this test in complx: %s" % repr(self.preconditions.encode())
