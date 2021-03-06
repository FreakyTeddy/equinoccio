#ifndef E_REDBLACK_TREE
#define E_REDBLACK_TREE


#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <iostream>
#include <deque>

//  CONVENTIONS:  
//                Function names: Each word in a function name begins with 
//                a capital letter.  An example funcntion name is  
//                CreateRedTree(a,b,c). Furthermore, each function name 
//                should begin with a capital letter to easily distinguish 
//                them from variables. 
//                                                                     
//                Variable names: Each word in a variable name begins with 
//                a capital letter EXCEPT the first letter of the variable 
//                name.  For example, int newLongInt.  Global variables have 
//                names beginning with "g".  An example of a global 
//                variable name is gNewtonsConstant. 


#ifndef MAX_INT
#define MAX_INT INT_MAX // some architechturs define INT_MAX not MAX_INT
#endif


template <class N>
class RedBlackTree;


/** 
 * Clase que representa los nodos a insertar en el arbol rojo-negro.
 * @see RedBlackTree
 */
template <class N>
class RedBlackTreeNode {
  friend class RedBlackTree<N>;
public:
  void Print(RedBlackTreeNode<N>*,
	     RedBlackTreeNode<N>*) const;
  RedBlackTreeNode();
  RedBlackTreeNode(N*);
  ~RedBlackTreeNode();
protected:
  N* key;
  int red; /* if red=0 then the node is black */
  RedBlackTreeNode<N> * left;
  RedBlackTreeNode<N> * right;
  RedBlackTreeNode<N> * parent;
};

/** 
 * Implementacion de arbol rojo-negro.  El parametro N especifica la
 * clase a utilizar como clave para el ordenamiento del arbol. Dicha
 * clase debe tener definidos los siguientes metodos: operator>,
 * operator<, unir(const N&), std::string print().  Los primeros dos
 * se utilizan para posicionar la clave en el arbol. La tercera es
 * invocada en caso de que la clave ya estuviese en el arbol. En ese
 * caso, no se inserta, y se intentan unir las claves para que
 * compartan el mismo nodo. El ultimo metodo es opcional, y se utiliza
 * para imprimir por salida estandar un grafo dirigido con el formato
 * 'dot' de GraphViz cuando se invoca al metodo Print del arbol.
 */
template <class N>
class RedBlackTree {
public:
  RedBlackTree();
  ~RedBlackTree();

  /** 
   * Imprime por salida estandar la representacion del arbol como un
   * grafo dirigido con el formato 'dot' de GraphViz
   */
  void Print() const;
  N* DeleteNode(RedBlackTreeNode<N> *);

  /** 
   * Dado una clave de referencia, remueve del arbol la menor clave
   * mayor o igual a la dada.
   * 
   * @param referencia Clave contra la cual se compara.
   *
   * @return La clave encontrada o NULL si ninguna cumple el criterio
   * de busqueda.
   */
  N* RemoverMayorIgual(const N& referencia);
  
  /** 
   * Inserta una clave en el arbol. Si la clave esta repetida, intenta
   * unirlos invocando al metodo unir del objeto clave y no lo inserta.
   * 
   * @param newEntry La clave a insertar.
   *
   * @return El nodo insertado o NULL si la clave ya existia.
   */
  RedBlackTreeNode<N> * Insert(N* newEntry);
  RedBlackTreeNode<N> * GetPredecessorOf(RedBlackTreeNode<N> *) const;
  RedBlackTreeNode<N> * GetSuccessorOf(RedBlackTreeNode<N> *) const;

  /** 
   * Busca una clave en el arbol.
   * 
   * @param referencia La clave a buscar.
   *
   * @return True si encontro la clave.
   */
  bool Search(const N& referencia);
  std::deque<RedBlackTreeNode<N> *> * Enumerate(int low, int high);
  void CheckAssumptions() const;
  
