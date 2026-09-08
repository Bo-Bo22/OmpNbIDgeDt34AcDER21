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
    
    MoveInterval = moveInterval; // Bilanciamento all'IA infallibile
}

// ============================================================================
// STRUTTURE PER LA CODA DELLA BFS (Senza STL)
// ============================================================================
struct QueueNode {
    int x, y;
    QueueNode* next;
};

struct Queue {
    QueueNode* head = NULL;
    QueueNode* tail = NULL;
};

void enqueue(Queue &q, int x, int y) {
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

void dequeue(Queue &q) {
    if (q.head == NULL) return;
    QueueNode* tmp = q.head;
    q.head = q.head->next;
    
    if (q.head == NULL) q.tail = NULL;
    delete tmp;
}

struct connect {
    int x, y;
};

// Visita in ampiezza (Breadth-First Search) su matrice di adiacenza per calcolare
// il percorso minimo verso il player. L'albero dei cammini viene memorizzato
// nell'array bidimensionale 'parent' e risalito a ritroso (backtracking) dal target
// per estrarre la singola direzione ortogonale ottimale del prossimo passo.
Direction ChaserEnemy::calcolaBFS(Player &Pl, Map &Mappa) {
    
    int visited[MAP_RIGHE][MAP_COLONNE] = {};     
    connect parent[MAP_RIGHE][MAP_COLONNE];       
    Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    
    int dy[4] = {-1, 1, 0, 0};
    int dx[4] = {0, 0, -1, 1};
    
    int h = Mappa.getHeight();
    int w = Mappa.getWidth();

    for (int i = 0; i < h && i < 100; i++) {
        for (int j = 0; j < w && j < 150; j++) {
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
// AGGIORNAMENTO DEL CHASER
// ============================================================================
void ChaserEnemy::update(Map &Mappa, Player &pl) {
    if (!isAlive()) {
        erase(Mappa);
        return;
    }
    
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
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
    int h = Mappa.getHeight();
    int w = Mappa.getWidth();
    bool piazzato = false;
    
    while (!piazzato) {
        int ry = rand() % (h - 2) + 1;
        int rx = rand() % (w - 2) + 1;
        
            // Controlla che le coordinate siano al di fuori della safe zone iniziale (3x5)
        if (ry > 3 || rx > 5) {
            if (Mappa.GetPos(ry, rx) == 0) {
                bool occupato = false;
                for (int i = 0; i < n; i++) {
                    if (altri[i] != NULL && altri[i]->getX() == rx && altri[i]->getY() == ry) {
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
}