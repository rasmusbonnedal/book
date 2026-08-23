# What
A bookkeeping software for swedish AB in C++ using Dear Imgui as UI.

# How to build
From the root, `cmake --build build`. Run tests with `ctest --test-dir build`

# What to test
This is a GUI application so many things need manual testing. When the agent make changes
it can add stuff to this list and the human will test it in the GUI.
* Example item

# Todo List
* Upgrade Imgui from 1.89 to 1.92 - some build errors ✅
  * When pressing enter on a combo box in "Nytt verifikat" the focus moves to the
    pengar input box but the text is not selected. I think this is a regression. ✅
* Change the code to support Imgui docking ✅
* When entering a new verifikat make it impossible to click cancel by hitting enter on keyboard - It's easy to tab there and cancel by mistake ✅
* When entering many verifikat from an account statement make a feature where you enter an account which must be present in all entered verifikats