  /** 
   * 
   * 
   * @param estado 
   */
  void LiberarTodo(bool estado){ liberarTodo=estado; }
protected:
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when RedBlackTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  RedBlackTreeNode<N> * root;
  RedBlackTreeNode<N> * nil;
  bool liberarTodo;		/**< Flag para indicar si se deben
				   liberar las claves al destruir el
				   arbol */
  void LeftRotate(RedBlackTreeNode<N> *);
  void RightRotate(RedBlackTreeNode<N> *);
  int TreeInsertHelp(RedBlackTreeNode<N> *);
  void TreePrintHelper(RedBlackTreeNode<N> *) const;
  void FixUpMaxHigh(RedBlackTreeNode<N> *);
  void DeleteFixUp(RedBlackTreeNode<N> *);
};


/******************************************************************************/

// If the symbol CHECK_RB_TREE_ASSUMPTIONS is defined then the
// code does a lot of extra checking to make sure certain assumptions
// are satisfied.  This only needs to be done if you suspect bugs are
// present or if you make significant changes and want to make sure
// your changes didn't mess anything up.
// #define CHECK_RB_TREE_ASSUMPTIONS 1


const int MIN_INT=-MAX_INT;

template <class N>
RedBlackTreeNode<N>::RedBlackTreeNode(){
};

template <class N>
RedBlackTreeNode<N>::RedBlackTreeNode(N *newEntry)
  : key(newEntry) {
};

template <class N>
RedBlackTreeNode<N>::~RedBlackTreeNode(){
};

template <class N>
RedBlackTree<N>::RedBlackTree()
{

  liberarTodo = false; // por defecto no liberamos las claves
  nil = new RedBlackTreeNode<N>;
  nil->left = nil->right = nil->parent = nil;
  nil->red = 0;
  nil->key = NULL;

  root = new RedBlackTreeNode<N>;
  root->parent = root->left = root->right = nil;
  root->red=0;
  root->key = NULL;
}

/***********************************************************************/
/*  FUNCTION:  LeftRotate */
/**/
/*  INPUTS:  the node to rotate on */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input: this, x */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly.  */
/***********************************************************************/
template <class N>
void RedBlackTree<N>::LeftRotate(RedBlackTreeNode<N>* x) {
  RedBlackTreeNode<N>* y;
 
  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls LeftRotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when DeleteFixUP */
  /*  calls LeftRotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  y=x->right;
  x->right=y->left;

  if (y->left != nil) y->left->parent=x; /* used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */
  
  y->parent=x->parent;   

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  if( x == x->parent->left) {
    x->parent->left=y;
  } else {
    x->parent->right=y;
  }
  y->left=x;
  x->parent=y;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
  CheckAssumptions();
#elif defined(DEBUG_ASSERT)
  Assert(!nil->red,"nil not red in RedBlackTree::LeftRotate");
#endif
}

/***********************************************************************/
/*  FUNCTION:  RighttRotate */
/**/
/*  INPUTS:  node to rotate on */
/**/
/*  OUTPUT:  None */
/**/
/*  Modifies Input?: this, y */
/**/
/*  EFFECTS:  Rotates as described in _Introduction_To_Algorithms by */
/*            Cormen, Leiserson, Rivest (Chapter 14).  Basically this */
/*            makes the parent of x be to the left of x, x the parent of */
/*            its parent before the rotation and fixes other pointers */
/*            accordingly.  */
/***********************************************************************/

template <class N>
void RedBlackTree<N>::RightRotate(RedBlackTreeNode<N>* y) {
  RedBlackTreeNode<N>* x;

  /*  I originally wrote this function to use the sentinel for */
  /*  nil to avoid checking for nil.  However this introduces a */
  /*  very subtle bug because sometimes this function modifies */
  /*  the parent pointer of nil.  This can be a problem if a */
  /*  function which calls LeftRotate also uses the nil sentinel */
  /*  and expects the nil sentinel's parent pointer to be unchanged */
  /*  after calling this function.  For example, when DeleteFixUP */
  /*  calls LeftRotate it expects the parent pointer of nil to be */
  /*  unchanged. */

  x=y->left;
  y->left=x->right;

  if (nil != x->right)  x->right->parent=y; /*used to use sentinel here */
  /* and do an unconditional assignment instead of testing for nil */

  /* instead of checking if x->parent is the root as in the book, we */
  /* count on the root sentinel to implicitly take care of this case */
  x->parent=y->parent;
  if( y == y->parent->left) {
    y->parent->left=x;
  } else {
    y->parent->right=x;
  }
  x->right=y;
  y->parent=x;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
  CheckAssumptions();
#elif defined(DEBUG_ASSERT)
  Assert(!nil->red,"nil not red in RedBlackTree::RightRotate");
#endif
}

