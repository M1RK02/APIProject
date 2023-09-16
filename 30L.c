#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CARS 512

struct Station
{
    int distance;
    int num_vehicles;
    int *autonomies;
    char color;
    struct Station *parent, *left, *right;
};

struct Road
{
    struct Station *root, *nil;
};

struct Station *road_minimum(struct Road *road, struct Station *x)
{
    while (x->left != road->nil)
    {
        x = x->left;
    }
    return x;
}

struct Station *road_successor(struct Road *road, struct Station *x)
{
    if (x->right != road->nil)
    {
        return road_minimum(road, x->right);
    }
    struct Station *y = x->parent;
    while (y != road->nil && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

struct Station *road_maximum(struct Road *road, struct Station *x)
{
    while (x->right != road->nil)
    {
        x = x->right;
    }
    return x;
}

struct Station *road_predecessor(struct Road *road, struct Station *x)
{
    if (x->left != road->nil)
    {
        return road_maximum(road, x->left);
    }
    struct Station *y = x->parent;
    while (y != road->nil && x == y->left)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

void road_left_rotate(struct Road *road, struct Station *x)
{
    struct Station *y = x->right;
    x->right = y->left;
    if (y->left != road->nil)
    {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == road->nil)
    {
        road->root = y;
    }
    else if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void road_right_rotate(struct Road *road, struct Station *x)
{
    struct Station *y = x->left;
    x->left = y->right;
    if (y->right != road->nil)
    {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == road->nil)
    {
        road->root = y;
    }
    else if (x == x->parent->right)
    {
        x->parent->right = y;
    }
    else
    {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

void road_insert_fixup(struct Road *road, struct Station *z)
{
    while (z->parent->color == 1)
    {
        if (z->parent == z->parent->parent->left)
        {
            struct Station *y = z->parent->parent->right;
            if (y->color == 1)
            {
                z->parent->color = 0;
                y->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    road_left_rotate(road, z);
                }
                z->parent->color = 0;
                z->parent->parent->color = 1;
                road_right_rotate(road, z->parent->parent);
            }
        }
        else
        {
            struct Station *y = z->parent->parent->left;
            if (y->color == 1)
            {
                z->parent->color = 0;
                y->color = 0;
                z->parent->parent->color = 1;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    road_right_rotate(road, z);
                }
                z->parent->color = 0;
                z->parent->parent->color = 1;
                road_left_rotate(road, z->parent->parent);
            }
        }
    }
    road->root->color = 0;
}

int road_insert(struct Road *road, struct Station *z)
{
    struct Station *y = road->nil;
    struct Station *x = road->root;
    while (x != road->nil)
    {
        y = x;
        if (z->distance == x->distance)
        {
            return 0;
        }
        if (z->distance < x->distance)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    z->parent = y;
    if (y == road->nil)
    {
        road->root = z;
    }
    else if (z->distance < y->distance)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }
    z->left = road->nil;
    z->right = road->nil;
    z->color = 1;
    road_insert_fixup(road, z);
    return 1;
}

void road_delete_fixup(struct Road *road, struct Station *x)
{
    while (x != road->root && x->color == 0)
    {
        if (x == x->parent->left)
        {
            struct Station *w = x->parent->right;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                road_left_rotate(road, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == 0 && w->right->color == 0)
            {
                w->color = 1;
                x = x->parent;
            }
            else
            {
                if (w->right->color == 0)
                {
                    w->left->color = 0;
                    w->color = 1;
                    road_right_rotate(road, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->right->color = 0;
                road_left_rotate(road, x->parent);
                x = road->root;
            }
        }
        else
        {
            struct Station *w = x->parent->left;
            if (w->color == 1)
            {
                w->color = 0;
                x->parent->color = 1;
                road_right_rotate(road, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == 0 && w->left->color == 0)
            {
                w->color = 1;
                x = x->parent;
            }
            else
            {
                if (w->left->color == 0)
                {
                    w->right->color = 0;
                    w->color = 1;
                    road_left_rotate(road, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = 0;
                w->left->color = 0;
                road_right_rotate(road, x->parent);
                x = road->root;
            }
        }
    }
    x->color = 0;
}

void road_delete(struct Road *road, struct Station *z)
{
    struct Station *x;
    struct Station *y;
    if (z->left == road->nil || z->right == road->nil)
    {
        y = z;
    }
    else
    {
        y = road_successor(road, z);
    }
    if (y->left != road->nil)
    {
        x = y->left;
    }
    else
    {
        x = y->right;
    }
    x->parent = y->parent;
    if (y->parent == road->nil)
    {
        road->root = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }
    if (y != z)
    {
        z->distance = y->distance;
        z->num_vehicles = y->num_vehicles;
        z->autonomies = y->autonomies;
    }
    if (y->color == 0)
    {
        road_delete_fixup(road, x);
    }
    free(y);
}

struct Station *road_search(struct Road *road, int distance)
{
    struct Station *x = road->root;
    while (x != road->nil && distance != x->distance)
    {
        if (distance < x->distance)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }
    return x;
}

int find_max_autonomy(struct Station *x)
{
    int max = 0;
    for (int i = 0; i < x->num_vehicles; i++)
    {
        if (x->autonomies[i] > max)
        {
            max = x->autonomies[i];
        }
    }
    return max;
}

void aggiungi_stazione(struct Road *road, int distance, int num_vehicles)
{
    struct Station *new_station = (struct Station *)malloc(sizeof(struct Station));
    new_station->distance = distance;
    new_station->num_vehicles = num_vehicles;
    new_station->autonomies = (int *)calloc(MAX_CARS, sizeof(int));
    new_station->parent = road->nil;
    new_station->left = road->nil;
    new_station->right = road->nil;
    for (int i = 0; i < num_vehicles; i++)
    {
        int scan = scanf("%d", &new_station->autonomies[i]);
        if (!scan)
            return;
    }
    if (road_insert(road, new_station))
    {
        printf("aggiunta\n");
    }
    else
    {
        printf("non aggiunta\n");
        free(new_station->autonomies);
        free(new_station);
    }
}

void demolisci_stazione(struct Road *road, int distance)
{
    struct Station *to_delete = road_search(road, distance);
    if (to_delete == road->nil)
    {
        printf("non demolita\n");
    }
    else
    {
        road_delete(road, to_delete);
        printf("demolita\n");
    }
}

void aggiungi_auto(struct Road *road, int distance, int autonomy)
{
    struct Station *x = road_search(road, distance);
    if (x == road->nil)
    {
        printf("non aggiunta\n");
    }
    else
    {
        if (x->num_vehicles == MAX_CARS)
        {
            printf("non aggiunta\n");
        }
        else
        {
            x->autonomies[x->num_vehicles] = autonomy;
            x->num_vehicles++;
            printf("aggiunta\n");
        }
    }
}

void rottama_auto(struct Road *road, int distance, int autonomy)
{
    struct Station *x = road_search(road, distance);
    if (x == road->nil)
    {
        printf("non rottamata\n");
    }
    else
    {
        for (int i = 0; i < x->num_vehicles; i++)
        {
            if (x->autonomies[i] == autonomy)
            {
                x->autonomies[i] = x->autonomies[x->num_vehicles - 1];
                x->num_vehicles--;
                printf("rottamata\n");
                return;
            }
        }
        printf("non rottamata\n");
    }
}

void pianifica_percorso_dritto(struct Road *road, int start, int end)
{
    int dim = 0;
    struct Station *x = road_search(road, start);
    struct Station *y = x;
    while (y != road->nil && y->distance <= end)
    {
        dim++;
        y = road_successor(road, y);
    }

    int *distance = (int *)malloc(dim * sizeof(int));
    int *max = (int *)malloc(dim * sizeof(int));
    int *queue = (int *)malloc(dim * sizeof(int));
    int *visited = (int *)malloc(dim * sizeof(int));
    int *parents = (int *)malloc(dim * sizeof(int));
    for (int i = 0; i < dim; i++)
    {
        distance[i] = x->distance;
        max[i] = find_max_autonomy(x);
        visited[i] = 0;
        parents[i] = -1;
        x = road_successor(road, x);
        if (distance[i] + max[i] < x->distance)
        {
            visited[i] = 1;
        }
    }

    int head = 0;
    int tail = 0;
    queue[tail] = 0;
    tail++;
    visited[0] = 1;
    while (head != tail)
    {
        int u = queue[head];
        head++;
        for (int i = u + 1; i < dim; i++)
        {
            if (visited[i] == 0)
            {
                if (distance[u] + max[u] >= distance[i])
                {
                    queue[tail] = i;
                    tail++;
                    visited[i] = 1;
                    parents[i] = u;
                }
                else
                {
                    break;
                }
            }
        }
    }

    if (visited[dim - 1] == 0)
    {
        printf("nessun percorso\n");
    }
    else
    {
        int *path = (int *)malloc(dim * sizeof(int));
        int i = dim - 1;
        int j = 0;
        while (i != -1)
        {
            path[j] = distance[i];
            i = parents[i];
            j++;
        }
        for (int k = j - 1; k > 0; k--)
        {
            printf("%d ", path[k]);
        }
        printf("%d\n", path[0]);
        free(path);
    }

    free(distance);
    free(max);
    free(queue);
    free(visited);
    free(parents);
}

void pianifica_percorso_rovescio(struct Road *road, int start, int end)
{
    int dim = 0;
    struct Station *x = road_search(road, start);
    struct Station *y = x;
    while (y != road->nil && y->distance >= end)
    {
        dim++;
        y = road_predecessor(road, y);
    }

    int *distance = (int *)malloc(dim * sizeof(int));
    int *queue = (int *)malloc(dim * sizeof(int));
    int *visited = (int *)malloc(dim * sizeof(int));
    int *parents = (int *)malloc(dim * sizeof(int));
    int *max = (int *)malloc(dim * sizeof(int));
    for (int i = 0; i < dim; i++)
    {
        distance[dim - 1 - i] = x->distance;
        max[dim - 1 - i] = find_max_autonomy(x);
        visited[i] = 0;
        parents[i] = -1;
        x = road_predecessor(road, x);
        if (distance[dim - 1 - i] - max[dim - 1 - i] > x->distance)
        {
            visited[i] = 1;
        }
    }

    int head = 0;
    int tail = 0;
    queue[tail] = 0;
    tail++;
    visited[0] = 1;
    while (head != tail)
    {
        int u = queue[head];
        head++;
        for (int i = u + 1; i < dim; i++)
        {
            if (visited[i] == 0)
            {
                if (distance[i] - max[i] <= distance[u])
                {
                    queue[tail] = i;
                    tail++;
                    visited[i] = 1;
                    parents[i] = u;
                }
            }
        }
    }

    if (visited[dim - 1] == 0)
    {
        printf("nessun percorso\n");
    }
    else
    {
        int i = dim - 1;
        while (parents[i] != -1)
        {
            printf("%d ", distance[i]);
            i = parents[i];
        }
        printf("%d\n", distance[i]);
    }

    free(max);
    free(queue);
    free(visited);
    free(parents);
    free(distance);
}

void pianifica_percorso(struct Road *road, int start, int end)
{
    if (start == end)
    {
        printf("%d\n", start);
        return;
    }
    if (start < end)
        pianifica_percorso_dritto(road, start, end);
    else
        pianifica_percorso_rovescio(road, start, end);
}

int main()
{
    int distance, num_vehicles, autonomy, start, end;
    int scan;
    struct Road road;
    road.nil = (struct Station *)malloc(sizeof(struct Station));
    road.nil->color = 0;
    road.root = road.nil;

    char command[20];
    while (scanf("%s", command) != EOF)
    {
        int selection = strlen(command) - 12;
        if (command[0] == 'p')
            selection++;

        switch (selection)
        {
        case 0:
            scan = scanf("%d %d", &distance, &autonomy);
            rottama_auto(&road, distance, autonomy);
            break;
        case 1:
            scan = scanf("%d %d", &distance, &autonomy);
            aggiungi_auto(&road, distance, autonomy);
            break;
        case 5:
            scan = scanf("%d %d", &distance, &num_vehicles);
            aggiungi_stazione(&road, distance, num_vehicles);
            break;
        case 6:
            scan = scanf("%d", &distance);
            demolisci_stazione(&road, distance);
            break;
        case 7:
            scan = scanf("%d %d", &start, &end);
            pianifica_percorso(&road, start, end);
            break;
        }

        if (!scan)
            return -1;
    }

    return 0;
}