#include <iostream>
#include <string>

void PrintMenu();
std::string RemoveSpace(std::string target);
bool IsInt(std::string num);

struct Order {
    int OID;//編號
    int Arrival;//下單時刻
    int Duration;// 製作耗時
    int Timeout; // 逾時時刻
};

typedef Node Node;
struct Node {
    Order data;
    Node* next;
    Node(Order& ord) : data(ord), next(nullptr) {}
};

class Queue {
private:
    Node* head;
    Node* tail;
    int size;

public:
    Queue() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }
    ~Queue() {
        clear();
    }
    void clear() {
        while (head) {
            Node* t = head;
            head = head->next;
            delete t;
        }
        tail = nullptr;
    }

    bool enqueue(Order& ord) {
        if (is_full()) return false;
        Node* node = new Node(ord);
        if (tail == nullptr) {
            head = tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
        size++;
        return true;
    }
    
    bool dequeue(Order& out) {//移除的東西我感覺我之後會用到先寫上去
        if (is_empty()) return false;
        Node* cur = head;
        out = head->data;
        head = head->next;
        if (head == nullptr) {
            tail = nullptr;
        }
        delete cur;
        size--;
        return true;
    }

    bool getFront(Order& out){
        if (is_empty()) return false;
        out = head->data;
        return true;
    }
    int GetSize() const {
        return size;
    }
    bool is_empty() const {
        return size == 0;
    }
    bool is_full() const { 
        return size >= 3;
    }
    
};

int main() {
    std::string garbage;
    int verb = -1;

    while(true) {
        PrintMenu();
        if (!(std::cin >> verb)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\nCommand does not exist!\n\n";
            continue;
        }
        getline(std::cin , garbage);
        if (verb == 0) break;
        else if (verb == 1) {
            std::cout << "\nInput a file number: ";
            std::string num;
            garbage = RemoveSpace(garbage);
            if (garbage != "") {
                num = garbage;
            } else {
                getline(std::cin , num);
                num = RemoveSpace(num);
            }
            std::string filename = "input" + num + ".txt";  



        } else if (verb == 2) {
            
        } else if (verb == 3) {
            
        } else if (verb == 4) {
            
        } else {
            std::cout << "\nCommand does not exist!\n\n";
        }
    }
    return 0;
}


void PrintMenu() {
    std::cout << "*** (^_^) Data Structure (^o^) ***" << std::endl;
    std::cout << "** Simulate FIFO Queues by SQF ***" << std::endl;
    std::cout << "* 0. Quit                        *" << std::endl;
    std::cout << "* 1. Sort a file                 *" << std::endl;
    std::cout << "* 2. Simulate one FIFO queue     *" << std::endl;
    std::cout << "* 3. Simulate two queues by SQF  *" << std::endl;
    std::cout << "* 4. Simulate some queues by SQF *" << std::endl;
    std::cout << "**********************************" << std::endl;
    std::cout << "Input a command(0, 1, 2, 3, 4): ";
    return;
}

bool IsInt(std::string num) {
  for (int i = 0; i < num.size(); i++) {
    if(num[i] > '9' || num[i] < '0') {
      return false;
    }
  }
  return true;
}

std::string RemoveSpace(std::string target) {
  std::string to_return;
  for (int i = 0; i < target.size() ;i++) {
    if (target[i] != ' ' && target[i] != '\t' && target[i] != '\n') {
      to_return = to_return + target[i];
    }
  }
  return to_return;
}

void sort(Order* a, int size) {//我不確定希爾排序是不是長這樣，但我懶得測嘿嘿所以先這樣，我是看google隨便查的圖片錯就再說
    int gap = size / 2;
    while (gap > 0) {
        for (int i = 0 ; i < size ; i += gap) {
            bool change = false;
            if (a[i + gap].Arrival > a[i].Arrival) {
                change = true;
            } else if (a[i + gap].Arrival == a[i].Arrival && a[i + gap].OID > a[i].OID) {
                change = true;
            }

            if (change) {
                Order temp = a[i];
                a[i] = a[i + gap];
                a[i + gap] = temp;
            }
        }
        gap /= 2;
    }
}