/***********************************************************************/
/*  FUNCTION:  TreeInsertHelp  */
/**/
/*  INPUTS:  z is the node to insert */
/**/
/*  OUTPUT:  none */
/**/
/*  Modifies Input:  this, z */
/**/
/*  EFFECTS:  Inserts z into the tree as if it were a regular binary tree */
/*            using the algorithm described in _Introduction_To_Algorithms_ */
/*            by Cormen et al.  This funciton is only intended to be called */
/*            by the Insert function and not by the user */
/***********************************************************************/

template <class N>
int RedBlackTree<N>::TreeInsertHelp(RedBlackTreeNode<N>* z) {
  /*  This function should only be called by RedBlackTree::Insert */
  RedBlackTreeNode<N>* x;
  RedBlackTreeNode<N>* y;
    
  z->left=z->right=nil;
  y=root;
  x=root->left;
  while( x != nil) {
    y=x;
    if ( *(x->key) > *(z->key)) { 
      x=x->left;
    } else if (*(x->key) < *(z->key)){ 
      x=x->right;
    }
    else{ /* x->key == z->key */
	 x->key->unir(*(z->key));
	 return 1;
    }
  }
  z->parent=y;
  if ( (y == root) ||
       (*(y->key) > *(z->key)) ) { 
    y->left=z;
  } else {
    y->right=z;
  }

#if defined(DEBUG_ASSERT)
  Assert(!nil->red,"nil not red in RedBlackTree::TreeInsertHelp");
#endif

  return 0;
}

/*  Before calling InsertNode  the node x should have its key set */

/***********************************************************************/
/*  FUNCTION:  InsertNode */
/**/
/*  INPUTS:  newEntry is the entry to insert*/
/**/
/*  OUTPUT:  This function returns a pointer to the newly inserted node */
/*           which is guarunteed to be valid until this node is deleted. */
/*           What this means is if another data structure stores this */
/*           pointer then the tree does not need to be searched when this */
/*           is to be deleted. */
/**/
/*  Modifies Input: tree */
/**/
/*  EFFECTS:  Creates a node node which contains the appropriate key and */
/*            info pointers and inserts it into the tree. */
/***********************************************************************/
template <class N>
RedBlackTreeNode<N> * RedBlackTree<N>::Insert(N* newEntry)
{
  RedBlackTreeNode<N> * y;
  RedBlackTreeNode<N> * x;
  RedBlackTreeNode<N> * newNode;

  x = new RedBlackTreeNode<N>(newEntry);
  if(TreeInsertHelp(x)){
       delete x;
       return NULL;
  }
  newNode = x;
  x->red=1;
  while(x->parent->red) { /* use sentinel instead of checking for root */
    if (x->parent == x->parent->parent->left) {
      y=x->parent->parent->right;
      if (y->red) {
	x->parent->red=0;
	y->red=0;
	x->parent->parent->red=1;
	x=x->parent->parent;
      } else {
	if (x == x->parent->right) {
	  x=x->parent;
	  LeftRotate(x);
	}
	x->parent->red=0;
	x->parent->parent->red=1;
	RightRotate(x->parent->parent);
      } 
    } else { /* case for x->parent == x->parent->parent->right */
             /* this part is just like the section above with */
             /* left and right interchanged */
      y=x->parent->parent->left;
      if (y->red) {
	x->parent->red=0;
	y->red=0;
	x->parent->parent->red=1;
	x=x->parent->parent;
      } else {
	if (x == x->parent->left) {
	  x=x->parent;
	  RightRotate(x);
	}
	x->parent->red=0;
	x->parent->parent->red=1;
	LeftRotate(x->parent->parent);
      } 
    }
  }
  root->left->red=0;
  return(newNode);

#ifdef CHECK_RB_TREE_ASSUMPTIONS
  CheckAssumptions();
#elif defined(DEBUG_ASSERT)
  Assert(!nil->red,"nil not red in RedBlackTree::Insert");
  Assert(!root->red,"root not red in RedBlackTree::Insert");
#endif
}

