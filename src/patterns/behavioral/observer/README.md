# Observer Pattern

![Structure](structure.png)

## Intent:
The main intent of the Observer Pattern is to define a dependency between objects such that when the state of one object changes, all its dependents are notified and updated without creating a tightly coupled system.

## Components:
Subject (Observable): The object that holds the state and notifies observers when a change occurs. It maintains a list of its dependents (observers).

Observer: The object that watches the subject and gets notified when the subject's state changes. It defines an updating interface to be invoked when the subject changes.

ConcreteSubject: An implementation of the Subject. It maintains the state of the object and when a change occurs, it notifies the attached observers.

ConcreteObserver: An implementation of the Observer. It keeps a reference to the ConcreteSubject and implements the update interface to keep its state consistent with the subject's state.

## How it Works:
Observers subscribe to the subject to get updates.

When the state of the subject changes, it notifies all its observers.

Each observer then queries the subject to synchronize their state (pull model), or the subject sends the updates as part of the notification (push model).

## Advantages:
Decoupling: The subject and its observers are loosely coupled. The subject knows only about the observer list, not the concrete implementation, making the system more modular and easier to update or extend.

Dynamic Relationships: Observers can be added or removed at runtime, offering a dynamic relationship.

Broadcast Communication: Enables a broadcast communication mechanism where the subject can notify multiple observers at once.

## Disadvantages:
Potential Performance Issues: If not implemented carefully, the notification mechanism can lead to performance bottlenecks.

Unexpected Updates: In some cases, observers might get notified and updated in ways they didn't anticipate, leading to unexpected behaviors.

## Use cases
1. **Event Management Systems**:
GUI Tools: Almost every GUI toolkit uses the Observer pattern to monitor user interactions. For example, when a user clicks a button, the button notifies its subscribers about this event.

2. **Real-time Data Monitoring**:
Stock Market: Stock tickers can use this pattern. When a stock's price changes (the subject), all registered investors (observers) get notified.

3. **Subscription Model**:
News portals and magazines often use a subscription model. When there's a new article or issue, subscribers are notified.

4. **Collaborative Tools**:
Shared Documents: When someone makes a change in a collaborative document, other users viewing or editing the document get real-time updates.

5. **Sensor Systems**:
Imagine a system with multiple sensors. Whenever a sensor detects a change, it notifies a central system or other connected devices.

6. **Gaming**:
In multiplayer games, when an event occurs with one player, others might need to be informed, e.g., when a player scores, the scoreboard and other players are updated.

7. **Model-View-Controller (MVC) Architecture**:
It's used in software development, especially in frameworks for building user interfaces. The model contains the core data and logic, and the view displays the data. When data in the model changes, the view needs to be updated. Using the Observer pattern, the model becomes the subject, and the views are observers.

8. **Social Media Notifications**:
When someone posts an update or a picture, their followers or friends get notified.

9. **E-commerce and Bidding Platforms**:
On an online bidding platform, when someone places a new bid on an item, other bidders are informed about the new bid.

10. **Job Portals**:
When companies post new job openings, job seekers who have subscribed to notifications for relevant job categories or keywords are informed.

11. **IoT (Internet of Things)**:
In a smart home setup, an event detected by one device (like a motion sensor) can trigger actions in other devices (like turning on lights).

12. **Chat applications**:
When a user sends a message, all members of the chat group need to see it.

##
The Observer Pattern is particularly useful in event-driven systems, where certain entities (observers) need to take action or update themselves when another entity (subject) changes. Examples include GUI systems, where UI components update in response to underlying data changes, or in applications that implement real-time monitoring or notifications.
