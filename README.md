# Bomberman C++ (Terminal Edition)

Versione arcade per terminale del classico videogioco **Bomberman**, sviluppata interamente in C++ con interfaccia a caratteri tramite la libreria **ncurses**. Il progetto adotta un'architettura rigorosamente orientata agli oggetti, realizzata sotto stringenti vincoli didattici: assenza di container della Standard Template Library (STL), divieto di array dinamici (`new T[]`) e gestione manuale delle strutture dati dinamiche.

---

## Caratteristiche Principali

* **Architettura Modulare a Oggetti**: incapsulamento rigoroso distribuito su classi dedicate (`Player`, `Map`, `Bomb`, `Enemy`, `GameEngine`, `LevelManager`, `UIManager`).
* **Strutture Dati Senza STL**:
  * **Lista doppiamente concatenata** per la navigazione bidirezionale e la persistenza dei livelli esplorati (`LevelManager`)[cite: 6, 7].
  * **Lista concatenata circolare** per la selezione ciclica dei pulsanti grafici nel menu interattivo (`MainMenu`).
  * **Coda dinamica personalizzata** basata su nodi singoli a puntatore per l'algoritmo di pathfinding[cite: 6, 7].
* **Intelligenza Artificiale Differenziata**:
  * `BasicEnemy`: pattugliamento con rimbalzo lineare su ostacoli[cite: 7].
  * `RandomEnemy`: traiettorie casuali a tempo[cite: 7].
  * `EasyChaserEnemy`: inseguimento diretto con scivolamento perpendicolare su ostacolo[cite: 7].
  * `ChaserEnemy`: calcolo del percorso minimo verso il giocatore tramite algoritmo di visita in ampiezza (**BFS**) privo di STL[cite: 7].
* **Mappa Procedurale e Gameplay**:
  * Generazione controllata di muri distruttibili/indistruttibili con *safe zone* garantita attorno all'area di spawn[cite: 7].
  * Power-up casuali nascosti sotto i muri (Aumento raggio fiamma, Bombe extra, Vite, Passamuri)[cite: 7].
  * Porta d'uscita dinamica sbloccabile solo dopo l'eliminazione di tutti i nemici presenti[cite: 7].
* **Persistenza Dati e Classifica**:
  * Gestione file di testo (`leaderboard.txt`) in sola lettura/scrittura formattata[cite: 7].
  * Ordinamento dei punteggi migliori tramite algoritmo **Bubble Sort** decrescente in memoria.

---

## Controlli di Gioco

| Comando / Tasto | Azione |
| :--- | :--- |
| `↑` `↓` `←` `→` (Frecce direzionali) | Spostamento del giocatore / Navigazione nei menu[cite: 7] |
| `Spazio` (`SPACE`) | Rilascio di un ordigno[cite: 7] |
| `Invio` / `Enter` | Selezione dell'opzione attiva nel menu[cite: 7] |
| `Q` | Abbandono della sessione corrente e rientro al menu principale[cite: 7] |

---

## Requisiti di Sistema

* **Compilatore C++** supportante C++11 o superiore (`g++`, `clang++`)[cite: 7].
* **Libreria ncurses** installata sul sistema[cite: 7].
* **Dimensioni Minime Terminale**: **50 colonne $\times$ 24 righe** (il gioco verifica le dimensioni all'avvio segnalando eventuali discrepanze)[cite: 7].

### Installazione delle Dipendenze

* **Ubuntu / Debian**:
  ```bash
  sudo apt-get update
  sudo apt-get install build-essential libncurses5-dev libncursesw6
