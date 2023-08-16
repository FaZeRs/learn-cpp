# State Pattern

State is a behavioral design pattern that lets an object alter its behavior when its internal state changes. It appears as if the object changed its class.

![Structure](structure.png)

## Intent:
The main intent of the State Pattern is to allow an object to change its behavior when its internal state changes, without having to modify the class of the object.

## Components:
Context: The class that has a state that changes, leading to different behaviors. It maintains an instance of one of the concrete state subclasses which indicates the current state.

State: An interface or abstract class that defines methods representing the behavior associated with a particular state of the Context.

ConcreteState: Individual subclasses for each state which implement the behavior associated with a specific state of the Context.

## How it Works:
The context delegates state-specific behavior to different state objects.

When the state of the context changes, it switches the state object it delegates to, allowing for a change in behavior.

## Advantages:
Organized State Transitions: Encapsulates the state transition rules within the state classes, making it easier to manage and modify.

Encapsulation: Each state behavior is encapsulated in its own class, promoting the Single Responsibility Principle.

Extensibility: New states can be added easily without altering existing state logic.

## Disadvantages:
Class Proliferation: If there are many states, the design can lead to a large number of state classes.

State Transitions: Ensuring correct and valid state transitions can become challenging if there's complex logic involved.

## Use cases
1. **GUI Tools**:
Many graphical user interfaces have tools that change their behavior based on the current state. For example, a drawing tool may have various states like 'draw', 'erase', 'fill', etc. Each state has its own behavior for mouse actions.

2. **Game Character Behaviors**:
In video games, characters might have different behaviors like 'idle', 'attacking', 'defending', or 'running'. Each state might have different animations, speeds, or actions associated with it.

3. **Workflow Processes**:
Consider a document in a workflow system. The document can be in various states - 'draft', 'review', 'approved', or 'rejected'. Each state might dictate different actions that can be taken on the document.

4. **Network Connection Handling**:
A network connection can have various states such as 'connecting', 'connected', 'disconnected', or 'error'. Different states might trigger different actions or handle messages differently.

5. **Operational Modes of Devices**:
Consider a device like a smartphone that can be in 'normal', 'silent', or 'vibration' mode. Each mode would dictate how the phone reacts to an incoming call or message.

6. **ATMs**:
Automated Teller Machines (ATMs) might have states like 'idle', 'processing', 'dispensing cash', or 'out of cash'. Depending on the state, the machine's responses and UI would differ.

7. **Traffic Light System**:
A traffic light typically has states like 'red', 'green', and 'yellow'. The transitions between these states are often based on timers, sensors, or manual inputs.

8. **Order Management**:
An e-commerce system's order might have states such as 'new', 'processed', 'shipped', 'delivered', or 'returned'. Different states might trigger different actions like sending emails to customers or processing refunds.

9. **Finite State Machines**:
Any system that can be modeled as a finite state machine (FSM) can use the State pattern to represent its states and transitions. This includes parsers, compilers, and many other types of systems.

10. **Media Players**:
Players can have states such as 'playing', 'paused', 'stopped', 'fast forwarding', or 'rewinding'. The behavior of the player (like how it responds to a play or pause button) changes based on its current state.

##
The State Pattern is useful when a particular object can be in a number of distinct states, and its behavior changes significantly based on its current state. It helps in organizing and managing different states more efficiently by decentralizing state-specific behaviors into separate classes.
