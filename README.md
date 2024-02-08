# Noter - C++ Qt Notepad Clone with Spell Check

Noter is a simple notepad clone developed in C++ using the Qt framework. It provides basic text editing functionalities such as cut, copy, paste, undo, redo, new, open, save, and save as.

Additionally, Noter incorporates a spell check feature implemented using a Trie data structure.

The application utilizes QT threading api's to separate the main thread 
from other tasks such as reading words and storing them in the trie,
providing spell check and suggestion resulting a responsive user interface.

## Demo

https://github.com/SaurabhYadav388/Noter/assets/128916471/c56d37e9-b4de-429e-aa9f-a707fc499a1e


## Features

- **Text Editing:**
  - Cut , Copy , Paste , Undo , Redo

- **File Operations:**
  - New , Open , Save , Save As

- **Spell Check:**
  - Utilizes Trie data structure for efficient spell checking.

- **Spell Suggestions:**
  - Spell Suggestion using Levenshtein distance with QTconcurrent and Future for Threading.
