# Iterator Pattern

![Structure](structure.png)

## Intent:
The primary intent of the Iterator Pattern is to provide a mechanism to access the elements of an aggregate object sequentially without exposing its internal details. It abstracts the traversal of different types of collections.

## Components:
Iterator: This is the interface that defines methods to access and traverse the collection. Common methods include next(), hasNext(), and possibly remove().

ConcreteIterator: An implementation of the Iterator interface.

Aggregate: This is an interface that defines the method for creating an Iterator object.

ConcreteAggregate: An implementation of the Aggregate interface. This is the collection object that can be iterated.

## How it Works:
The client requests an iterator from the aggregate object.

The client then uses the iterator to traverse the elements in the aggregate.

The aggregate object's internal representation is hidden from the client.

## Advantages:
Decoupling: Separates the responsibility of traversal from the collection, promoting the single responsibility principle.

Uniformity: Provides a consistent way to traverse various collections.

Abstracted Internal Representation: The client does not need to know about the internal structure of the collection.

## Disadvantages:
Overhead: Introduces additional classes and objects, which might be an overkill for simple collections.

## Use cases
1. **Browsing a Collection**:
If you have a collection of objects (like a list, tree, or graph), and you want to traverse through its elements without exposing its internal structure, an iterator can be handy.

2. **Different Iteration Methods**:
If you want to provide multiple ways to traverse a collection, iterators allow you to implement various traversal algorithms. For example, a binary tree might support in-order, pre-order, and post-order traversals.

3. **Paging and Lazy Loading**:
If you have a large data set (like a database result), you might not want to load everything into memory at once. Using an iterator, you can load items page-by-page or on-demand.

4. **Abstracting Complex Data Structures**:
If you have a complex data structure (like a composite structure with nested elements), using an iterator can help simplify client code that needs to process each element.

5. **Polymorphic Iteration**:
If you have a group of collections that should be iterated over in a consistent manner but have different internal representations, using an iterator can offer a common interface for iteration.

6. **Maintaining State**:
Some iterations might be more complicated than just moving through a list. For instance, when traversing a maze or a tree, the iterator might need to maintain some state (like visited nodes). This state management is abstracted away from the client code and encapsulated in the iterator.

7. **Filtering Elements**:
Iterators can be implemented in a way that they only return certain elements of a collection based on some criteria, effectively allowing for filtered iterations. For example, an iterator might return only even numbers from a collection of integers.

8. **Supporting Multiple Iterators**: Sometimes, you might want to have more than one iteration ongoing at the same time over the same collection. Dedicated iterator objects allow you to do this without interference.

9. **Ensuring a Safe Iteration**: By decoupling the iteration process from the collection, you can ensure that the underlying collection isn't modified during the iteration (unless explicitly allowed).

10. **Adapting Different Interfaces**: If you are working with third-party or legacy collections that have different interfaces for iteration, you can implement a unifying iterator that adapts these diverse interfaces to a common one.

##
The Iterator Pattern is particularly beneficial when you have complex data structures (like trees or graphs) and you want to provide a uniform interface for traversal. It also provides a clear separation of concerns, ensuring that the collection is responsible for storage and the iterator is responsible for traversal.
