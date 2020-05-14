"""Module defining the lc3 unit test case class and utilities.

The main class is LC3UnitTestCase and is intended to be used as a base for a python unit test on student code.

The class keeps track of the preconditions and environment set on the LC3State before the code is ran.
Then when things are checked on the LC3State if any assertion fails a replay string is generated so students,
can emulate the environment in comp and complx.

It is required to only call methods of this class prefixed with set, expect, and assert and not to access
any methods of the underlying LC3State to do assertions.  The assertions provided by the LC3UnitTestCase class
will produce the replay string upon the assertion failing.

The replay strings produced are only populated with data from calling the methods of this class only.  If any
modifications to the LC3State object are done directly and not through the LC3UnitTestCase class will not be
recorded leading to a frustrating debugging experience.

For examples and demos of real live test and assembly code that passes them see
https://github.com/TricksterGuy/pylc3-examples
"""

import base64
import enum
import json
from .. import pylc3
import os
import re
import six
import struct
import unittest


DEFAULT_MAX_EXECUTIONS = 1000000


class LC3InternalAssertion(Exception):
    pass


def _toUShort(value):
    """Converts a value into a 16 bit unsigned short.

    Args:
        value: Integer - Value to convert.

    Returns:
        The value as a 16 bit unsigned short.
    """
    return value & 0xFFFF

def _toShort(value):
    """Converts a value into a 16 bit signed short.

    Args:
        value: Integer - Value to convert.

    Returns:
        The value as a 16 bit signed short.
    """
    value = _toUShort(value)
    return value if value < 32767 else value - 65536


class AssertionType(enum.Enum):
    # Fatal assertions indicate fatal errors that can't be recovered from.
    # Example. File failed to assemble.
    # Also used when an internal assertion fails.
    fatal = 0
    # Hard assertions indicate serious errors. The test will continue, but future assertions will automatically not be tested.
    hard = 1
    # Soft assertions indicate a nonfatal error, such as an incorrect answer.
    # They are logged and will increase error count.
    soft = 2
    # Warnings are also a type of soft assertion that doesn't increase error count.
    warning = 3


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
    # LC3 Version.
    lc3_version = 8

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
    # Simulate a call to a subroutine following lc-3 calling convention.
    subroutine = 24
    # Simulate a call to a trap or a subroutine using pass by register.
    pass_by_regs = 25
    # Directly set an address to a value.
    direct_set = 26
    # Directly set a string in memory.
    direct_string = 27
    # Directly set an array in memory.
    direct_array = 28
    # Set a node in memory.
    # Format (data) -> Leaf node (of linked list)
    # (next, data) -> Linked List
    # (left, right, data) -> Tree Node
    node = 29
    # An arbitrary struct/record/tuple.
    data = 30

    end_of_preconditions = 0xff


class PostconditionFlag(enum.Enum):
    invalid = 0
    # Halted or Returned from Subroutine Flag.
    end_state = 1
    # Checks a register.
    register = 2
    # Checks the PC value.
    pc = 3
    # Checks a value at a label.
    value = 4
    # Check a value at an address pointed to by a label.
    pointer = 5
    # Checks a region starting at the address pointed to by label.
    array = 6
    # Checks a string starting at the address pointed to by label.
    string = 7
    # Checks console output.
    output = 8
    # Check a value at a memory address.
    direct_value = 9
    # Check a string starting at a memory address.
    direct_string = 10
    # Check a sequence of values starting at a memory address.
    direct_array = 11
    # Check a node at a memory address
    node = 12
    # Checks arbitrary data structure at a memory address.
    data = 13
    # Checks return value for subroutine.
    return_value = 14
    # Checks if registers are unchanged.
    registers_unchanged = 15
    # Checks if calling convention was followed.
    calling_convention_followed = 16
    # Checks for subroutine calls.
    subroutine_call = 17
    # Checks for a subroutine/trap using pass by register.
    pass_by_regs = 18

    end_of_postconditions = 0xff


class DataItem(enum.Enum):
    # End of Data.
    end_of_data = 0
    # A single 16 bit integer representing a value, or memory address.
    number = 1
    # A null terminated string which each char being 16 bits.
    string = 2
    # An array of 16 bit integers.
    array = 3
    # An embedded data item.
    data = 0xFF


def tuple_to_data(t):
    """Converts any tuple to a data item."""
    assert isinstance(t, tuple), 'Param is not a tuple, type found: %s' % type(t)
    l = list(t)
    expanded = []
    for item in l:
        if isinstance(item, int):
            expanded.append(DataItem.number)
            expanded.append(item)
        elif isinstance(item, str):
            expanded.append(DataItem.string)
            expanded.append(len(item))
            expanded.append(item)
        elif isinstance(item, list):
            expanded.append(DataItem.array)
            expanded.append(len(item))
            expanded.append(item)
        elif isinstance(item, tuple):
            expanded.append(DataItem.data)
            expanded.extend(tuple_to_data(item))
            expanded.append(DataItem.end_of_data)
    return expanded

def tuple_to_data_spec(t):
    """Converts any tuple to a data item specification."""
    assert isinstance(t, tuple), 'Param is not a tuple, type found: %s' % type(t)
    l = list(t)
    expanded = []
    for item in l:
        if isinstance(item, int):
            expanded.append(DataItem.number)
        elif isinstance(item, str):
            expanded.append(DataItem.string)
            expanded.append(len(item))
        elif isinstance(item, list):
            expanded.append(DataItem.array)
            expanded.append(len(item))
        elif isinstance(item, tuple):
            expanded.append(DataItem.data)
            expanded.extend(tuple_to_data_spec(item))
            expanded.append(DataItem.end_of_data)
    return expanded


def _formDataPreconditions(t):
    assert isinstance(t, tuple), 'Param is not a tuple, type found: %s' % type(t)
    l = list(t)
    expanded = []
    for item in l:
        if isinstance(item, int):
            expanded.append(DataItem.number.value)
            expanded.append(item)
        elif isinstance(item, str):
            expanded.append(DataItem.string.value)
            expanded.append(len(item))
            expanded.extend([ord(c) for c in item])
        elif isinstance(item, list):
            expanded.append(DataItem.array.value)
            expanded.append(len(item))
            expanded.extend(item)
        elif isinstance(item, tuple):
            expanded.append(DataItem.data.value)
            expanded.extend(_formDataPreconditions(item))
            expanded.append(DataItem.end_of_data.value)
    return expanded


def _cstringifyData(data):
    return [six.u(d) + u'\0' if isinstance(d, str) else d for d in list(data)]


class SubroutineCallMode(enum.Enum):
    """Internal Enum to ensure subroutine call conventions are consistent."""
    lc3_calling_convention = 0
    pass_by_register = 1


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

    def describe(self, include_environment=True):
        """Returns a string describing the preconditions."""
        return ""

    def encode(self):
        """Returns a base64 encoded string with the data."""
        return base64.b64encode(self._formBlob())


