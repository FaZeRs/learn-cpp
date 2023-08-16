# Proxy Pattern

Proxy is a structural design pattern that lets you provide a substitute or placeholder for another object. A proxy controls access to the original object, allowing you to perform something either before or after the request gets through to the original object.

![Structure](structure.png)

## Intent:
Provide a substitute or placeholder for another object to control the access to it. This pattern is particularly useful for delaying the creation and initialization of expensive objects until they're needed, or for accessing objects that are remote or protected in some manner.

## Components:
Subject: An interface that both the RealObject and the Proxy classes implement. It defines the common interface for both.

RealSubject (RealObject): The actual object that the Proxy represents.

Proxy: Maintains a reference to the RealObject. It controls access to the real object and may be responsible for its creation and maintenance.

## How it Works:
The Proxy class is introduced as an intermediary between the client and the real object. The client interacts with the Proxy instead of the real object.

Based on the requirement, the Proxy can perform additional operations (like lazy initialization, logging, access controls) before forwarding the request to the real object.

## Advantages:
Lazy Initialization: Expensive resources can be loaded on demand, thereby improving application startup time.

Control: The proxy can add additional behaviors (like logging, access controls) before forwarding the call to the real object.

Separation of Concerns: Non-core responsibilities can be separated out, making the system cleaner.

Remote Proxy: Can represent objects in different address spaces.

## Disadvantages:
Overhead: Introducing proxy can add a layer of indirection, which might cause a slight overhead.

Complexity: Design and implementation can become more complex.

## Use cases
1. **Lazy Initialization**:
When instantiation of an object is resource-intensive or time-consuming, a proxy can delay this instantiation until the object's functionality is actually needed.

2. **Access Control**:
Use a proxy when you want to control the operations that can be performed on an object based on certain conditions (e.g., certain users can only perform read operations).

3. **Logging and Auditing**:
Before forwarding the request to the real object, a proxy can log details about who accessed the object and what operations were performed.

4. **Shared Resources**:
When multiple clients need to share a resource, a proxy can ensure that only one client can access the resource at a time.

5. **Network Connections**:
Use a proxy to hide the complexity of accessing a remote service. The proxy can handle network connections, retries, and even failovers transparently to the client.

6. **Cache**:
When a particular operation is expensive, the results can be cached within the proxy. This way, subsequent requests can be served from the cache instead of recalculating or fetching the results.

7. **Reference Counting**:
In systems where memory management is crucial, a proxy can keep a count of how many clients are using a resource. When the count drops to zero, the proxy can safely deallocate the resource.

8. **API Wrappers**:
If you're using an external API or service, a proxy can be useful for wrapping that API and providing a simpler or more domain-specific interface for your application.

8. **Middleware**:
In modern microservices architectures, proxies are often used as middleware components. For example, they can handle tasks like request routing, load balancing, and authentication.

9. **Smart Loading**:
If you have large datasets or multimedia, a proxy can manage smart loading, like on-demand loading of sections of data or streaming.

##
The Proxy Pattern can be found in many real-world applications. For instance, Remote Method Invocation (RMI) in Java utilizes a stub (proxy) as an intermediary for remote method calls.