#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

struct Node {
    int value;
    Node* next;
    mutex* nod_mutex;

    Node(int val) : value(val), next(nullptr) {
        nod_mutex = new mutex();
    }

    ~Node() {
        delete nod_mutex;
    }
};

class FineGrainedQueue {
private:
    Node* head;
    mutex* queue_mutex;

public:
    FineGrainedQueue() : head(nullptr) {
        queue_mutex = new mutex();
    }

    ~FineGrainedQueue() {
        //чистим
        Node* current = head;
        while (current != nullptr) {
            Node* next_node = current->next;
            delete current;
            current = next_node;
        }
        delete queue_mutex;
    }

    // втавка в середину, либо в конец
    void insert(int value, int pos) {
        Node* new_node = new Node(value);

        lock_guard<mutex> head_lock(*queue_mutex); //" 'б' -лочим " головной указатель

        if (head == nullptr) { 
            head = new_node;
            return;
        }

        if (pos <= 0) { 
            new_node->next = head;
            head = new_node;
            return;
        }

        Node* current = head;
        Node* prev = nullptr;
        int current_pos = 0;

        // переход к месту вставки в список
        while (current != nullptr && current_pos < pos) {
            if (prev != nullptr) { //"раз(б)лочивание" mutex предыдущего узла, если пройден
                prev->nod_mutex->unlock();
            }
            prev = current;
            current = current->next;
            if (prev != nullptr) { //блокировка mutex
                prev->nod_mutex->lock();
            }
            current_pos++;
        }

        // Если current - nullptr, это означает, что pos больше, чем длина списка
  // вставка в конец
        if (current == nullptr) {
            if (prev != nullptr) {
                prev->next = new_node;
                prev->nod_mutex->unlock(); //разблокирование последнего узла
            }
            else { // Не должно случиться, если head не null и pos > 0
                head = new_node;
            }
        }
        else { // вставка в середину
            new_node->next = current;
            if (prev != nullptr) {
                prev->next = new_node;
                prev->nod_mutex->unlock(); // "разлочивание" предыдущего узла
            }
            else {
                head = new_node;
            }
        }
    }

    void print_list() {
        lock_guard<mutex> head_lock(*queue_mutex); // 
        Node* current = head;
        cout << "List: ";
        while (current != nullptr) {
            cout << current->value << " ";
            current = current->next;
        }
        cout << endl;
    }
};

int main() {
    FineGrainedQueue queue;

   
    queue.insert(10, 0); 
    queue.insert(20, 1);
    queue.insert(30, 2);

    queue.print_list(); // проверка элтов в списке

    // вставка в середину
    queue.insert(15, 1); 
    queue.print_list(); 

    // вставка в конец (pos > length)
    queue.insert(40, 10);
    queue.print_list(); 

    // всравка в середину
    queue.insert(25, 3); 
    queue.print_list(); 

    return 0;

}
