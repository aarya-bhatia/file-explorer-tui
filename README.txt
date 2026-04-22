# README

TODO: 
- scrolling, handle out of bound entries
- lru cache for previews
- global dirty flag in app state - do not render views if false.
- give memory to views so they only update if necessary
- move file logic to FileNavigator class
- implement state pattern with a mode stack - current mode handles input, when done it pops from stack.
- error handling - show message in status for timeout seconds.
- search - add search mode
- sort - sort strategy
- consider std::filesystem api
- future - readdir in background thread and load async
