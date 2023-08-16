# Bridge Pattern

Bridge is a structural design pattern that lets you split a large class or a set of closely related classes into two separate hierarchies—abstraction and implementation—which can be developed independently of each other.

![Structure](structure.png)

## Intent:
The main goal of the Bridge Pattern is to separate abstraction from its implementation, allowing both to be developed independently. This pattern focuses on the principle of composition over inheritance.

## Components:
Abstraction: This defines the abstraction's interface and maintains a reference to the Implementor.

RefinedAbstraction: Extends the Abstraction to define more specific operations.

Implementor: This defines the interface for all concrete implementations. It doesn't have to correlate with the Abstraction interface; in fact, the two interfaces can be entirely different.

ConcreteImplementor: Implements the Implementor interface and does the real work.

## How it Works:
The Bridge Pattern separates the client-facing Abstraction from the actual functional Implementor.

Both the Abstraction and the Implementor have their own class hierarchies. Changes in the implementation will not affect client code.

## Advantages:
Separation and Extension: Abstraction and implementation can be extended independently.

Flexibility: Provides greater flexibility in how the classes are structured and combined.

Sharing: The Bridge pattern can also enable more objects to share an implementation.

## Disadvantages:
Complexity: Can increase the system's complexity as the number of classes can grow.

Initiation Difficulty: It's a bit challenging to understand and initiate the pattern in an already established system.

## Use cases
1. **UI Platforms & Themes**:
When building a UI toolkit, you may have different platforms (like Windows, Linux, Mac) and different themes (like dark mode, light mode). The Bridge pattern allows you to create a UI abstraction that can be implemented differently on various platforms and can be styled differently with various themes.

2. **Device Drivers**:
Operating systems use drivers to communicate with hardware devices. The Bridge pattern can be used to create a common interface (abstraction) for all devices while having specific implementations for each device type (printer, monitor, keyboard).

3. **Remote Controllers & Devices**:
Think of a remote control and the devices it can control, like a TV, radio, or DVD player. The remote is the abstraction and the devices are the implementations. You can extend the range of devices without changing the remote's interface and vice versa.

4. **Graphics Rendering**:
Suppose you're building a graphics engine that could use different rendering methods (DirectX, OpenGL, Vulkan). The Bridge pattern can help in creating a common abstraction for rendering while having specific implementations for each rendering method.

5. **File Formats & Savers**:
An application might support saving data in various formats (e.g., CSV, XML, JSON). Using the Bridge pattern, you can decouple the data representation from the saving mechanism.

6. **Payment Gateways**:
In e-commerce systems, supporting multiple payment gateways (like PayPal, Stripe, or credit card processing) becomes more manageable with the Bridge pattern. The checkout process can remain consistent (abstraction) while each payment gateway has its own implementation.

7. **Communication Protocols**:
A software might need to support multiple communication protocols (like TCP, UDP, HTTP). By using the Bridge pattern, you can define a common interface for sending/receiving messages, with each protocol having its separate implementation.

8. **Databases & ORMs**:
An ORM (Object-Relational Mapping) system might need to interact with different types of databases (like MySQL, PostgreSQL, SQLite). The Bridge pattern allows creating a consistent interface for database operations, with each database type having a separate implementation.

9. **Different Resources on Cloud Providers**:
When building a cloud management tool, you might need to manage similar resources (like compute instances, storage) across different cloud providers (AWS, GCP, Azure). The Bridge pattern can provide a consistent interface for managing these resources, with specific implementations for each cloud provider.

##
The Bridge Pattern is particularly useful when both abstractions and their implementations need to be extended separately. It's also handy when an abstraction can have multiple implementations, or when the implementation needs to be chosen/switched at runtime.