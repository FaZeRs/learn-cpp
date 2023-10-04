# Composite Pattern

![Structure](structure.png)

## Intent:
The primary goal of the Composite Pattern is to treat both single (primitive) and composite objects uniformly, enabling a client to ignore differences between compositions of objects and individual objects.

## Components:
Component: This is an abstract class or interface that declares the interface for objects in the composition. It might also provide a default implementation.

Leaf: Represents leaf objects in the composition. It implements all Component methods relevant to individual objects.

Composite: Stores child components, either Leaf or Composite. It implements methods defined in the Component interface and manages child components.

## How it Works:
Leaf objects perform a request directly, and Composite objects forward requests to their child components recursively downwards the tree structure.

This allows clients to use composite structures and individual objects uniformly.

## Advantages:
Simplicity: Clients interact with both composites and individual objects uniformly. This simplifies client code.

Easy Extension: It's easy to add new types of components as long as they adhere to the Component interface.

Hierarchy Representation: It efficiently models recursive structures like directories, organization hierarchies, etc.

## Disadvantages:
Overgeneralization: Can make the design overly general. Some component methods might make sense only for leaf or composite, leading to potential confusion.

## Use cases
1. **Graphic Systems**:
The most classic example of the Composite pattern is graphic systems where shapes can be composed of other shapes. For instance, a drawing can consist of lines, circles, and more complex shapes, which themselves can be composed of other shapes.

2. **File & Directory Systems**:
In operating systems, files and directories can be treated using the Composite pattern. A directory can contain files or other directories. Yet, from a user's perspective, both files and directories can be treated as filesystem elements that have a size, can be rendered (e.g., shown in a GUI), etc.

3. **Organizational Structures**:
If you want to represent organizational hierarchies where an organization is composed of sub-units, and those units can have their own sub-units, and so on. A manager can have employees, but some of those employees might be other managers with their own subordinates.

4. **Menu Systems**:
In GUI toolkits, menus can have menu items, but some of those items can be sub-menus themselves. The Composite pattern can be used to treat both simple menu items and complex menus uniformly.

5. **Renderers and Layout Managers**:
In GUI frameworks, renderers and layout managers often need to deal with both simple components (like a button) and composite components (like a panel containing other components).

6. **Network & Communication Systems**:
Composite can be applied to systems where different components may need to forward messages or data. For instance, in networking, a composite of nodes could represent a network where nodes can be simple end-devices or complex routing devices containing other nodes.

7. **Decision-making structures**:
In some algorithms, decisions are composed of smaller decisions, which can further be broken down. Each of these decisions can be represented as a component in the Composite pattern.

8. **Spatial Partitions**:
In game development or simulation software, you often need to partition space for efficient queries (like collision detection). Structures like QuadTrees or OctTrees can be viewed as composites where each node either contains data or references to its child nodes.

9. **Workflow Systems**:
In systems where tasks can be complex, consisting of multiple sub-tasks, the Composite pattern allows you to treat both simple tasks and complex workflows uniformly.

10. **HTML or XML Parsing**:
When parsing these documents, elements can contain other elements in a tree-like structure. Each element, whether it's a simple text node or a complex div with other elements inside, can be treated uniformly.

##
The Composite Pattern is very useful when dealing with hierarchical systems or structures where you need to process all elements in the structure in a uniform manner, such as graphical systems, organizational structures, or document systems.