/***********************************************************************/
/*  FUNCTION:  GetSuccessorOf  */
/**/
/*    INPUTS:  x is the node we want the succesor of */
/**/
/*    OUTPUT:  This function returns the successor of x or NULL if no */
/*             successor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/
template <class N>
RedBlackTreeNode<N> * RedBlackTree<N>::GetSuccessorOf(RedBlackTreeNode<N> * x) const
{ 
  RedBlackTreeNode<N>* y;

  if (nil != (y = x->right)) { /* assignment to y is intentional */
    while(y->left != nil) { /* returns the minium of the right subtree of x */
      y=y->left;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->right) { /* sentinel used instead of checking for nil */
      x=y;
      y=y->parent;
    }
    if (y == root) return(nil);
    return(y);
  }
}

/***********************************************************************/
/*  FUNCTION:  GetPredecessorOf  */
/**/
/*    INPUTS:  x is the node to get predecessor of */
/**/
/*    OUTPUT:  This function returns the predecessor of x or NULL if no */
/*             predecessor exists. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:  uses the algorithm in _Introduction_To_Algorithms_ */
/***********************************************************************/
template <class N>
RedBlackTreeNode<N> * RedBlackTree<N>::GetPredecessorOf(RedBlackTreeNode<N> * x) const {
  RedBlackTreeNode<N>* y;

  if (nil != (y = x->left)) { /* assignment to y is intentional */
    while(y->right != nil) { /* returns the maximum of the left subtree of x */
      y=y->right;
    }
    return(y);
  } else {
    y=x->parent;
    while(x == y->left) { 
      if (y == root) return(nil); 
      x=y;
      y=y->parent;
    }
    return(y);
  }
}

/***********************************************************************/
/*  FUNCTION:  Print */
/**/
/*    INPUTS:  none */
/**/
/*    OUTPUT:  none  */
/**/
/*    EFFECTS:  This function recursively prints the nodes of the tree */
/*              inorder. */
/**/
/*    Modifies Input: none */
/**/
/*    Note:    This function should only be called from ITTreePrint */
/***********************************************************************/
template <class N>
void RedBlackTreeNode<N>::Print(RedBlackTreeNode<N> * nil,
				RedBlackTreeNode<N> * root) const {
  printf("\"%s\"",key->print().c_str());
  printf("[label=\"%s\" ",key->print().c_str());
  if(red == 1)
    printf("fillcolor=red");
  else
    printf("fillcolor=black fontcolor=white");
  printf("]\n");


  if( left != nil) 
    printf("\"%s\" -> \"%s\" \n", key->print().c_str(), left->key->print().c_str());
  else{
    printf("\"l%s\"[label=\"NIL\" fillcolor=black fontcolor=white]\n", key->print().c_str());
    printf("\"%s\" -> \"l%s\"\n", key->print().c_str(),key->print().c_str());
  }

  if( right != nil) 
    printf("\"%s\" -> \"%s\" \n", key->print().c_str(), right->key->print().c_str());
  else{
    printf("\"r%s\"[label=\"NIL\" fillcolor=black fontcolor=white]\n", key->print().c_str());
    printf("\"%s\" -> \"r%s\"\n", key->print().c_str(),key->print().c_str());
  }

  
}

template <class N>
void RedBlackTree<N>::TreePrintHelper( RedBlackTreeNode<N>* x) const {
  
  if (x != nil) {
    TreePrintHelper(x->left);
    x->Print(nil,root);
    TreePrintHelper(x->right);
  }
}

