#!/usr/bin/python3
import csp.csplib

# Get the one and only logstream there is.
log = csp.csplib.log()

# We output all the logging to this file.
log.logToFile("my-logging-example.log")

# We want all the log columns available. Date, time, filename (if possible) etc.
log.setFlags(log.cVerbose)

# Controls the priority of logging. cDebug will log everything. If you set
# cWarning instead you will get warning, error and fatal messages only.
log.setPriority(log.cDebug)

# Do some logging in ascending priority order.
log.debug("debug information")
log.info("This is an informational message")
log.warning("I warn you about this situation.")
log.error("Some error happened.")

# A problem has been encounted. Log this error with a stack trace if possible.
# This will also abort this program.
log.fatal("I can't continue anymore. This is fatal and the end of time.")
