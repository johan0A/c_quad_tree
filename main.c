#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include<stdbool.h>

struct Node {
    void *childrens[4];
    struct Node *parent;
};

struct QuadTree {
    struct Node *root;
    int posMax[2];
    int posMin[2];
    int height;
};

static struct Node* createNode() {
    struct Node* node = malloc(sizeof(struct Node));
    for (int i = 0; i < 4; i++) {
        node->childrens[i] = NULL;
    }
    node->parent = NULL;
    return node;
}

struct QuadTree* createQuadTree() {
    struct QuadTree *tree = (struct QuadTree *)malloc(sizeof(struct QuadTree));
    tree->root = createNode();
    tree->posMax[0] = -1;
    tree->posMax[1] = -1;
    tree->posMin[0] = 1;
    tree->posMin[1] = 1;
    tree->height = -1;
    return tree;
}

static void expand_quadtree(struct QuadTree* quadTree, int quadrant) {
    struct Node* temp_root = quadTree->root;
    quadTree->root = createNode();
    quadTree->root->childrens[quadrant] = temp_root;
    temp_root->parent = quadTree->root;
    quadTree->height++;
    return;
}

void insert_in_quadtree(struct QuadTree* quadTree, void* leaf, int positionX, int positionY) {
    while (true) {
        int overMaxX = (quadTree->posMax[0] < positionX) + (quadTree->posMin[0] > positionX * 2);
        int overMaxY = (quadTree->posMax[1] < positionY) + (quadTree->posMin[1] > positionY * 2);
        if (overMaxX || overMaxY) {
            if (quadTree->height == -1){
                quadTree->posMax[0] = positionX + 1;
                quadTree->posMax[1] = positionY + 1;
                quadTree->posMin[0] = positionX;
                quadTree->posMin[1] = positionY;
                quadTree->height = 1;
                quadTree->root->childrens[0] = leaf;
                return;
            }
            expand_quadtree(quadTree, (overMaxX-1) * 2 + (overMaxY-1));
            break;
        } else {
            break;
        }
    }

    struct Node* currentNode = quadTree->root;
    int currentX = quadTree->posMin[0];
    int currentY = quadTree->posMin[1];
    int height = quadTree->height - 1;
    while (height > 0) {
        height--;
        int quadrant = 0;
        if (positionX > currentX + (1 << (height-1))) {
            quadrant += 1;
            currentX += (1 << (height-1));
        }
        if (positionY > currentY + (1 << (height-1))) {
            quadrant += 2;
            currentY += (1 << (height-1));
        }
        if (currentNode->childrens[quadrant] == NULL) {
            currentNode->childrens[quadrant] = createNode();
            ((struct Node*)currentNode->childrens[quadrant])->parent = currentNode;
        }
        currentNode = (struct Node*)currentNode->childrens[quadrant];
    }
    int quadrant = (positionX > currentX) + (positionY > currentY) * 2;
    ((struct Node*)currentNode)->childrens[quadrant] = leaf;
}

static void _show_quadtree_as_tree(struct QuadTree* quadTree, struct Node* currentNode, int height, void printLeaf(void* leaf)) {
    if (currentNode == NULL) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        if (currentNode->childrens[i] != NULL && height > 1) {
            for(int j = 0; j < quadTree->height - height + 1; j++) {
                printf("  ");
            }
            printf("%d: Node\n", i);
            _show_quadtree_as_tree(quadTree, (struct Node*)currentNode->childrens[i], height-1, printLeaf);
        } else if (currentNode->childrens[i] != NULL && height == 1) {
            for(int j = 0; j < quadTree->height - height + 1; j++) {
                printf("  ");
            }
            printf("%d: ", i);
            printLeaf(currentNode->childrens[i]);
            printf("\n");
        } else {
            // for(int j = 0; j < quadTree->height - height + 1; j++) {
            //     printf("  ");
            // }
            // printf("%d: Node\n", i);
        }
    }
    return;
}

void show_quadtree_as_tree(struct QuadTree* quadTree, void printLeaf(void* leaf)) {
    printf("Root\n");
    _show_quadtree_as_tree(quadTree, quadTree->root, quadTree->height, printLeaf);
    return;
}

struct Leaf {
    int position[2];
    int ID;
};

void printLeaf(void* leaf) {
    struct Leaf* leaf_ = (struct Leaf*)leaf;
    printf("ID: %d pos: (%d, %d)", leaf_->ID, leaf_->position[0], leaf_->position[1]);
    return;
}

struct Leaf* createLeaf(int positionX, int positionY, int ID) {
    struct Leaf* leaf = malloc(sizeof(struct Leaf));
    leaf->position[0] = positionX;
    leaf->position[1] = positionY;
    leaf->ID = ID;
    return leaf;
}

int main(int argc, char const *argv[]) {
    struct QuadTree* quadTree = createQuadTree();
    struct Leaf* leaf = createLeaf(0, 0, 0);
    insert_in_quadtree(quadTree, leaf, 0, 0);
    // show_quadtree_as_tree(quadTree, printLeaf);
    leaf = createLeaf(50, 50, 1);
    insert_in_quadtree(quadTree, leaf, 50, 50);
    show_quadtree_as_tree(quadTree, printLeaf);
    // leaf = createLeaf(2, 2, 2);
    // insert_in_quadtree(quadTree, leaf, 2, 2);
    // show_quadtree_as_tree(quadTree, printLeaf);
    // leaf = createLeaf(1, 50, 50);
    // insert_in_quadtree(quadTree, leaf, 50, 50);
    // show_quadtree_as_tree(quadTree, printLeaf);
    return 0;
}