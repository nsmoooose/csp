from .Command import Command
from lxml.etree import tostring as toxml
from lxml.builder import E as tag
import math
import os.path
import random
import wx
from csp.tools.layout2.scripts.ui.controls.ProjectTree import ProjectTree

class CloudBoxException(Exception):
    pass

class CloudBoxGenerator(object):
    def __init__(self):
        self.filename = "cloudbox01.xml"
        self.directory = ""
        self.width = 70
        self.depth = 70
        self.height = 20
        self.sprite_count = 40
        self.sprite_size = 10
        self.lod_path = ""
        self.color_path = ""
        self.opacity_path = ""
        self.ellipsoid = True

    def generate(self):
        # Things to improve:
        # * Remove sprites that are too close. What distance should this be?
        #
        # * Sprites near the center of the cloudbox could be much larger than
        #   the ones near the edges? This to reduce the number of sprites
        #   needed.

        if not os.path.exists(self.directory):
            raise IOError("Directory %s not found" % self.directory)

        if not self.filename.endswith(".xml"):
            raise CloudBoxException("Filename doesn't have .xml extension")

        class Sprite(object):
            def __init__(self, position, rotation, size):
                self.position = position
                self.rotation = rotation
                self.size = size

        sprites = []

        def inside_ellipsoid(x, y, z):
            value1 = \
                (x * x) / (self.width * self.width) + \
                (y * y) / (self.depth * self.depth)
            value2 = \
		(x * x) / (self.width * self.width) + \
		(z * z) / (self.height * self.height)
            value3 = \
		(y * y) / (self.depth * self.depth) + \
		(z * z) / (self.height * self.height)
            return value1 < 1 and value2 < 1 and value3 < 1

        def inside_box(x, y, z):
            return True

        bounds_checker = inside_box
        if self.ellipsoid:
            bounds_checker = inside_ellipsoid

        # first we generate X number of sprites.
        while len(sprites) < self.sprite_count:
            x = random.uniform(0 - self.width, self.width)
            y = random.uniform(0 - self.depth, self.depth)
            z = random.uniform(0 - self.height, self.height)

            if bounds_checker(x, y, z):
                rotation = random.uniform(0, 6.28)
                width = self.sprite_size
                height = self.sprite_size
                sprites.append(Sprite((x, y, z), rotation, (width, height)))

        # This section provides detection if sprites are too close to each
        # other. Commented it out for the time beeing since it doesn't
        # work well. Wonder what distance is best to use?
        # index = len(sprites) - 1
        # while index >= 0:
        #     index2 = len(sprites) - 1
        #
        #     while index2 >= 0:
        #         # don't compare with myself
        #         if index != index2:
        #             s1 = sprites[index]
        #             s2 = sprites[index2]
        #             delta = (
        #                 s1.position[0] - s2.position[0],
        #                 s1.position[1] - s2.position[1],
        #                 s1.position[2] - s2.position[2])
        #             distance = math.sqrt(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2])
        #             if distance < self.sprite_size:
        #                 # print("Removing sprite since distance was only: %.2f" % distance)
        #                 del sprites[index]
        #                 break
        #         index2 = index2 - 1
        #
        #     index = index - 1

        with open(os.path.join(self.directory, self.filename), "w") as f:
            xml = tag.Object(
                {"class" : "CloudBox"},
                tag.Path(self.lod_path, name="lod"),
                tag.Path(self.color_path, name="color"),
                tag.Path(self.opacity_path, name="opacity"),
                tag.Vector3("%d %d %d" % (self.width, self.depth, self.height), name="size"),
                tag.List(*tuple([(
                                tag.Object({"class":"CloudSprite"},
                                           tag.Vector3("%.1f %.1f %.1f" %
                                                       (sprite.position[0],
                                                        sprite.position[1],
                                                        sprite.position[2]),
                                                       name="position"),
                                           tag.Float("%.1f" % sprite.rotation,
                                                     name="rotation"),
                                           tag.Vector2("%.1f %.1f" %
                                                       (sprite.size[0],
                                                        sprite.size[1]),
                                                       name="size"))
                                ) for sprite in sprites]), name="sprites")
                )
            xml_str = toxml(xml, pretty_print=True)
            f.write(xml_str)
            f.flush()
            return xml_str

