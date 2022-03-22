import unittest,importlib,BeautifulReport,cff

def load():
    src = open("./Test/src/extract.c").read()

    builder = cffi.FFI()

    builder.set_source("addLib",src)
    builder.compile()
    md = importlib.import_module("addLib")

    return md.lib

md = load()

class AddTestCase(unittest.TestCase):

    def test1(self):
        self.assert