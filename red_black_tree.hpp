#ifndef RED_BLACK_TREE_HPP
# define RED_BLACK_TREE_HPP

#include <iostream>
#include <map>

# define BLACK 0
# define RED 1

namespace ft{

    template<class Pair>
    class TNode
    {
        public:
            typedef Pair                            value_type;

            value_type                              *data; 
            TNode                                   *left;
            TNode                                   *right;
            TNode                                   *parent;
            bool                                    color;
        

        TNode():data(NULL), left(NULL), right(NULL), parent(NULL), color(RED){}
        
        TNode(const TNode<Pair>& obj){*this = obj;}
        TNode(const value_type& data):data(data), left(NULL), right(NULL), parent(NULL), color(RED){}
        
        TNode<Pair>& operator=(const TNode<Pair>& obj)
        {
            this->data = obj->data;
            this->left = obj->left;
            this->right = obj->right;
            this->parent = obj->parent;
            this->color = obj.color;
        }

        ~TNode(){}


        TNode*      getGrandFather()
        {
            if (parent) 
                return parent->parent;
            return NULL;
        }

        TNode*      getUncle()
        {
            if (getGrandFather() != NULL)
            {
                if (parent->isLeftChild())
                    return getGrandFather()->right;
                else
                    return getGrandFather()->left;
            }
            return NULL;
        }

        TNode*      getSibling()
        {
            if (parent != NULL)
            {
                if (this->isLeftChild())
                    return this->parent->right;
                else
                    return this->parent->left;
            }
            return NULL;
        }

        void        flipColor() { (color!= BLACK) ? color = BLACK : color = RED;}

        bool        isLeftChild(){return this == parent->left;};
        bool        isRightChild(){return this == parent->right;};
  
};

/**
 * In red black tree we use recoloring and rotation
 * if recoloring doesn't work, then we go for rotation
 * we are dealing here with red and black nodes instead of height
 */

/*
  >> is interpreted as shift operator which may cause syntax error , it must be separated
 */


    template<class Pair, class Compare, class Allocator >
    class RBtree
    {

        public:

            typedef Pair                                                                    value_type;
            typedef typename Pair::first_type                                               key_type;
            typedef Allocator                                                               type_allocator;
            typedef typename Allocator::template rebind<ft::TNode<Pair> >::other	        node_allocator;
            typedef size_t                                                                  size_type;


        private:
            ft::TNode<value_type>*                                                          _tree;
            type_allocator                                                                  _myPairAlloc;
            node_allocator                                                                  _myNodeAlloc;
            size_type                                                                       _size;
            Compare                                                                         _compare;
            ft::TNode<value_type>*                                                          _minMax;

        public:
            RBtree():_tree(NULL), _myPairAlloc(), _myNodeAlloc(), _size(0)
            {
                _minMax = _myNodeAlloc.allocate(1);
                _minMax->left = NULL;
                _minMax->right = NULL;
            }

            RBtree(const RBtree& x):_tree(NULL),_myPairAlloc(x._myPairAlloc), _myNodeAlloc(x._myNodeAlloc)
            {
                _minMax = _myNodeAlloc.allocate(1);
                _minMax->left = NULL;
                _minMax->right = NULL;
                if (x._tree == NULL)
                    return;
                preorder(x._tree);
                _size = x._size;
            }

            RBtree& operator=(const RBtree& x)
            {
                delete_all(_tree);
                _myPairAlloc = x._myPairAlloc;
                _myNodeAlloc = x._myNodeAlloc;
                _minMax->left = NULL;
                _minMax->right = NULL;
                _compare = x._compare;
                _compare = x._compare;
                inorder(x._tree);
                _size = x._size;
                return (*this);
            }

            void clear(){
                if (_tree != NULL)
                    delete_all(_tree);
                _size = 0;
                _tree = NULL;
            }

            ~RBtree(){this->clear();_myNodeAlloc.deallocate(_minMax,1);}

            void deleteNode(TNode<value_type> * &node)
            {
                _myPairAlloc.destroy(node->data);
                _myPairAlloc.deallocate(node->data, 1);
                _myNodeAlloc.destroy(node);
                _myNodeAlloc.deallocate(node, 1);
            }

