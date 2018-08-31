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
        self.runCode()
        self.assertHalted()
        self.assertValue("A", 2)
        self.assertValue("B", 3)
        self.assertValue("ANS", 5)





if __name__ == '__main__':
    unittest.main()