class Postconditions(object):
    """Represents the things checked at the end of the test

    Generates base64 string to verify the test in complx."""
    def __init__(self):
        # List of (PostconditionFlag, char type, <int,string> val, num_params, params)
        # List of (PostconditionFlag, 0, char bool)
        # List of (PostconditionFlag, 1, int val, num_params, params)
        # List of (PostconditionFlag, 2, string label, num_params, params)
        self._data = []

    def add(self, type_id, value, data=None):
        assert type_id.value <= 18, 'Internal error, Unknown Postcondition Flag.'
        if data is None:
            data = []
        if value is True or value is False:
            self._data.append((type_id.value, 0, 0 if value is False else 1, 0, None))
        elif isinstance(value, int):
            self._data.append((type_id.value, 1, value, len(data), data))
        elif isinstance(value, str):
            self._data.append((type_id.value, 2, value, len(data), data))

    def _formBlob(self):
        file = six.BytesIO()

        self._data.sort(key = lambda x: x[0])
        for id, type, value, num_params, params in self._data:
            if type == 0:
                file.write(struct.pack('=B', id))
                file.write(struct.pack('=B', type))
                file.write(struct.pack('=I', value))
            elif type == 1:
                file.write(struct.pack('=B', id))
                file.write(struct.pack('=B', type))
                file.write(struct.pack('=I', value))
                file.write(struct.pack('=I', num_params))
                params = [param & 0xFFFF for param in params]
                file.write(struct.pack('=%dH' % num_params, *params))
            elif type == 2:
                value = six.b(value)
                file.write(struct.pack('=B', id))
                file.write(struct.pack('=B', type))
                file.write(struct.pack('=I', len(value)))
                file.write(struct.pack('=%ds' % len(value), value))
                file.write(struct.pack('=I', num_params))
                params = [param & 0xFFFF for param in params]
                file.write(struct.pack('=%dH' % num_params, *params))

        file.write(struct.pack('=B', 0xff))

        blob = file.getvalue()
        file.close()

        return blob

    def describe(self, include_environment=True):
        """Returns a string describing the postconditions."""
        return ""

    def encode(self):
        """Returns a base64 encoded string with the data."""
        return base64.b64encode(self._formBlob())


