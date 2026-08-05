/*
// ============================================================================
// STRUTTURA E FUNZIONI PER LA GESTIONE MANUALE DELLA CODA (QUEUE) PER BFS
// ============================================================================
struct QueueNode {
    int x, y;
    QueueNode* next;
};
typedef QueueNode* pQueue;

void enqueue(pQueue &c, int x, int y) {
    pQueue tmp = new QueueNode;
    tmp->x = x;
    tmp->y = y;
    tmp->next = NULL;
    if (c == NULL) {
        c = tmp;
        return;
    }
    pQueue tmp2 = c;
    while(tmp2->next != NULL) tmp2 = tmp2->next;
    tmp2->next = tmp;
}

void dequeue(pQueue &c) {
    if (c == NULL) return;
    pQueue tmp = c;
    c = c->next;
    delete tmp;
}

struct connect {
    int x, y;
};

// ============================================================================
// ALGORITMO DI VISITA IN AMPIEZZA (BFS) PER IL CHASER ENEMY
// ============================================================================
Direction BFS(Player &Pl, Enemy En, Map &Mappa) {
    // Sincronizzato dinamicamente con le costanti statiche della classe Map per evitare overflow
    int visited[20][40] = {};     
    connect parent[20][40];       
    Direction dirs[4] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    
    int dy[4] = {-1, 1, 0, 0};
    int dx[4] = {0, 0, -1, 1};
    
    // Inizializzazione basata sui confini reali della mappa
    for (int i = 0; i < Map::getHeight(); i++) {
        for (int j = 0; j < Map::getWidth(); j++) {
            parent[i][j].x = -1;
            parent[i][j].y = -1;
            visited[i][j] = 0;
        }
    }
    
    pQueue next_step = NULL; 
    visited[En.getY()][En.getX()] = 1;        
    enqueue(next_step, En.getX(), En.getY()); 
    
    bool found = false;
    
    while (next_step != NULL && !found) {
        int cx = next_step->x;
        int cy = next_step->y;
        dequeue(next_step);    
        
        if (cx == Pl.getX() && cy == Pl.getY()) {
            found = true;
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i];
            int ny = cy + dy[i];
            
            // Controllo dei limiti basato sulla configurazione globale di Map
            if (ny >= 0 && ny < Map::getHeight() && nx >= 0 && nx < Map::getWidth() &&
                !visited[ny][nx] && Mappa.GetPos(ny, nx) == 0) {
                visited[ny][nx] = 1;
                parent[ny][nx].x = cx; 
                parent[ny][nx].y = cy;
                enqueue(next_step, nx, ny); 
            }
        }
    }
    
    while (next_step != NULL) dequeue(next_step);
    
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
    
    Dir = BFS(Pl, *this, Mappa); 
    MoveInCurrDirection(Mappa);   
}

void ChaserEnemy::spawna_casuale(Map &Mappa, Enemy** altri, int n) {
    bool occupato = false;
    int h = Map::getHeight();
    int w = Map::getWidth();
    
    // Sincronizzato con i confini reali di Map anziché YMax/XMax dello schermo esterno
    for (int x = w - 2; x >= 1; x--) {
        if (Mappa.GetPos(h - 2, x) == 0) {
            for(int k = 0; k < n; k++) {
                if(altri[k]->getX() == x && altri[k]->getY() == h - 2) {
                    occupato = true;
                }
            }
            if(!occupato) { YLoc = h - 2; XLoc = x; }
        }
    }
    
    if (!occupato) {
        for (int y = h - 2; y >= 1; y--) {
            if (Mappa.GetPos(y, w - 2) == 0) {
                for(int k = 0; k < n; k++) {
                    if(altri[k]->getY() == y && altri[k]->getX() == w - 2) {
                        occupato = true;
                    }
                }
                if(!occupato) { YLoc = y; XLoc = w - 2; }
            }
        }
    }
    
    if (!occupato) {
        do {
            YLoc = rand() % (h - 2) + 1;
            XLoc = rand() % (w - 2) + 1;
        } while(Mappa.GetPos(YLoc, XLoc) != 0 || (YLoc <= 5 && XLoc <= 5));
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
*/