            void delete_all(TNode<value_type>* &node)
            {
                if (node != NULL)
                {
                    delete_all(node->left);
                    delete_all(node->right);
                    _myPairAlloc.destroy(node->data);
                    _myPairAlloc.deallocate(node->data, 1);
                    node->data = NULL;
                    _myNodeAlloc.deallocate(node, 1);
                    node = NULL;
                }
                _minMax->left = NULL;
                _minMax->right = NULL;
                _size = 0;
            }

            void swap(RBtree &x)
            {
                TNode<value_type>*tmp = _tree;
                _tree = x._tree;
                x._tree = tmp;
                
                size_t sizetmp = _size;
                _size = x._size;
                x._size = sizetmp;
                TNode<value_type>*tmpmM = _minMax;

                _minMax = x._minMax;
                x._minMax = tmpmM;

            }
    
            void RBTinsert(const value_type& val)
            {
                if (!search(val.first))
                {
                    _size++;
                    TNode<value_type>* new_element = _createNode(val);
                    if (_tree == NULL)
                    {
                        new_element->flipColor();
                        _tree = new_element;
                        _minMax->right = _tree;
                        _minMax->left = _tree;
                        return;
                    }
                    else
                    {
                       TNode<value_type>* temp = _tree;
                       TNode<value_type>* prev_temp = NULL;

                       while (temp != NULL)
                       {
                           prev_temp = temp;
                           if (_compare(val.first , temp->data->first))
                                temp = temp->left;
                            else
                                temp = temp->right;
                       } 
                       new_element->parent = prev_temp;
                        if (_compare(val.first , prev_temp->data->first))
                            prev_temp->left = new_element;
                        else
                            prev_temp->right = new_element;
                    }
                    _fixBalanceAfterInsert(new_element);
                    _minMax->right = getMax(getRoot());
                    _minMax->left = getMin(getRoot());
                }
            }

       
            int RBTdelete(const key_type& k)
            {
                TNode<value_type> *nodeToDelete = search(k);
                if (nodeToDelete)
                {
                    --_size;
                    if (nodeToDelete == _tree)
                        _deleteRoot();
                    else
                        _BSTdelete(nodeToDelete);
                    _minMax->right = getMax(getRoot());
                    _minMax->left = getMin(getRoot());
                    return 1;
                }
                return 0;
            }
        
            bool isEmpty() const
            {
                if (_tree)
                    return false;
                return true;
            }
        

            TNode<value_type>* search(const key_type& k)const
            {
                if (_tree == NULL)
                    return NULL;
                TNode<value_type>* tmp = _tree;
                while (tmp && tmp->data->first != k)
                {
                    if (_compare(k , tmp->data->first))
                        tmp = tmp->left;
                    else
                        tmp = tmp->right;
                }
                return tmp;
            }

            TNode<value_type>* getMax(TNode<value_type>* subtree)const
            {
                TNode<value_type>* tmp = subtree;
                while (tmp && tmp->right != NULL)
                    tmp = tmp->right;
                return tmp;
            }

            TNode<value_type>* getMin(TNode<value_type>* subtree)const
            {
                TNode<value_type>* tmp = subtree;
                while (tmp && tmp->left != NULL)
                    tmp = tmp->left;
                return tmp;
            }

            TNode<value_type>* get_min_max() const{return(_minMax);}
            
            TNode<value_type>* getRoot()const{return _tree;}

            void postorder(TNode<value_type>* node)
            {
                if (node == NULL)
                    return;
                postorder(node->left);
                postorder(node->right);
                RBTinsert(*(node->data));
            }

            void inorder(TNode<value_type>* node)
            {
                if (node == NULL)
                    return;
                inorder(node->left);
                RBTinsert(*(node->data));
                inorder(node->right);
            }

            void preorder(TNode<value_type>* node)
            {
                if (node == NULL)
                    return;
                RBTinsert(*(node->data));
                preorder(node->left);
                preorder(node->right);
            }

        /**
         * 
         *  =============================== display the tree ===============================  
         */

        // void	display_tree(const std::string &prefix, TNode<value_type>* node, bool isLeft)
        // {
		//     if(node != NULL)
		//     {
        //         std::cout << prefix;
        //         std::cout << (isLeft?"├──" : "└──");
        //         std::cout << "|" << node->data->first << " : " <<node->data->second << " : ";
        //         (node->color == 1)? std::cout<<"RED" :std::cout<<"BLACK";
        //         std::cout<<std::endl;

