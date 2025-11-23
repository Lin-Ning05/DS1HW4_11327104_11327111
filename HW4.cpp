#include <iostream>
#include <string>
#include <limits>
#include <fstream>

void PrintMenu();
std::string RemoveSpace(std::string target);
bool IsInt(std::string num);


struct Order {
    int OID;//編號
    int Arrival;//下單時刻
    int Duration;// 製作耗時
    int Timeout; // 逾時時刻
};

struct Node {
    Order data;
    Node* next;
    Node(Order& ord) : data(ord), next(nullptr) {}
};
typedef Node Node;

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

    void sort();
    bool LoadFromFile(std::string &filename);
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

void Queue::sort() {//我不確定希爾排序是不是長這樣，但我懶得測嘿嘿所以先這樣，我是看google隨便查的圖片錯就再說
    int gap = size / 2;
    Node* temp = head;
    while (gap > 0) {
        for (int i = 0 ; i < size ; i += gap) {
            bool change = false;
            Node* totry;
            for (int j = 0 ; j < gap ; j++) {
                totry = temp->next;
            }
            if (totry->data.Arrival > temp->data.Arrival) {
                change = true;
            } else if (totry->data.Arrival == temp->data.Arrival && totry->data.OID > temp->data.OID) {
                change = true;
            }

            if (change) {
                Order t = temp->data;
                temp->data = totry->data;
                totry->data = t;
            }
        }
        gap /= 2;
    }
}

bool Queue::LoadFromFile(std::string &filename) {
    std::ifstream fin(filename); //fin 型別是 ifstream，全名是「input file stream」，意思是輸入檔案串流，類似cin有buffer
    if (!fin.is_open()) {
        std::cout << "\n" << filename << " does not exist!\n\n";
        return false;
    }
    


    fin.close();
    return true;
}



