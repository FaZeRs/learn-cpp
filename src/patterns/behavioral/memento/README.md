# Memento Pattern

Memento is a behavioral design pattern that lets you save and restore the previous state of an object without revealing the details of its implementation.

![Structure](structure.png)

## Intent:
The main intent of the Memento Pattern is to capture and externalize an object's internal state without violating encapsulation. This allows the object to be returned to this state later if required.

## Components:
Originator: The object for which the state needs to be saved. It creates the memento and uses it to restore its previous state.

Memento: The object that stores the internal state of the originator. This state should not be accessible by other objects.

Caretaker: The object that keeps track of the various states of the originator by holding onto the mementos. However, it never examines or operates on the contents of a memento.

## How it Works:
The caretaker requests a memento from the originator to save the current state.

When required, the caretaker provides the memento back to the originator to revert to a previous state.

## Advantages:
State Restoration: Allows for easy restoration of an object's state, useful for "undo" operations.

Encapsulation: Object's internal state is saved without exposing its internal structure.

Simplified Originator: The responsibility of storing state can be offloaded to the memento, simplifying the originator's role.

## Disadvantages:
Memory Overhead: If frequent mementos are saved, it can lead to high memory usage.

Complexity: Introducing mementos and caretakers can increase the complexity of the design.

## Use cases
1. **Undo/Redo Functionality**:
One of the most common uses of the Memento pattern is to implement undo and redo operations in software applications, like text editors, graphic editors, and database systems.

2. **Snapshots**:
Sometimes it's necessary to take a snapshot of an application's state so that it can be restored later. This is useful in software like virtual machine snapshots, database backups, and some game save points.

3. **Database Transactions**:
The ability to rollback a transaction is essential in database systems. If a transaction fails or if there's a need to abort a transaction, the system can use a memento to restore the database to its state before the transaction started.

4. **Version History**:
Software like version control systems (e.g., Git) or document editors might need to maintain a history of versions. The Memento pattern can be used to capture the state of the document or codebase at various points in time.

5. **State Checkpoints**:
In some simulations or algorithms, it might be essential to save the state at various checkpoints. If something goes wrong or if there's a need to analyze the progression, these states can be revisited.

6. **Browser Back/Forward**:
Browsers maintain history to allow users to move back and forth among the visited web pages. The Memento pattern can be used to keep track of the pages and states.

7. **Prototype Cloning**:
If you need to clone or copy complex objects that might have numerous internal states, you can use the Memento pattern to capture the state and initialize the new object with that state.

8. **CareTaker**:
In AI or machine learning algorithms, especially reinforcement learning, the system might need to revert to a previous state if a particular path leads to a non-optimal solution. The memento can be used to restore to an older, more promising state.

9. **Online Gaming**:
Multiplayer online games often require the ability to save and restore game states. If a player loses connection and then reconnects, the game might need to restore their last known state.

10. **Configuration Management**:
Systems that allow dynamic configuration changes might use Memento to store previous configurations. If a new configuration causes issues, the system can easily revert to a previous working configuration.

##
The Memento Pattern is particularly useful in applications where state restoration or undo operations are crucial, such as text editors, graphic editors, or games where players might want to revert to a previous game state.
