# What
A bookkeeping software for swedish AB in C++ using Dear Imgui as UI.

# How to build
From the root, `cmake --build build`. Run tests with `ctest --test-dir build`

# What to test
This is a GUI application so many things need manual testing. When the agent make changes
it can add stuff to this list and the human will test it in the GUI.
* Example item
* Edit a verifikat and choose **Struk** on a row. The row should become
  read-only and struck through, the remaining rows should still balance, and
  after Update the struck row should not affect Saldo or reports.

# Todo List
* Upgrade Imgui from 1.89 to 1.92 - some build errors ✅
  * When pressing enter on a combo box in "Nytt verifikat" the focus moves to the
    pengar input box but the text is not selected. I think this is a regression. ✅
* Change the code to support Imgui docking ✅
* When entering a new verifikat make it impossible to click cancel by hitting enter on keyboard - It's easy to tab there and cancel by mistake ✅
* When entering many verifikat from an account statement make a feature where you enter an account which must be present in all entered verifikats ✅
  * My suggestion for UI: Have a lock checkbox beside the Konto combobox in the Saldo dialog. If it's checked, new verifikat dialog should have that acct prefilled on the first line and should be considered invalid if it does not contain an entry with that account ✅
* Add a feature to mark rows in verifikat as deleted (struken). ✅
* Make it illegal / impossible to edit rows on a verifikat unless they were entered on the same day. The user should only be able to strike rows and add new ones. ✅
* Add feature bokföringsorder. They are preliminary verifikat but marked in the data model. They are not generally included in reports but are shown in transactions lists, with a clear marking. They are not exported to sie files. Implement it by doing the following steps:
  1. Add it to the data model. Add tests to make sure it serializes and deserializes well. ✅
  2. Make it possible to create bokföringsorder instead of New verifikat. ✅
  3. Show in verifikatlist that it is a bokföringsorder - another color ✅
  4. Bokföringsorder should not be included in rapporter ✅
  5. Show rows in Saldo window from bokföringsorder in another color ✅
  6. Add a test that verifies that a bokföringsorder is not exported in sie
  7. Make a button in edit bokföringsorder to promote it to verifikat
  
