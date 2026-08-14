#include "BFS.hpp"
// #include "ChaserEnemy.hpp" // Decommenta se la classe ChaserEnemy ha un suo file .hpp
#include <cstdlib>
#include <chrono>

// ============================================================================
// STRUTTURA E FUNZIONI PER LA GESTIONE MANUALE DELLA CODA OTTIMIZZATA
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

// ============================================================================
// ALGORITMO DI VISITA IN AMPIEZZA (BFS) PER IL CHASER ENEMY
// ============================================================================
Direction BFS::getDirection(Player &Pl, Enemy &En, Map &Mappa) {
    
    const int MAX_Y = 100;
    const int MAX_X = 150;
    
    int visited[MAX_Y][MAX_X] = {};     
    connect parent[MAX_Y][MAX_X];       
    Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    
    int dy[4] = {-1, 1, 0, 0};
    int dx[4] = {0, 0, -1, 1};
    
    int h = Map::getHeight();
    int w = Map::getWidth();

    // Inizializzazione sicura
    for (int i = 0; i < h && i < MAX_Y; i++) {
        for (int j = 0; j < w && j < MAX_X; j++) {
            parent[i][j].x = -1;
            parent[i][j].y = -1;
            visited[i][j] = 0;
        }
    }
    
    Queue q; 
    visited[En.getY()][En.getX()] = 1;        
    enqueue(q, En.getX(), En.getY()); 
    
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
    while(parent[cy][cx].x != En.getX() || parent[cy][cx].y != En.getY()) {
        int nx = parent[cy][cx].x;
        int ny = parent[cy][cx].y;
        if (nx < 0 || ny < 0) return dirs[rand() % 4]; 
        cx = nx;
        cy = ny;
    }
    
    for(int i = 0; i < 4; i++) {
        if (cx == En.getX() + dx[i] && cy == En.getY() + dy[i])
            return dirs[i];
    }
    
    return dirs[rand() % 4];
}

// ============================================================================
// AGGIORNAMENTO LOGICO DEI METODI SOTTO-CLASSIFICATI (POLIMORFISMO)
// ============================================================================

void ChaserEnemy::update(Map &Mappa, Player &Pl) {
    if (!isAlive()) {
        erase(Mappa);
        return;
    }
    
    auto now = std::chrono::steady_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastMove).count();
    
    if (elapsed < MoveInterval) return; 
    lastMove = now;

    int deltaX = Pl.getX() - XLoc;
    int deltaY = Pl.getY() - YLoc;

    if (abs(deltaX) + abs(deltaY) == 1) {
        if (deltaX == 1)       Dir = Direction::RIGHT;
        else if (deltaX == -1) Dir = Direction::LEFT;
        else if (deltaY == 1)  Dir = Direction::DOWN;
        else if (deltaY == -1) Dir = Direction::UP;
    } else {
    
    // 1. DICHIARAZIONE: Questa riga deve esserci ed essere fuori da eventuali graffe!
    BFS calcolatorePercorso;
    
    // 2. UTILIZZO: Ora il compilatore sa cos'è "calcolatorePercorso"
    Dir = calcolatorePercorso.getDirection(Pl, *this, Mappa); 
    }   
    MoveInCurrDirection(Mappa);   

}

void ChaserEnemy::spawna_casuale(Map &Mappa, Enemy** altri, int n) {
    int h = Map::getHeight();
    int w = Map::getWidth();
    bool piazzato = false;
    
    for (int x = w - 2; x >= 1 && !piazzato; x--) {
        if (Mappa.GetPos(h - 2, x) == 0) {
            bool cellaLibera = true;
            for(int k = 0; k < n; k++) {
                if(altri[k] != nullptr && altri[k]->getX() == x && altri[k]->getY() == h - 2) {
                    cellaLibera = false;
                    break;
                }
            }
            if(cellaLibera) { 
                YLoc = h - 2; 
                XLoc = x; 
                piazzato = true; 
            }
        }
    }
    
    for (int y = h - 2; y >= 1 && !piazzato; y--) {
        if (Mappa.GetPos(y, w - 2) == 0) {
            bool cellaLibera = true;
            for(int k = 0; k < n; k++) {
                if(altri[k] != nullptr && altri[k]->getY() == y && altri[k]->getX() == w - 2) {
                    cellaLibera = false;
                    break;
                }
            }
            if(cellaLibera) { 
                YLoc = y; 
                XLoc = w - 2; 
                piazzato = true; 
            }
        }
    }
    
    while (!piazzato) {
        int rndY = rand() % (h - 2) + 1;
        int rndX = rand() % (w - 2) + 1;
        
        if (Mappa.GetPos(rndY, rndX) == 0 && (rndY > 5 || rndX > 5)) {
            YLoc = rndY;
            XLoc = rndX;
            piazzato = true;
        }
    }
    
    Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    int dy[4] = {-1, 1, 0, 0};
    int dx[4] = {0, 0, -1, 1};
    for (int i = 0; i < 4; i++) {
        if (Mappa.GetPos(YLoc + dy[i], XLoc + dx[i]) == 0) {
            Dir = dirs[i];
            return;
        }
    }
}