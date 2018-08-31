"""Module defining the lc3 unit test case class and utilities.


"""
import base64
import enum
import pylc3
import StringIO
import struct
import unittest


DEFAULT_MAX_EXECUTIONS = 1000000


class EnvironmentFlag(enum.Enum):
    invalid = 0
    # True Traps Setting Flag.
    true_traps = 1
    # Interrupts Enabled Flag.
    interrupts = 2
    # Disable Plugins Flag.
    plugins = 3
    # Strategy for Initialization of Memory (0: Fill with Value 1: Seeded Fill 2: Seeded Full Randomization).
    memory_strategy = 4
    # Parameter for Memory Strategy.
    memory_strategy_value = 5
    # Breakpoint address for Subroutine Testing.
    break_address = 6


class MemoryFillStrategy(enum.Enum):
    fill_with_value = 0
    random_fill_with_seed = 1
    completely_random_with_seed = 2


class PreconditionFlag(enum.Enum):
    invalid = 0
    # Set a Register.
    register = 1
    # Set the PC.
    pc = 2
    # Set a value at a label.
    value = 3
    # Set a value at an address pointed to by label.
    pointer = 4
    # Sets a region of memory starting at the address pointed to by label.
    array = 5
    # Sets a string starting at the address pointed to by label.
    string = 6
    # Sets console input
    input = 7
    # Simulate a call to a subroutine (following lc-3 calling convention).
    subroutine = 8


class Preconditions(object):
    """Represents the setup environment for a test case for replay."""
    def __init__(self):
        # Dict of EnvironmentFlag to data.
        self._environment_data = dict()
        # List of Tuple (PreconditionFlag, string label, num_params, params)
        self._precondition_data = []

    def addPrecondition(self, type_id, label, data):
        if isinstance(data, int):
            self._precondition_data.append((type_id.value, label, 1, [data]))
        else:
            self._precondition_data.append((type_id.value, label, len(data), data))

    def addEnvironment(self, type_id, data):
        self._environment_data[type_id.value] = data

    def _formBlob(self):
        blob = None

        file = StringIO.StringIO()
        for id, value in self._environment_data:
            file.write(struct.pack('II', id, value))
        for id, label, num_params, params in self._precondition_data:
            file.write(struct.pack('I%ssI%dI' % (len(label)+1, num_params), id, label, num_params, *params))
        blob = file.getvalue()
        file.close()
            
       
        return blob
    
    def encode(self):
        return base64.encodestring(self._formBlob())