        //         display_tree(prefix+(isLeft?"│   "
        //             : "    "),
		//         node->left, true);
        //             display_tree(prefix+(isLeft?"│   "
        //             : "    "),
		//         node->right, false);
		//     }
	    // }
        
	    // void	display_tree(){
		//     std::cout << std::endl;
		//     display_tree("$", _tree, false);
          
	    // }
        size_type size()const{return _size;}
        size_type max_size()const {return _myNodeAlloc.max_size();}
        
        private:

            TNode<value_type>*   _createNode(const value_type& data)
            {
                TNode<value_type> *new_element = _myNodeAlloc.allocate(1);
                _myNodeAlloc.construct(new_element);
    

                new_element->data = _myPairAlloc.allocate(1);
                _myPairAlloc.construct(new_element->data, data);

                return (new_element);
            }

            void        _rightRotation(TNode<value_type>* node)
            {
                TNode<value_type>* parent = node->parent;
                TNode<value_type>* left_node = node->left;

                node->left = left_node->right;

                if (left_node->right != NULL)
                    left_node->right->parent = node;
            
                left_node->right = node;
                node->parent = left_node;

                /* Replace parents Child */
                if (parent == NULL)
                    _tree = left_node;
                else if (parent->left == node)
                    parent->left = left_node;
                else
                    parent->right = left_node;
                if (left_node != NULL)
                    left_node->parent = parent;
            }

            void _leftRotation(TNode<value_type>* node)
            {
                TNode<value_type>* right_node = node->right;
                TNode<value_type>* parent = node->parent;

                node->right = right_node->left;

                if (right_node->left != NULL)
                    right_node->left->parent = node;

                right_node->left = node;
                node->parent = right_node;

                /* Replace parent child */
                if (parent == NULL)
                    _tree = right_node;
                else if (parent->left == node)
                    parent->left = right_node;
                else
                    parent->right = right_node;
                if (right_node != NULL)
                    right_node->parent = parent;
        }

        void _fixBalanceAfterInsert(TNode<value_type> *curr_node)
        {
            TNode<value_type> *parent        = curr_node->parent;
            TNode<value_type> *grandfather   = curr_node->getGrandFather();
            TNode<value_type> *uncle         = curr_node->getUncle();

            if (curr_node && curr_node != _tree )//! curr_node->getParent() == NULL
            {
                if (parent && parent != _tree && parent->color == RED)
                {
                    if (uncle && uncle->color == RED) // only recoloring
                    {
                        parent->flipColor(); // BLACK
                        uncle->flipColor(); // BLACK
                        if (grandfather && grandfather != _tree) //it's not the root
                            grandfather->flipColor();
                        _fixBalanceAfterInsert(grandfather);
                    }
                    else //uncle black or NULL
                    {
                        //case1 : RR, RL
                        if (parent && parent->isRightChild())
                        {
                            if (curr_node->isLeftChild())
                            {
                                _rightRotation(parent);
                                parent = curr_node;
                            }
                            _leftRotation(grandfather);
                            //recolorRightPart(curr_node, parent, grandfather));
                        }
                        //case2 : LL, LR
                        else if (parent && parent->isLeftChild())
                        {
                            if (curr_node->isRightChild())
                            {
                                _leftRotation(parent);
                                parent = curr_node;
                            }
                            _rightRotation(grandfather);
                            //recolorLeftPart(curr_node, parent, grandfather));
                        }
                        parent->flipColor();
                        grandfather->color = RED;
                    }
                }
            }
        }

        void _fixBalanceAfterDeletion(TNode<value_type> *node)
        {
            if (node == _tree)
                return;
            TNode<value_type> *sibling = node->getSibling();
            TNode<value_type> *parent = node->parent;
            if (!sibling)
                _fixBalanceAfterDeletion(parent);
            else
            {
                if (sibling->color == RED)
                {
                    parent->color = RED;
                    sibling->color = BLACK;
                    if (sibling->isRightChild())
                        _leftRotation(parent);
                    else
                        _rightRotation(parent);
                    _fixBalanceAfterDeletion(node);
                }
                else
                {
                    if ((sibling->left!=NULL && sibling->left->color == RED) ||
                    (sibling->right!= NULL && sibling->right->color==RED))
                    {
                        if (sibling->left != NULL and sibling->left->color == RED)
                        {
                            if (sibling->isLeftChild())
                            {
                                sibling->left->color = sibling->color;
                                sibling->color = parent->color;
                                _rightRotation(parent);
                            }
                            else
                            {
                                sibling->left->color = parent->color;
                                _rightRotation(sibling);
                                _leftRotation(parent);
                            }
                        }
                    else
                    {
                        if (sibling->isLeftChild())
                        {
                            sibling->right->color = parent->color;
                            _leftRotation(sibling);
                            _rightRotation(parent);
                        }
                        else
                        {
                            sibling->right->color = sibling->color;
                            sibling->color = parent->color;
                            _leftRotation(parent);
                        }
                    }
                    parent->color = BLACK;
                    } 
                    else
                    {
                        sibling->color = RED;
                        if (parent->color == BLACK)
                            _fixBalanceAfterDeletion(parent);
                        else
                            parent->color = BLACK;
                    }
                }
            }
        }

