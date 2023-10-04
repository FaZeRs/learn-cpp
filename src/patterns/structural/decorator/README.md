# Decorator Pattern

![Structure](structure.png)

## Intent:
The main aim of the Decorator Pattern is to add new responsibilities to an object dynamically. It provides an alternative to subclassing for extending functionality.

## Components:
Component: This is an abstract class or interface that defines the object to which additional responsibilities can be attached.

ConcreteComponent: The actual object for which you want to add new behaviors or responsibilities.

Decorator: An abstract class that inherits from the Component. It has a reference to a Component object and mirrors the interface of the Component it decorates.

ConcreteDecorator: These are the individual classes that add responsibilities to the ConcreteComponent.

## How it Works:
Decorators wrap a Component and can be used to add or override its behaviors.
Decorators can be combined or stacked to form varied functionalities.

## Advantages:
Flexibility: It provides a flexible alternative to subclassing for extending functionality.

Composability: Decorators can be stacked/combined in any order or combination, making it easy to produce varied functionality.

Single Responsibility Principle: Each decorator class follows the single responsibility principle, having only one reason to change.

## Disadvantages:
Complexity: The system can become complex and hard to understand, especially when there are a lot of decorators involved.

Maintenance: Changes to the core component interface or changes in decorator setup can introduce bugs.

## Use cases
1. **Graphical User Interfaces (GUIs)**:
This is a classic example. Consider window objects in a GUI. A window could be decorated with a scrollbar, border, or a menu. Each of these additional features can be considered a decorator of the window.

2. **I/O Classes**:
Java's InputStream is a typical example. There are various decorators like BufferedInputStream, DataInputStream, etc., that add additional functionality (like buffering, reading primitive data types) to the basic FileInputStream.

3. **Middleware/Web Frameworks**:
Middleware components can be used to add functionalities like logging, authentication, and caching. For instance, in the Express framework (Node.js), middleware functions can be thought of as decorators around the core request-processing logic.

4. **Logging and Monitoring**:
Instead of extending the core functionality of a class to add logging, a decorator can be used to wrap the main class and add this logging aspect. This way, logging can be turned on and off by either using or removing the decorator.

5. **Dynamic Abilities**:
When objects need to gain responsibilities or features on-the-fly during runtime, decorators can provide a flexible alternative to subclassing.

6. **Validation and Sanitization**:
Before processing data, decorators can be used to validate or sanitize the input.

7. **Caching**:
For expensive operations, decorators can be used to add a caching layer. Before delegating the operation to the original object, the decorator can check if the result is already in cache.

8. **Access Control**:
In situations where access control or permissions need to be managed, a decorator can be employed to check if the current context or user has the right permissions to execute a method or access an object.

9. **Transactions**:
When methods need to be executed within a transactional context (e.g., in databases), decorators can start, commit, or rollback transactions.

10. **Notifications**:
If you want an object to notify other systems or components when a particular action takes place, decorators can be used to send these notifications without modifying the original object's logic.

11. **Performance Measurement**:
Decorators can measure the time taken to execute operations and then, for example, log or alert if the execution time is above a certain threshold.

##
The Decorator Pattern is valuable when you have a mature, stable component that requires extended functionalities. It provides a flexible and reusable solution compared to the rigidity of subclassing.
