#include <iostream>
#include <exception>
#include <iterator>
#include <string>

#define TAIL true

template<typename T>
class List {
  class Node;

  template<typename U>
  class iterator_impl;
public:
  explicit List(): _size(0), tail(new Node(TAIL)) {}
  ~List() {
    Node* cur = head;
    Node* next;
    while (cur) {
      next = cur->next;
      delete cur;
      cur = next;
    }
  }

  void push_back(T const& val) {
    Node* new_node = new Node(val);
    if (_size == 0) {
      head = new_node;
      head->next = tail;
      tail->prev = head;
    } else {
      tail->value = val;
      tail->tail = false;
      tail->next = new_node;
      new_node->prev = tail;
      tail = new_node;
      new_node->tail = true;
    }

    ++_size;
  }

  void push_front(T const& val) {
    Node* new_node = new Node(val);
    if (_size == 0) {
      head = new_node;
      head->next = tail;
      tail->prev = head;
    } else {
      head->prev = new_node;
      new_node->next = head;
      head = new_node;
    }

    ++_size;
  }

  void insert(int64_t i, T const& val) {
    if (i < 0 || i >= _size)
    throw std::out_of_range("Very informative message");

    if (i == _size - 1)
      push_back(val);
    else {
      Node* new_node = new Node(val);
      Node* cur = head;
      for (size_t j = 0; j != i; ++j)
      cur = cur->next;

      if(cur->next){
        Node* next = cur->next;
        next->prev = new_node;
        new_node->next = next;
      }

      cur->next = new_node;
      new_node->prev = cur;

      ++_size;
    }
  }

  void pop_front() {
    if (_size == 0)
    throw std::logic_error("The list is empty");
    else if (_size == 1) {
      delete head;
      head = nullptr;
    } else {
      Node* old_head = head;
      head = head->next;
      delete old_head;
    }

    --_size;
  }

  void pop_back() {
    if (_size == 0)
    throw std::logic_error("The list is empty");
    else if (_size == 1) {
      delete head;
      head = nullptr;
    } else {
      Node* old_tail = tail->prev;
      tail->prev = tail->prev->prev;
      tail->prev->next = tail;
      delete old_tail;
    }

    --_size;
  }

  void remove(size_t i) {
    if (i < 0 || i >= _size)
      throw std::out_of_range("Very informative message");
    else if (i == _size - 1)
      pop_back();
    else if (i == 0)
      pop_front();
    else {
      Node* cur = head;
      for (size_t j = 0; j != i; ++j)
      cur = cur->next;
      cur->prev->next = cur->next;
      cur->next->prev = cur->prev;
      delete cur;
      --_size;
    }
  }

  T& operator[](size_t i) {
    if (i < 0 || i >= _size)
      throw std::out_of_range("Very informative message");
    else {
      Node* cur = head;
      for (int j = 0; j != _size; ++j) {
        if (j == i)
          return cur->value;
        cur = cur->next;
      }
    }
  }

  T const& operator[](size_t i) const {
    if (i < 0 || i >= _size)
      throw std::out_of_range("Very informative message");
    else {
      Node* cur = head;
      for (int j = 0; j != _size; ++j) {
        if (j == i)
          return cur->value;
        cur = cur->next;
      }
    }
  }

  T& at(size_t i) {
    return operator[](i);
  }

  T const& at(size_t i) const {
    return operator[](i);
  }

  size_t size() const {
    return _size;
  }

  bool empty() const {
    return !_size;
  }

  using iterator = iterator_impl<T>;

  iterator begin() {
    return iterator(head, head, tail);
  }

  iterator end() {
    return iterator(tail, head, tail);
  }

  using const_iterator = iterator_impl<T const>;

  const_iterator cbegin() {
    return const_iterator(head, head, tail);
  }

  const_iterator cend() {
    return const_iterator(tail, head, tail);
  }

  using reverse_iterator = std::reverse_iterator<iterator>;

  reverse_iterator rbegin() {
    iterator it = end();
    return std::make_reverse_iterator(it);
  }

  reverse_iterator rend() {
    iterator it = begin();
    return std::make_reverse_iterator(it);
  }

  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  const_reverse_iterator crbegin() {
    const_iterator it = cend();
    return std::make_reverse_iterator(it);
  }

