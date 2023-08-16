# Strategy Pattern

Strategy is a behavioral design pattern that lets you define a family of algorithms, put each of them into a separate class, and make their objects interchangeable.

![Structure](structure.png)

## Intent:
The main intent of the Strategy Pattern is to define a set of algorithms, encapsulate each one of them, and make them interchangeable. It allows the client to choose the algorithm from a family of algorithms independently.

## Components:
Strategy: This is an interface or abstract class used to define a family of algorithms or behaviors.

ConcreteStrategy: These are concrete implementations of the Strategy interface, each providing a different behavior or algorithm.

Context: Holds a reference to a Strategy object and is instrumental in switching between different algorithms or behaviors. The Context class delegates the responsibility to execute an operation to the Strategy object.

## How it Works:
Clients (or contexts) can select and interchange algorithms (strategies) at runtime without altering the code that uses the algorithm.

The context delegates the work to the strategy object instead of implementing multiple versions of the algorithm on its own.

## Advantages:
Flexibility: Strategy pattern provides a way to configure a class with one of many behaviors, leading to more flexible and dynamic behavior.

Decoupling: It helps in decoupling the algorithm implementation from the context class, promoting the Open/Closed Principle.

Interchangeability: Different strategies can be easily interchanged or extended without modifying the client code.

## Disadvantages:
Overhead: If there are too many strategies, the design might introduce many small, granular classes which can increase the complexity.

Client Exposure: The client must be aware of the differences between the strategies to choose the appropriate one.

## Use cases
1. **Sorting Algorithms**: 
Consider a collection object that needs to support multiple sorting methods. Instead of hardcoding the sorting algorithm, you can use the Strategy pattern to switch between quicksort, bubblesort, mergesort, etc., based on the need.

2. **Payment Methods**:
In an e-commerce application, users might want to pay using credit card, PayPal, Bitcoin, or other payment methods. Each method will have different implementations for processing payment. The Strategy pattern can be used to switch between these methods seamlessly.

3. **Compression Algorithms**:
If you have an application that needs to save or send data in various compression formats (like ZIP, RAR, TAR), you can use the Strategy pattern to change the compression algorithm on-the-fly.

4. **Validation Strategies**:
For form or data validation, you might have different validation criteria. For instance, you could have different validation strategies for email, phone numbers, or usernames.

5. **Travel Route Planning**:
Imagine a GPS application where you need to suggest routes based on quickest path, shortest path, least traffic, or most scenic route. Each of these can be implemented as a separate strategy.

6. **Image Rendering**:
Different image rendering algorithms can be used for antialiasing, dithering, etc. Depending on the desired quality and speed, you can switch between these algorithms using the Strategy pattern.

7. **Discount Strategies**:
In a shopping application, there might be multiple discount strategies like festive offer, buy one get one free, or seasonal sales. The Strategy pattern lets you apply the appropriate discount based on the active offer.

8. **User Authentication**:
Applications might need to support various authentication mechanisms like password-based, token-based, biometric, or even third-party OAuth providers. The Strategy pattern can encapsulate each of these methods.

9. **Machine Learning Algorithms**:
In data science projects, researchers often experiment with different machine learning algorithms to find the best one for their specific problem. The Strategy pattern can be used to switch between different models easily.

10. **Logging Mechanisms**:
An application might support various logging mechanisms like console logging, file logging, or remote server logging. By using the Strategy pattern, developers can change the logging behavior without altering the code that uses the logging service.

##
The Strategy Pattern is especially useful when you have several versions of an algorithm or when you have multiple ways of doing something and want to let the client choose which one to use. It provides an alternative to subclassing for selecting behaviors.