template <class N>
RedBlackTree<N>::~RedBlackTree() {
  RedBlackTreeNode<N> * x = root->left;
  std::deque<RedBlackTreeNode<N> *> stuffToFree;

  if (x != nil) {
    if (x->left != nil) {
      stuffToFree.push_back(x->left);
    }
    if (x->right != nil) {
      stuffToFree.push_back(x->right);
    }
    if(liberarTodo)
	 delete x->key;
    delete x;
    while( !stuffToFree.empty() ) {
      x = stuffToFree.back(); stuffToFree.pop_back();
      if (x->left != nil) {
	stuffToFree.push_back(x->left);
      }
      if (x->right != nil) {
	stuffToFree.push_back(x->right);
      }
      if(liberarTodo)
	   delete x->key;
      delete x;
    }
  }
  delete nil;
  delete root;
}


/***********************************************************************/
/*  FUNCTION:  Print */
/**/
/*    INPUTS:  none */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  This function recursively prints the nodes of the tree */
/*             inorder. */
/**/
/*    Modifies Input: none */
/**/
/***********************************************************************/
template <class N>
void RedBlackTree<N>::Print() const {
  printf("digraph g{\n node[style=filled]\n");
  TreePrintHelper(root->left);
  printf("}");
}

/***********************************************************************/
/*  FUNCTION:  DeleteFixUp */
/**/
/*    INPUTS:  x is the child of the spliced */
/*             out node in DeleteNode. */
/**/
/*    OUTPUT:  none */
/**/
/*    EFFECT:  Performs rotations and changes colors to restore red-black */
/*             properties after a node is deleted */
/**/
/*    Modifies Input: this, x */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/
template <class N>
void RedBlackTree<N>::DeleteFixUp(RedBlackTreeNode<N>* x) {
  RedBlackTreeNode<N> * w;
  RedBlackTreeNode<N> * rootLeft = root->left;

  while( (!x->red) && (rootLeft != x)) {
    if (x == x->parent->left) {
      w=x->parent->right;
      if (w->red) {
	w->red=0;
	x->parent->red=1;
	LeftRotate(x->parent);
	w=x->parent->right;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
	w->red=1;
	x=x->parent;
      } else {
	if (!w->right->red) {
	  w->left->red=0;
	  w->red=1;
	  RightRotate(w);
	  w=x->parent->right;
	}
	w->red=x->parent->red;
	x->parent->red=0;
	w->right->red=0;
	LeftRotate(x->parent);
	x=rootLeft; /* this is to exit while loop */
      }
    } else { /* the code below is has left and right switched from above */
      w=x->parent->left;
      if (w->red) {
	w->red=0;
	x->parent->red=1;
	RightRotate(x->parent);
	w=x->parent->left;
      }
      if ( (!w->right->red) && (!w->left->red) ) { 
	w->red=1;
	x=x->parent;
      } else {
	if (!w->left->red) {
	  w->right->red=0;
	  w->red=1;
	  LeftRotate(w);
	  w=x->parent->left;
	}
	w->red=x->parent->red;
	x->parent->red=0;
	w->left->red=0;
	RightRotate(x->parent);
	x=rootLeft; /* this is to exit while loop */
      }
    }
  }
  x->red=0;

#ifdef CHECK_RB_TREE_ASSUMPTIONS
  CheckAssumptions();
#elif defined(DEBUG_ASSERT)
  Assert(!nil->red,"nil not black in RedBlackTree::DeleteFixUp");
#endif
}


