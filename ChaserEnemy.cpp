#include "ChaserEnemy.hpp"
#include "Map.hpp"
#include "Player.hpp"
#include <cstdlib>

// ============================================================================
// COSTRUTTORE (Il simbolo che mancava al Linker)
// ============================================================================
ChaserEnemy::ChaserEnemy(int startX, int startY, char sym, WINDOW* win, Direction dir, int moveInterval, int cp)
    : Enemy(startX, startY, sym, win, dir, cp) {
    lastMove = std::chrono::steady_clock::now();
    
    MoveInterval = 200; // Bilanciamento all'IA infallibile
}

// ============================================================================
// STRUTTURE PER LA CODA DELLA BFS (Senza STL)
// ============================================================================
struct QueueNode {
    int x, y;
    QueueNode* next;
};

struct Queue {
    QueueNode* head = nullptr;
    QueueNode* tail = nullptr;
};

static void enqueue(Queue &q, int x, int y) {
    QueueNode* tmp = new QueueNode;
    tmp->x = x;
    tmp->y = y;
    tmp->next = nullptr;
    
    if (q.head == nullptr) {
        q.head = tmp;
        q.tail = tmp;
    } else {
        q.tail->next = tmp;
        q.tail = tmp;
    }
}

static void dequeue(Queue &q) {
    if (q.head == nullptr) return;
    QueueNode* tmp = q.head;
    q.head = q.head->next;
    
    if (q.head == nullptr) q.tail = nullptr;
    delete tmp;
}

struct connect {
    int x, y;
};

// ============================================================================
// ALGORITMO DI VISITA IN AMPIEZZA (BFS)
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
    
    while (q.head != nullptr && !found) {
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
    
    while (q.head != nullptr) dequeue(q);
    
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
// AGGIORNAMENTO DEL CHASER
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
    
    Dir = calcolaBFS(pl, Mappa); 
    MoveInCurrDirection(Mappa);   
}

// ============================================================================
// POSIZIONAMENTO CASUALE LONTANO DALLO SPAWN
// ============================================================================
void ChaserEnemy::spawna_casuale(Map &Mappa, Enemy** altri, int n) {
    int h = Map::getHeight();
    int w = Map::getWidth();
    bool piazzato = false;
    
    while (!piazzato) {
        int ry = rand() % (h - 2) + 1;
        int rx = rand() % (w - 2) + 1;
        
        // Evita la safe zone (3x5 in alto a sinistra)
        if (ry <= 3 && rx <= 5) continue;
        
        if (Mappa.GetPos(ry, rx) == 0) {
            bool occupato = false;
            for (int i = 0; i < n; i++) {
                if (altri[i] != nullptr && altri[i]->getX() == rx && altri[i]->getY() == ry) {
                    occupato = true;
                    break;
                }
            }
            if (!occupato) {
                XLoc = rx;
                YLoc = ry;
                piazzato = true;
            }
        }
    }
}