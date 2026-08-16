#include "ChaserEnemy.hpp"

struct QueueNode {
    int x, y;
    QueueNode* next;
};

struct Queue {
    QueueNode* head = NULL;
    QueueNode* tail = NULL;
};

static void enqueue(Queue &q, int x, int y) {
    QueueNode* tmp = new QueueNode;
    tmp->x = x;
    tmp->y = y;
    tmp->next = NULL;
    
    if (q.head == NULL) {
        q.head = tmp;
        q.tail = tmp;
    } else {
        q.tail->next = tmp;
        q.tail = tmp;
    }
}

static void dequeue(Queue &q) {
    if (q.head == NULL) return;
    QueueNode* tmp = q.head;
    q.head = q.head->next;
    
    if (q.head == NULL) q.tail = NULL;
    delete tmp;
}

struct connect {
    int x, y;
};

// ============================================================================
// ALGORITMO BFS INTEGRATO IN CHASER ENEMY
// ============================================================================
Direction ChaserEnemy::calcolaBFS(Player &Pl, Map &Mappa) {
    const int MAX_Y = 100;
    const int MAX_X = 150;
    
    int visited[MAX_Y][MAX_X] = {};     
    connect parent[MAX_Y][MAX_X];       
    Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    
    int dy[4] = {-1, 1, 0, 0};
    int dx[4] = {0, 0, -1, 1};
    
    int h = Map::getHeight();
    int w = Map::getWidth();

    for (int i = 0; i < h && i < MAX_Y; i++) {
        for (int j = 0; j < w && j < MAX_X; j++) {
            parent[i][j].x = -1;
            parent[i][j].y = -1;
            visited[i][j] = 0;
        }
    }
    
    Queue q; 
    visited[YLoc][XLoc] = 1;        
    enqueue(q, XLoc, YLoc); 
    
    bool found = false;
    
    while (q.head != NULL && !found) {
        int cx = q.head->x;
        int cy = q.head->y;
        dequeue(q);    
        
        if (cx == Pl.getX() && cy == Pl.getY()) {
            found = true;
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            
            if (ny >= 0 && ny < h && nx >= 0 && nx < w && !visited[ny][nx]) {
                bool isPlayer = (nx == Pl.getX() && ny == Pl.getY());
                if (Mappa.GetPos(ny, nx) == 0 || isPlayer) {
                    visited[ny][nx] = 1;
                    parent[ny][nx].x = cx; 
                    parent[ny][nx].y = cy;
                    enqueue(q, nx, ny); 
                }
            }
        }
    }
    
    while (q.head != NULL) dequeue(q);
    
    if (!found) return dirs[rand() % 4];
    
    int cx = Pl.getX(), cy = Pl.getY();
    while (parent[cy][cx].x != XLoc || parent[cy][cx].y != YLoc) {
        int nx = parent[cy][cx].x;
        int ny = parent[cy][cx].y;
        if (nx < 0 || ny < 0) return dirs[rand() % 4]; 
        cx = nx;
        cy = ny;
    }
    
    for (int i = 0; i < 4; i++) {
        if (cx == XLoc + dx[i] && cy == YLoc + dy[i])
            return dirs[i];
    }
    
    return dirs[rand() % 4];
}

// ============================================================================
// AGGIORNAMENTO DEL NEMICO
// ============================================================================
void ChaserEnemy::update(Map &Mappa, Player &pl) {
    if (!isAlive()) {
        erase(Mappa);
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();
    
    if (elapsed < MoveInterval) return; 
    lastMove = now;
    
    // Chiamata diretta al metodo interno di classe
    Dir = calcolaBFS(pl, Mappa); 
    
    MoveInCurrDirection(Mappa);   
}