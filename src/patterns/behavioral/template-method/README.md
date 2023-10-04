# Template Method Pattern

![Structure](structure.png)

## Intent:
The main intent of the Template Method Pattern is to define a structure of an algorithm, allowing some steps to be redefined by subclasses, thus providing a way to avoid code duplication and adhere to the Don't Repeat Yourself (DRY) principle.

## Components:
AbstractClass: This class defines abstract primitive operations that concrete subclasses define to implement steps of an algorithm. It also implements a template method defining the skeleton of an algorithm.

ConcreteClass: Subclass that implements the primitive operations to carry out subclass-specific steps of the algorithm.

## How it Works:
The abstract class defines the steps of an algorithm as a set of methods.

One or more steps are declared abstract, so subclasses are forced to provide custom implementations for them.

Concrete subclasses provide the specifics of these abstract steps without changing the overall flow.

## Advantages:
Code Reusability: By defining the structure of the algorithm in one place (the abstract class), there's less code duplication.

Flexibility: Subclasses can redefine certain steps of an algorithm without changing its overall structure.

Promotes Principle of Least Astonishment: Consumers of the concrete classes can be assured that the general algorithm structure remains consistent across different implementations.

## Disadvantages:
Potential Overhead: For simpler algorithms, the template method pattern might introduce unnecessary complexity.

Must Modify the Abstract Base: If you need to add a new step in the middle of the existing process, you'd likely need to adjust the abstract class and all subclasses.

## Use cases
1. **Sequential Processes and Workflows**:
When you have a series of steps that should be executed in a particular order, but some of those steps might have different implementations depending on the context.
Example: An application that needs to process data from various sources like XML, CSV, or database. The steps might be read, parse, and store. While the order is fixed, the actual implementation of the read and parse might differ based on the data source.

2. **Frameworks and Libraries**:
When building frameworks or libraries where you want to provide users a way to extend or customize certain behaviors while retaining the main flow.
Example: A UI framework might have a method to render a widget. This method sets up the necessary environment and then calls a customizable draw method that subclasses can override to customize the rendering.

3. **Code Reusability**:
When there's a significant amount of shared code, and only a few parts need to be customized. This ensures that you don't repeat the shared code and only allow variation where needed.
Example: In a game, there might be several types of characters. They might have a common way of takingDamage or attacking, but the specifics of how they do this can vary.

4. **Enforcing Algorithm Structure**:
When you want to ensure that the structure of an algorithm remains unchanged but still allow some flexibility in its components.
Example: In a baking application, the steps for making bread might be mix, knead, proof, and bake. While the order should remain the same, the details of mix or knead can vary based on the type of bread.

5. **Database Connections**:
When connecting to different databases, there might be variations in the way a connection is established, queries are made, or results are parsed, but the overall structure remains the same.

6. **Compilers and Translators**:
Different languages might have different syntax and semantics, but the process of lexical analysis, parsing, semantic analysis, and code generation can be templated, with specific parts left for customization.

7. **Test Frameworks**:
The structure of setting up a test, executing it, and then tearing it down can be templated. Specific tests can then provide the exact details of what should be tested and how.

##
The Template Method Pattern is particularly useful when there's a well-defined series of steps for an algorithm, but the implementation of some steps can vary. It provides a framework allowing for consistent application of the algorithm while providing flexibility in how certain steps get implemented.
