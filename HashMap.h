#include <array>
#include <initializer_list>
#include <stdexcept>
#include <cmath>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    class Node {
    public:
        std::pair<const KeyType, ValueType> *p = nullptr;
        bool in_arr = false;
        bool end = false;
        size_t psl = 0;

        Node() = default;

        Node(const std::pair<const KeyType, ValueType> &p_, size_t psl_) : in_arr(true), psl(psl_) {
            p = new std::pair<const KeyType, ValueType>(p_);
        }

        ~Node() {
            if (p) {
                delete p;
            }
        }

        Node &operator=(const Node &node) {
            if (this == &node) {
                return *this;
            }
            delete p;
            p = node.p;
            psl = node.psl;
            in_arr = node.in_arr;
            end = node.end;
            return *this;
        }
    };

    const size_t min_size = 77;
    Node *arr = nullptr;
    size_t arr_size = 0;
    size_t cur_size = 0;
    Hash hash;
    const double load_factor = 0.777;

public:
    class iterator {
    public:
        Node *ptr = nullptr;

        iterator() : ptr(nullptr) {}

        explicit iterator(Node *ptr_) : ptr(ptr_) {}

        iterator& operator++() {
            if (ptr->end) {
                ++ptr;
                return *this;
            }
            ++ptr;
            while (!ptr->in_arr) {
                if (ptr->end) {
                    ++ptr;
                    return *this;
                }
                ++ptr;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator cur(*this);
            ++(*this);
            return cur;
        }

        bool operator==(const iterator &it) const {
            return ptr == it.ptr;
        }

        bool operator!=(const iterator &it) const {
            return ptr != it.ptr;
        }

        std::pair<const KeyType, ValueType> *operator->() const {
            return ptr->p;
        }

        std::pair<const KeyType, ValueType> &operator*() const {
            return *(ptr->p);
        }
    };

    class const_iterator {
    public:
        const Node *ptr = nullptr;

        const_iterator() : ptr(nullptr) {}

        explicit const_iterator(const Node *ptr_) : ptr(ptr_) {}

        const_iterator& operator++() {
            if (ptr->end) {
                ++ptr;
                return *this;
            }
            ++ptr;
            while (!ptr->in_arr) {
                if (ptr->end) {
                    ++ptr;
                    return *this;
                }
                ++ptr;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator cur(*this);
            ++(*this);
            return cur;
        }

        bool operator==(const const_iterator &it) const {
            return ptr == it.ptr;
        }

        bool operator!=(const const_iterator &it) const {
            return ptr != it.ptr;
        }

        const std::pair<const KeyType, ValueType> *operator->() const {
            return ptr->p;
        }

        const std::pair<const KeyType, ValueType> &operator*() const {
            return *(ptr->p);
        }
    };

    explicit HashMap(Hash hash_ = Hash()) : hash(hash_) {
        arr_size = min_size;
        arr = new Node[arr_size];
        arr[arr_size - 1].end = true;
    }

    template<class Iterator>
    HashMap(Iterator begin, Iterator end, Hash hash_ = Hash()) : hash(hash_) {
        arr_size = min_size;
        arr = new Node[arr_size];
        arr[arr_size - 1].end = true;
        while (begin != end) {
            insert(*begin);
            begin++;
        }
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> pairs, Hash hash_ = Hash()) : hash(hash_) {
        arr_size = min_size;
        arr = new Node[arr_size];
        arr[arr_size - 1].end = true;
        for (const auto &pair: pairs) {
            insert(pair);
        }
    }

    HashMap(const HashMap &map){
        arr_size = min_size;
        arr = new Node[arr_size];
        cur_size = 0;
        arr[arr_size - 1].end = true;
        for (size_t i = 0; i < map.arr_size; ++i) {
            if (map.arr[i].in_arr) {
                insert(*(map.arr[i].p));
            }
        }
    }

    ~HashMap() {
        delete[] arr;
    }

    HashMap &operator=(const HashMap &map) {
        if (this == &map) {
            return *this;
        }
        delete[] arr;
        arr_size = min_size;
        arr = new Node[arr_size];
        cur_size = 0;
        arr[arr_size - 1].end = true;
        for (size_t i = 0; i < map.arr_size; ++i) {
            if (map.arr[i].in_arr) {
                insert(*(map.arr[i].p));
            }
        }
        return *this;
    }

    size_t size() const {
        return cur_size;
    }

    bool empty() const {
        return cur_size == 0;
    }

    Hash hash_function() const {
        return hash;
    }

    iterator begin() {
        for (size_t i = 0; i <= arr_size; ++i) {
            if (i == arr_size || arr[i].in_arr) {
                return iterator(arr + i);
            }
        }
        return end();
    }

    iterator end() {
        return iterator(arr + arr_size);
    }

    const_iterator begin() const {
        for (size_t i = 0; i <= arr_size; ++i) {
            if (i == arr_size || arr[i].in_arr ) {
                return const_iterator(arr + i);
            }
        }
        return end();
    }

    const_iterator end() const {
        return const_iterator(arr + arr_size);
    }

    iterator find(const KeyType &key) {
        size_t pos = hash(key) % arr_size;
        while (arr[pos].in_arr) {
            if (arr[pos].p->first == key) {
                return iterator(&arr[pos]);
            }
            pos = (pos + 1) % arr_size;
        }
        return end();
    }

    const_iterator find(const KeyType &key) const {
        size_t pos = hash(key) % arr_size;
        while (arr[pos].in_arr) {
            if (arr[pos].p->first == key) {
                return const_iterator(&arr[pos]);
            }
            pos = (pos + 1) % arr_size;
        }
        return end();
    }

    void check_size() {
        if (load_factor * arr_size <= (double) cur_size + 1) {
            size_t arr_size2 = arr_size * 2;
            Node *arr2 = new Node[arr_size2];
            std::swap(arr_size, arr_size2);
            std::swap(arr, arr2);
            cur_size = 0;
            arr[arr_size - 1].end = true;
            for (size_t i = 0; i < arr_size2; ++i) {
                Node *cur = arr2 + i;
                if (cur->in_arr) {
                    insert(*(cur->p));
                }
            }
            delete[] arr2;
        }
    }

    void swap_nodes(Node &node1, Node &node2) {
        std::swap(node1.p, node2.p);
        std::swap(node1.psl, node2.psl);
        std::swap(node1.in_arr, node2.in_arr);
    }

    void insert(const std::pair<KeyType, ValueType> &pair) {
        if (find(pair.first) != end()) {
            return;
        }
        check_size();
        size_t pos = hash(pair.first) % arr_size;
        Node to_add(pair, 0);
        while (true) {
            if (!arr[pos].in_arr) {
                swap_nodes(arr[pos], to_add);
                break;
            } else {
                if (to_add.psl > arr[pos].psl) {
                    swap_nodes(arr[pos], to_add);
                }
                pos = (pos + 1) % arr_size;
                to_add.psl += 1;
            }
        }
        cur_size += 1;
        arr[arr_size - 1].end = true;
    }

    void erase(const KeyType &key) {
        iterator it = find(key);
        if (it == end()) {
            return;
        }
        it.ptr->in_arr = false;
        Node *cur_ptr = it.ptr;
        Node *next_ptr = ++cur_ptr;
        --cur_ptr;
        if (next_ptr == arr + arr_size) {
            next_ptr = arr;
        }
        while (next_ptr->in_arr && (next_ptr->psl != 0)) {
            swap_nodes(*cur_ptr, *next_ptr);
            --(cur_ptr->psl);
            cur_ptr = next_ptr;
            next_ptr = ++cur_ptr;
            --cur_ptr;
            if (next_ptr == arr + arr_size) {
                next_ptr = arr;
            }
        }
        --cur_size;
        arr[arr_size - 1].end = true;
    }

    ValueType &operator[](const KeyType &key) {
        iterator it = find(key);
        if (it == end()) {
            insert(std::make_pair(key, ValueType()));
            it = find(key);
        }
        return it->second;
    }

    const ValueType &at(const KeyType &key) const {
        const_iterator it = find(key);
        const_iterator End = end();
        if (it == End) {
            throw std::out_of_range("HashMap: out of range");
        }
        return it->second;
    }

    void clear() {
        delete[] arr;
        cur_size = 0;
        arr_size = min_size;
        arr = new Node[arr_size];
        arr[arr_size - 1].end = true;
    }
};
