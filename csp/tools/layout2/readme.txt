Features to implement:

 * All features from old version of course
 * Make it possible to view scene from different angles. One from top, side and one free floating camera.
 * All commands are sent to the view with the current focus.
 * Rebuild the sim.dar file.
 * Convert 3d file using osgconv.exe to the model.osg.
 * Object browser. Make it possible to display an object before we insert it. We could have a list of all the objects that are insertable.
 * All commands that can be issued should be its own object. ex:
     MoveCameraToHomeCommand
     
     
     DeleteCommand
     UndoCommand
     ExitCommand
     FileLoadCommand (To generic name for this class)
     FileSaveCommand (To generic name for this class)
 * Display the underlying terrain.
