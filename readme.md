# Chrome Tabs

An implementation of Chrome/Firefox style draggable tabs in C++ for Windows.

* Creates (or clones) main windows if a tab is dropped outside of the current
window
* Can drag tabs from one window to other.
* Windows without any tabs left will be closed.
* Clicking on a tab will switch the child window belong to the tab in front.

## Screenshot

![Screenshot](/img/sc.png)

## To Do

* The user should be able to arrange the tabs.
* Display a thumbnail of the window when the user drags the tab
  out of the tab area.

## Building

If you're using MinGW/MinGW64, edit files in 'build\win' to include
your MinGW directories.

If you're using Visual Studio, just create an empty Windows Application
project and add the content in 'src' and 'res' directories into it.