/***********************************************************************/
/*  FUNCTION:  DeleteNode */
/**/
/*    INPUTS:  tree is the tree to delete node z from */
/**/
/*    OUTPUT:  returns the RedBlackEntry stored at deleted node */
/**/
/*    EFFECT:  Deletes z from tree and but don't call destructor */
/**/
/*    Modifies Input:  z */
/**/
/*    The algorithm from this function is from _Introduction_To_Algorithms_ */
/***********************************************************************/
template <class N>
N* RedBlackTree<N>::DeleteNode(RedBlackTreeNode<N> * z){
  RedBlackTreeNode<N>* y;
  RedBlackTreeNode<N>* x;
  N* returnValue = z->key;

  y= ((z->left == nil) || (z->right == nil)) ? z : GetSuccessorOf(z);
  x= (y->left == nil) ? y->right : y->left;
  if (root == (x->parent = y->parent)) { /* assignment of y->p to x->p is intentional */
    root->left=x;
  } else {
    if (y == y->parent->left) {
      y->parent->left=x;
    } else {
      y->parent->right=x;
    }
  }
  if (y != z) { /* y should not be nil in this case */

#ifdef DEBUG_ASSERT
    Assert( (y!=nil),"y is nil in DeleteNode \n");
#endif
    /* y is the node to splice out and x is its child */
  
    y->left=z->left;
    y->right=z->right;
    y->parent=z->parent;
    z->left->parent=z->right->parent=y;
    if (z == z->parent->left) {
      z->parent->left=y; 
    } else {
      z->parent->right=y;
    }
    if (!(y->red)) {
      y->red = z->red;
      DeleteFixUp(x);
    } else
      y->red = z->red; 
    delete z;
#ifdef CHECK_RB_TREE_ASSUMPTIONS
    CheckAssumptions();
#elif defined(DEBUG_ASSERT)
    Assert(!nil->red,"nil not black in RedBlackTree::Delete");
#endif
  } else {
    if (!(y->red)) DeleteFixUp(x);
    delete y;
#ifdef CHECK_RB_TREE_ASSUMPTIONS
    CheckAssumptions();
#elif defined(DEBUG_ASSERT)
    Assert(!nil->red,"nil not black in RedBlackTree::Delete");
#endif
  }
  return returnValue;
}


/***********************************************************************/
/*  FUNCTION:  Enumerate */
/**/
/*    INPUTS:  tree is the tree to look for keys between [low,high] */
/**/
/*    OUTPUT:  stack containing pointers to the nodes between [low,high] */
/**/
/*    Modifies Input: none */
/**/
/*    EFFECT:  Returns a stack containing pointers to nodes containing */
/*             keys which in [low,high]/ */
/**/
/***********************************************************************/

template <class N>
std::deque<RedBlackTreeNode<N> *> * RedBlackTree<N>::Enumerate(int low, 
							       int high)  {
  std::deque<RedBlackTreeNode<N> *> * enumResultStack = 
    new std::deque<RedBlackTreeNode<N> *>(4);

  RedBlackTreeNode<N>* x=root->left;
  RedBlackTreeNode<N>* lastBest=NULL;

  while(nil != x) {
       if ( *(x->key) > high ) {
      x=x->left;
    } else {
      lastBest=x;
      x=x->right;
    }
  }
  while ( (lastBest) && (low <= lastBest->key) ) {
    enumResultStack->push_back(lastBest);
    lastBest=GetPredecessorOf(lastBest);
  }
  return(enumResultStack);
}

template <class N>
void RedBlackTree<N>::CheckAssumptions() const {
 // VERIFY(nil->key == MIN_INT);
 // VERIFY(root->key == MAX_INT);
 // VERIFY(nil->storedEntry == NULL);
 // VERIFY(root->storedEntry == NULL);
 // VERIFY(nil->red == 0);
 // VERIFY(root->red == 0);
}


template <class N>
N* RedBlackTree<N>::RemoverMayorIgual(const N& referencia){
     RedBlackTreeNode<N> *x=root;
     RedBlackTreeNode<N> *y=root->left;
     RedBlackTreeNode<N> *ultimo_izquierda=NULL;
     
     while(y != nil){
	  x=y;
	  if(*(y->key) < referencia){
	       y=y->right;
	  }
	  else if(*(y->key) > referencia){
	       ultimo_izquierda = y;
	       y=y->left;
	  }
	  else{
	       ultimo_izquierda=y;
	       break;
	  }
     }
     
     if(ultimo_izquierda == NULL)
	  return NULL;
     return DeleteNode(ultimo_izquierda);
}

template <class N>
bool RedBlackTree<N>::Search(const N& buscado){
     bool encontrado=false;
     RedBlackTreeNode<N> *x=root->left;

     while(x != nil && !encontrado){
	  if(*(x->key) > buscado)
	       x=x->left;
	  else if(*(x->key) < buscado)
	       x=x->right;
	  else
	       encontrado=true;
     }

     return encontrado;
}

#endif
