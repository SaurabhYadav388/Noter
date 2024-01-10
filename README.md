# Noter - C++ Qt Notepad Clone with Spell Check

Noter is a simple notepad clone developed in C++ using the Qt framework. It provides basic text editing functionalities such as cut, copy, paste, undo, redo, new, open, save, and save as.

Additionally, Noter incorporates a spell check feature implemented using a Trie data structure.

The application utilizes QThread to separate the main thread from other tasks such as reading words and storing them in the trie, 
providing a responsive user interface.

## Features

- **Text Editing:**
  - Cut , Copy , Paste , Undo , Redo

- **File Operations:**
  - New , Open , Save , Save As

- **Spell Check:**
  - Utilizes Trie data structure for efficient spell checking.

- **Threading:**
  - QThread implementation for background tasks.