class LC3UnitTestCase(unittest.TestCase):

    def setUp(self):
        self.state = pylc3.LC3State()
        self.break_address = None
        self.preconditions = Preconditions()
        self.longMessage = True

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

        self.preconditions.addEnvironment(EnvironmentFlag.memory_strategy, strategy)
        self.preconditions.addEnvironment(EnvironmentFlag.memory_strategy_value, value)

    def load(self, file):
        """Loads an assembly file.

        Args:
            file: String - Full path to the assembly file to load.
        """
        self.state.load(file, True)

    def setTrueTraps(self, setting):
        """Enables or disables True Traps.

        Args:
            setting: True to enable.
        """
        self.state.true_traps = setting

        self.preconditions.addEnvironment(EnvironmentFlag.true_traps, setting)

    def setInterrupts(self, setting):
        """Enables or disables Interrupts.

        Args:
            setting: True to enable.
        """
        self.state.interupts = setting

        self.preconditions.addEnvironment(EnvironmentFlag.interrupts, setting)

    def setPluginsEnabled(self, setting):
        """Enables or disables plugins.

        This should be called before load.

        Args:
            setting: True to enable.
        """
        self.disable_plugins = setting

        self.preconditions.addEnvironment(EnvironmentFlag.plugins, setting)

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
        self.state.pc = value

        self.preconditions.addPrecondition(PreconditionFlag.pc, "PC", value)

    def setValue(self, label, value):
        """Sets a value at a label.

        This exactly performs state.memory[label] = value.

        Args:
            label: String - Label pointing at the address to set.
            value: Integer - Value to write at that address
        """
        addr = self.state.lookup(label)
        self.state.set_memory(addr, value)

        self.preconditions.addPrecondition(PreconditionFlag.value, label, value)
        
    def setPointer(self, label, value):
        """Sets a value at an address pointed to by label

        This exactly performs state.memory[state.memory[label]] = value

        Args:
            label: String - Label pointing at the address which in turn contains the address to set.
            value: Integer - Value to write at that address.
        """
        addr = self.state.lookup(label)
        self.state.set_memory(self.state.get_memory(addr), value)

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
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        for addr, elem in enumerate(arr, start_addr):
            self.state.set_memory(addr, elem)

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
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        for addr, elem in enumerate(text, start_addr):
            self.state.set_memory(addr, ord(elem))
        self.state.set_memory(start_addr + len(text), 0)

        self.preconditions.addPrecondition(PreconditionFlag.string, label, [ord(char) for char in text])
    
    def setConsoleInput(self, input):
        """Sets console input for the test.

        Args:
            input: String - Input to set to the console.
        """
        self.state.input = input

        self.preconditions.addPrecondition(PreconditionFlag.input, "IN", [ord(char) for char in input])

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
        self.state.pc = self.state.lookup(subroutine)
        self.state.r5 = r5
        self.state.r6 = r6 - len(params)
        self.state.r7 = r7
        self.break_address = r7
        self.state.add_breakpoint(r7)
        for addr, param in enumerate(params, self.state.r6):
            self.state.set_memory(addr, param)

        self.preconditions.addEnvironment(EnvironmentFlag.break_address, r7)
        self.preconditions.addPrecondition(PreconditionFlag.subroutine, subroutine, [r5, r6, r7] + params)

    def runCode(self, max_executions=DEFAULT_MAX_EXECUTIONS):
        """Runs the program until it halts or max_executions instructions has been executed.

        Args:
            max_executions: Integer - Maximum number of instructions to execute.
        """
        self.state.run(max_executions)

    def assertHalted(self):
        """Asserts that the LC3 has been halted normally.

        This is achieved by reaching a HALT statement, or code triggered a breakpoint at the break address
        set by a previous call to callSubroutine.
        """
        self.assertTrue(self.state.halted or self.state.pc == self.break_address)

    def assertRegister(self, register_number, value):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
        """
        assert register_number >= 0 and register_number < 8, 'Invalid register number'
        self.assertEqual(self.state.get_register(register_number), value, self._generateReplay())

    def assertPc(self, value):
        """Asserts that the PC is a certain value.

        This exactly checks if state.pc == value

        Args:
            value: Integer - Expected value.
        """
        self.assertEqual(self.state.pc, value, self._generateReplay())

    def assertValue(self, label, value):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
        """
        addr = self.state.lookup(label)
        self.assertEqual(self.state.get_memory(addr), value, self._generateReplay())

    def assertPointer(self, label, value):
        """Asserts that a value at an address pointed to by label is a certain value.

        This exactly checks if state.memory[state.memory[label]] == value

        Args:
            label: String - Label pointing at the address which in turn contains the address to check.
            value: Integer - Expected value.
        """
        addr = self.state.lookup(label)
        self.assertEqual(self.state.get_memory(self.state.get_memory(addr)), value, self._generateReplay())
        
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
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        actual_arr = []
        for addr, _ in enumerate(arr, start_addr):
            actual_arr.append(self.state.get_memory(addr))
        self.assertListEqual(arr, actual_arr, self._generateReplay())

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
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        actual_str = []
        for addr, _ in enumerate(text, start_addr):
            # TODO more error checking, character could be > 255 which throws an error.
            actual_str.append(chr(self.state.get_memory(addr)))
        actual_str.append(self.state.get_memory(start_addr + len(text)))
        expected_str = list(text)
        expected_str.append(0)
        self.assertListEqual(expected_str, actual_str, self._generateReplay())
    
    def assertConsoleOutput(self, output):
        """Asserts that console output is a certain string.

        Args:
            input: String - Expected console output.
        """
        self.assertEqual(self.state.output, output, self._generateReplay())

    def _generateReplay(self):
        return "String to set up this test in complx: %s" % self.preconditions.encode()

