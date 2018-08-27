import pylc3
import unittest

DEFAULT_MAX_EXECUTIONS = 1000000

class LC3UnitTestCase(unittest.TestCase):

    def setUp(self):
        self.state = pylc3.LC3State()
        self.break_address = None

    def load(self, file):
        """Loads an assembly file.

        Args:
            file: String - Full path to the assembly file to load.
        """
        self.state.load(file, True)

    def setValue(self, label, value):
        """Sets a value at a label.

        This exactly performs state.memory[label] = value.

        Args:
            label: String - Label pointing at the address to set.
            value: Integer - Value to write at that address
        """
        addr = self.state.lookup(label)
        self.state.set_memory(addr, value)
        
    def setPointer(self, label, value):
        """Sets a value at an address pointed to by label

        This exactly performs state.memory[state.memory[label]] = value

        Args:
            label: String - Label pointing at the address which in turn contains the address to set.
            value: Integer - Value to write at that address.
        """
        addr = self.state.lookup(label)
        self.state.set_memory(self.state.get_memory(addr), value)
        
    def setArray(self, label, arr):
        """Sets a sequence of values starting at the address pointed to by label.

        This exactly performs:
            state.memory[state.memory[label]] = arr[0]
            state.memory[state.memory[label] + 1] = arr[1]
            [...]
            state.memory[state.memory[label] + len(arr) - 1] = arr[len(arr) - 1]

        Args:
            label: String - Label pointing at the address which in turn contains the first address to set.
            value: Iterable of Integers - Values to write sequentially in memory.
        """ 
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        for addr, elem in enumerate(arr, start_addr):
            self.state.set_memory(addr, elem)

    def setString(self, label, text):
        """Sets a sequence of characters followed by a nul terminator starting at the address pointed to by label.

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
            self.state.set_memory(addr, elem)
        self.state.set_memory(start_addr + len(text), 0)
    
    def setConsoleInput(self, input):
        """Sets console input for the test.

        Args:
            input: String - Input to set to the console.
        """
        self.state.input = input

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

    def assertValue(self, label, value, points=0):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
            points: Integer - Points to assign for this check.
        """
        addr = self.state.lookup(label)
        self.assertEquals(self.state.get_memory(addr), value)

    def assertPointer(self, label, value, points=0):
        """Asserts that a value at an address pointed to by label is a certain value.

        This exactly checks if state.memory[state.memory[label]] == value

        Args:
            label: String - Label pointing at the address which in turn contains the address to check.
            value: Integer - Expected value.
            points: Integer - Points to assign for this check.
        """
        addr = self.state.lookup(label)
        self.assertEquals(self.state.get_memory(self.state.get_memory(addr)), value)
        
    def assertArray(self, label, arr, points=0):
        """Asserts that a sequence of values starting at the address pointed to by label are certain values.

        This exactly checks if:
            state.memory[state.memory[label]] == arr[0]
            state.memory[state.memory[label] + 1] == arr[1]
            [...]
            state.memory[state.memory[label] + len(arr) - 1] == arr[len(arr) - 1]

        Args:
            label: String - Label pointing at the address which in turn contains the first address to start checking.
            arr: Iterable of Integers - Expected values to check sequentially.
            points: Integer - Points to assign for this check.
        """ 
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        actual_arr = []
        for addr, _ in enumerate(arr, start_addr):
            actual_arr.append(self.state.get_memory(addr))
        self.assertListEqual(arr, actual_arr)

    def assertString(self, label, text, points=0):
        """Asserts that sequence of characters followed by a nul terminator starting at the address pointed to by label are certain values.

        This exactly checks if:
            state.memory[state.memory[label]] == text[0]
            state.memory[state.memory[label] + 1] == text[1]
            [...]
            state.memory[state.memory[label] + len(text) - 1] == text[len(text) - 1]
            state.memory[state.memory[label] + len(text)] == 0

        Args:
            label: String - Label pointing at the address which in turn contains the first address to start checking.
            text: String - Expected characters to check sequentially.
            points: Integer - Points to assign for this check.
        """ 
        label_addr = self.state.lookup(label)
        start_addr = self.state.get_memory(label_addr)
        actual_str = []
        for addr, _ in enumerate(text, start_addr):
            actual_str.append(self.state.get_memory(addr))
        actual_str.append(self.state.get_memory(start_addr + len(text)))
        expected_str = list(text)
        expected_str.append(0)
        self.assertListEqual(expected_str, actual_str)
    
    def assertConsoleOutput(self, output, points=0):
        """Asserts that console output is a certain string.

        Args:
            input: String - Expected console output.
            points: Integer - Points to assign for this check.
        """
        self.assertEquals(self.state.output, output)