class CloudBoxWizard(wx.Dialog):
    keys = [
        "filename", "width", "depth", "height",
        "sprite_count", "sprite_size", "color_path", "opacity_path",
        "lod_path", "ellipsoid" ]

    def __init__(self, directory, *args, **kwargs):
        wx.Dialog.__init__(self, *args, **kwargs)

        sizerPanel = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizerPanel)

        panel = wx.Panel(self)
        sizerPanel.Add(panel, flag=wx.EXPAND)

        grid = wx.FlexGridSizer(11, 2)
        panel.SetSizer(grid)

        application = wx.GetApp()
        self.generator = CloudBoxGenerator()
        self.GetLastValues(self.generator, self.keys)

        self.filename = self.AddControlPair(panel, grid, "Filename:", self.generator.filename)
        self.directory = self.AddControlPair(panel, grid, "Directory:", directory)
        self.width = self.AddControlPair(panel, grid, "Width:", str(self.generator.width))
        self.depth = self.AddControlPair(panel, grid, "Depth:", str(self.generator.depth))
        self.height = self.AddControlPair(panel, grid, "Height:", str(self.generator.height))
        self.sprite_count = self.AddControlPair(panel, grid, "Sprite count:", str(self.generator.sprite_count))
        self.sprite_size = self.AddControlPair(panel, grid, "Sprite size:", str(self.generator.sprite_size))
        self.color_path = self.AddControlPair(panel, grid, "CloudColorLevels path:", self.generator.color_path)
        self.opacity_path = self.AddControlPair(panel, grid, "CloudOpacityLevels path:", self.generator.opacity_path)
        self.lod_path = self.AddControlPair(panel, grid, "CloudLOD path:", self.generator.lod_path)
        self.ellipsoid = self.AddControlPair(panel, grid, "Ellipsoid:", "1")

        buttonOK = wx.Button(self, id = wx.ID_OK, label = "OK")
        buttonOK.Bind(wx.EVT_BUTTON, self.onOK_Click)
        sizerPanel.AddSpacer(10)
        sizerPanel.Add(buttonOK)

        buttonCANCEL = wx.Button(self, id = wx.ID_CANCEL, label = "Cancel")
        sizerPanel.Add(buttonCANCEL)

    def GetLastValues(self, obj, keys):
        application = wx.GetApp()
        for key in keys:
            fullKey = "LayoutApplication.CloudBoxWizard." + key
            if fullKey in application.Configuration:
                setattr(obj, key, application.Configuration[fullKey])

    def StoreLastValues(self, obj, keys):
        application = wx.GetApp()
        for key in keys:
            fullKey = "LayoutApplication.CloudBoxWizard." + key
            application.Configuration[fullKey] = getattr(obj, key)

    def AddControlPair(self, parent, grid, label, default=''):
        grid.Add(wx.StaticText(parent, wx.ID_ANY, label), flag=wx.ALIGN_CENTER_VERTICAL )
        control = wx.TextCtrl(parent, wx.ID_ANY, default)
        control.SetMinSize(wx.Size(200, control.GetSize().GetHeight()))
        grid.Add(control, flag=wx.EXPAND)
        return control

    def onOK_Click(self, event):
        self.generator.filename = self.filename.GetValue()
        self.generator.directory = self.directory.GetValue()
        self.generator.width = int(self.width.GetValue())
        self.generator.depth = int(self.depth.GetValue())
        self.generator.height = int(self.height.GetValue())
        self.generator.sprite_count = int(self.sprite_count.GetValue())
        self.generator.sprite_size = int(self.sprite_size.GetValue())
        self.generator.lod_path = self.lod_path.GetValue()
        self.generator.color_path = self.color_path.GetValue()
        self.generator.opacity_path = self.opacity_path.GetValue()
        self.generator.ellipsoid = True if self.ellipsoid.GetValue() == "1" else False
        self.generator.generate()
        self.StoreLastValues(self.generator, self.keys)
        self.Close()

class CloudBoxWizardCommand(Command):
    """Starts a wizard that helps you define a new cloud box object
    and store it into an xml file."""

    caption = "CloudBox wizard"
    tooltip = "CloudBox wizard lets you create a cloudbox xml file"
    toolbarimage = "generic.png"

    def Execute(self):
        dlg = CloudBoxWizard(ProjectTree.Instance.GetSelectedFile(), None, title=self.caption, style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.MAXIMIZE_BOX)
        dlg.ShowModal()
        dlg.Destroy()

    @staticmethod
    def Enabled():
        path = ProjectTree.Instance.GetSelectedFile()
        return os.path.isdir(path)
