#include <iostream>
#include <vector>

template <typename T>
class Container;

template <typename T>
class ContainerIterator {
public:
    explicit ContainerIterator(const Container<T>* container)
        : m_container(container) {}

    [[nodiscard]] bool hasNext() const {
        return m_index < m_container->size();
    }

    T& next() {
        return m_container->get(m_index++);
    }

private:
    const Container<T>* m_container;
    std::size_t m_index{0};
};

template <typename T>
class Container {
public:
    void add(const T& item) {
        m_items.emplace_back(item);
    }

    std::size_t size() const {
        return m_items.size();
    }

    T& get(std::size_t index) const {
        return m_items[index];
    }

    ContainerIterator<T> iterator() const {
        return ContainerIterator<T>(this);
    }

private:
    mutable std::vector<T> m_items;
};

int main() {
    Container<int> container;
    container.add(1);
    container.add(2);
    container.add(3);

    ContainerIterator<int> it = container.iterator();

    while (it.hasNext()) {
        std::cout << it.next() << ' ';
    }
    std::cout << std::endl;

    return 0;
}
