#include <iostream>
#include <vector>

template <typename T>
class SS_Traits{
public:
    typedef T  value_t;
    typedef std::vector<value_t> container_t;
    typedef value_t pcontainer_t;
    typedef typename container_t::iterator iterator_t;

    class simple_search{
    public:
        bool operator() (container_t a, value_t v){
            return false;
        }
    };

    class post_order_print{
    public:
        void operator() (void){
            std::cout << "post order" << std::endl;
        }
    };

    typedef simple_search functor_t;
    typedef post_order_print print_t;
};

template <typename T>
class BS_Traits{
public:
    typedef T  value_t;
    typedef std::vector<value_t> container_t;
    typedef std::vector<value_t*> pcontainer_t;
    typedef typename container_t::iterator iterator_t;

    class binary_search{
    public:
        bool operator() (container_t a, value_t v){
            return false;
        }
    };

    class pre_order_print{
    public:
        void operator() (void){
            std::cout << "pre order" << std::endl;
        }
    };

    typedef binary_search functor_t;
    typedef pre_order_print print_t;
};

template <typename T, int ORDER>
class BNode {
public:
    typedef typename T::value_t value_t;
    typedef typename T::container_t container_t;
    typedef typename T::pcontainer_t pcontainer_t;

    value_t data[ORDER + 1];
    BNode<T,ORDER> *children[ORDER+2];
    int count; // to count the number of elements
    std::size_t  order;

    BNode(void):order(ORDER){
        count = 0;
        for (int i = 0; i < order + 2; i++) {
            children[i] = NULL;
        }
    }

    void insert_in_node(int pos, const value_t &value) {
        int j = count;
        while (j > pos) {
            data[j] = data[j - 1];
            children[j + 1] = children[j];
            j--;
        }
        data[j] = value;
        children[j + 1] = children[j];

        count++;
    }

    bool is_overflow() { return count > ORDER; }

    ~BNode(void){}
};

template <typename T, int S>
class BTree {
public:
    enum state {
        BT_OVERFLOW,
        NORMAL,
    };

    typedef typename T::value_t value_t;
    typedef typename T::functor_t functor_t;
    typedef typename T::print_t print_t;

    BNode<T,S>* root;
    //print_t print;
    functor_t search;

    BTree(void){
        root = new BNode<T,S>;
    }

    ~BTree(void){}

    void insert(const value_t& value){
        int state = insert(root, value);
        if (state == BT_OVERFLOW) {
            split_root(root, value);
        }
    }

    int insert(BNode<T,S> *ptr, const value_t &value) {

        int pos = 0;
        while (pos < ptr->count && ptr->data[pos] < value) {
            pos++;
        }
        //pos now have the real position

        if (ptr->children[pos] != NULL) {
            // long page_id = children[pos];
            int state = insert(ptr->children[pos], value);
            if (state == BT_OVERFLOW) {
                split(ptr, pos);
            }
        } else {
            ptr->insert_in_node(pos, value);
        }
        return ptr->is_overflow() == true ? BT_OVERFLOW : NORMAL;
    }

    void split(BNode<T,S> *ptr, int pos) {
        BNode<T,S> *node_in_overflow = ptr->children[pos];
        BNode<T,S> *child1 = node_in_overflow;
        child1->count = 0;
        BNode<T,S> *child2 = new BNode<T,S>();

        int iter = 0;
        int i;
        for (i = 0; iter < S / 2; i++) {
            child1->children[i] = node_in_overflow->children[iter];
            child1->data[i] = node_in_overflow->data[iter];
            child1->count++;

            iter++;
        }
        child1->children[i] = node_in_overflow->children[iter];

        ptr->insert_in_node(pos, node_in_overflow->data[iter]);

        iter++; // the middle element

        for (i = 0; iter < S + 1; i++) {
            child2->children[i] = node_in_overflow->children[iter];
            child2->data[i] = node_in_overflow->data[iter];
            child2->count++;

            iter++;
        }
        child2->children[i] = node_in_overflow->children[iter];

        ptr->children[pos] = child1;
        ptr->children[pos + 1] = child2;
    }

    void split_root(BNode<T,S> *ptr, const value_t &value) {
        BNode<T,S> *node_in_overflow = ptr;
        BNode<T,S> *child1 = new BNode<T,S>();
        BNode<T,S> *child2 = new BNode<T,S>();

        int pos = 0;
        int iter = 0;
        int i;
        for (i = 0; iter < S / 2; i++) {
            child1->children[i] = node_in_overflow->children[iter];
            child1->data[i] = node_in_overflow->data[iter];
            child1->count++;

            iter++;
        }
        child1->children[i] = node_in_overflow->children[iter];
        iter++; // the middle element
        for (i = 0; iter < S + 1; i++) {
            child2->children[i] = node_in_overflow->children[iter];
            child2->data[i] = node_in_overflow->data[iter];
            child2->count++;

            iter++;
        }
        child2->children[i] = node_in_overflow->children[iter];

        ptr->children[pos] = child1;
        ptr->data[0] = node_in_overflow->data[S / 2];
        ptr->children[pos + 1] = child2;
        ptr->count = 1;
    }

    bool find(const value_t = 0) const{
        // TODO :: SEARCH
        // search(x); inside each page
        return false;
    }

    void print() {
        print(root, 0);
        std::cout << "________________________\n";
    }
    void print(BNode<T,S> *ptr, int level) {
        if (ptr) {
            int i;
            for (i = ptr->count - 1; i >= 0; i--) {
                print(ptr->children[i + 1], level + 1);

                for (int k = 0; k < level; k++) {
                    std::cout << "    ";
                }
                std::cout << ptr->data[i] << "\n";
            }
            print(ptr->children[i + 1], level + 1);
        }
    }

    template <typename _T, int _S>
    friend std::ostream& operator<< (std::ostream& out, BTree<_T,_S> tree){
        tree.print();// (out)
        // IN PRE POST LEVEL ORDER
        return out;
    }

};

int main() {
    typedef BS_Traits<int> btrait_t;
    BTree<btrait_t,3> tree;
    tree.find(10);

    tree.insert(4);
    tree.insert(5);
    tree.insert(6);
    tree.insert(2);

    std::cout<<tree<< std::endl;


    typedef SS_Traits<float> strait_t;
    //BTree<strait_t,10> stree;
    //std::cout<<stree<< std::endl;

}
