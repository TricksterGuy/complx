import comp
import pylc3
import unittest


class CompTest(unittest.TestCase):
    def setUp(self):
        self.state = pylc3.LC3State()
        self.comp = comp.Comp(lc3_state=self.state, show_help=False, disable_message=True)

    def testParse(self):
        param = self.comp.parse("")
        self.assertEqual(param, tuple())

        param = self.comp.parse("1", int)
        self.assertEqual(param, (1,))

        param = self.comp.parse("1", int, num_required=0)
        self.assertEqual(param, (1,))

        param = self.comp.parse("", int, num_required=0)
        self.assertEqual(param, (None,))

        param = self.comp.parse("", int, num_required=0, defaults=[1])
        self.assertEqual(param, (1,))

        param = self.comp.parse("hello", int)
        self.assertIsNone(param)

        param = self.comp.parse("1 hello", int, str)
        self.assertEqual(param, (1, "hello"))

        param = self.comp.parse("1 hello", int, str, num_required=0)
        self.assertEqual(param, (1, "hello"))

        param = self.comp.parse("", int, str, num_required=0)
        self.assertEqual(param, (None, None))

        param = self.comp.parse("", int, str, num_required=0, defaults=[0, ""])
        self.assertEqual(param, (0, ""))

        param = self.comp.parse("1 hello", int, str, num_required=1)
        self.assertEqual(param, (1, "hello"))

        param = self.comp.parse("", int, str, num_required=1)
        self.assertIsNone(param)

        param = self.comp.parse("", int, str, num_required=1, defaults=[""])
        self.assertIsNone(param)

        param = self.comp.parse("1", int, str, num_required=1)
        self.assertEqual(param, (1, None))

        param = self.comp.parse("1", int, str, num_required=1, defaults=[""])
        self.assertEqual(param, (1, ""))

    def testExecute(self):
        sample_prog = """
        .orig x3000
            AND R0, R0, 0
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            HALT
        .end
        """
        self.state.loadCode(sample_prog)
        self.state.max_undo_stack_size = 10

        self.comp.onecmd("step")
        self.assertEqual(self.state.r0, 0)

        self.comp.onecmd("step 2")
        self.assertEqual(self.state.r0, 2)

        self.comp.onecmd("back")
        self.assertEqual(self.state.r0, 1)

        self.comp.onecmd("step 3")
        self.comp.onecmd("back 2")
        self.assertEqual(self.state.r0, 2)

        self.comp.onecmd("next")
        self.assertEqual(self.state.r0, 3)

        self.comp.onecmd("previous")
        self.assertEqual(self.state.r0, 2)

        self.comp.onecmd("run")
        self.assertEqual(self.state.r0, 10)
        self.assertTrue(self.state.halted)

        self.comp.onecmd("rewind")
        self.assertTrue(self.state.r0, 2)

        self.comp.onecmd("finish")
        self.assertTrue(self.state.r0, 10)
        self.assertTrue(self.state.halted)

    def testExecuteAdvanced(self):
        sample_prog = """
        .orig x3000
            AND R0, R0, 0
            JSR DUMMY_SUB
            JSR DUMMY_SUB
            JSR DUMMY_SUB2
            HALT

            DUMMY_SUB
                RET
            
            DUMMY_SUB2
                ADD R0, R0, 1
                RET
        .end
        """
        self.state.loadCode(sample_prog)
        self.state.max_undo_stack_size = 10

        self.comp.onecmd("step")
        self.assertEqual(self.state.r0, 0)

        self.comp.onecmd("step")
        self.assertEqual(self.state.pc, 0x3005)

        self.comp.onecmd("step")
        self.assertEqual(self.state.pc, 0x3002)

        self.comp.onecmd("previous")
        self.assertEqual(self.state.pc, 0x3001)

        self.comp.onecmd("next 2")
        self.assertEqual(self.state.pc, 0x3003)

        self.comp.onecmd("next")
        self.assertEqual(self.state.r0, 1)

        self.comp.onecmd("previous")
        self.comp.onecmd("step")
        self.assertEqual(self.state.pc, 0x3006)

        self.comp.onecmd("finish")
        self.assertEqual(self.state.pc, 0x3004)
        self.assertFalse(self.state.halted)

    def testBreakpoints(self):
        sample_prog = """
        .orig x3000
            AND R0, R0, 0
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            STOP_HERE ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            DONT_STOP_HERE ADD R0, R0, 1
            ADD R0, R0, 1
            HALT
        .end
        """
        self.state.loadCode(sample_prog)

        self.comp.onecmd("breakpoint STOP_HERE")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x3005)
        breakpoint = self.state.breakpoints[0x3005]
        self.assertEqual(breakpoint.hit_count, 1)


        self.comp.onecmd("tempbreak x3006")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x3006)
        breakpoint = self.state.breakpoints[0x3006]
        self.assertEqual(breakpoint.hit_count, 1)

        self.comp.onecmd("back")
        self.comp.onecmd("step 2")
        self.assertEqual(self.state.pc, 0x3008)
        self.assertEqual(breakpoint.hit_count, 1)

        self.comp.onecmd("breakpoint DONT_STOP_HERE 'R0 == 25'")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x300b)
        self.assertTrue(self.state.halted)
        breakpoint = self.state.breakpoints[0x3009]
        self.assertEqual(breakpoint.hit_count, 0)

    def testRegisterWatchpoints(self):
        sample_prog = """
        .orig x3000
            AND R0, R0, 0
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R0, R0, 1
            ADD R1, R0, 0
            HALT
        .end
        """
        self.state.loadCode(sample_prog)

        self.comp.onecmd("watchpoint R0 'R0 == 4'")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x3005)
        watchpoint = self.state.register_watchpoints[0]
        self.assertEqual(watchpoint.hit_count, 1)

        self.comp.onecmd("watchpoint R1 'R1 == 25'")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x300c)
        self.assertTrue(self.state.halted)
        watchpoint = self.state.register_watchpoints[1]
        self.assertEqual(watchpoint.hit_count, 0)

    def testMemoryWatchpoints(self):
        sample_prog = """
        .orig x3000
            AND R0, R0, 0
            ADD R0, R0, 1
            ST R0, ADDR
            STI R0, IND_ADDR
            HALT
            ADDR .blkw 1
            IND_ADDR .fill x4000
        .end
        """
        self.state.loadCode(sample_prog)

        self.comp.onecmd("watchpoint ADDR 'MEM[ADDR] == 1'")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x3003)
        watchpoint = self.state.memory_watchpoints[0x3005]
        self.assertEqual(watchpoint.hit_count, 1)

        self.comp.onecmd("watchpoint x4000 'MEM[x4000] != 1'")
        self.comp.onecmd("run")
        self.assertEqual(self.state.pc, 0x3004)
        self.assertTrue(self.state.halted)
        watchpoint = self.state.memory_watchpoints[0x4000]
        self.assertEqual(watchpoint.hit_count, 0)
  

if __name__ == '__main__':
    unittest.main()