        void _deleteRoot()
        {
            if (_tree->left == NULL && _tree->right == NULL)
            {
                deleteNode(_tree);
                _tree = NULL;
            }
            else if (_tree->left == NULL || _tree->right == NULL)
            {
                TNode<value_type>* tmp = _tree;

                if (_tree->left)
                {
                    _tree = _tree->left;
                    _tree->color = BLACK;
                    deleteNode(tmp);
                  
                    tmp = NULL;
                }
                else
                {
                    _tree = _tree->right;
                    _tree->color = BLACK;
                    deleteNode(tmp);
                  
                    tmp = NULL;
                }
            }
            else if (_tree->left != NULL && _tree->right != NULL) //it will always be black
            {
                TNode<value_type>* inorder_successor = getMin(_tree->right);
                
                _myPairAlloc.destroy(_tree->data);
                _myPairAlloc.construct(_tree->data, *(inorder_successor->data));
                _BSTdelete(inorder_successor);
                
            }
        }

        void _BSTdelete(TNode<value_type> *tmp)
        {
           
            /*  Node to be deleted is the leaf */
            if (tmp->right == NULL && tmp->left == NULL)
            {
                        //consider case when tmp->color == black => double black
                TNode<value_type>* parent = tmp->parent;
                if (tmp->color == BLACK)
                    _fixBalanceAfterDeletion(tmp);
                           // std::cout<<"fixDoubleBlack(tmp)"<<std::endl;
                else
                {
                    if (tmp->getSibling())
                        tmp->getSibling()->color = RED;
                }
                if (tmp->isRightChild())
                    parent->right = NULL;
                else
                    parent->left = NULL;
                deleteNode(tmp);
                  
               // tmp = NULL;
               
                return;
            }
                    //fix balance called here

                /** 
                  * Node to be deleted has only one child 
                  * I forgot to update the parent
                  */

            else if (tmp->left== NULL || tmp->right == NULL)
            {
                TNode<value_type>* parent = tmp->parent;
                TNode<value_type>* toReplaceBy;
                bool colorDeleted = tmp->color;
                        
                if (tmp->isRightChild()) // We check if right or left to update the parent
                {
                    if (tmp->left != NULL)
                    {
                        parent->right = tmp->left;
                        tmp->left->parent = parent;
                    }
                    else
                    {
                        parent->right = tmp->right;
                        tmp->right->parent = parent;
                    }
                    toReplaceBy = parent->right;
                }
                else
                {
                    if (tmp->left != NULL)
                    {  
                        parent->left = tmp->left;
                        tmp->left->parent = parent;
                    }
                    else
                    {
                        parent->left = tmp->right;
                        tmp->right->parent = parent;
                    }
                    toReplaceBy = parent->left;
                }
                    deleteNode(tmp);
                  
                    tmp = NULL;
                if ((toReplaceBy == NULL || toReplaceBy->color == BLACK) && colorDeleted == BLACK)
                    _fixBalanceAfterDeletion(toReplaceBy);
                    else
                        toReplaceBy->color = BLACK;
                    return;
                }

                /**  
                 * Node to be deleted has two children 
                 * In this case look for the largest in left subtree
                 * or the min in the right subtree
                 */

                else if (tmp->left!= NULL && tmp->right != NULL)
                {
                    TNode<value_type>* inorder_successor = getMin(tmp->right);
                    _myPairAlloc.destroy(tmp->data);
                    _myPairAlloc.construct(tmp->data, *(inorder_successor->data));
                    _BSTdelete(inorder_successor);
                }
        }

        
    };
}