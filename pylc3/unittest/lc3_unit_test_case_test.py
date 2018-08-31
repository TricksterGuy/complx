import lc3_unit_test_case
import unittest


class LC3UnitTestCaseTest(lc3_unit_test_case.LC3UnitTestCase):

    def loadCode(self, snippet):
        self.state.loadCode(snippet)

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
        self.assertEqual(self.state.lookup("A"), 0x3005)
        self.assertEqual(self.state.get_memory(0x3005), 2)

        self.runCode()
        self.assertHalted()
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
        self.assertEqual(self.state.lookup("A"), 0x3005)
        self.assertEqual(self.state.get_memory(0x3005), 0x4000)
        self.assertEqual(self.state.get_memory(0x4000), 2)

        self.runCode()
        self.assertHalted()
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
        self.assertEqual(self.state.lookup("ARR"), 0x3003)
        self.assertEqual(self.state.get_memory(0x3003), 0x4000)
        self.assertEqual(self.state.get_memory(0x3004), 0x4002)
        self.assertEqual(self.state.get_memory(0x4000), 5)
        self.assertEqual(self.state.get_memory(0x4001), 3)
        self.assertEqual(self.state.get_memory(0x4002), 1)
        self.assertEqual(self.state.get_memory(0x4003), 666)

        self.runCode()
        self.assertHalted()
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
        self.assertEqual(self.state.lookup("STRING_LOC"), 0x3004)
        self.assertEqual(self.state.lookup("STRING"), 0x3005)
        self.assertEqual(self.state.get_memory(0x3004), 0x3005)
        self.assertEqual(self.state.get_memory(0x3005), ord('H'))
        self.assertEqual(self.state.get_memory(0x3006), ord('E'))
        self.assertEqual(self.state.get_memory(0x3007), ord('L'))
        self.assertEqual(self.state.get_memory(0x3008), ord('L'))
        self.assertEqual(self.state.get_memory(0x3009), ord('O'))
        self.assertEqual(self.state.get_memory(0x300A), 0)

        self.runCode()
        self.assertHalted()
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
        self.assertConsoleOutput("BEC")


if __name__ == '__main__':
    unittest.main()
