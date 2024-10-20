#!/usr/bin/python
from csp.tools.data.compile import Compiler
from csp.tools.layout2.scripts.ui.commands.CloudBoxWizardCommand import CloudBoxException
from csp.tools.layout2.scripts.ui.commands.CloudBoxWizardCommand import CloudBoxGenerator
from lxml.etree import tostring as toxml
from lxml.builder import E as tag
import os
import os.path
import shutil
import tempfile
import unittest

class CloudBoxTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.mkdtemp()

        f = open(os.path.join(self.directory, "color_levels.xml"), "w")
        f.write(toxml(tag.Object({"class" : "CloudColorLevels"})))
        f.close()

        f = open(os.path.join(self.directory, "opacity_levels.xml"), "w")
        f.write(toxml(tag.Object({"class" : "CloudOpacityLevels"})))
        f.close()

        f = open(os.path.join(self.directory, "lod.xml"), "w")
        f.write(toxml(tag.Object({"class" : "CloudLOD"})))
        f.close()

    def tearDown(self):
        shutil.rmtree(self.directory)

    def testInputDirectory(self):
        generator = CloudBoxGenerator()
        generator.directory = "directory doesnt exist"
        self.assertRaises(IOError, generator.generate)

    def testInputFilename(self):
        generator = CloudBoxGenerator()
        generator.directory = self.directory
        generator.filename = "no xml extension"
        self.assertRaises(CloudBoxException, generator.generate)

    def testCreateAndCompile(self):
        generator = CloudBoxGenerator()
        generator.directory = self.directory
        generator.filename = "my_cloud_box.xml"
        generator.color_path = "my_file:color_levels"
        generator.opacity_path = "my_file:opacity_levels"
        generator.lod_path = "my_file:lod"
        xml = generator.generate()
        print(xml)

        self.assertTrue(os.path.exists(os.path.join(self.directory, "my_cloud_box.xml")), "cloud box file wasn't created")

        # Collect compiler errors in this variable.
        self.comp_errors = ""
        def printCompilerMessage(event):
            self.comp_errors = self.comp_errors + event.GetMessage() + "\n"

        dar = os.path.join(self.directory, "my_file.dar")
        compiler = Compiler()
        compiler.GetCompilerSignal().Connect(printCompilerMessage)
        self.assertTrue(compiler.CompileAll(self.directory, dar), "Compilation failed for xml: %s" % self.comp_errors)

if __name__ == '__main__':
    unittest.main()
