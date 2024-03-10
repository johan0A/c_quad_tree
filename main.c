#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include<stdbool.h>

struct Node {
    void *children[4]; // 0: bottom left 1: top left 2: bottom right 3: top right
    struct Node *parent;
};

struct QuadTree {
    struct Node *root;
    int global_offset[2];
    int local_max_pos[2];
    int height;
};

static struct Node* createNode() {
    struct Node* node = malloc(sizeof(struct Node));
    for (int i = 0; i < 4; i++) {
        node->children[i] = NULL;
    }
    node->parent = NULL;
    return node;
}

struct QuadTree* createQuadTree() {
    struct QuadTree *tree = (struct QuadTree *)malloc(sizeof(struct QuadTree));
    tree->root = createNode();
    tree->global_offset[0] = -1;
    tree->global_offset[1] = -1;
    tree->local_max_pos[0] = -1;
    tree->local_max_pos[1] = -1;
    tree->height = -1;
    return tree;
}


static void expand_quadtree(struct QuadTree* quadTree, int quadrant) {
    struct Node* temp_root = quadTree->root;
    quadTree->root = createNode();
    quadTree->root->children[quadrant] = temp_root;
    temp_root->parent = quadTree->root;
    quadTree->height++;
    quadTree->global_offset[0] -= (quadrant % 2) * (1 << (quadTree->height-1));
    quadTree->global_offset[1] -= (quadrant / 2) * (1 << (quadTree->height-1));
    quadTree->local_max_pos[0] = 1 << (quadTree->height-1);
    quadTree->local_max_pos[1] = 1 << (quadTree->height-1);
}



void insert_in_quadtree(struct QuadTree* quadTree, void* leaf, int positionX, int positionY) {
    int local_positionX;
    int local_positionY;
    // expand the quadtree if necessary
    while (true) {
        local_positionX = positionX - quadTree->global_offset[0];
        local_positionY = positionY - quadTree->global_offset[1];
        int quadrantX = !(local_positionX > quadTree->local_max_pos[0]) * 2 - (local_positionX < 0);
        int quadrantY = !(local_positionY < 0) * 2 - (local_positionY > quadTree->local_max_pos[1]);

        if (quadrantX == 2 && quadrantY == 2) {
            break;
        }

        if (quadTree->height == -1){
            quadTree->height = 1;
            quadTree->global_offset[0] = positionX;
            quadTree->global_offset[1] = positionY;
            quadTree->local_max_pos[0] = 1;
            quadTree->local_max_pos[1] = 1;
            quadTree->root->children[0] = leaf;
            return;
        }

        expand_quadtree(quadTree, (quadrantX - 1) + (quadrantY - 1) * 2);
    }

    // insert the leaf
    struct Node* currentNode = quadTree->root;
    int height = quadTree->height - 1;
    int quadrant;
    for (int i = 0; i < height; i++) {
        quadrant = (local_positionX > (1 << (height - i - 1))) + (local_positionY > (1 << (height - i - 1))) * 2;
        if (currentNode->children[quadrant] == NULL) {
            currentNode->children[quadrant] = createNode();
            ((struct Node*)currentNode->children[quadrant])->parent = currentNode;
        }
        currentNode = (struct Node*)currentNode->children[quadrant];
    }
    quadrant = (local_positionX > 0) + (local_positionY > 0) * 2;
    currentNode->children[quadrant] = leaf;
}

static void _show_quadtree_as_tree(struct QuadTree* quadTree, struct Node* currentNode, int height, void printLeaf(void* leaf)) {
    if (currentNode == NULL) {
        return;
    }
    for (int i = 0; i < 4; i++) {
        if (currentNode->children[i] != NULL && height > 1) {
            for(int j = 0; j < quadTree->height - height + 1; j++) {
                printf("  ");
            }
            printf("%d: Node\n", i);
            _show_quadtree_as_tree(quadTree, (struct Node*)currentNode->children[i], height-1, printLeaf);
        } else if (currentNode->children[i] != NULL && height == 1) {
            for(int j = 0; j < quadTree->height - height + 1; j++) {
                printf("  ");
            }
            printf("%d: ", i);
            printLeaf(currentNode->children[i]);
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
    show_quadtree_as_tree(quadTree, printLeaf);
    leaf = createLeaf(8, 8, 1);
    insert_in_quadtree(quadTree, leaf, leaf->position[0], leaf->position[1]);
    leaf = createLeaf(7, 7, 2);
    insert_in_quadtree(quadTree, leaf, leaf->position[0], leaf->position[1]);
    
    // show_quadtree_as_tree(quadTree, printLeaf);
    // leaf = createLeaf(8, 8, 2);
    // insert_in_quadtree(quadTree, leaf, leaf->position[0], leaf->position[1]);
    show_quadtree_as_tree(quadTree, printLeaf);
    // leaf = createLeaf(2, 2, 2);
    // insert_in_quadtree(quadTree, leaf, 2, 2);
    // show_quadtree_as_tree(quadTree, printLeaf);
    // leaf = createLeaf(1, 50, 50);
    // insert_in_quadtree(quadTree, leaf, 50, 50);
    // show_quadtree_as_tree(quadTree, printLeaf);
    return 0;
}