  const_reverse_iterator crend() {
    const_iterator it = cbegin();
    return std::make_reverse_iterator(it);
  }

  template<typename U>
  friend std::ostream& operator<<(std::ostream& out, List<U> const& list);
private:
  class Node {
  public:
    Node(bool is_tail=false): tail(is_tail) {}
    Node(T const& value, bool is_tail=false): value(value), tail(is_tail) {}

    bool is_tail() const {
      return tail;
    }

    friend class List;
    template<typename U>
    friend std::ostream& operator<<(std::ostream& out, List<U> const& list);

    T value;
  private:
    Node* next = nullptr;
    Node* prev = nullptr;
    bool tail = false;
  };

  template <typename U>
  class iterator_impl: public std::iterator<std::bidirectional_iterator_tag, U> {
  public:
    explicit iterator_impl(Node* node, Node* head, Node* tail): _node(node),
    head(head),
    tail(tail) {}

    iterator_impl<U> operator++() {
      if (_node == tail)
        throw std::logic_error("Increment end iterator or decrement rbegin iterator");
      _node = _node->next;
      return *this;
    }

    iterator_impl<U> operator++(int) {
      iterator_impl<U> res(*this);
      ++(*this);
      return res;
    }

    iterator_impl<U> operator--() {
      if (_node == head)
        throw std::logic_error("Decrement begin iterator or operator* on rend iterator");
      _node = _node->prev;
      return *this;
    }

    iterator_impl<U> operator--(int) {
      iterator_impl<U> res(*this);
      --(*this);
      return res;
    }

    bool operator==(iterator_impl<U> const& rhs) const {
      return _node == rhs._node;
    }

    bool operator!=(iterator_impl<U> const& rhs) const {
      return _node != rhs._node;
    }

    U& operator*() {
      if (_node == tail)
        throw std::logic_error("operator* on end interator");
      return _node->value;
    }

    U* operator->() {
      return &_node->value;
    }

  private:
    Node* _node;
    Node* tail;
    Node* head;
  };

  Node* head = nullptr;
  Node* tail = nullptr;
  size_t _size = 0;
};

template<typename U>
std::ostream& operator<<(std::ostream& out, List<U> const& list) {
  typename List<U>::Node* cur = list.head;
  if (cur) {
    while (!cur->next->is_tail()) {
      out << cur->value << " <=> ";
      cur = cur->next;
    }

    out << cur->value;
  }

  return out;
}

int main() {
  List<std::string> l;
  l.push_front("pf_0");
  l.push_back("pb_0");
  l.push_front("pf_1");
  l.push_back("pb_1");
  l.push_front("pf_2");
  l.push_back("pb_2");
  l.push_back("pb_3");
  l.push_front("pf_3");
  l.pop_front();
  l.pop_front();
  l.pop_back();
  l.pop_back();
  l.insert(2, "i_2");
  l.remove(2);
  std::cout << l << std::endl;

  std::cout << "l[3] = " << l[3] << std::endl;

  List<std::string>::iterator l_it = l.begin();
  std::cout << "*l_it: " << *l_it << std::endl;
  l_it++;
  std::cout << "*l_it: " << *l_it << std::endl;
  --l_it;
  std::cout << "*l_it: " << *l_it << std::endl;
  ++l_it, ++l_it, ++l_it, ++l_it;
  std::cout << (l_it == l.end()) << std::endl;

  List<std::string>::const_iterator const_it = l.cbegin();

  for (l_it = l.begin(); l_it != l.end(); ++l_it)
    std::cout << l_it->at(0) << ' ';
  std::cout << std::endl;
  for (auto l_el: l)
    std::cout << l_el << ' ';
  std::cout << std::endl;

  List<std::string>::reverse_iterator r_it = l.rbegin();
  std::cout << "r_it: " << *r_it << std::endl;
  for (; r_it != l.rend(); ++r_it)
    std::cout << *r_it << ' ';
  std::cout << std::endl;

  // List<int>::iterator l_it = l.begin();
  // std::cout << "l_it value: " << (*l_it).value << std::endl;
  // std::cout << l << std::endl;
  // (*l_it).value = 1488;
  // std::cout << "l_it value: " << (*l_it).value << std::endl;
  // std::cout << l << std::endl;
  //
  // l.pop_back();
  // std::cout << l << std::endl;
  return 0;
}