class LC3UnitTestCase(unittest.TestCase):
    """LC3UnitTestCase class eases testing of LC3 code from within python.

    The methods init, setXXX, expectXXX and callSubroutine will automatically log the
    state set as preconditions. Later when the test runs and a assertion fails the state
    logged by the preconditions will output a base64 encoded string to ease setting up
    the test case in complx and comp.

    It is important to not touch the state attribute and set things on it since these
    changes are not logged automatically and will result in the test environment being
    different from here and running in complx and comp.

    It is also important not use unittest.TestCase assertion methods since these do not
    produce the replay string upon failure.
    """

    failed_assertions_per_test = {}
    passed_assertions_per_test = {}

    @classmethod
    def setUpClass(cls):
        pass

    @classmethod
    def tearDownClass(cls):
        cls.form_json_test_report()

    @classmethod
    def form_json_test_report(cls):
        name = cls.__name__
        filename = name + '.json'
        tests = set()
        tests.update(cls.failed_assertions_per_test.keys())
        tests.update(cls.passed_assertions_per_test.keys())
        json_obj = {name: []}
        for test_name in tests:
            for check_name in cls.passed_assertions_per_test.get(test_name, []):
                json_obj[name].append({'display-name': '%s/%s' % (test_name, check_name), 'passed': True})
            for check_name, msg in cls.failed_assertions_per_test.get(test_name, []):
                json_obj[name].append({'display-name': '%s/%s' % (test_name, check_name), 'passed': False, 'message': msg})
        with open(filename, 'w') as f:
            json.dump(json_obj, f)

    def setUp(self):
        self.state = pylc3.LC3State(testing_mode=True)
        self.break_address = None
        self.preconditions = Preconditions()
        self.postconditions = Postconditions()
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
        # Subroutine specifications. Dict of String to List of interested registers. Only for SubroutineCallMode.pass_by_register.
        self._subroutine_call_mode = None
        self.subroutine_specifications = dict()
        self.passed_assertions = []
        self.failed_assertions = []
        self._hard_failed = False
        self.display_name = None
        self.warnings = []

    def tearDown(self):
        def form_failure_message():
            return 'The test failed due to the following checks that failed shown below:\n----\n%s%s' % ('\n'.join(['name: %s Reason: %s' % (name, msg) for name, msg in self.failed_assertions]), self.replay_msg)
        if self.failed_assertions:
            self.fail(form_failure_message())
        assert self.display_name is not None, 'Internal error self.display_name needs to be set per test case.'
        self.failed_assertions_per_test[self.display_name] = self.failed_assertions
        self.passed_assertions_per_test[self.display_name] = self.passed_assertions

    def init(self, strategy, value):
        """Initializes LC3 state memory.

        Args:
            strategy: a MemoryFillStrategy telling how memory should be initialized.
            value: Parameter for Memory Fill Strategy, either a fill value or random seed.
        """
        if strategy == pylc3.MemoryFillStrategy.fill_with_value:
            self.state.init(False, _toUShort(value))
        elif strategy == pylc3.MemoryFillStrategy.single_random_value_fill:
            self.state.seed(value)
            self.state.init(False, self.state.random())
        elif strategy == pylc3.MemoryFillStrategy.completely_random:
            self.state.seed(value)
            self.state.init(True)

        self.preconditions.addEnvironment(PreconditionFlag.memory_strategy, strategy)
        self.preconditions.addEnvironment(PreconditionFlag.memory_strategy_value, value)

    def _internalAssert(self, name, cond, msg, level=AssertionType.soft, internal=False):
        name = name if not internal else 'internal'
        if self._hard_failed:
            if name != 'internal':
                self.failed_assertions.append((name, 'Not checked due to previous failures.'))
            return

        if cond:
            if not internal:
                self.passed_assertions.append(name)
        else:
            if level == AssertionType.fatal:
                raise LC3InternalAssertion(msg)
            self.failed_assertions.append((name, msg))
            self._hard_failed = self._hard_failed or level == AssertionType.hard

    def loadAsmFile(self, file, lc3_version=1):
        """Loads an assembly file.

        Will assert if the file failed to assemble.
        Note this function uses my own assembler.

        Args:
            file: String - Full path to the assembly file to load.
            lc3_version: Integer - which version of the LC-3 to use.
        """
        status = self.state.load(file, disable_plugins=not self.enable_plugins, process_debug_comments=False)
        self._internalAssert('assembles', not status, 'Unable to load file %s\nReason: %s' % (file, status), AssertionType.fatal)
        self._internalAssert('version', self.state.lc3_version == lc3_version, 'File uses different lc3 version than grader version: %d expected: %d\n' % (self.state.lc3_version, lc3_version), AssertionType.hard)
        self.setLC3Version(lc3_version)

    def loadPattObjAndSymFile(self, obj_file, sym_file):
        """Loads an assembled object and symbol table file.

        Note this function is only to be used on files assembled with the Patt/Patel's lc3tools.
        It is preferred that loadAsmFile be used.

        Args:
            obj_file: String - Full path to the obj file to load.
            sym_file: String - Full path to the sym file to load.
        """
        with open(obj_file, 'rb') as f:
            loc, = struct.unpack('>H', f.read(2))
            bytes = f.read(2)
            while bytes:
                data, = struct.unpack('>H', bytes)
                self._writeMem(loc, data)
                bytes = f.read(2)
                loc += 1

        with open(sym_file) as f:
            line = f.readline()
            while line:
                m = re.search('//\t(\w+)\s*([0-9A-Fa-f]{4})', line)
                if m:
                    symbol, location = m.group(1), int(m.group(2), 16)
                    self.state.add_symbol(symbol, location)
                line = f.readline()

    def _lookup(self, label):
        """Gets the address of a label by looking it up in the symbol table.

        This will assert if the label is not found.

        Args:
            label: String - Label to find.

        Returns:
            The address where the label is located.
        """
        addr = self.state.lookup(label)
        self._internalAssert('lookup %s' % label, addr != -1, "Label %s was not found in the assembly code." % label, AssertionType.fatal, internal=True)
        return _toUShort(addr)

    def _reverse_lookup(self, address):
        """Gets the label associated with address.

        This will assert if no label found at address.

        Args:
            address: Integer - Memory address to search for label.

        Returns:
            The label associated with the address.
        """
        label = self.state.reverse_lookup(_toUShort(address))
        self._internalAssert('reverse_lookup %x' % address, label, "Address %x is not associated with a label." % address, AssertionType.fatal, internal=True)
        return label

    def _readMem(self, addr, unsigned=False):
        """Reads a value at a memory address.

        Args:
            addr: Integer - Address to load from.
            unsigned: Boolean - Convert the value to be an unsigned short

        Returns:
            The value stored at that address.
        """
        value = self.state.get_memory(_toUShort(addr))
        return value if not unsigned else _toUShort(value)

    def _writeMem(self, addr, value):
        """Write a value to a memory address.

        Args:
            addr: Integer - Address to write to.
            value: Integer - Value to write.
        """
        self.state.set_memory(_toUShort(addr), value)

    def _readReg(self, reg, unsigned=False):
        """Reads a value in a register.

        Args:
            addr: Integer - Address to load from.
            unsigned: Boolean - Convert the value to be an unsigned short

        Returns:
            The value stored at that address.
        """
        self._internalAssert('readReg %x' % reg, reg >= 0 and reg < 8, 'Invalid register number %d' % reg, AssertionType.fatal, internal=True)
        value = self.state.get_register(reg)
        return value if not unsigned else _toUShort(value)

    def _writeReg(self, reg, value):
        """Write a value to a register.

        Args:
            addr: Integer - Address to write to.
            value: Integer - Value to write.
        """
        self._internalAssert('writeReg %x' % reg, reg >= 0 and reg < 8, 'Invalid register number %d' % reg, AssertionType.fatal, internal=True)
        self.state.set_register(reg, value)

    def _writeData(self, address, data):
        """Writes arbitrary data.
        """
        def _writeDataInternal(address, dataitems, index=0):
            i = index
            while i < len(dataitems):
                t = dataitems[i]
                if t == DataItem.number:
                    self._writeMem(address, dataitems[i + 1])
                    address += 1
                    i += 2
                elif t == DataItem.string:
                    length = dataitems[i + 1]
                    string = dataitems[i + 2]
                    for addr, elem in enumerate(string, address):
                        self._writeMem(addr, ord(elem))
                    self._writeMem(address + length, 0)
                    address += length + 1
                    i += 3
                elif t == DataItem.array:
                    length = dataitems[i + 1]
                    array = dataitems[i + 2]
                    for addr, elem in enumerate(array, address):
                        self._writeMem(addr, elem)
                    address += length
                    i += 3
                elif t == DataItem.data:
                    address, i = _writeDataInternal(address, dataitems, index=i+1)
                    self._internalAssert('writeData', address != -1, 'No end of data segment found.', AssertionType.fatal, internal=True)
                    i += 1
                elif t == DataItem.end_of_data:
                    return address, i
            return -1, -1
        a, b = _writeDataInternal(address, tuple_to_data(data))
        self._internalAssert('writeData', a == -1, 'Extra end of data segment found.', AssertionType.fatal, internal=True)

    def _readData(self, address, data):
        def _readDataInternal(address, dataspec, index=0):
            i = index
            data = []
            while i < len(dataspec):
                t = dataspec[i]
                if t == DataItem.number:
                    data.append(self._readMem(address))
                    address += 1
                    i += 1
                elif t == DataItem.string:
                    length = dataspec[i + 1]
                    string = []
                    for addr in range(address, address + length + 1):
                        string.append(six.unichr(self._readMem(addr, unsigned=True)))
                    data.append(''.join(string))
                    address += length + 1
                    i += 2
                elif t == DataItem.array:
                    length = dataspec[i + 1]
                    array = []
                    for addr in range(address, address + length):
                        array.append(self._readMem(addr))
                    data.append(array)
                    address += length
                    i += 2
                elif t == DataItem.data:
                    inner_data, address, i = _readDataInternal(address, dataspec, index=i+1)
                    data.append(inner_data)
                    self._internalAssert('readData', address != -1, 'No end of data segment found.', AssertionType.fatal, internal=True)
                    i += 1
                elif t == DataItem.end_of_data:
                    return tuple(data), address, i
            return tuple(data), -1, -1
        result, a, b = _readDataInternal(address, tuple_to_data_spec(data))
        self._internalAssert('readData', a == -1, 'Extra end of data segment found.', AssertionType.fatal, internal=True)
        return result

    def setLC3Version(self, version):
        """Sets the LC-3 Version.

        This function should be used to change the LC3 behavior.

        0 - Default Version
        1 - 2019 Revision (LEA no longer sets CC / Trap pushes PSR,PC on stack, RTI returns from trap or interrupt)
        """
        self._internalAssert('setLC3Version', version >= 0 and version <= 1, 'Invalid LC-3 Version Given. Valid values are 0 or 1.', AssertionType.fatal, internal=True)
        self.state.lc3_version = version
        self.preconditions.addEnvironment(PreconditionFlag.lc3_version, version)

    def setTrueTraps(self, setting):
        """Enables or disables True Traps.

        This should be called before loadXXX.

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
        self.state.interrupts = setting
        self.state.enable_keyboard_interrupt()

        self.preconditions.addEnvironment(PreconditionFlag.interrupts, setting)

    def setKeyboardInterruptDelay(self, delay):
        """Sets speed of which keyboard interrupts are generated.

        Args:
            delay: The delay in number of instructions.
        """
        self.state.keyboard_interrupt_delay = delay

    def setPluginsEnabled(self, setting):
        """Enables or disables plugins.

        This should be called before loadXXX.

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
        self._internalAssert('setRegister', register_number >= 0 and register_number < 8, 'Invalid register number given %d' % register_number, AssertionType.fatal, internal=True)
        self.state.set_register(register_number, value)

        self.preconditions.addPrecondition(PreconditionFlag.register, str(register_number), value)

    def setPc(self, value):
        """Sets the PC.

        This exactly performs state.pc = value

        Args:
            value: Integer - Value to write to that register.
        """
        self.state.pc = _toUShort(value)

        self.preconditions.addPrecondition(PreconditionFlag.pc, "", value)

    def setValue(self, label, value):
        """Sets a value at a label.

        This exactly performs state.memory[label] = value.

        Args:
            label: String - Label pointing at the address to set.
            value: Integer - Value to write at that address
        """
        self._writeMem(self._lookup(label), value)

        self.preconditions.addPrecondition(PreconditionFlag.value, label, value)

    def setPointer(self, label, value):
        """Sets a value at an address pointed to by label

        This exactly performs state.memory[state.memory[label]] = value

        Args:
            label: String - Label pointing at the address which in turn contains the address to set.
            value: Integer - Value to write at that address.
        """
        self._writeMem(self._readMem(self._lookup(label)), value)

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
            arr: Iterable of Shorts - Values to write sequentially in memory.
        """
        start_addr = self._readMem(self._lookup(label))
        for addr, elem in enumerate(arr, start_addr):
            self._writeMem(addr, elem)

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
        start_addr = self._readMem(self._lookup(label))
        for addr, elem in enumerate(text, start_addr):
            self._writeMem(addr, ord(elem))
        self._writeMem(start_addr + len(text), 0)

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

        For lc-3 calling convention pass in a list of params.
        For pass by register pass in a dict of register number to params.

        Args:
            subroutine: String - Label pointing at the start of the subroutine.
            params: List of Integer - (LC-3 Calling Convention Style) Parameters to the subroutine
                    Dict of Integer to Integer - (Pass by Register Style) Map of Register number to value. (Do not set R5-R7 in this dictionary).
            r5: Integer - Dummy value to store in r5 (frame pointer) for the test.
            r6: Integer - Dummy value to store in r6 (stack pointer) for the test.
            r7: Integer - Dummy value to store in r7 (return address) for the test.
        """
        self.state.pc = self._lookup(subroutine)
        if r5:
            self.state.r5 = r5
        self.state.r7 = r7
        self.break_address = r7
        self.state.add_breakpoint(r7)

        if isinstance(params, list):
            self.state.r6 = r6 - len(params)
            for addr, param in enumerate(params, self.state.r6):
                self._writeMem(addr, param)
            self.state.add_subroutine_info(subroutine, len(params))
            self._internalAssert('callSubroutine', self._subroutine_call_mode is None or self._subroutine_call_mode == SubroutineCallMode.lc3_calling_convention, "Can't mix subroutine call styles in the same test.", AssertionType.fatal, internal=True)
            self._subroutine_call_mode = SubroutineCallMode.lc3_calling_convention
            self.preconditions.addPrecondition(PreconditionFlag.subroutine, subroutine, [r5, r6, r7] + params)
        elif isinstance(params, dict):
            self.state.r6 = r6
            self._internalAssert('callSubroutine', 5 not in params, 'R5 present in params list in pass by register mode, use kwarg r5 instead.', AssertionType.fatal, internal=True)
            self._internalAssert('callSubroutine', 6 not in params, 'R6 present in params list in pass by register mode, use kwarg r6 instead.', AssertionType.fatal, internal=True)
            self._internalAssert('callSubroutine', 7 not in params, 'R7 present in params list in pass by register mode, use kwarg r7 instead.', AssertionType.fatal, internal=True)
            for register, value in params.items():
                self._writeReg(register, value)
            self.subroutine_specifications[subroutine] = params.keys()
            if r5:
                params[5] = r5
            params[6] = r6
            params[7] = r7
            self._internalAssert('callSubroutine', self._subroutine_call_mode is None or self._subroutine_call_mode == SubroutineCallMode.pass_by_register, "Can't mix subroutine call styles in the same test.", AssertionType.fatal, internal=True)
            self._subroutine_call_mode = SubroutineCallMode.pass_by_register
            data = list(six.moves.reduce(lambda a, b: a + b, params.items()))
            self.preconditions.addPrecondition(PreconditionFlag.pass_by_regs, subroutine, data)
        self.preconditions.addEnvironment(PreconditionFlag.break_address, r7)

    # TODO Add callTrap, support both lc-3 and the 2019 revision.

    def expectSubroutineCall(self, subroutine, params, optional=False):
        """Expects that a subroutine call was made with parameters.

        This function verifies only the first level of subroutine calls *not* all of them if the subroutine is recursive or calls other subroutines that call subroutines.
        The methodology of this is to verify the subroutine through inductive reasoning.

        That is, if I have this factorial pseudocode

        def fact(n):
          if (n <= 1)
            return 1
          return n * fact(n-1)

        Then for a subroutine call n = 4 I would expect fact(3) to be called.

        Note that passing in the same (subroutine, params, optional) triplet is not supported. Nor is
        passing in the same (subroutine, params) with both optional set and not set.

        Args:
            subroutine: String - Label pointing at the start of the subroutine.
            params: List of Integer - (LC-3 Calling Convention Style) Parameters to the subroutine
                    Dict of Integer to Integer - (Pass by Register Style) Map of Register number to value.
            optional: Mark this as a optional subroutine call, if this subroutine call is found it is not checked.
        """
        inner_value = None
        if isinstance(params, list):
            self._addSubroutineInfo(subroutine, len(params))
            inner_value = tuple(params)
            self._internalAssert('expectSubroutineCall', self._subroutine_call_mode is None or self._subroutine_call_mode == SubroutineCallMode.lc3_calling_convention, "Can't mix subroutine call styles in the same test.", AssertionType.fatal, internal=True)
            self._subroutine_call_mode = SubroutineCallMode.lc3_calling_convention
        elif isinstance(params, dict):
            if subroutine not in self.subroutine_specifications:
                self.subroutine_specifications[subroutine] = params.keys()
            inner_value = tuple([(k, v) for k, v in params.items()])
            self._internalAssert('expectSubroutineCall', self._subroutine_call_mode is None or self._subroutine_call_mode == SubroutineCallMode.pass_by_register, "Can't mix subroutine call styles in the same test.", AssertionType.fatal, internal=True)
            self._subroutine_call_mode = SubroutineCallMode.pass_by_register
        else:
            self._internalAssert('expectSubroutineCall', False, 'Invalid Type for params. Expected (list or dict) Got: %s.' % type(params), AssertionType.fatal, internal=True)

        set_to_add = self.expected_subroutines
        value = (subroutine, inner_value)
        if optional:
            set_to_add = self.optional_subroutines

        self._internalAssert('expectSubroutineCall', value not in set_to_add, 'Duplicate subroutine found %s, multiple copies of [subroutine, params] is not supported.' % str(value), AssertionType.fatal, internal=True)
        set_to_add.add(value)

        self._internalAssert('expectSubroutineCall', not (value in self.expected_subroutines and value in self.optional_subroutines), 'Subroutine %s found in both expected and optional subroutine calls.' % str(value), AssertionType.fatal, internal=True)

        if isinstance(params, list):
            self.postconditions.add(PostconditionFlag.subroutine_call, subroutine, params)
        else:
            data = list(six.moves.reduce(lambda a, b: a + b, params.items()))
            self.postconditions.add(PostconditionFlag.pass_by_regs, subroutine, data)

    def expectTrapCall(self, vector, params, optional=False):
        """Expects that a trap was made with parameters in registers.

        Note that passing in the same (vector, params, optional) triplet is not supported. Nor is
        passing in the same (vector, params) with both optional set and not set.

        This function will only check the first level trap calls made not all of them if the trap is recursive, or the trap called calls other traps.

        It is undefined to call this with the same trap vector, but a different set of register parameters.

        Do not call this function with vector = x25. It is assumed that HALT will be called and is checked for via assertHalted.

        Args:
            vector: Integer - Label pointing at the start of the subroutine.
            params: Dict of Integer to Integer - Map of Register number to value.
            optional: Mark this as a optional trap call, if this trap call is found it is not checked.
        """
        self._internalAssert('expectTrapCall', vector != 0x25, 'Method expectTrapCall called with vector=0x25, use assertHalted instead.', AssertionType.fatal, internal=True)

        if vector not in self.trap_specifications:
            self.trap_specifications[vector] = params.keys()

        set_to_add = self.expected_traps
        value = (vector, tuple([(k, v) for k, v in params.items()]))

        if optional:
            set_to_add = self.optional_traps

        self._internalAssert('expectTrapCall', value not in set_to_add, 'Duplicate trap found %s, multiple copies of [subroutine, params] is not supported.' % str(value), AssertionType.fatal, internal=True)

        set_to_add.add(value)

        self._internalAssert('expectTrapCall', not (value in self.expected_subroutines and value in self.optional_subroutines), 'Trap %s found in both expected and optional subroutine calls.' % str(value), AssertionType.fatal, internal=True)

        data = list(reduce(lambda a, b: a + b, params.items()))
        self.postconditions.add(PostconditionFlag.pass_by_regs, 'x%02x' % vector, data)

    def fillValue(self, address, value):
        """Fills a value at an address.

        This exactly performs state.memory[address] = value.

        *NOTICE* It is an error to call this function on an labeled address, the reasoning for this is that there is
        the potential to overwrite user data past the end of the string in the original template if the text parameter
        is larger. Use setString for that use case since it avoids that problem by having the string be at a address
        pointed to by a label.

        Args:
            address: Short - Address to set.
            value: Integer - Value to write at that address
        """
        label = self.state.reverse_lookup(address)
        self._internalAssert('fillValue', not label, 'fillValue is not to be used on a labeled address, use setValue instead. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)

        self._writeMem(address, value)

        self.preconditions.addPrecondition(PreconditionFlag.direct_set, '%04x' % address, value)

    def fillString(self, address, text):
        """Sets a sequence of characters followed by a NUL terminator starting at the address given.

        This exactly performs:
            state.memory[address] = text[0]
            state.memory[address + 1] = text[1]
            [...]
            state.memory[address + len(text) - 1] = text[len(text) - 1]
            state.memory[address + len(text)] = 0

        *NOTICE* It is an error to call this function on an labeled address, the reasoning for this is that there is
        the potential to overwrite user data past the end of the string in the original template if the text parameter
        is larger. Use setString for that use case since it avoids that problem by having the string be at a address
        pointed to by a label.

        This function is intended to dump a string out in memory to pass the address as an argument to a subroutine.

        Args:
            address: Short - Address to set.
            value: String - String to write in memory.

        Raises:
            ValueError if the address has a label (use setString instead).
        """
        label = self.state.reverse_lookup(address)
        self._internalAssert('fillString', not label, 'fillString is not to be used on a labeled address, use setString instead. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)

        for addr, elem in enumerate(text, address):
            self._writeMem(addr, ord(elem))
        self._writeMem(address + len(text), 0)

        self.preconditions.addPrecondition(PreconditionFlag.direct_string, '%04x' % address, [ord(c) for c in text])

    def fillArray(self, address, arr):
        """Sets a sequence of values starting at the address given.

        This exactly performs:
            state.memory[address] = arr[0]
            state.memory[address + 1] = arr[1]
            [...]
            state.memory[address + len(arr) - 1] = arr[len(arr) - 1]

        *NOTICE* It is an error to call this function on an labeled address, the reasoning for this is that there is
        the potential to overwrite user data past the end of the string in the original template if the parameter
        is larger. Use setArray for that use case since it avoids that problem by having the string be at a address
        pointed to by a label.

        This function is intended to dump an array out in memory to pass the address as an argument to a subroutine.

        Args:
            address: Short - Address to set.
            arr: Iterable of Shorts - Values to write sequentially in memory.

        Raises:
            ValueError if the address has a label (use setArray instead).
        """
        label = self.state.reverse_lookup(address)
        self._internalAssert('fillArray', not label, 'fillArray is not to be used on a labeled address, use setArray instead. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)

        for addr, elem in enumerate(arr, address):
            self._writeMem(addr, elem)

        self.preconditions.addPrecondition(PreconditionFlag.direct_array, '%04x' % address, arr)

    def fillNode(self, address, next, data):
        """Sets a node in memory at the address given.

        This can fill nodes of arbitrary type: linked list nodes or n-ary tree nodes
        This exactly performs:
            state.memory[address] = next_info[0]
            state.memory[address + 1] = next_info[1]
            ...
            state.memory[address + n] = data

        *NOTICE* It is an error to call this function on an labeled address.

        This function is intended to dump an node out in memory.

        It is *greatly* preferred to use this function over fillValue/fillArray to improve tooling in the future.

        Args:
            address: Short - Address to set.
            next_info: 1) None to indicate leaf node or last node. (Note only one 0 is written in this case).
                       2) Short - Next address.
                       3) Iterable of Shorts/None - Next addresses, None to indicate no next address.
            data: Tuple - Node data. (See fillData for a more detailed description on the tuple's contents.)
        """
        label = self.state.reverse_lookup(address)

        self._internalAssert('fillNode', not label, 'fillNode is not to be used on a labeled address. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)

        node_data = []
        size_next = 1
        if not next:
            self._writeMem(address, 0)
            node_data = ([0], data)
        elif isinstance(next, int):
            self._writeMem(address, next)
            node_data = ([next], data)
        else:
            next = [elem if elem else 0 for elem in next]
            for addr, elem in enumerate(next, address):
                self._writeMem(addr, 0 if elem is None else elem)
            size_next = len(next)
            node_data = (next, data)

        self._writeData(address + size_next, data)

        self.preconditions.addPrecondition(PreconditionFlag.node, '%04x' % address, _formDataPreconditions(node_data))

    def fillData(self, address, data):
        """Sets an arbitrary data structure/struct/record at the address given.

        Argument data should contain a tuple with any of the following data types
            An Integer
            A List of Integers
            A String
            Another Tuple following the above structure

        This exactly performs:

        *NOTICE* It is an error to call this function on an labeled address.

        This function is intended to dump a set of related data out in memory.

        It is *greatly* preferred to use this function over fillValue/fillString/fillArray to improve tooling in the future.

        Args:
            address: Short - Address to set.
            data: Tuple - Node data.
        """
        label = self.state.reverse_lookup(address)
        self._internalAssert('fillData', not label, 'fillData is not to be used on a labeled address. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)
        self._writeData(address, data)

        self.preconditions.addPrecondition(PreconditionFlag.data, '%04x' % address, _formDataPreconditions(data))

    def _addSubroutineInfo(self, subroutine, num_params):
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

    def _assertShortEqual(self, expected, actual, name, msg, level=AssertionType.soft, internal=False):
        """Helper to assert if two 16 bit values are equal."""
        self._internalAssertEqual(_toUShort(expected), _toUShort(actual), name, msg=msg, level=level, internal=internal)

    def _assertEqual(self, expected, actual, name, msg, level=AssertionType.soft, internal=False):
        self._internalAssertEqual(expected, actual, name, msg=msg, level=level, internal=internal)

    def _internalAssertEqual(self, expected, actual, name, msg, level=AssertionType.soft, internal=False):
        self._internalAssert(name, expected == actual, msg=msg, level=level, internal=internal)

    def assertReturned(self, level=AssertionType.hard):
        """Assert that the code successfully returned from a subroutine call.

        This is achieved by hitting the breakpoint set at r7 when callSubroutine was called.
        This function should only be used if callSubroutine/callTrap was called.
        """
        self._internalAssert('assertReturned', self.break_address is not None, "self.assertReturned() should only be used when a previous call to self.callSubroutine() was made.", AssertionType.fatal, internal=True)
        instruction = self.state.disassemble(self.state.pc, 1)
        malformed = '*' in instruction
        failure_msg = 'Code did not return from subroutine correctly.\n'
        if malformed:
            failure_msg += 'This was due to executing data which was interpreted to a malformed instruction.\n'
        elif instruction == 'HALT':
            failure_msg += 'This was due to the code reaching a HALT statement unexpectedly.\n'
        else:
            failure_msg += 'This was probably due to an infinite loop in the code.\n'
        failure_msg += 'This may indicate that your handling of the stack is incorrect or that R7 was clobbered.\n'
        failure_msg += 'PC: x%04x\nExecuted: %d instructions\nInstruction last on: %s\n' % (self.state.pc, self.state.executions, instruction)
        self._assertShortEqual(self.state.pc, self.break_address, 'returned', failure_msg, level=level)
        self.postconditions.add(PostconditionFlag.end_state, False)

    def assertHalted(self, level=AssertionType.hard):
        """Asserts that the LC3 has been halted normally.

        This is achieved by reaching a HALT statement or if true_traps is set and the MCR register's 15 bit is set.
        This function should not be called if callSubroutine/callTrap was called instead use assertReturned.
        """
        assert self.break_address is None, "self.callSubroutine was previously used in this test, a call to assertReturned() should be made instead of assertHalted()."
        # Don't use self.state.halted here as that is set if a Malformed instruction is executed.
        instruction = self.state.disassemble(self.state.pc, 1)
        malformed = '*' in instruction
        failure_msg = 'Code did not halt normally.\n'
        failure_msg += 'This was due to executing data which was interpreted to a malformed instruction.\n' if malformed else ''
        failure_msg += 'This was probably due to an infinite loop in the code.\n' if not malformed else ''
        failure_msg += 'PC: x%04x\nExecuted: %d instructions\nInstruction last on: %s\n' % (self.state.pc, self.state.executions, instruction)
        if self.true_traps:
            self._assertEqual(state.get_memory(0xFFFE) >> 15 & 1, 0, 'halted', failure_msg, level=level)
        else:
            self._assertShortEqual(self.state.get_memory(self.state.pc), 0xF025, 'halted', failure_msg, level=level)
        self.postconditions.add(PostconditionFlag.end_state, True)

    def assertNoWarnings(self, level=AssertionType.warning):
        """Asserts that no warnings were reported during execution of the code."""
        self._internalAssert('warnings', not self.state.warnings, 'Code generated warnings shown below:\n----\n%s' % self.state.warnings, level=level)

    def assertRegister(self, register_number, value, level=AssertionType.soft):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
        """
        self._internalAssert('assertRegister %x' % register_number, register_number >= 0 and register_number < 8, 'Invalid register number %d' % register_number, AssertionType.fatal, internal=True)
        actual = self._readReg(register_number)
        expected = _toShort(value)
        self._assertShortEqual(expected, actual, 'R%d' % register_number, 'R%d was expected to be (%d x%04x) but code produced (%d x%04x)\n' % (register_number, expected, _toUShort(expected), actual, _toUShort(actual)), level=level)
        self.postconditions.add(PostconditionFlag.register, register_number, [value])

    def assertPc(self, value, level=AssertionType.soft):
        """Asserts that the PC is a certain value.

        This exactly checks if state.pc == value

        Args:
            value: Integer - Expected value.
        """
        actual = self.state.pc
        expected = _toUShort(value)
        self._assertShortEqual(expected, actual, 'PC', 'PC was expected to be x%04x but code produced x%04x\n' % (expected, actual), level)
        self.postconditions.add(PostconditionFlag.pc, 0, [value])

    def assertValue(self, label, value, level=AssertionType.soft):
        """Asserts that a value at a label is a certain value.

        This exactly checks if state.memory[label] == value

        Args:
            label: String - Label pointing at the address to check.
            value: Integer - Expected value.
        """
        actual = self._readMem(self._lookup(label))
        expected = _toShort(value)
        self._assertShortEqual(expected, actual, 'value: %s' % label, 'MEM[%s] was expected to be (%d x%04x) but code produced (%d x%04x)\n' % (label, expected, _toUShort(expected), actual, _toUShort(actual)), level=level)
        self.postconditions.add(PostconditionFlag.value, label, [value])

    def assertPointer(self, label, value, level=AssertionType.soft):
        """Asserts that a value at an address pointed to by label is a certain value.

        This exactly checks if state.memory[state.memory[label]] == value

        Args:
            label: String - Label pointing at the address which in turn contains the address to check.
            value: Integer - Expected value.
        """
        actual = self._readMem(self._readMem(self._lookup(label)))
        expected = _toShort(value)
        self._assertShortEqual(expected, actual, 'pointer: %s' % label, 'MEM[MEM[%s]] was expected to be (%d x%04x) but code produced (%d x%04x)\n' % (label, expected, _toUShort(expected), actual, _toUShort(actual)), level=level)
        self.postconditions.add(PostconditionFlag.pointer, label, [value])

    def assertArray(self, label, arr, level=AssertionType.soft):
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
        start_addr = self._readMem(self._lookup(label))
        actual_arr = []
        for addr, _ in enumerate(arr, start_addr):
            actual_arr.append(self._readMem(addr))
        self._assertEqual(arr, actual_arr, 'array: %s' % label, 'Sequence of values starting at MEM[%s] was expected to be %s but code produced %s\n' % (label, arr, actual_arr), level=level)
        self.postconditions.add(PostconditionFlag.array, label, arr)

    def assertString(self, label, text, level=AssertionType.soft):
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
        start_addr = self._readMem(self._lookup(label))
        actual_str = []
        for addr, _ in enumerate(text, start_addr):
            actual_str.append(six.unichr(self._readMem(addr, unsigned=True)))
        actual_str.append(six.unichr(self._readMem(start_addr + len(text), unsigned=True)))
        expected_str = list(six.u(text))
        expected_str.append(u'\0')
        self._assertEqual(expected_str, actual_str, 'string: %s' % label, 'String of characters starting at MEM[%s] was expected to be %s but code produced %s\n' % (label, repr(''.join(expected_str)), repr(''.join(actual_str))), level=level)
        self.postconditions.add(PostconditionFlag.string, label, [ord(char) for char in text])

    def assertConsoleOutput(self, output, level=AssertionType.soft):
        """Asserts that console output is a certain string.

        Args:
            input: String - Expected console output.
        """
        expected = output
        actual = self.state.output
        self._assertEqual(expected, actual, 'console output', 'Console output was expected to be %s but code produced %s\n' % (repr(expected), repr(actual)), level=level)
        self.postconditions.add(PostconditionFlag.output, 0, [ord(char) for char in output])

    def assertValueAt(self, address, value, level=AssertionType.soft):
        """Asserts that a value at an address is a certain value.

        This exactly checks if state.memory[address] == value
        This is different from assertValue as it doesn't require a label in the assembly code.

        Args:
            address: Short - Address to check.
            value: Integer - Expected value.
        """
        address = _toUShort(address)
        label = self.state.reverse_lookup(address)
        self._internalAssert('fillValue', not label, 'assertValueAt is not to be used on a labeled address, use assertValue instead. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)

        actual = self._readMem(address)
        expected = _toShort(value)
        self._assertShortEqual(expected, actual, 'address: x%04x' % address, 'MEM[x%04x] was expected to be (%d x%04x) but code produced (%d x%04x)\n' % (address, expected, _toUShort(expected), actual, _toUShort(actual)), level=level)
        self.postconditions.add(PostconditionFlag.direct_value, '%04x' % address, [value])

    def assertArrayAt(self, address, arr, level=AssertionType.soft):
        """Asserts that a sequence of values starting at the address given are certain values.

        This exactly checks if:
            state.memory[address] == arr[0]
            state.memory[address + 1] == arr[1]
            [...]
            state.memory[address + len(arr) - 1] == arr[len(arr) - 1]

        *NOTICE* It is an error to call this function on an labeled address, the reasoning for this is that there is
        the potential to overwrite user data past the end of the array in the original template if the text parameter
        is larger. Use assertArray for that use case since it avoids that problem by having the string be at a address
        pointed to by a label.

        Args:
            address: Short - Starting address of the array to check.
            arr: Iterable of Integers - Expected values to check sequentially.
        """
        start_addr = _toUShort(address)
        label = self.state.reverse_lookup(start_addr)
        self._internalAssert('assertArrayAt', not label, 'assertArrayAt is not to be used on a labeled address, use assertArray instead. x%04x has label %s' % (start_addr, label), AssertionType.fatal, internal=True)

        actual_arr = []
        for addr, _ in enumerate(arr, start_addr):
            actual_arr.append(self._readMem(addr))
        self._assertEqual(arr, actual_arr, 'arrayAt: x%04x' % start_addr, 'Sequence of values at MEM[x%04x] was expected to be %s but code produced %s\n' % (start_addr, arr, actual_arr), level=level)
        self.postconditions.add(PostconditionFlag.direct_array, '%04x' % start_addr, arr)

    def assertStringAt(self, address, text, level=AssertionType.soft):
        """Asserts that sequence of characters followed by a NUL terminator starting at the address pointed to by label are certain values.

        This exactly checks if:
            state.memory[address] == text[0]
            state.memory[address + 1] == text[1]
            [...]
            state.memory[address + len(text) - 1] == text[len(text) - 1]
            state.memory[address + len(text)] == 0

        *NOTICE* It is an error to call this function on an labeled address, the reasoning for this is that there is
        the potential to overwrite user data past the end of the string in the original template if the text parameter
        is larger. Use setString for that use case since it avoids that problem by having the string be at a address
        pointed to by a label.

        Args:
            address: Short - Starting address of the string to check.
            text: String - Expected characters to check sequentially.
        """
        start_addr = _toUShort(address)
        label = self.state.reverse_lookup(start_addr)
        self._internalAssert('assertStringAt', not label, 'assertStringAt is not to be used on a labeled address, use assertString instead. x%04x has label %s' % (start_addr, label), AssertionType.fatal, internal=True)

        actual_str = []
        for addr, _ in enumerate(text, start_addr):
            actual_str.append(six.unichr(self._readMem(addr, unsigned=True)))
        actual_str.append(six.unichr(self._readMem(start_addr + len(text), unsigned=True)))
        expected_str = list(six.u(text))
        expected_str.append(u'\0')
        self._assertEqual(expected_str, actual_str, 'stringAt: x%04x' % start_addr, 'String of characters at MEM[x%04x] was expected to be %s but code produced %s\n' % (start_addr, repr(''.join(expected_str)), repr(''.join(actual_str))), level=level)
        self.postconditions.add(PostconditionFlag.direct_string, '%04x' % start_addr, [ord(char) for char in text])

    def assertNodeAt(self, address, next, data, level=AssertionType.soft):
        """Asserts that a node starting at the address given is a certain value

        This can check nodes of arbitrary type: linked list nodes or n-ary tree nodes
        This exactly checks if:
            state.memory[address] == next_info[0]
            state.memory[address + 1] == next_info[1]
            ...
            state.memory[address + n] == data

        *NOTICE* It is an error to call this function on an labeled address.

        It is *greatly* preferred to use this function over assertValueAt/assertArrayAt to improve tooling in the future.

        Args:
            address: Short - Address where the node is located.
            next_info: 1) None to indicate leaf node or last node. (Note only one 0 is checked in this case).
                       2) Short - Next address.
                       3) Iterable of Shorts/None - Next addresses, None to indicate no next address.
            data: NamedTuple - A NamedTuple describing the data and the contents.
        """
        def assertDataHelper(t1, t2, fields):
            failure_msg = "Node's data starting at MEM[x%04x] was not equal.\nNonmatching fields below.\n-------------------------\n" % (address + size_next)
            if fields is None:
                fields = ['Item %d' % i for i in range(len(t2))]
            for v1, v2, field in zip(t1, t2, fields):
                if v1 != v2:
                    failure_msg += 'Data field: %s was not equal. expected: %s actual: %s\n' % (field, v1, v2)
            self._assertEqual(t1, t2, 'nodeAtData: x%04x' % (address + size_next), failure_msg, level=level)

        label = self.state.reverse_lookup(address)

        self._internalAssert('assertNodeAt', not label, 'assertNodeAt is not to be used on a labeled address. x%04x has label %s' % (address, label), AssertionType.fatal, internal=True)

        size_next = 1
        actual_next = []
        if not next:
            actual_next.append(self._readMem(address, unsigned=True))
            next_info = [0]
        elif isinstance(next, int):
            actual_next.append(self._readMem(address, unsigned=True))
            next_info = [_toUShort(next)]
        else:
            next_info = [_toUShort(elem) if elem else 0 for elem in next]
            for addr, _ in enumerate(next_info, address):
                actual_next.append(self._readMem(addr, unsigned=True))
            size_next = len(next_info)

        actual = data._make(self._readData(address + size_next, data))
        expected = data._make(_cstringifyData(data))

        def hexify(arr):
            return ['x%04x' % elem for elem in arr]

        self._assertEqual(next_info, actual_next, 'nodeAtNext: x%04x' % address, "Node's next at MEM[x%04x] was expected to be %s but code produced %s\n" % (address, hexify(next_info), hexify(actual_next)), level=level)
        assertDataHelper(expected, actual, data._fields)

        self.postconditions.add(PostconditionFlag.node, '%04x' % address, _formDataPreconditions((next_info, data)))

    def assertDataAt(self, address, named_tuple, level=AssertionType.soft):
        """Asserts that an arbitrary data structure starting at the address given is a certain value

        Args:
            address: - Address where the data is located
            named_tuple: A NamedTuple describing the data and its contents.
        """
        def assertDataHelper(t1, t2, fields):
            failure_msg = 'Data starting at MEM[%s] was not equal.\nNonmatching fields below.\n-------------------------\n'
            if fields is None:
                fields = ['Item %d' % i for i in range(len(t2))]
            for v1, v2, field in zip(t1, t2, fields):
                if v1 != v2:
                    failure_msg += 'Data field: %s was not equal. expected: %s actual: %s\n' % (field, v1, v2)
            self._assertEqual(t1, t2, 'data: x%04x' % address, failure_msg, level=level)

        label = self.state.reverse_lookup(address)
        if label:
            raise ValueError('assertData is not to be used on a labeled address.')
        actual = named_tuple._make(self._readData(address, named_tuple))
        expected = named_tuple._make(_cstringifyData(named_tuple))
        assertDataHelper(expected, actual, named_tuple._fields)
        self.postconditions.add(PostconditionFlag.data, '%04x' %  address, _formDataPreconditions(named_tuple))

    def assertReturnValue(self, answer, level=AssertionType.soft):
        """Asserts that the correct answer was returned.

        This is for verifying that the lc3 calling convention postcondition that memory[r6] == answer

        Args:
            answer: Integer - Expected answer.
        """
        expected = _toShort(answer)
        actual = self._readMem(self.state.r6)
        self._assertShortEqual(expected, actual, 'return value', 'Return value was expected to be (%d x%04x) but code produced (%d x%04x)\n' % (expected, _toUShort(expected), actual, _toUShort(actual)), level=level)
        self.postconditions.add(PostconditionFlag.return_value, 0, [answer])

    def assertRegistersUnchanged(self, registers=None, level=AssertionType.soft):
        """Asserts that registers value are the same as the beginning of execution.

        This is for verifying that caller saved registers are not clobbered as part of the lc3 calling convention.

        Args:
            registers: List(Integer) - Register numbers's to check. None to check all except R6.
        """
        if not registers:
            registers = [0, 1, 2, 3, 4, 5, 7]

        original_values = {num: self.registers[num] for num in registers}
        current_values = {num: self._readReg(num) for num in registers}

        changed_registers = ['R%d' % reg for reg in registers if original_values[reg] != current_values[reg]]
        all_registers = ['R%d' % reg for reg in registers]
        self._internalAssert('registers unchanged', not changed_registers, 'Expected %s to be unchanged after program/subroutine execution.\nThese registers have changed %s\n' % (all_registers, changed_registers), level=level)

        bits = sum([1 << a for a in registers])
        self.postconditions.add(PostconditionFlag.registers_unchanged, bits, [original_values[reg] for reg in sorted(registers)])

    def assertStackManaged(self, stack, return_address, old_frame_pointer, level=AssertionType.soft):
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
        actual_return_address = self._readMem(self.state.r6 - 1, unsigned=True)
        actual_old_frame_ptr = self._readMem(self.state.r6 - 2, unsigned=True)

        self._assertShortEqual(self.state.r6, stack, 'stack', 'Calling convention not followed.\nExpected R6 to be x%04x after returning from subroutine code produced x%04x\n' % (_toUShort(self.state.r6), _toUShort(stack)), level=level)
        self._assertShortEqual(return_address, actual_return_address, 'return address', 'Expected return address x%04x not found on stack in correct location code produced x%04x\n' % (return_address, actual_return_address), level=level)
        self._assertShortEqual(old_frame_pointer, actual_old_frame_ptr, 'old frame pointer', 'Expected old frame pointer x%04x not found on stack in correct location code produced x%04x\n' % (old_frame_pointer, actual_old_frame_ptr), level=level)
        self.postconditions.add(PostconditionFlag.calling_convention_followed, 0, [stack, return_address, old_frame_pointer])

    def assertSubroutineCallsMade(self, level=AssertionType.soft):
        """Asserts that the expected subroutine calls were made with no unexpected ones made.

        It is required to call expectSubroutineCall in order for this function to work. If it is
        not then it will check for no subroutines to be called.
        """
        def subroutine_list(subrs):
            if self._subroutine_call_mode == SubroutineCallMode.lc3_calling_convention:
                return ' '.join(['%s(%s)' % (name, ','.join(map(str, map(_toShort, params)))) for name, params in subrs])
            else:
                subr_strs = []
                for subr in subrs:
                    name, params = subr
                    params_pairs = list(params)
                    params = ', '.join(['R%d=(%d x%04x)' % (reg, value, _toShort(value)) for reg, value in params_pairs])
                    subr_strs.append('%s(%s)' % (name, params) if params else trap_name)
                return ' '.join(subr_strs)

        def subroutine_matches(a, b):
            a_address, a_params = a
            b_address, b_params = b
            if a_address != b_address:
                return False
            if self._subroutine_call_mode == SubroutineCallMode.lc3_calling_convention:
                for a_val, b_val in zip(a_params, b_params):
                    if _toUShort(a_val) != _toUShort(b_val):
                        return False
            else:
                return a_params == b_params
            return True

        actual_subroutines = set()
        for call in self.state.first_level_calls:
            if self._subroutine_call_mode == SubroutineCallMode.lc3_calling_convention:
                actual_subroutines.add((self._reverse_lookup(call.address), tuple([_toShort(param) for param in call.params])))
            else:
                params = tuple([(reg, param) for reg, param in enumerate(call.regs) if reg in self.subroutine_specifications[self._reverse_lookup(call.address)]])
                actual_subroutines.add((self._reverse_lookup(call.address), params))

        made_calls = set()
        missing_calls = set()
        optional_calls = set()
        unknown_calls = set()

        for subroutine in actual_subroutines:
            if any(subroutine_matches(subroutine, subr) for subr in self.expected_subroutines):
                made_calls.add(subroutine)
            elif any(subroutine_matches(subroutine, subr) for subr in self.optional_subroutines):
                optional_calls.add(subroutine)
            else:
                unknown_calls.add(subroutine)
        for subroutine in self.expected_subroutines:
            if not any(subroutine_matches(subroutine, subr) for subr in actual_subroutines):
                missing_calls.add(subroutine)

        #made_calls = self.expected_subroutines & actual_subroutines
        #missing_calls = self.expected_subroutines - actual_subroutines
        #optional_calls = self.optional_subroutines & actual_subroutines
        #unknown_calls = actual_subroutines - (self.expected_subroutines | self.optional_subroutines)

        status_message = ''
        status_message += 'Expected the following subroutine calls to be made: %s\n' % subroutine_list(self.expected_subroutines) if self.expected_subroutines else 'Expected no subroutines to have been called.\n'
        status_message += 'Calls made correctly: %s\n' % (subroutine_list(made_calls) if made_calls else 'none')
        status_message += 'Required calls missing: %s\n' % (subroutine_list(missing_calls) if missing_calls else 'none')
        status_message += 'Accepted optional calls made: %s\n' % subroutine_list(optional_calls) if optional_calls else ''
        status_message += 'Unknown subroutine calls made: %s\n' % subroutine_list(unknown_calls) if unknown_calls else ''

        self._internalAssert('subroutine calls made', len(self.expected_subroutines) == len(made_calls) and not missing_calls and not unknown_calls, status_message, level=level)

    def assertTrapCallsMade(self, level=AssertionType.soft):
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
                params = ', '.join(['R%d=(%d x%04x)' % (reg, value, _toUShort(value)) for reg, value in params_pairs])
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

        self._internalAssert('trap calls made', len(self.expected_traps) == len(made_calls) and not missing_calls and not unknown_calls, status_message, level=level)

    def _generateReplay(self):
        return "\nString to set up this test in complx: %s\nString to check results in complx: %s\nPlease include the full output from this grader in questions to TA's/piazza\n" % (repr(self.preconditions.encode()), repr(self.postconditions.encode()))
