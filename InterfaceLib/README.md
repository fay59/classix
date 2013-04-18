# Classix implementation of InterfaceLib

`InterfaceLib` is the biggest and most important library for non-Carbon
applications. Despite its name, it handles memory management, resource
management, window management, drawing, communication with other devices, events
and a couple more responsibilities.

Because the event model from Mac OS Classic doesn't map too well with the Cocoa
event model, the library has a headless Classix side that communicates with
another program whose sole responsibility is to catch events and draw windows.
Another process was chosen over a thread because the Cocoa framework
**requires** the UI to be run on the main thread, and that becomes a world of
hurt to handle with the rest of Classix, that is otherwise thread-agnostic.

Procedure calls between the two is handled with unnammed pipes (using the
`pipe` call) as a quick round of testing showed it's one of the fastest and
safest way to do it: named pipes are slower and less safe, sockets are also
slower and less safe, SysV IPC message queues are faster by a hair-thin margin
but weirder to use, less safe and prone to system-wide leaks. I didn't try Mach
ports because [I couldn't even figure out how to share them between a parent and
a child process][1].

Window records are identified by the integer value of their address in the
emulator process. This ensures that each window has a unique ID without having
to add or hijack a field in the structure, which would be an ABI-incompatible
change.

Drawing itself is realized in the emulation process and shared with the head
process using `IOSurface`s. Right now, both use `CGContext`s to manipulate the
pixel data. Pixel data from the emulator process is flushed to the head process
when event routines are called (like `Button` and `GetNextEvent`). This is not
the same as a single-buffered window, but I suspect there's no way to reproduce
that exact behavior, and this approach ensures that if the user should take an
action, at least the screens are up-to-date.

There is still a lot to do, on both sides of the library. The next step are
probably menus and actually using resources.

  [1]: http://stackoverflow.com/q/15723273/sharing-mach-ports