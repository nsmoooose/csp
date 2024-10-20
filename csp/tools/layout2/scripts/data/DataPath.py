import os

class DataPath:
    """
    A class for managing file and xml paths in the csp xml data hierarchy.  The
    hierarchy has a root path, and the file path is any file ending with a
    '.xml' extension under that root.  An xml path (or "data path" for lack
    of a better term) is a special notation for referring to such files in
    a platform neutral way.  Forward or backward slashes are replaced by '.'
    and the '.xml' extension is stripped.  All path components above the
    xml root are also removed.  A leading '.' indicates an "absolute" xml
    path.  For example, if the root is "/a/b/c" then the absolute xml path of
    "/a/b/c/d/e.xml" is just ".d.e".  Similarly, if "c:\\xml" is the root then
    "c:\\xml\\a\\b\\c.xml" becomes ".a.b.c" or just "b.c" when referred to with
    a relative path from "c:\\xml\\a\\d.xml".
    """

    def __init__(self, path, root):
        """
        Create a DataPath for the specified file path and xml root path.  Both
        paths can be relative to the current working directory or absolute.  The
        file path must end with '.xml'.
        """
        root = os.path.abspath(root)
        path = os.path.abspath(path)
        assert(path.startswith(root))
        assert(path.lower().endswith('.xml'))
        datapath = path[len(root):-4]
        datapath = datapath.replace(os.sep, '.')
        self._root = root
        self._path = path
        self._datapath = datapath

    def NoPath(root):
        """
        Factory for creating a placeholder DataPath under the specified xml root
        path.  This can be useful when creating a new objects; see isNoPath().
        """
        path = os.path.join(root, '__NOPATH.xml')
        return DataPath(path, root)
    NoPath = staticmethod(NoPath)

    def isNoPath(self):
        """
        Returns true if this is a placeholder DataPath that does not yet have
        a real value.
        """
        return os.path.basename(self._path) == '__NOPATH.xml'

    def datapath(self):
        """
        Return the absolute xml data path of self (starts with '.').
        """
        return self._datapath

    def abspath(self):
        """
        Return the absolute file system path of self (ending in '.xml')
        """
        return self._path

    def absroot(self):
        """
        Return the absolute file system path of the root directory of the xml
        hierarchy containing self.
        """
        return self._root

    def makeRelativeFilePath(self, datapath):
        """
        Convert an xml path (relative or absolute) to an absolute file path (ending
        with '.xml').  The conversion is with respect to self.absroot() for absolute
        xml paths and with respect to dirname(self.abspath()) for relative xml paths.
        """
        if datapath.startswith('.'):
            base = self.absroot()  # absolute
        else:
            base = os.path.dirname(self.abspath())  # relative
        components = [base] + datapath.split('.')
        return os.path.join(*components) + '.xml'

    def makeRelativeXmlPath(self, datapath):
        """
        Shorten an absolute xml path to a relative xml path if the target is under
        the same the node as this path.  For example, if self refers to '.a.b.c'
        then the following transformations would occur:
          .a.b.d -> d
          .a.b.d.e -> d.e
          .x.y.z -> .x.y.z
        """
        thispath = '.'.join(self._datapath.split('.')[:-1]) + '.'
        print(datapath, self._datapath, thispath)
        if datapath.startswith('.') and datapath.startswith(thispath):
            print('truncated: ', datapath[len(thispath):])
            return datapath[len(thispath):]
        return datapath
