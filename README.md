[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/pAwGQi_N)

The following tests are performed:
### Insertion (and find) tests
| TEST                              | Description                                                                                                                                                                                                                                 |
|-----------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| BasicInsertTest();                | Tests basic insertion (and finding). Verified by Find() returning the inserted item                                                                                                                                                         |
| InsertAfterRehashTest();          | Insert an item, force a rehash, then attempt to find the item                                                                                                                                                                               |
| InsertDuplicateTest();            | Insert an item with a key that already exists (should overwrite previous item), affirms intended behavior.                                                                                                                                  |
| InsertDuplicateAfterRehashTest(); | Insert an item, force a rehash, then insert a duplicate item. This affirms that items are inserted into the correct position after a rehash                                                                                                 |
| RecursiveTableInsert();           | When defining the table template, make the Value be another hash table. This affirms that linked nodes are allocated/copied/shuffled correctly (incorrect pointers will cause double free errors or invalid accesses, ask me how I know :P) |

### Find tests
| TEST                   | Description                                                                                                                                                          |
|------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| FindNonexistentTest(); | Try finding an item that doesn't exist (should return HashTable.end()                                                                                                |
| FindEmptyTest();       | Try finding an item in an empty table (passes if program doesn't crash and returns HashTable.end()). Tests that Find() handles container_array_ = nullptr correctly. | 

### Delete tests
|TEST| Description                                                                                                                                                                       |
|---|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|BasicDeleteTest();| Test that the deletion function works on a table with one item                                                                                                                    |
|DeleteNonexistentTest();| Test that the deletion function deletes nothing if the key does not exist (items are present in table). Roughly verifies that no deletion is attempted if the item doesn't exist. |
|DeleteEmptyTest();| Test that the program doesn't crash if Delete() is called on an empty table. Tests that Delete() handles container_array_ = nullptr correctly.                                    |
|DeleteAfterRehashTest();| Delete an item after a rehash that was inserted before a rehash. Similar to Insert rehash testing, this verifies that the item is in its expected position.                       |

## Dataset Sanitation
The dataset contained empty values and non-printable characters. Empty values were ignored (except empty categories are set to 'NA' in-situ as needed).
Non-printable characters were being interpreted in the linux terminal as escape sequences. A filter program in python was written that erased values outside ASCII 32->127 (except '\n').
A side effect is that the Home Décor category is now called Home Dcor.


# Old README below this line

---

# PA4 Skeleton Code
We expect a fully functioninig command line REPL application for an inventory querying system. Feel free to modify the skeleton code as you see fit.

### How to run the current skeleton code
`make` will compile and execute the skeleton code

Feel free to modify Makefile as you see fit.
