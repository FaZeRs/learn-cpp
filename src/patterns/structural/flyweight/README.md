# Flyweight Pattern

![Structure](structure.png)

## Intent:
Reduce memory usage by sharing portions of an object state (intrinsic state) among multiple objects, so that any remaining portion of the object (extrinsic state) can be passed in when required.

## Components:
Flyweight: This is an interface that defines methods which can receive and act on the extrinsic state.

ConcreteFlyweight: Implements the Flyweight interface and stores intrinsic state.

UnsharedConcreteFlyweight (not always required): Not all flyweight subclasses need to be shared.

FlyweightFactory: This maintains a pool of shared flyweights. When a client requests a flyweight, the factory supplies an existing object or creates one, if none exists.

## How it Works:
Intrinsic state is stored in the flyweight object. This state is sharable and common to many objects.

Extrinsic state is stored outside the flyweight and passed to the flyweight when its operations are invoked.

## Advantages:
Memory Efficient: Significant memory savings, especially when dealing with a large number of objects that share significant amounts of information.

Scalability: By externalizing the common data, the application can scale more easily.

## Disadvantages:
Complexity: Introducing the Flyweight pattern can make the system more complex by splitting the state into intrinsic and extrinsic.

Code Maintainability: Need to be cautious about how and when shared state is accessed or modified.

## Use cases
1. **Text Formatting Applications**:
Suppose you have a word processor that allows the user to change font, style, size, and color of text. Instead of associating formatting attributes with every character in the document, you can use the Flyweight pattern to associate them only once, and then reference this shared data among all characters that have the same formatting.

2. **Graphics**:
In graphic applications, such as computer-aided design (CAD) systems, thousands of overlapping geometric shapes can exist. Using the Flyweight pattern can help share data related to color, texture, and other graphic attributes.

3. **Game Development**:
For large scale multiplayer games where there are many instances of player objects, environmental objects, and more. The Flyweight pattern can be used to reduce the memory footprint by sharing common data like textures, mesh data, or AI behaviors.

4. **Web Page Rendering**:
When rendering a web page, there might be numerous identical elements, such as buttons or icons. Flyweight can be applied to reuse a single object multiple times with different extrinsic attributes.

5. **Spreadsheet Applications**:
In applications like Excel, where each cell might have similar formatting or styling, Flyweight can help in sharing these properties across cells.

6. **String Interning**:
Some languages, like Java, use string interning to save memory. This means that strings that are equal will usually reference the same instance in memory, rather than having multiple identical copies.

7. **Terrains in Virtual Worlds**:
In 3D worlds, terrains may be represented using a grid of tiles, where each tile might share the same texture or set of properties. The Flyweight pattern can be applied to share this data among tiles.

8. **Databases**:
In scenarios where there are a lot of shared/repeated values in large databases, the Flyweight pattern can be utilized to minimize memory consumption.

9. **Animations**:
In animations, especially with sprites, there might be multiple characters or elements that use the same animation frames or sequences. Using the Flyweight pattern can help share these sequences across the entities.

10. **Real-world Geographic Data**:
When representing geographical information, many elements like roads, buildings, or parks might have shared attributes. The Flyweight pattern can be employed to optimize the memory usage in these scenarios.

##
The Flyweight Pattern shines in scenarios where there are many instances of a class and most of their state can be extrinsic. By separating intrinsic and extrinsic state, objects become more lightweight, hence the name of the pattern.
