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
        self.assertEqual(self.lookup("A"), 0x3005)
        self.assertEqual(self.readMem(0x3005), 2)

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
        self.assertEqual(self.lookup("A"), 0x3005)
        self.assertEqual(self.readMem(0x3005), 0x4000)
        self.assertEqual(self.readMem(0x4000), 2)

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
        self.assertEqual(self.lookup("ARR"), 0x3003)
        self.assertEqual(self.readMem(0x3003), 0x4000)
        self.assertEqual(self.readMem(0x3004), 0x4002)
        self.assertEqual(self.readMem(0x4000), 5)
        self.assertEqual(self.readMem(0x4001), 3)
        self.assertEqual(self.readMem(0x4002), 1)
        self.assertEqual(self.readMem(0x4003), 666)

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
        self.assertEqual(self.lookup("STRING_LOC"), 0x3004)
        self.assertEqual(self.lookup("STRING"), 0x3005)
        self.assertEqual(self.readMem(0x3004), 0x3005)
        self.assertEqual(self.readMem(0x3005), ord('H'))
        self.assertEqual(self.readMem(0x3006), ord('E'))
        self.assertEqual(self.readMem(0x3007), ord('L'))
        self.assertEqual(self.readMem(0x3008), ord('L'))
        self.assertEqual(self.readMem(0x3009), ord('O'))
        self.assertEqual(self.readMem(0x300A), 0)

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
        self.assertEqual(self.lookup("RETURN7"), 0x3000)
        self.assertEqual(self.readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self.readReg(6, unsigned=True), 0xF000)
        self.assertEqual(self.readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(7)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFF, answer=7, return_address=0x8000, old_frame_pointer=0xCAFE)
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
        self.assertEqual(self.lookup("MYADD"), 0x4000)
        self.assertEqual(self.readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self.readReg(6, unsigned=True), 0xEFFD)
        self.assertEqual(self.readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(211)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFC, answer=211, return_address=0x8000, old_frame_pointer=0xCAFE)
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
        # For assertSubrotuineCallMade.
        self.expectSubroutineCall("B", params=[3, 5])
        self.expectSubroutineCall("B", params=[3, 7])
        self.expectSubroutineCall("B", params=[5, 7])
        self.expectSubroutineCall("C", params=[3])
        # Don't call expectTrapCall here since A doesn't directly use it. Only the first level calls are logged.

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(72)
        self.assertRegistersUnchanged([5, 7])
        self.assertStackManaged(stack=0xEFFC, answer=72, return_address=0x8000, old_frame_pointer=0xCAFE)
        self.assertSubroutineCallsMade()
        self.assertReadAnswersFromCalls()

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
        self.assertStackManaged(stack=0xEFFE, answer=40, return_address=0x8000, old_frame_pointer=0xCAFE)
        # We didn't call return7 but thats okay it was optional.
        self.assertSubroutineCallsMade()
        self.assertReadAnswersFromCalls()

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
        self.assertStackManaged(stack=0xEFFE, answer=40, return_address=0x8000, old_frame_pointer=0xCAFE)
        self.assertSubroutineCallsMade()
        self.assertReadAnswersFromCalls()

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
        self.assertEqual(self.lookup("MYADD"), 0x4000)
        self.assertEqual(self.readReg(5, unsigned=True), 0xCAFE)
        self.assertEqual(self.readReg(6, unsigned=True), 0xEFFD)
        self.assertEqual(self.readReg(7, unsigned=True), 0x8000)

        self.runCode()
        self.assertReturned()
        self.assertNoWarnings()
        self.assertReturnValue(211)
        # Then they should all be unchanged.
        self.assertRegistersUnchanged([0, 1, 2, 3, 4, 5, 7])
        self.assertStackManaged(stack=0xEFFC, answer=211, return_address=0x8000, old_frame_pointer=0xCAFE)
        # Checks that no subroutines were called.
        self.assertSubroutineCallsMade()

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
        self.init(value=-1)
        self.setTrueTraps(True)
        self.setInterrupts(True)
        self.setPluginsEnabled(True)
        self.setStrictExecution(False)
        self.loadCode(snippet)
        self.setRegister(7, 0x4001)
        self.setPc(0x500)
        self.setValue("AHH", 7)
        self.setPointer("BLAH", 678)
        self.setArray("CAWCAW", [5, 2, 9, 0x3000, 0xFFFF])
        self.setString("PAPA", "MAMA")
        self.setConsoleInput("RAHRAH")
        self.callSubroutine("TATA", [2, 5, 7], r5=5, r6=0x4004, r7=0x8000)

        blob = self.preconditions._formBlob()

        self.assertEqual(blob,
            '\x01\x01\x00\x00\x00'
            '\x02\x01\x00\x00\x00'
            '\x03\x01\x00\x00\x00'
            '\x04\x00\x00\x00\x00'
            '\x05\x00\x00\x00\x00'
            '\x06\xff\xff\xff\xff'
            '\x07\x00\x80\x00\x00'
            '\x11\x01\x00\x00\x007\x01\x00\x00\x00\x01@'
            '\x12\x00\x00\x00\x00\x01\x00\x00\x00\x00\x05'
            '\x13\x03\x00\x00\x00AHH\x01\x00\x00\x00\x07\x00'
            '\x14\x04\x00\x00\x00BLAH\x01\x00\x00\x00\xa6\x02'
            '\x15\x06\x00\x00\x00CAWCAW\x05\x00\x00\x00\x05\x00\x02\x00\t\x00\x000\xff\xff'
            '\x16\x04\x00\x00\x00PAPA\x04\x00\x00\x00M\x00A\x00M\x00A\x00'
            '\x17\x00\x00\x00\x00\x06\x00\x00\x00R\x00A\x00H\x00R\x00A\x00H\x00'
            '\x18\x04\x00\x00\x00TATA\x06\x00\x00\x00\x05\x00\x04@\x00\x80\x02\x00\x05\x00\x07\x00')


if __name__ == '__main__':
    unittest.main()
