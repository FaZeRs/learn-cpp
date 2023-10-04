# Command Pattern

![Structure](structure.png)

## Intent:
The primary intent of the Command Pattern is to decouple the sender (invoker) from the receiver (the object performing the action). By doing so, it allows operations to be decoupled from their implementation.

## Components:
Command: This is the interface for executing operations. It typically has a method named execute().

ConcreteCommand: Classes that implement the Command interface and specify the binding between the receiver object and an action.

Invoker: Asks the command to carry out the request.

Receiver: Knows how to carry out the operation associated with the command.

Client: The client creates the ConcreteCommand and sets its receiver.

## How it Works:
The client creates one or more concrete command objects and sets its/their receiver.

The client passes the command object to the invoker.

The invoker invokes commands.

## Advantages:
Decoupling: Separates objects that issue commands from objects that perform the command.

Flexibility: New commands can be added without altering the existing code structure.

Command Queueing and Logging: Commands can be queued, delayed, or logged easily.

Undo/Redo: Can be extended to support undo and redo functionalities.

## Disadvantages:
Increased Number of Classes: Each action might require a separate command class, which can lead to a proliferation of classes.

## Use cases
1. **Undo/Redo Mechanism**: 
The Command pattern allows you to maintain a history of commands executed. By doing so, you can provide undo and redo functionalities, especially beneficial in applications like graphic editors or word processors.

2. **Queueing Requests**: 
If you want to execute requests at different times or under different conditions, commands can be placed into a queue and executed when required. This is commonly used in background job systems.

3. **Logging and Auditing**:
Since commands capture all the data associated with an action, it's straightforward to log or audit them. If an application crash occurs, you can see exactly what operations were in progress.

4. **Macro Recording**: 
In applications where you want to allow users to create scripts or macros of multiple actions, the Command pattern allows bundling a set of commands together and executing them in sequence.

5. **Transactional Behavior**:
Some systems require that if one action fails, all other actions should be rolled back. By using the Command pattern, you can encapsulate all actions as command objects and if one fails, you can use the undo method to roll back the changes.

6. **Remote Execution**: 
Commands can be serialized and sent across the network to be executed on a different machine. This is commonly seen in remote server management tools.

7. **Parameterized Actions**:
If you have a set of methods that need to be executed with different parameters, commands can encapsulate these actions and parameters. For example, in GUI libraries, menu items can be associated with command objects to perform various actions when clicked.

8. **Scheduler Systems**: 
In scenarios where you want to schedule specific actions to occur at given intervals or specific times, command objects can be used to encapsulate the action and the scheduler will execute them accordingly.

9. **Task-Based Multithreading**: 
For applications that use multithreading, you can encapsulate tasks as commands, enabling a more structured and scalable approach to concurrent task execution.

10. **Progressive Command Execution**:
For long-running operations, it's possible to break down the operation into smaller commands that can be executed progressively. This allows feedback to users about the progress of the operation and can make operations resumable if interrupted.

##
The Command Pattern is particularly beneficial when there's a need for operations to be executed at different times or in sequences, when the history of operations is essential (undo/redo), or when the requests need to be parameterized, queued, or logged.
