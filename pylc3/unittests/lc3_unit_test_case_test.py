from .. import pylc3
import base64
import collections
import logging
import os
import six
import struct
import sys
import unittest
import zlib
from pylc3.unittests import lc3_unit_test_case
from pylc3.unittests.lc3_unit_test_case import DataItem


# Note this file is not to be used as a template for writing student tests.
# See https://github.com/TricksterGuy/pylc3-examples for example tests

class LC3UnitTestCaseTest(lc3_unit_test_case.LC3UnitTestCase):

    def setUp(self):
        super(LC3UnitTestCaseTest, self).setUp()
        self.maxDiff = None

    def tearDown(self):
        def form_failure_message():
            return 'Internal Error at least one assert has failed\n%s' % ('\n'.join(['name: %s Reason: %s' % (name, msg) for name, msg in self.failed_assertions]))
        if self.failed_assertions:
            self.fail(form_failure_message())

    # Overridden to not generate a json file.
    @classmethod
    def tearDownClass(cls):
        pass

    def loadCode(self, snippet):
        # This function is test only, Only use loadAsmFile for student code.
        self.state.loadCode(snippet)
        self.asm_filename = 'this is a test'

    def testInit(self):
        self.init(pylc3.MemoryFillStrategy.fill_with_value, 0x8000)
        self.assertEqual(self._readMem(0x3005) & 0xFFFF, 0x8000)

    def testLoadFailed(self):
        snippet = """
        .orig x3000
            AND R1, R1, 0
            ADD R1, R1 R0
            ADD R1, R1, R1
            HALT
        .end
        """
        text_file = open("syntax_error.asm", "w")
        text_file.write(snippet)
        text_file.close()

        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.loadAsmFile("syntax_error.asm")

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        os.remove("syntax_error.asm")

    def testLoadPatt(self):
        obj = six.b('\x00\x00\x20\x01\xF0\x25\x00\x00\x00\x02')
        sym = ('// Symbol table\n'
        '// Scope level 0:\n'
        '//	Symbol Name       Page Address\n'
        '//	----------------  ------------\n'
        '//	A                 0002\n'
        '//	B                 0003\n\n')
        with open('sample.obj', 'wb') as f:
            f.write(obj)
        with open('sample.sym', 'w') as f:
            f.write(sym)
        self.loadPattObjAndSymFile('sample.obj', 'sample.sym')

        self.assertEqual(self._lookup("A"), 2)
        self.assertEqual(self._lookup("B"), 3)
        self.assertEqual(self._readMem(0x0000), 0x2001)
        self.assertEqual(self._readMem(0x0001) & 0xFFFF, 0xf025)
        self.assertEqual(self._readMem(0x0002), 0x0000)
        self.assertEqual(self._readMem(0x0003), 0x0002)

        os.remove('sample.obj')
        os.remove('sample.sym')

    def testBadSets(self):
        snippet = """
        .orig x3000
            A .fill 1
            B .fill 2
            C .fill 3
            D .fill 4
        .end
        """
        self.loadCode(snippet)
        self.setValue("A", 3)
        self.setPointer("B", 10)
        self.setArray("C", [255])
        self.setString("D", "H")

        def badSets(label, ignore_type):
            self.assertEqual(self._modified_labels[label], ignore_type)
            if ignore_type != 'VALUE':
                with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
                    self.setValue(label, 10)
            if ignore_type != 'POINTER':
                with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
                    self.setPointer(label, 10)
            if ignore_type != 'ARRAY':
                with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
                    self.setArray(label, [10])
            if ignore_type != 'STRING':
                with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
                    self.setString(label, 'STR')

        badSets("A", "VALUE")
        badSets("B", "POINTER")
        badSets("C", "ARRAY")
        badSets("D", "STRING")

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testFillsBeforeLoad(self):
        # No file loaded
        self.asm_filename = ''

        # It is an error to fill addresses with values before loading the code.
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillValue(0x4000, 10)
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillString(0x4000, "Failure")
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillArray(0x4000, [10, 23, 45])
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillNode(0x4000, 0x4000, (23,))
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillData(0x4000, (23, 24))

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testRegister(self):
        snippet = """
        .orig x3000
            AND R1, R1, 0
            ADD R1, R1, R0
            ADD R1, R1, R1
            HALT
        .end
        """
        self.loadCode(snippet)
        self.setRegister(0, 10)

        # Sanity checks
        self.assertEqual(self.state.r0, 10)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertRegister(0, 10)
        self.assertRegister(1, 20)

    def testPC(self):
        snippet = """
        .orig x4000
            LD R0, ADDR
            JMP R0
            ADDR .fill x5000
        .end
        .orig x5000
            HALT
        .end
        """
        self.loadCode(snippet)
        self.setPc(0x4000)

        # Sanity checks
        self.assertEqual(self.state.pc, 0x4000)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        # Halting decrements PC by 1.
        self.assertPc(0x5000)

    def testValue(self):
        snippet = """
        .orig x3000
            LD R0, A
            LD R1, B
            ADD R2, R1, R0
            ST R2, ANS
            HALT
            A .blkw 1
            B .blkw 1
            ANS .blkw 1
        .end
        """
        self.loadCode(snippet)
        self.setValue("A", 2)
        self.setValue("B", 3)

        # Sanity checks
        self.assertEqual(self._lookup("A"), 0x3005)
        self.assertEqual(self._readMem(0x3005), 2)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertValue("A", 2)
        self.assertValue("B", 3)
        self.assertValue("ANS", 5)

    def testPointer(self):
        snippet = """
        .orig x3000
            LDI R0, A
            LDI R1, B
            ADD R2, R1, R0
            STI R2, ANS
            HALT
            A .fill x4000
            B .fill x4001
            ANS .fill x4002
        .end
        """
        self.loadCode(snippet)
        self.setPointer("A", 2)
        self.setPointer("B", 3)

        # Sanity checks
        self.assertEqual(self._lookup("A"), 0x3005)
        self.assertEqual(self._readMem(0x3005), 0x4000)
        self.assertEqual(self._readMem(0x4000), 2)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertPointer("A", 2)
        self.assertPointer("B", 3)
        self.assertPointer("ANS", 5)

    def testArray(self):
        snippet = """
        .orig x3000
            AND R0, R0, 0
            STI R0, LOC
            HALT
            ARR .fill x4000
            LOC .fill x4002
        .end

        .orig x4000
            .blkw 3
            .fill 666
        .end
        """
        self.loadCode(snippet)
        self.setArray("ARR", [5, 3, 1])

        # Sanity checks
        self.assertEqual(self._lookup("ARR"), 0x3003)
        self.assertEqual(self._readMem(0x3003), 0x4000)
        self.assertEqual(self._readMem(0x3004), 0x4002)
        self.assertEqual(self._readMem(0x4000), 5)
        self.assertEqual(self._readMem(0x4001), 3)
        self.assertEqual(self._readMem(0x4002), 1)
        self.assertEqual(self._readMem(0x4003), 666)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertArray("ARR", [5, 3, 0, 666])

    def testString(self):
        # Generally bad to do it this way since students can
        # define labels after STRING and the replacement can trample
        # their labels.
        snippet = """
        .orig x3000
            LDI R0, STRING_LOC
            ADD R0, R0, 2
            ST R0, STRING
            HALT
            STRING_LOC .fill STRING
            STRING .blkw 6
        .end
        """
        self.loadCode(snippet)
        self.setString("STRING_LOC", "HELLO")

        # Sanity checks
        self.assertEqual(self._lookup("STRING_LOC"), 0x3004)
        self.assertEqual(self._lookup("STRING"), 0x3005)
        self.assertEqual(self._readMem(0x3004), 0x3005)
        self.assertEqual(self._readMem(0x3005), ord('H'))
        self.assertEqual(self._readMem(0x3006), ord('E'))
        self.assertEqual(self._readMem(0x3007), ord('L'))
        self.assertEqual(self._readMem(0x3008), ord('L'))
        self.assertEqual(self._readMem(0x3009), ord('O'))
        self.assertEqual(self._readMem(0x300A), 0)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertString("STRING_LOC", "JELLO")

    def testConsoleIO(self):
        snippet = """
        .orig x3000
            GETC
            ADD R0, R0, 1
            OUT
            GETC
            ADD R0, R0, 1
            OUT
            GETC
            ADD R0, R0, 1
            OUT
            HALT
        .end
        """
        self.loadCode(snippet)
        self.setConsoleInput("ADB")
        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertConsoleOutput("BEC")

    def testFillValue(self):
        snippet = """
        .orig x3000
            LD R0, A
            LD R1, 4
            ADD R2, R1, R0
            ST R2, 3
            HALT
            A .blkw 1
            .blkw 1 ; B
            .blkw 1 ; ANS
        .end
        """
        self.loadCode(snippet)

        # Sanity checks I
        # This is an error as it is a labelled address, setValue should be used here.
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillValue(0x3005, 2)

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        self.setValue("A", 2)
        self.fillValue(0x3006, 3)

        # Sanity checks II
        self.assertEqual(self._readMem(0x3006), 3)

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()

        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.assertValueAt(0x3005, 2)

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        self.assertValue("A", 2)
        self.assertValueAt(0x3006, 3)
        self.assertValueAt(0x3007, 5)

    def testFillString(self):
        # This is not really testing assembly code see the others for more full fledged examples with code snippets.
        snippet = """
        .orig x3000
            NOTHERE .stringz "HI"
        .end
        """
        self.loadCode(snippet)

        # Sanity checks
        # This is an error as it is a labelled address, setString should be used here (on a label with an address
        # to put the string) as there is potential to overwrite data past the label. Assembly code should not be setup
        # like so due to this since the fill will happen *after* the code is assembled and loaded.  In either case even
        # if this replacement was still done before assembling data after the label could cause code not to assemble.
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillString(0x3000, "HELLO")

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        self.fillString(0x4000, "HELLO")

        self.assertEqual(self._readMem(0x4000), ord('H'))
        self.assertEqual(self._readMem(0x4001), ord('E'))
        self.assertEqual(self._readMem(0x4002), ord('L'))
        self.assertEqual(self._readMem(0x4003), ord('L'))
        self.assertEqual(self._readMem(0x4004), ord('O'))
        self.assertEqual(self._readMem(0x4005), 0)

    def testFillArray(self):
        # This is not really testing assembly code see the others for more full fledged examples with code snippets.
        snippet = """
        .orig x3000
            NOTHERE .blkw 10
        .end
        """
        self.loadCode(snippet)

        # Sanity checks
        # This is an error as it is a labelled address, setArray should be used here (on a label with an address
        # to put the array) as there is potential to overwrite data past the label. Assembly code should not be setup
        # like so due to this since the fill will happen *after* the code is assembled and loaded.  In either case even
        # if this replacement was still done before assembling data after the label could cause code not to assemble.
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillArray(0x3000, [10, 12, 15])

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        self.fillArray(0x4000, [10, 12, 15])

        self.assertEqual(self._readMem(0x4000), 10)
        self.assertEqual(self._readMem(0x4001), 12)
        self.assertEqual(self._readMem(0x4002), 15)

    def testFillNode(self):
        # This is not really testing assembly code see the others for more full fledged examples with code snippets.
        snippet = """
        .orig x3000
            NOTHERE .blkw 3
        .end
        """
        self.loadCode(snippet)

        # Sanity checks
        # This is an error as it is a labelled address, setArray should be used here (on a label with an address
        # to put the array) as there is potential to overwrite data past the label. Assembly code should not be setup
        # like so due to this since the fill will happen *after* the code is assembled and loaded.  In either case even
        # if this replacement was still done before assembling data after the label could cause code not to assemble.
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.fillNode(0x3000, next=None, data=(3,))

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        # Ending node of linkedlist if you want.
        self.fillNode(0x4000, next=None, data=(27,))
        self.assertEqual(self._readMem(0x4000), 0)
        self.assertEqual(self._readMem(0x4001), 27)

        # Linkedlist node.
        self.fillNode(0x5000, next=0x4000, data=(32,))
        self.assertEqual(self._readMem(0x5000), 0x4000)
        self.assertEqual(self._readMem(0x5001), 32)

        # Still a Linkedlist node.
        self.fillNode(0x5050, next=[0x4000], data=(32,))
        self.assertEqual(self._readMem(0x5050), 0x4000)
        self.assertEqual(self._readMem(0x5051), 32)

        # Binary tree node.
        self.fillNode(0x6000, next=[0x5000, 0x4000], data=(102,))
        self.assertEqual(self._readMem(0x6000), 0x5000)
        self.assertEqual(self._readMem(0x6001), 0x4000)
        self.assertEqual(self._readMem(0x6002), 102)

        # What the heck a ternary tree node or maybe a skiplist node, its up to your imagination.
        self.fillNode(0x7000, next=[0x6000, 0x5000, 0x4000], data=(1382,))
        self.assertEqual(self._readMem(0x7000), 0x6000)
        self.assertEqual(self._readMem(0x7001), 0x5000)
        self.assertEqual(self._readMem(0x7002), 0x4000)
        self.assertEqual(self._readMem(0x7003), 1382)

    def testAssertNodeAt(self):
        # This is not really testing assembly code see the others for more full fledged examples with code snippets.
        snippet = """
        .orig x3000
            A .blkw 3
        .end
        .orig x4000
            .fill 0
            .fill 27
        .end
        .orig x5000
            .fill x4000
            .fill 32
        .end
        .orig x5050
            .fill x4000
            .fill 32
        .end
        .orig x6000
            .fill x5000
            .fill x4000
            .fill 102
        .end
        .orig x7000
            .fill x6000
            .fill x5000
            .fill 0
            .fill x4000
            .fill 1382
        .end
        .orig x8000
            .fill x8000
            .fill 1 ; id
            .fill 123 ; stats
            .fill 456
            .stringz "fred" ; name
            .fill 1 ; extra
            .fill 2
            .fill 3
        .end
        """
        self.loadCode(snippet)

        blah = collections.namedtuple('blah', 'num')

        # Sanity checks
        # This is an error as it is a labelled address, setArray should be used here (on a label with an address
        # to put the array) as there is potential to overwrite data past the label. Assembly code should not be setup
        # like so due to this since the fill will happen *after* the code is assembled and loaded.  In either case even
        # if this replacement was still done before assembling data after the label could cause code not to assemble.
        with self.assertRaises(lc3_unit_test_case.LC3InternalAssertion):
            self.assertNodeAt(0x3000, next=None, data=blah(3))

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

        # Ending node of linkedlist if you want.
        self.assertNodeAt(0x4000, next=None, data=blah(27))

        # Linkedlist node.
        self.assertNodeAt(0x5000, next=0x4000, data=blah(32))

        # Still a Linkedlist node.
        self.assertNodeAt(0x5050, next=[0x4000], data=blah(32))

        # Binary tree node.
        self.assertNodeAt(0x6000, next=[0x5000, 0x4000], data=blah(102))

        # What the heck a quaternary tree node or maybe a skiplist node, its up to your imagination.
        self.assertNodeAt(0x7000, next=[0x6000, 0x5000, None, 0x4000], data=blah(1382))

        # Node with a more complex datastructure for data.
        person = collections.namedtuple('person', 'id stats name extra')
        self.assertNodeAt(0x8000, next=[0x8000], data=person(1, [123, 456], 'fred', (1, 2, 3)))

    def testFillData(self):
        snippet = """
        .orig x3000
            ;@plugin filename=lc3_udiv vector=x80
            ;@plugin filename=lc3_multiply
            LD R0, STUDENT_DATA
            PUTS

            CHECK LDR R7, R0, 0
            BRZ DONE
            ADD R0, R0, 1
            BR CHECK
            DONE ADD R2, R0, 1

            LDR R3, R2, 0 ; num tests
            LDR R4, R2, 1 ; num hws
            ADD R2, R2, 2
            AND R0, R0, 0
            ADD_TEST LDR R5, R2, 0
            ADD R0, R0, R5
            ADD R2, R2, 1
            ADD R3, R3, -1
            BRP ADD_TEST
            LD R1, TEST_FACTOR
            MUL R0, R0, R1
            ADD R6, R0, 0
            AND R0, R0, 0
            ADD_HW LDR R5, R2, 0
            ADD R0, R0, R5
            ADD R2, R2, 1
            ADD R4, R4, -1
            BRP ADD_HW
            LD R1, HW_FACTOR
            MUL R0, R0, R1
            ADD R0, R0, R6
            LD R1, HUNNIT
            UDIV
            STR R0, R2, 0
            HALT
            ; NAME
            ; TEST GRADES
            ; HW GRADES
            STUDENT_DATA .fill x4000
            TEST_FACTOR .fill 10 ; 10 * 3 = 30
            HW_FACTOR .fill 14 ; 14 * 5 = 70
            HUNNIT .fill 100
        .end
        """
        self.loadCode(snippet)
        # struct Student {
        #   char* name;
        #   short num_tests;
        #   short num_homeworks;
        #   short tests[];
        #   short homeworks[];
        # }
        self.fillData(0x4000, ("Student", 3, 5, [100, 75, 80], [100, 50, 80, 60, 100]))

        # Sanity Checks
        self.assertEqual(self._readMem(0x4000), ord('S')) # name = Student
        self.assertEqual(self._readMem(0x4001), ord('t'))
        self.assertEqual(self._readMem(0x4002), ord('u'))
        self.assertEqual(self._readMem(0x4003), ord('d'))
        self.assertEqual(self._readMem(0x4004), ord('e'))
        self.assertEqual(self._readMem(0x4005), ord('n'))
        self.assertEqual(self._readMem(0x4006), ord('t'))
        self.assertEqual(self._readMem(0x4007), 0)
        self.assertEqual(self._readMem(0x4008), 3) # num_tests = 3
        self.assertEqual(self._readMem(0x4009), 5) # num_homeworks = 5
        self.assertEqual(self._readMem(0x400A), 100) # tests[3] = {100, 75, 80}
        self.assertEqual(self._readMem(0x400B), 75)
        self.assertEqual(self._readMem(0x400C), 80)
        self.assertEqual(self._readMem(0x400D), 100) # homeworks[5] = {100, 50, 80, 60, 100)
        self.assertEqual(self._readMem(0x400E), 50)
        self.assertEqual(self._readMem(0x400F), 80)
        self.assertEqual(self._readMem(0x4010), 60)
        self.assertEqual(self._readMem(0x4011), 100)

        self.runCode()

        self.assertHalted()
        self.assertNoWarnings()

        Student = collections.namedtuple('Student', ['name', 'num_tests', 'num_homeworks', 'tests', 'homeworks', 'grade'])
        expected = Student('Student', 3, 5, [100, 75, 80], [100, 50, 80, 60, 100], 80)
        self.assertDataAt(0x4000, expected)

    def testTrapCall(self):
        snippet = """
        ;@plugin filename=lc3_udiv vector=x80
        .orig x3000
                LD R0, A
                LD R1, B
                UDIV
                HALT
            A .fill 2000
            B .fill 8
        .end
        """
        self.loadCode(snippet)

        self.expectTrapCall(0x80, params={0: 2000, 1: 8})

        self.runCode()

        self.assertHalted()
        self.assertNoWarnings()

        self.assertTrapCallsMade()

    def testTrapCallNegative(self):
        snippet = """
        ;@plugin filename=lc3_udiv vector=x80
        .orig x3000
                LD R0, A
                LD R1, B
                UDIV
                HALT
            A .fill -2000
            B .fill -8
        .end
        """
        self.loadCode(snippet)

        self.expectTrapCall(0x80, params={0: -2000, 1: -8})

        self.runCode()

        self.assertHalted()
        self.assertNoWarnings()

        self.assertTrapCallsMade()

    def testSubroutineCallBasic(self):
        snippet = """
        .orig x3000
            ; def return7():
            ;     return 7
            RETURN7
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                AND R5, R5, 0
                ADD R5, R5, 7
                STR R5, R6, 2
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
        .end
        """
        self.loadCode(snippet)
        self.callSubroutine("RETURN7", params=[], r5=0xCAFE, r6=0xF000, r7=0x8000)

        # Sanity checks
        self.assertEqual(self.state.pc, 0x3000)
        self.assertEqual(self._lookup("RETURN7"), 0x3000)
        self.assertEqual(self._readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self._readReg(6, unsigned=True), 0xF000)
        self.assertEqual(self._readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(7)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFF, return_address=0x8000, old_frame_pointer=0xCAFE)
        # Checks that no subroutines were called.
        self.assertSubroutineCallsMade()

    def testSubroutineCallWithParams(self):
        snippet = """
        .orig x4000
            ; def MYADD(x, y, z):
            ;     return x + y + z
            MYADD
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                LDR R0, R5, 4
                LDR R1, R5, 5
                LDR R2, R5, 6
                ADD R0, R0, R1
                ADD R0, R0, R2
                STR R0, R5, 3
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
        .end
        """
        self.loadCode(snippet)
        self.callSubroutine("MYADD", params=[7, 81, 123], r5=0xCAFE, r6=0xF000, r7=0x8000)

        # Sanity checks
        self.assertEqual(self.state.pc, 0x4000)
        self.assertEqual(self._lookup("MYADD"), 0x4000)
        self.assertEqual(self._readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self._readReg(6, unsigned=True), 0xEFFD)
        self.assertEqual(self._readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(211)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFC, return_address=0x8000, old_frame_pointer=0xCAFE)
        # Checks that no subroutines were called.
        self.assertSubroutineCallsMade()

    def testSubroutineCallWithParamsInRegisters(self):
        snippet = """
        .orig x4000
            ;x in R0, y in R1, z in R2.
            ; ans in R0
            ; def MYADD(x, y, z):
            ;     return x + y + z
            MYADD
                ADD R6, R6, -2
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                ADD R0, R0, R1
                ADD R0, R0, R2
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
        .end
        """
        self.loadCode(snippet)
        self.callSubroutine("MYADD", {0: 7, 1: 81, 2: 123}, r5=0xCAFE, r6=0xF000, r7=0x8000)

        # Sanity checks
        self.assertEqual(self.state.pc, 0x4000)
        self.assertEqual(self._lookup("MYADD"), 0x4000)
        self.assertEqual(self._readReg(0, unsigned=True), 7)
        self.assertEqual(self._readReg(1, unsigned=True), 81)
        self.assertEqual(self._readReg(2, unsigned=True), 123)
        self.assertEqual(self._readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self._readReg(6, unsigned=True), 0xF000)
        self.assertEqual(self._readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertRegister(0, 211)
        self.assertRegistersUnchanged([1, 2, 5, 7])
        self.assertStackManaged(stack=0xF000, return_address=0x8000, old_frame_pointer=0xCAFE)
        # Checks that no subroutines were called.
        self.assertSubroutineCallsMade()

    def testSubroutineCallWithParamsAndCalls(self):
        # Psuedocode
        # int a(int x, int y, int z) {
        #   return b(x, y) + b(x, z) + b(y, z) + c(x);
        # }
        #
        # int b(int x, int y) {
        #    return x * y;
        # }
        #
        # int c(int x) {
        #    return x / 2;
        # }
        snippet = """
        ;@plugin filename=lc3_multiply
        ;@plugin filename=lc3_udiv vector=x80

        .orig x3000
            A
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                LDR R0, R5, 4
                LDR R1, R5, 5
                ADD R6, R6, -3
                STR R0, R6, 0
                STR R1, R6, 1
                JSR B
                LDR R0, R6, 0
                ADD R6, R6, 1
                STR R0, R5, 0
                LDR R0, R5, 4
                LDR R1, R5, 6
                STR R0, R6, 0
                STR R1, R6, 1
                JSR B
                LDR R0, R6, 0
                ADD R6, R6, 1
                LDR R1, R5, 0
                ADD R0, R0, R1
                STR R0, R5, 0
                LDR R0, R5, 5
                LDR R1, R5, 6
                STR R0, R6, 0
                STR R1, R6, 1
                JSR B
                LDR R0, R6, 0
                ADD R6, R6, 2
                LDR R1, R5, 0
                ADD R0, R0, R1
                STR R0, R5, 0
                LDR R0, R5, 4
                STR R0, R6, 0
                JSR C
                LDR R0, R6, 0
                ADD R6, R6, 2
                LDR R1, R5, 0
                ADD R0, R0, R1
                STR R0, R5, 3
                ADD R6, R5, 3
                LDR R7, R5, 2
                LDR R5, R5, 1
                RET
            B
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                LDR R0, R5, 4
                LDR R1, R5, 5
                MUL R0, R1, R0
                STR R0, R5, 3
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
            C
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                LDR R0, R5, 4
                AND R1, R1, 0
                ADD R1, R1, 2
                UDIV
                STR R0, R5, 3
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
           .end
        """
        self.loadCode(snippet)
        self.callSubroutine("A", params=[3, 5, 7], r5=0xCAFE, r6=0xF000, r7=0x8000)
        # For assertSubroutineCallMade.
        self.expectSubroutineCall("B", params=[3, 5])
        self.expectSubroutineCall("B", params=[3, 7])
        self.expectSubroutineCall("B", params=[5, 7])
        self.expectSubroutineCall("C", params=[3])
        # Don't call expectTrapCall here since A doesn't directly use it. Only the first level calls are logged.

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        #self.assertReturnValue(72)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFC, return_address=0x8000, old_frame_pointer=0xCAFE)
        self.assertSubroutineCallsMade()

    def testSubroutineCallWithParamsAndCallsInRegisters(self):
        # Psuedocode
        # X: R0 Y: R1 Z: R2
        # ANS: R0
        # int a(int x, int y, int z) {
        #   return b(x, y) + b(x, z) + b(y, z) + c(x);
        # }
        #
        # X: R3 Y: R4
        # ANS R3
        # int b(int x, int y) {
        #    return x * y;
        # }
        #
        # X: R4
        # ANS: R2
        # int c(int x) {
        #    return x / 2;
        # }
        snippet = """
        ;@plugin filename=lc3_multiply
        ;@plugin filename=lc3_udiv vector=x80

        .orig x3000
            A
                ADD R6, R6, -2
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                ADD R6, R6, -3
                STR R2, R6, 0
                STR R3, R6, 1
                STR R4, R6, 2

                ADD R3, R0, 0
                ADD R4, R1, 0
                JSR B

                ADD R6, R6, -1
                STR R3, R6, 0

                ADD R3, R0, 0
                ADD R4, R2, 0
                JSR B

                ADD R6, R6, -1
                STR R3, R6, 0

                ADD R3, R1, 0
                ADD R4, R2, 0
                JSR B

                ADD R6, R6, -1
                STR R3, R6, 0

                ADD R4, R0, 0
                JSR C

                ADD R0, R2, 0
                LDR R2, R6, 0
                ADD R0, R0, R2
                LDR R2, R6, 1
                ADD R0, R0, R2
                LDR R2, R6, 2
                ADD R0, R0, R2
                ADD R6, R6, 3

                LDR R2, R6, 0
                LDR R3, R6, 1
                LDR R4, R6, 2
                LDR R5, R6, 3
                LDR R7, R6, 4
                ADD R6, R6, 5
                RET

            B
                ADD R6, R6, -2
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                MUL R3, R3, R4
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET

            C
                ADD R6, R6, -2
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                ADD R6, R6, -2
                STR R0, R6, 0
                STR R1, R6, 1
                ADD R0, R4, 0
                AND R1, R1, 0
                ADD R1, R1, 2
                UDIV
                ADD R2, R0, 0
                LDR R0, R6, 0
                LDR R1, R6, 1
                ADD R6, R6, 2
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
           .end
        """
        self.loadCode(snippet)
        self.callSubroutine("A", params={0: 3, 1: 5, 2: 7}, r5=0xCAFE, r6=0xF000, r7=0x8000)
        # For assertSubroutineCallMade.
        self.expectSubroutineCall("B", params={3: 3, 4: 5})
        self.expectSubroutineCall("B", params={3: 3, 4: 7})
        self.expectSubroutineCall("B", params={3: 5, 4: 7})
        self.expectSubroutineCall("C", params={4: 3})
        # Don't call expectTrapCall here since A doesn't directly use it. Only the first level calls are logged.

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertRegister(0, 72)
        self.assertRegistersUnchanged([1, 2, 3, 4, 5, 6, 7])
        self.assertStackManaged(stack=0xF000, return_address=0x8000, old_frame_pointer=0xCAFE)
        self.assertSubroutineCallsMade()

    def testCallSubroutineNegative(self):
        snippet = """
        .orig x3000
            LD R6, STACK
            LD R0, A
            LD R1, B
            ADD R6, R6, -2
            STR R0, R6, 0
            STR R1, R6, 1
            JSR MYSTERY
            ADD R6, R6, 3
            HALT

            MYSTERY
                RET

            A .blkw 1
            B .blkw 1
            STACK .fill xF000
        .end
        """
        self.loadCode(snippet)
        self.setValue("A", -2000)
        self.setValue("B",-8)
        self.expectSubroutineCall("MYSTERY", params=[-2000, -8])

        self.runCode()
        self.assertHalted()
        self.assertNoWarnings()
        self.assertSubroutineCallsMade()

    def testSubroutineCallWithOptionalNotTaken(self):
        snippet = """
        ;@plugin filename=lc3_multiply
        .orig x3000
            ; (x + 1) * (x + 7)
            ; def compute_quadratic(x):
            ;   return x * x + 8 * x + return7();
            ;
            ; def return7():
            ;     return 7
            COMPUTE_QUADRATIC
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                LDR R0, R5, 4
                MUL R1, R0, R0
                MUL R2, R0, 8
                ADD R3, R1, R2
                ADD R3, R3, 7
                STR R3, R5, 3
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET

            RETURN7
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                AND R5, R5, 0
                ADD R5, R5, 7
                STR R5, R6, 2
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
        .end
        """
        self.loadCode(snippet)
        self.callSubroutine("COMPUTE_QUADRATIC", params=[3], r5=0xCAFE, r6=0xF000, r7=0x8000)
        self.expectSubroutineCall("RETURN7", params=[], optional=True)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(40)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFE, return_address=0x8000, old_frame_pointer=0xCAFE)
        # We didn't call return7 but thats okay it was optional.
        self.assertSubroutineCallsMade()

    def testSubroutineCallWithOptionalTaken(self):
        snippet = """
        ;@plugin filename=lc3_multiply
        .orig x3000
            ; (x + 1) * (x + 7)
            ; def compute_quadratic(x):
            ;   return x * x + 8 * x + return7();
            ;
            ; def return7():
            ;     return 7
            COMPUTE_QUADRATIC
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                LDR R0, R5, 4
                MUL R1, R0, R0
                MUL R2, R0, 8
                ADD R3, R1, R2
                ADD R6, R6, -1
                STR R3, R6, 0
                JSR RETURN7
                LDR R0, R6, 0
                ADD R6, R6, 1
                LDR R3, R6, 0
                ADD R6, R6, 1
                ADD R3, R3, R0
                STR R3, R5, 3
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET

            RETURN7
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                AND R5, R5, 0
                ADD R5, R5, 7
                STR R5, R6, 2
                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
        .end
        """
        self.loadCode(snippet)
        self.callSubroutine("COMPUTE_QUADRATIC", params=[3], r5=0xCAFE, r6=0xF000, r7=0x8000)
        self.expectSubroutineCall("RETURN7", params=[], optional=True)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(40)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFE, return_address=0x8000, old_frame_pointer=0xCAFE)
        self.assertSubroutineCallsMade()

    def testParanoidSubroutineCall(self):
        # They save all the registers.
        snippet = """
        .orig x4000
            ; def MYADD(x, y, z):
            ;     return x + y + z
            MYADD
                ADD R6, R6, -3
                STR R5, R6, 0
                STR R7, R6, 1
                ADD R5, R6, -1
                ADD R6, R6, -5
                STR R0, R6, 0
                STR R1, R6, 1
                STR R2, R6, 2
                STR R3, R6, 3
                STR R4, R6, 4

                LDR R0, R5, 4
                LDR R1, R5, 5
                LDR R2, R5, 6
                ADD R0, R0, R1
                ADD R0, R0, R2
                STR R0, R5, 3

                LDR R0, R6, 0
                LDR R1, R6, 1
                LDR R2, R6, 2
                LDR R3, R6, 3
                LDR R4, R6, 4
                ADD R6, R6, 5

                LDR R5, R6, 0
                LDR R7, R6, 1
                ADD R6, R6, 2
                RET
        .end
        """
        self.loadCode(snippet)
        self.callSubroutine("MYADD", params=[7, 81, 123], r5=0xCAFE, r6=0xF000, r7=0x8000)

        # Sanity checks
        self.assertEqual(self.state.pc, 0x4000)
        self.assertEqual(self._lookup("MYADD"), 0x4000)
        self.assertEqual(self._readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self._readReg(6, unsigned=True), 0xEFFD)
        self.assertEqual(self._readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(211)
        # Then they should all be unchanged.
        self.assertRegistersUnchanged([0, 1, 2, 3, 4, 5, 7])
        self.assertStackManaged(stack=0xEFFC, return_address=0x8000, old_frame_pointer=0xCAFE)
        # Checks that no subroutines were called.
        self.assertSubroutineCallsMade()

    def testInvalidSubroutineCall(self):
        snippet = """
        .orig x3000
            JSR #1
            DONE HALT
            RET
        .end
        """
        self.loadCode(snippet)

        self.expectSubroutineCall("DONE", params=[])
        self.runCode()
        self.assertSubroutineCallsMade()

        names = [tup[0] for tup in self.failed_assertions]
        msgs = [tup[1] for tup in self.failed_assertions]

        self.assertEqual(names, ['subroutine calls made'])
        self.assertIn('Expected the following subroutine calls to be made: DONE()\nCalls made correctly: none\nRequired calls missing: DONE()\nUnknown subroutine calls made: UnknownSubroutine@x3002()', msgs[0])

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testInvalidSubroutineCall2(self):
        snippet = """
        .orig x3000
            JSRR R0
            DONE HALT
            RET
        .end
        """
        self.loadCode(snippet)

        self.setRegister(0, 0x4000)
        self.expectSubroutineCall("DONE", params=[])
        self.runCode()
        self.assertSubroutineCallsMade()

        names = [tup[0] for tup in self.failed_assertions]
        msgs = [tup[1] for tup in self.failed_assertions]

        self.assertEqual(names, ['subroutine calls made'])
        self.assertIn('Expected the following subroutine calls to be made: DONE()\nCalls made correctly: none\nRequired calls missing: DONE()\nUnknown subroutine calls made: UnknownSubroutine@x4000()', msgs[0])

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testInvalidSubroutineCall3(self):
        snippet = """
        .orig x3000
            JSR DONE
            DONE HALT
            RET
        .end
        """
        self.loadCode(snippet)

        self.runCode()
        self.assertSubroutineCallsMade()

        names = [tup[0] for tup in self.failed_assertions]
        msgs = [tup[1] for tup in self.failed_assertions]

        self.assertEqual(names, ['subroutine calls made'])
        self.assertIn('Expected no subroutines to have been called.\nCalls made correctly: none\nRequired calls missing: none\nUnknown subroutine calls made: DONE', msgs[0])

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def PatchForMultipleLabelSubroutine(self, expected_label):
        the_label_addr = self._lookup(expected_label)
        found_label = self._reverse_lookup(the_label_addr)
        return found_label

    def testMultiSymbolSubroutineCall(self):
        # Sigh...
        snippet = """
        .orig x3000
            JSR DONE
            DONE ITSDONE HALT
            RET
        .end
        """
        self.loadCode(snippet)
        #self.expectSubroutineCall(self.PatchForMultipleLabelSubroutine("DONE"), params=[])
        self.expectSubroutineCall("DONE", params=[])

        self.runCode()
        self.assertSubroutineCallsMade()

        self.assertEqual(len(self.failed_assertions), 0)

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    '''def testMultiSymbolSubroutineCall2(self):
        # Sigh...
        snippet = """
        .orig x3000
            JSR DONE
            ITSDONE DONE HALT
            RET
        .end
        """
        self.loadCode(snippet)
        self.expectSubroutineCall(self.PatchForMultipleLabelSubroutine("DONE"), params=[])

        self.runCode()
        self.assertSubroutineCallsMade()

        self.assertEqual(len(self.failed_assertions), 0)

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testMultiSymbolSubroutineCall3(self):
        # Sigh...
        snippet = """
        .orig x3000
            JSR DONE
            DONE ITSDONE HALT
            RET
        .end
        """
        self.loadCode(snippet)
        self.expectSubroutineCall(self.PatchForMultipleLabelSubroutine("ITSDONE"), params=[])

        self.runCode()
        self.assertSubroutineCallsMade()

        self.assertEqual(len(self.failed_assertions), 0)

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testMultiSymbolSubroutineCall4(self):
        # Sigh...
        snippet = """
        .orig x3000
            JSR DONE
            ITSDONE DONE HALT
            RET
        .end
        """
        self.loadCode(snippet)
        self.expectSubroutineCall(self.PatchForMultipleLabelSubroutine("ITSDONE"), params=[])

        self.runCode()
        self.assertSubroutineCallsMade()

        self.assertEqual(len(self.failed_assertions), 0)

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []'''

    def testProgramWithInterrupts(self):
        snippet = """
        .orig x180
            .fill x4000
        .end

        .orig x3000
            LD R0, KBSRIE
            STI R0, KBSR
            SELF BR SELF
            KBSR .fill xFE00
            KBSRIE .fill x4000
        .end

        .orig x4000
            HALT
        .end
        """
        self.loadCode(snippet)

        self.setConsoleInput("ADB")
        self.setInterrupts(True)

        self.runCode(max_executions=10000)
        self.assertPc(0x4000)
        #self.assertEqual(self.state.executions, 102)

    def testProgramWithInterruptsAndDelay(self):
        snippet = """
        .orig x180
            .fill x4000
        .end

        .orig x3000
            LD R0, KBSRIE
            STI R0, KBSR
            SELF BR SELF
            KBSR .fill xFE00
            KBSRIE .fill x4000
        .end

        .orig x4000
            HALT
        .end
        """
        self.loadCode(snippet)

        self.setConsoleInput("ADB")
        self.setInterrupts(True)
        self.setKeyboardInterruptDelay(100)

        self.runCode(max_executions=110)
        self.assertPc(0x4000)
        #self.assertEqual(self.state.executions, 102)

    def testLC3Version0(self):
        snippet = """
        .orig x3000
            AND R0, R0, 0
            ADD R0, R0, -1
            LEA R0, -1
        .end
        """
        self.loadCode(snippet)
        self.setLC3Version(0)

        self.runCode(max_executions=3)
        # LEA should affect the CC.
        self.assertFalse(self.state.n)
        self.assertFalse(self.state.z)
        self.assertTrue(self.state.p)

    def testLC3Version1(self):
        snippet = """
        .orig x3000
            AND R0, R0, 0
            ADD R0, R0, -1
            LEA R0, -1
        .end
        """
        self.loadCode(snippet)
        self.setLC3Version(1)

        self.runCode(max_executions=3)
        # LEA should not affect the CC.
        self.assertTrue(self.state.n)
        self.assertFalse(self.state.z)
        self.assertFalse(self.state.p)

    def testReplayStringPassByRegs(self):
        snippet = """
        .orig x3000
            TATA RET
        .end
        """
        self.enable_compression = True
        self.loadCode(snippet)
        self.asm_filename = 'this_is_a_test.asm'
        self.callSubroutine("TATA", {0: 3, 4: 5})

        blob = self.preconditions.encode()

        expected_blob = b'\x07\x00\x80\x00\x00\x10\x19\x04\x00\x00\x00TATA\n\x00\x00\x00\x00\x00\x03\x00\x04\x00\x05\x00\x05\x00\xfe\xca\x06\x00\x00\xf0\x07\x00\x00\x80\xff'

        self.assertEqual(blob, expected_blob)
        print(self.preconditions.encode())
        print(self.postconditions.encode())
        print(self.generateReplayString())

    def testReplayString(self):
        snippet = """
        .orig x3000
            AHH .blkw 1
            BLAH .fill x4000
            CAWCAW .fill x5000
            PAPA .fill x6000
            TATA RET
        .end
        """
        self.enable_compression = True
        self.init(pylc3.MemoryFillStrategy.fill_with_value, -1)
        self.setTrueTraps(True)
        self.setInterrupts(True)
        self.setPluginsEnabled(True)
        self.setStrictExecution(False)
        self.loadCode(snippet)
        self.asm_filename = 'this_is_a_test.asm'
        self.setLC3Version(1)
        self.setRegister(4, 0x4001)
        self.setPc(0x500)
        self.setValue("AHH", 7)
        self.setPointer("BLAH", 678)
        self.setArray("CAWCAW", [5, 2, 9, 0x3000, 0xFFFF])
        self.setString("PAPA", "MAMA")
        self.setConsoleInput("RAHRAH")
        self.callSubroutine("TATA", [2, 5, 7], r5=5, r6=0x4040, r7=0x8000)
        self.fillValue(0x8000, 33)
        self.fillString(0x9000, "VAVA")
        self.fillArray(0xA000, [1, 32, 729])
        self.fillNode(0xB000, next=None, data=(34,))
        self.fillNode(0xB020, next=0xB000, data=(56,))
        self.fillNode(0xB050, next=[0xB000, 0xB001, 0xB002, 0xB003], data=(72,))
        self.fillData(0xC000, data=(0x9000, 'test', [44, 55], (72, [1, 2, 3])))

        def splitBlob(blob):
            preBlobettes = set()
            postBlobettes = set()
            index = 0
            while index < len(blob):
                t = struct.unpack('=B', blob[index:index+1])[0]
                if t == 16:
                    preBlobettes.add(blob[index:index+1])
                    index += 1
                    break
                elif t < 16:
                    preBlobettes.add(blob[index:index+5])
                    index += 5
                else:
                    self.fail("Index indicates chunk that is not an environment setting")
            while index < len(blob):
                t = struct.unpack('=B', blob[index:index+1])[0]
                if t < 16:
                    self.fail("Index indicates chunk that is an environment setting")
                elif t == 255:
                    postBlobettes.add(blob[index:index+1])
                    index += 1
                else:
                    start = index
                    size = struct.unpack('=I', blob[index+1:index+5])[0]
                    index += 5
                    index += size
                    size = struct.unpack('=I', blob[index:index+4])[0]
                    index += 4 + 2 * size
                    postBlobettes.add(blob[start:index])
            return preBlobettes, postBlobettes

        preBlob, postBlob = splitBlob(self.preconditions.encode())


        expected_preBlob, expected_postBlob = (
                    [b'\x01\x01\x00\x00\x00',
                    b'\x02\x01\x00\x00\x00',
                    b'\x03\x01\x00\x00\x00',
                    b'\x04\x00\x00\x00\x00',
                    b'\x05\x00\x00\x00\x00',
                    b'\x06\xff\xff\xff\xff',
                    b'\x07\x00\x80\x00\x00',
                    b'\x08\x01\x00\x00\x00',
                    b'\x10'],

                    [b'\x11\x01\x00\x00\x004\x01\x00\x00\x00\x01@',
                    b'\x12\x00\x00\x00\x00\x01\x00\x00\x00\x00\x05',
                    b'\x13\x03\x00\x00\x00AHH\x01\x00\x00\x00\x07\x00',
                    b'\x14\x04\x00\x00\x00BLAH\x01\x00\x00\x00\xa6\x02',
                    b'\x15\x06\x00\x00\x00CAWCAW\x05\x00\x00\x00\x05\x00\x02\x00\t\x00\x000\xff\xff',
                    b'\x16\x04\x00\x00\x00PAPA\x04\x00\x00\x00M\x00A\x00M\x00A\x00',
                    b'\x17\x00\x00\x00\x00\x06\x00\x00\x00R\x00A\x00H\x00R\x00A\x00H\x00',
                    b'\x18\x04\x00\x00\x00TATA\x06\x00\x00\x00\x05\x00@@\x00\x80\x02\x00\x05\x00\x07\x00',
                    b'\x1A\x04\x00\x00\x008000\x01\x00\x00\x00!\x00',
                    b'\x1B\x04\x00\x00\x009000\x04\x00\x00\x00V\x00A\x00V\x00A\x00',
                    b'\x1C\x04\x00\x00\x00a000\x03\x00\x00\x00\x01\x00 \x00\xd9\x02',
                    b'\x1D\x04\x00\x00\x00b000\x07\x00\x00\x00\x03\x00\x01\x00\x00\x00\xff\x00\x01\x00"\x00\x00\x00',
                    b'\x1D\x04\x00\x00\x00b020\x07\x00\x00\x00\x03\x00\x01\x00\x00\xb0\xff\x00\x01\x008\x00\x00\x00',
                    b'\x1D\x04\x00\x00\x00b050\n\x00\x00\x00\x03\x00\x04\x00\x00\xb0\x01\xb0\x02\xb0\x03\xb0\xff\x00\x01\x00H\x00\x00\x00',
                    b'\x1E\x04\x00\x00\x00c000\x15\x00\x00\x00\x01\x00\x00\x90\x02\x00\x04\x00t\x00e\x00s\x00t\x00\x03\x00\x02\x00,\x007\x00\xff\x00\x01\x00H\x00\x03\x00\x03\x00\x01\x00\x02\x00\x03\x00\x00\x00',
                    b'\xff'
        ])

        self.assertCountEqual(preBlob, expected_preBlob)
        self.assertCountEqual(postBlob, expected_postBlob)

        print(self.preconditions.encode())
        print(self.postconditions.encode())
        print(self.generateReplayString())

    def testVerificationStringPassByRegs(self):
        snippet = """
        .orig x3000
            TATA RET
        .end
        """
        self.loadCode(snippet)
        self.asm_filename = 'this_is_a_test.asm'
        self.expectSubroutineCall('TATA', {0: 6, 1: 7, 4: 8})
        blob = self.postconditions.encode()

        expected_blob = b'\x12\x02\x04\x00\x00\x00TATA\x06\x00\x00\x00\x00\x00\x06\x00\x01\x00\x07\x00\x04\x00\x08\x00\xff'

        self.assertEqual(blob, expected_blob)

    def testVerificationString(self):
        snippet = """
        .orig x3000
            AHH .blkw 1
            BLAH .fill x4000
            CAWCAW .fill x5000
            PAPA .fill x6000
            TATA RET
        .end
        """
        self.init(pylc3.MemoryFillStrategy.fill_with_value, -1)
        self.expectSubroutineCall('TATA', [6, 7, 8])
        self.loadCode(snippet)

        # Needed for assertRegistersUnchanged this is usually set by runCode.
        for id in range(8):
            self.registers[id] = id * 3

        self.assertHalted(level=lc3_unit_test_case.AssertionType.soft)
        self.assertRegister(6, 0x5030)
        self.assertPc(0x3000)
        self.assertValue('AHH', 0x4000)
        self.assertPointer('BLAH', 0x3456)
        self.assertArray('CAWCAW', [5, 2, 9, 0x3000, 0x5259, 0xFFFF])
        self.assertString('PAPA', 'MAMA')
        self.assertConsoleOutput('RAHRAH')

        self.assertValueAt(0x4000, 0x3444)
        self.assertStringAt(0x6000, "LALA")
        self.assertArrayAt(0x8000, [3, 4, 1024])

        blah = collections.namedtuple('blah', ['num'])
        self.assertNodeAt(0x9000, next=0xA000, data=blah(3))
        self.assertNodeAt(0xA000, next=None, data=blah(37))

        bob = collections.namedtuple('bob', 'addr name arr data')
        self.assertDataAt(0xC000, bob(0x9000, 'test', [44, 55], (72, [1, 2, 3])))

        self.assertSubroutineCallsMade()

        self.assertReturnValue(33)
        self.assertStackManaged(0xF000, 0xCAFE, 0x8000)
        self.assertRegistersUnchanged([2, 4, 6, 7])


        # Clear since these assertions failed.
        self.failed_assertions = []

        def splitBlob(blob):
            blobettes = []
            index = 0
            while index < len(blob):
                id = struct.unpack('=B', blob[index:index+1])[0]
                if id == 0xFF:
                    blobettes.append(b'\xff')
                    break
                start = index
                type = struct.unpack('=B', blob[index+1:index+2])[0]
                index += 2
                if type == 0:
                    index += 4
                elif type == 1:
                    index += 4
                    size = struct.unpack('=I', blob[index:index+4])[0]
                    index += 4 + 2 * size
                elif type == 2:
                    size = struct.unpack('=I', blob[index:index+4])[0]
                    index += 4 + size
                    size = struct.unpack('=I', blob[index:index+4])[0]
                    index += 4 + 2 * size
                blobettes.append(blob[start:index])
            return blobettes

        dataBlobs = splitBlob(self.postconditions.encode())
        expected_blobs = [
            b'\x01\x00\x01\x00\x00\x00',
            b'\x02\x01\x06\x00\x00\x00\x01\x00\x00\x000P',
            b'\x03\x01\x00\x00\x00\x00\x01\x00\x00\x00\x000',
            b'\x04\x02\x03\x00\x00\x00AHH\x01\x00\x00\x00\x00@',
            b'\x05\x02\x04\x00\x00\x00BLAH\x01\x00\x00\x00V4',
            b'\x06\x02\x06\x00\x00\x00CAWCAW\x06\x00\x00\x00\x05\x00\x02\x00\t\x00\x000YR\xff\xff',
            b'\x07\x02\x04\x00\x00\x00PAPA\x04\x00\x00\x00M\x00A\x00M\x00A\x00',
            b'\x08\x01\x00\x00\x00\x00\x06\x00\x00\x00R\x00A\x00H\x00R\x00A\x00H\x00',
            b'\x09\x02\x04\x00\x00\x004000\x01\x00\x00\x00D4',
            b'\x0A\x02\x04\x00\x00\x006000\x04\x00\x00\x00L\x00A\x00L\x00A\x00',
            b'\x0B\x02\x04\x00\x00\x008000\x03\x00\x00\x00\x03\x00\x04\x00\x00\x04',
            b'\x0C\x02\x04\x00\x00\x009000\x07\x00\x00\x00\x03\x00\x01\x00\x00\xa0\xff\x00\x01\x00\x03\x00\x00\x00',
            b'\x0C\x02\x04\x00\x00\x00a000\x07\x00\x00\x00\x03\x00\x01\x00\x00\x00\xff\x00\x01\x00%\x00\x00\x00',
            b'\x0D\x02\x04\x00\x00\x00c000\x15\x00\x00\x00\x01\x00\x00\x90\x02\x00\x04\x00t\x00e\x00s\x00t\x00\x03\x00\x02\x00,\x007\x00\xff\x00\x01\x00H\x00\x03\x00\x03\x00\x01\x00\x02\x00\x03\x00\x00\x00',
            b'\x0E\x01\x00\x00\x00\x00\x01\x00\x00\x00!\x00',
            b'\x0F\x01\xd4\x00\x00\x00\x04\x00\x00\x00\x06\x00\x0c\x00\x12\x00\x15\x00',
            b'\x10\x01\x00\x00\x00\x00\x03\x00\x00\x00\x00\xf0\xfe\xca\x00\x80',
            b'\x11\x02\x04\x00\x00\x00TATA\x03\x00\x00\x00\x06\x00\x07\x00\x08\x00',
            b'\xff'
        ]
        self.assertEqual(dataBlobs, expected_blobs)

    def testGenerateHeaderCompression(self):
        self.asm_filename = 'test.asm'
        self.enable_compression = True
        headerblob = self.generateReplayHeader(b'')
        expected = b'lc-3\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x08\x00\x00\x00test.asm'

        self.assertEqual(headerblob, expected)

    def testGenerateHeaderNoCompression(self):
        self.asm_filename = 'test.asm'
        self.enable_compression = False
        headerblob = self.generateReplayHeader(b'')
        expected = b'lc-3\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00test.asm'

        self.assertEqual(headerblob, expected)

    def testGenerateReplayString(self):
        snippet = """
        .orig x3000
            TATA RET
        .end
        """
        self.loadCode(snippet)
        self.asm_filename = 'test'
        self.callSubroutine("TATA", {0: 3, 4: 5})

        blob = self.preconditions.encode()
        expected_preblob = b'\x07\x00\x80\x00\x00\x10\x19\x04\x00\x00\x00TATA\n\x00\x00\x00\x00\x00\x03\x00\x04\x00\x05\x00\x05\x00\xfe\xca\x06\x00\x00\xf0\x07\x00\x00\x80\xff'
        self.assertEqual(blob, expected_preblob)

        blob = self.postconditions.encode()
        expected_postblob = b'\xff'
        self.assertEqual(blob, expected_postblob)

        header = self.generateReplayHeader(expected_preblob + expected_postblob)
        expected_header = b'lc-3\x01\x00\x00\x00\x00\x00\x00\x00)\x00\x00\x00\x16\xd5A\x0b\x00\x04\x00\x00\x00test'
        self.assertEqual(header, expected_header)

        expected_data = b'lc-3\x01\x00\x00\x00\x00\x00\x00\x00)\x00\x00\x00\x16\xd5A\x0b\x00\x04\x00\x00\x00test\x07\x00\x80\x00\x00\x10\x19\x04\x00\x00\x00TATA\n\x00\x00\x00\x00\x00\x03\x00\x04\x00\x05\x00\x05\x00\xfe\xca\x06\x00\x00\xf0\x07\x00\x00\x80\xff\xff'
        replay = self.generateReplayString()

        self.assertEqual(base64.b64decode(replay), expected_data)
        self.assertEqual(replay, base64.b64encode(expected_data))

        expected_replay = b'bGMtMwEAAAAAAAAAKQAAABbVQQsABAAAAHRlc3QHAIAAABAZBAAAAFRBVEEKAAAAAAADAAQABQAFAP7KBgAA8AcAAID//w=='
        self.assertEqual(replay, expected_replay)

    # -----------------------------------
    # ---- Internal tests begin here ----
    # -----------------------------------
    def testTupleToData(self):
        data = lc3_unit_test_case.tuple_to_data((1,))
        self.assertEqual(data, [DataItem.number, 1])

        data = lc3_unit_test_case.tuple_to_data(("LOL",))
        self.assertEqual(data, [DataItem.string, 3, "LOL"])

        data = lc3_unit_test_case.tuple_to_data(([1, 2, 3],))
        self.assertEqual(data, [DataItem.array, 3, [1, 2, 3]])

        data = lc3_unit_test_case.tuple_to_data((("LOL",),))
        self.assertEqual(data, [DataItem.data, DataItem.string, 3, "LOL", DataItem.end_of_data])

        data = lc3_unit_test_case.tuple_to_data(("Some Student", 76, [100, 52]))
        self.assertEqual(data, [DataItem.string, 12, "Some Student", DataItem.number, 76, DataItem.array, 2, [100, 52]])

        data = lc3_unit_test_case.tuple_to_data((1, 'test', [1, 44, -1, 0x8000, 0xFFFF, 32767], (5, ('test2', [44], (8,), 33))))
        self.assertEqual(data, [
            DataItem.number, 1,
            DataItem.string, 4, 'test',
            DataItem.array, 6, [1, 44, -1, 0x8000, 0xFFFF, 32767],

            DataItem.data,
                DataItem.number, 5,
                DataItem.data,
                    DataItem.string, 5, 'test2',
                    DataItem.array, 1, [44],
                    DataItem.data,
                        DataItem.number, 8,
                    DataItem.end_of_data,
                DataItem.number, 33,
                DataItem.end_of_data,
            DataItem.end_of_data])

    def testTupleToDataSpec(self):
        data = lc3_unit_test_case.tuple_to_data_spec((1,))
        self.assertEqual(data, [DataItem.number])

        data = lc3_unit_test_case.tuple_to_data_spec(("LOL",))
        self.assertEqual(data, [DataItem.string, 3])

        data = lc3_unit_test_case.tuple_to_data_spec(([1, 2, 3],))
        self.assertEqual(data, [DataItem.array, 3])

        data = lc3_unit_test_case.tuple_to_data_spec((("LOL",),))
        self.assertEqual(data, [DataItem.data, DataItem.string, 3, DataItem.end_of_data])

        data = lc3_unit_test_case.tuple_to_data_spec(("Some Student", 76, [100, 52]))
        self.assertEqual(data, [DataItem.string, 12, DataItem.number, DataItem.array, 2])

        data = lc3_unit_test_case.tuple_to_data_spec((1, 'test', [1, 44, -1, 0x8000, 0xFFFF, 32767], (5, ('test2', [44], (8,), 33))))
        self.assertEqual(data, [
            DataItem.number,
            DataItem.string, 4,
            DataItem.array, 6,

            DataItem.data,
                DataItem.number,
                DataItem.data,
                    DataItem.string, 5,
                    DataItem.array, 1,
                    DataItem.data,
                        DataItem.number,
                    DataItem.end_of_data,
                DataItem.number,
                DataItem.end_of_data,
            DataItem.end_of_data])

    def testReadData(self):
        self._writeMem(0x3000, 1)

        self._writeMem(0x3001, ord('t'))
        self._writeMem(0x3002, ord('e'))
        self._writeMem(0x3003, ord('s'))
        self._writeMem(0x3004, ord('t'))
        self._writeMem(0x3005, 0)

        self._writeMem(0x3006, 1)
        self._writeMem(0x3007, 44)
        self._writeMem(0x3008, -1)
        self._writeMem(0x3009, -32768)
        self._writeMem(0x300A, -1)
        self._writeMem(0x300B, 32767)

        self._writeMem(0x300C, 5)

        self._writeMem(0x300D, ord('t'))
        self._writeMem(0x300E, ord('e'))
        self._writeMem(0x300F, ord('s'))
        self._writeMem(0x3010, ord('t'))
        self._writeMem(0x3011, ord('2'))
        self._writeMem(0x3012, 0)

        self._writeMem(0x3013, 44)

        self._writeMem(0x3014, 8)

        self._writeMem(0x3015, 9901)
        data = self._readData(0x3000, (0, 'xxxx', [0] * 6, (0, ('xxxxx', [0], (0,), 0))))

        self.assertEqual(data, (1, six.u('test\0'), [1, 44, -1, -32768, -1, 32767], (5, (six.u('test2\0'), [44], (8,), 9901))))

    def testWriteData(self):
        data = (1, 'test', [1, 44, -1, 0x8000, 0xFFFF, 32767], (5, ('test2', [44], (8,), 9901)))
        self._writeData(0x3000, data)

        self.assertEqual(self._readMem(0x3000), 1)

        self.assertEqual(self._readMem(0x3001), ord('t'))
        self.assertEqual(self._readMem(0x3002), ord('e'))
        self.assertEqual(self._readMem(0x3003), ord('s'))
        self.assertEqual(self._readMem(0x3004), ord('t'))
        self.assertEqual(self._readMem(0x3005), 0)

        self.assertEqual(self._readMem(0x3006), 1)
        self.assertEqual(self._readMem(0x3007), 44)
        self.assertEqual(self._readMem(0x3008), -1)
        self.assertEqual(self._readMem(0x3009), -32768)
        self.assertEqual(self._readMem(0x300A), -1)
        self.assertEqual(self._readMem(0x300B), 32767)

        self.assertEqual(self._readMem(0x300C), 5)

        self.assertEqual(self._readMem(0x300D), ord('t'))
        self.assertEqual(self._readMem(0x300E), ord('e'))
        self.assertEqual(self._readMem(0x300F), ord('s'))
        self.assertEqual(self._readMem(0x3010), ord('t'))
        self.assertEqual(self._readMem(0x3011), ord('2'))
        self.assertEqual(self._readMem(0x3012), 0)

        self.assertEqual(self._readMem(0x3013), 44)

        self.assertEqual(self._readMem(0x3014), 8)

        self.assertEqual(self._readMem(0x3015), 9901)

    def testHardAssertion(self):
        snippet = """
        .orig x3000
            HALT
            A .fill 3
            B .fill 5
            C .fill 7
        .end
        """
        self.loadCode(snippet)
        self.runCode(max_executions=1)

        self.assertValue("A", 2, level=lc3_unit_test_case.AssertionType.hard)
        self.assertValue("B", 3)
        self.assertValue("C", 4)

        names = [tup[0] for tup in self.failed_assertions]
        msgs = [tup[1] for tup in self.failed_assertions]

        self.assertEqual(names, ['value: A', 'value: B', 'value: C'])
        self.assertIn('MEM[A] was expected to be (2 x0002) but code produced (3 x0003)\n', msgs[0])
        self.assertIn('Not checked due to previous failures.', msgs[1])
        self.assertIn('Not checked due to previous failures.', msgs[2])

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []

    def testSoftAssertion(self):
        snippet = """
        .orig x3000
            HALT
            A .fill 3
            B .fill 5
            C .fill 7
        .end
        """
        self.loadCode(snippet)
        self.runCode(max_executions=1)

        self.assertValue("A", 3, level=lc3_unit_test_case.AssertionType.hard)
        self.assertValue("B", 3)
        self.assertValue("C", 4)

        names = [tup[0] for tup in self.failed_assertions]
        msgs = [tup[1] for tup in self.failed_assertions]

        self.assertEqual(names, ['value: B', 'value: C'])
        self.assertIn('MEM[B] was expected to be (3 x0003) but code produced (5 x0005)\n', msgs[0])
        self.assertIn('MEM[C] was expected to be (4 x0004) but code produced (7 x0007)\n', msgs[1])

        # Clear so that the test doesn't fail during tearDown.
        self.failed_assertions = []


if __name__ == '__main__':
    logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)
    unittest.main()