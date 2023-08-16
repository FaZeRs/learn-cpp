# Visitor Pattern

Visitor is a behavioral design pattern that lets you separate algorithms from the objects on which they operate.

![Structure](structure.png)

## Intent:
The primary intent behind the Visitor pattern is to separate an algorithm from the object structure on which it operates, enabling new operations to be added to existing object structures without modifying those structures.

## Components:
Visitor: This is an interface or abstract class, declaring visit operations for each type of concrete element.

ConcreteVisitor: Implements each operation declared by the Visitor. Each operation implements a fragment of the algorithm designed for the corresponding class of objects.

Element: An interface or abstract class, which defines an accept method that takes a visitor object as an argument.

ConcreteElement: Implements the accept method defined by the Element interface/abstract class.

ObjectStructure: Often represents a collection or set of elements. It can iterate over the collection and allow a visitor to visit its elements.

## How it Works:
Concrete elements allow visitors to visit them.

The visitor knows the concrete class of the element and can access its specific properties if needed.

You can add further operations to the visitor without changing the elements.

## Advantages:
Flexibility: Allows adding new operations without modifying existing object structures. You can define a new operation over an object structure by adding a new visitor.

Separation of Concerns: Separates data and operations on data, following the Single Responsibility Principle.

## Disadvantages:
Growth: If new ConcreteElement classes are added frequently, then the pattern becomes less useful since the Visitor interfaces will also need frequent modification.

Access: A visitor might require access to private data members of the elements it visits, potentially breaking encapsulation.

Intrusiveness: Every class that accepts visitors needs to have an accept method, which can be seen as intrusive, especially if the class hierarchy isn't designed with the Visitor pattern in mind.

## Use cases
1. **Document Renderers**:
Imagine you have a set of objects that represent parts of a document, like paragraphs, images, and tables. By using the Visitor pattern, you can create a renderer that "visits" each of these parts and renders them accordingly, either to a webpage, a PDF, or any other format.

2. **Compiler Operations**:
If you've ever written or worked on a compiler, you've likely used something called an Abstract Syntax Tree (AST). Each node of the tree represents some construct in the source code. The Visitor pattern can be used to traverse the AST and perform various operations, such as semantic checks, code optimization, and code generation.

3. **Analyzing or Manipulating Hierarchical Data Structures**:
This can include operations on structures like XML or JSON. For instance, an XML or JSON processor might use the Visitor pattern to apply transformations or validations based on specific node types.

4. **Report Generation**:
Let's say you have a hierarchy of employee objects, and you want to generate different types of reports, like salary reports, performance reviews, etc. A Visitor can traverse the employee hierarchy and generate these reports accordingly.

5. **GUI Operations**:
Consider a collection of graphic objects like circles, rectangles, and lines in a drawing application. If you want to provide functionalities like "calculate area," "move," or "resize," visitors can be used to implement each of these operations, allowing the underlying objects to remain unmodified.

6. **Game Characters and Actions**:
In gaming, you might have different character types and want to apply various actions or effects to these characters without changing their structure. For instance, a SpellCaster visitor could apply different spells to different characters.

7. **Serialization**:
To convert objects into a format that can be stored or transmitted and later reconstructed, you can use the Visitor pattern. The visitor will handle the process of converting each object in the collection based on its type.

8. **State Validation**:
In complex structures or systems, you might want to validate that everything is in a correct or consistent state. A visitor can traverse the structure and check each part to ensure it meets specific criteria.

9. **Integration with Systems having Inflexible Interfaces**:
If you're integrating with third-party libraries or older systems that you can't change (or it's difficult to change), the Visitor pattern can help adapt or extend functionalities to those objects without modifying their source.

10. **Accumulative Operations**:
If you need to traverse a structure and accumulate some data based on each element (e.g., counting the number of certain types of objects or summing up values), a visitor can be designed for this purpose.

##
The Visitor Pattern is particularly useful when dealing with complex object structures, like an Abstract Syntax Tree (AST) in a compiler. When you have an established structure and want to perform varied operations on it without modification, the Visitor pattern shines.