# Bomberman C++ (Terminal Edition)

Versione arcade per terminale del classico videogioco **Bomberman**, sviluppata interamente in C++ con interfaccia a caratteri tramite la libreria **ncurses**. Il progetto adotta un'architettura rigorosamente orientata agli oggetti, realizzata sotto stringenti vincoli didattici: assenza di container della Standard Template Library (STL), divieto di array dinamici (`new T[]`) e gestione manuale delle strutture dati dinamiche.

---

## Caratteristiche Principali

* **Architettura Modulare a Oggetti**: incapsulamento rigoroso distribuito su classi dedicate (`Player`, `Map`, `Bomb`, `Enemy`, `GameEngine`, `LevelManager`, `UIManager`).
* **Strutture Dati Senza STL**:
  * **Lista doppiamente concatenata** per la navigazione bidirezionale e la persistenza dei livelli esplorati (`LevelManager`).
  * **Lista concatenata circolare** per la selezione ciclica dei pulsanti grafici nel menu interattivo (`MainMenu`).
  * **Coda dinamica personalizzata** basata su nodi singoli a puntatore per l'algoritmo di pathfinding.
* **Intelligenza Artificiale Differenziata**:
  * `BasicEnemy`: pattugliamento con rimbalzo lineare su ostacoli.
  * `RandomEnemy`: traiettorie casuali a tempo.
  * `EasyChaserEnemy`: inseguimento diretto con scivolamento perpendicolare su ostacolo.
  * `ChaserEnemy`: calcolo del percorso minimo verso il giocatore tramite algoritmo di visita in ampiezza (**BFS**) privo di STL.
* **Mappa Procedurale e Gameplay**:
  * Generazione controllata di muri distruttibili/indistruttibili con *safe zone* garantita attorno all'area di spawn.
  * Power-up casuali nascosti sotto i muri (Aumento raggio fiamma, Bombe extra, Vite, Passamuri).
  * Porta d'uscita dinamica sbloccabile solo dopo l'eliminazione di tutti i nemici presenti.
* **Persistenza Dati e Classifica**:
  * Gestione file di testo (`leaderboard.txt`) in sola lettura/scrittura formattata.
  * Ordinamento dei punteggi migliori tramite algoritmo **Bubble Sort** decrescente in memoria.

---

## Controlli di Gioco

| Comando / Tasto | Azione |
| :--- | :--- |
| `↑` `↓` `←` `→` (Frecce direzionali) | Spostamento del giocatore / Navigazione nei menu |
| `Spazio` (`SPACE`) | Rilascio di una bomba |
| `Invio` / `Enter` | Selezione dell'opzione attiva nel menu |
| `Q` | Abbandono della sessione corrente e rientro al menu principale |

---

## Requisiti di Sistema

* **Compilatore C++** supportante C++11 o superiore (`g++`, `clang++`).
* **Libreria ncurses** installata sul sistema.
* **Dimensioni Minime Terminale**: **50 colonne $\times$ 24 righe** (il gioco verifica le dimensioni all'avvio segnalando eventuali discrepanze).

### Installazione delle Dipendenze

* **Ubuntu / Debian**:
  ```bash
  sudo apt-get update
  sudo apt-get install build-essential libncurses5-dev libncursesw6
