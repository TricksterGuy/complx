import comp
import unittest


class CompTest(unittest.TestCase):
    def setUp(self):
        self.comp = comp.Comp(show_help=False)

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



if __name__ == '__main__':
    unittest.main()
