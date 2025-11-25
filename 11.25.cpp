#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <ctime>

void PrintMenu();
std::string RemoveSpace(std::string target);
bool IsInt(std::string num);


struct Order {
    int OID;//編號
    int Arrival;//下單時刻
    int Duration;// 製作耗時
    int Timeout; // 逾時時刻
    Order(int o , int a , int d , int t) : OID(o) , Arrival(a) , Duration(d) , Timeout(t) {}
};

struct AbortData {
    int OID;
    int Abort;// 取消時刻
    int Delay;// 延誤時間
    int CID; // 廚師編號
    AbortData *next = nullptr;
};

struct TimeoutData {
    int OID;
    int Departure;// 完成時刻
    int Delay;// 延誤時間
    int CID;
};

struct Node {
    Order data;
    Node* next;
    Node(Order& ord) : data(ord), next(nullptr) {} // Order& ord???
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
        Node* node = new Node(ord);
        if (tail == nullptr) {
            head = node;
            tail = head;
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
    int GetSize() {
        return size;
    }
    bool is_empty() {
        return size == 0;
    }
    bool is_full() { 
        return size >= 3;
    }

    void sort();
    bool LoadFromFile(std::string &filename);
    void Print();
    bool ToFile(std::string outname);
};

class Cook {
  private:
    Queue *queue = nullptr; 
    int idle_time; //據說這是閒置時刻的英文
  public:
    bool OrderOKK(Order &todo, AbortData *abort_list) {//判斷
        if (todo.Timeout < idle_time) {
            RemoveOrder(todo, abort_list);
            return false;
        }
        idle_time = idle_time + todo.Duration;
        return true;
    }

    void RemoveOrder(Order &todo, AbortData *abort_list) {
        AbortData *to_delete = new AbortData();;
        to_delete->OID = todo.OID; // 單號
        to_delete->Abort = idle_time; // 取消時刻為取出該訂單的閒置時刻
        to_delete->Delay = to_delete->Abort - todo.Arrival;// 延誤時間為取消時刻減去該訂單的下單時刻
        to_delete->CID = 0; // 廚師編號
        if (!abort_list) {
            abort_list = to_delete;
            return;
        }

        AbortData* cur = abort_list;
        while (cur->next) cur = cur->next;
        cur->next = to_delete;
    }

    void Work(Queue *allorder, AbortData *abort_list) {
        int all_size = allorder -> GetSize();
        Order to_add(0, 0, 0, 0);
        allorder -> dequeue(to_add);
        idle_time = to_add.Arrival + to_add.Duration; // 做第一個

        int i = 0;
        while (i < all_size) {
            while (queue && !(queue -> is_full())) {
                if (i > all_size) {
                    break;
                }
                allorder -> dequeue(to_add);
                queue -> enqueue(to_add);
                i++;
            }
            Order front(0, 0, 0, 0);
            allorder->getFront(front);
            if (front.Arrival >= idle_time) {//目前訂單完成
                Order to_work(0, 0, 0, 0);
                queue -> dequeue(to_work);
                while (!OrderOKK(to_work, abort_list)) { // 找目前佇列可做的那一個
                    if (queue->GetSize() < 1) {
                        break;
                    }
                    queue -> dequeue(to_work);
                }
            } else {
                if (i > all_size) {
                    break;
                }
                allorder -> dequeue(to_add);
                RemoveOrder(to_add, abort_list);
            }
            i++;
        }
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
            Queue queue;

            double timeStart = clock();
            queue.LoadFromFile(filename);
            double timeEnd = clock();
            int read_time = (timeEnd - timeStart) * 1000;

            timeStart = clock();
            queue.Print();
            timeEnd = clock();
            int print_time = (timeEnd - timeStart) * 1000;

            timeStart = clock();
            queue.sort();
            timeEnd = clock();
            int sort_time = (timeEnd - timeStart) * 1000;

            filename = "sorted" + num + ".txt";
            queue.ToFile(filename);

            std::cout << "Reading data: " << read_time << " us.\n\n";
            std::cout << "Sorting data: " << sort_time << " us.\n\n";
            std::cout << "Writing data: " << print_time << " us.\n\n";

        } else if (verb == 2) {
            std::cout << "\nInput a file number: ";
            std::string num;
            garbage = RemoveSpace(garbage);
            if (garbage != "") {
                num = garbage;
            } else {
                getline(std::cin , num);
                num = RemoveSpace(num);
            }
            std::string filename = "sorted" + num + ".txt"; 
            Queue queue;
            queue.LoadFromFile(filename);
            Cook cooker;
            AbortData* abort_list = nullptr;
            cooker.Work(&queue , abort_list);
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

Node* getNode(Node* start, int step) {
    Node* cur = start;
    for (int i = 0; i < step && cur != nullptr; i++)
        cur = cur->next;
    return cur;
}

void Queue::sort() {//我不確定希爾排序是不是長這樣，但我懶得測嘿嘿所以先這樣，我是看google隨便查的圖片錯就再說
    int gap = size / 2;
    while(gap > 0) {
        for (int i = gap ; i < size ; i++) {
            Node* temp = getNode(head, i);
            if (!temp) {
                continue;
            }
            int j = i;
            Order tempData = temp->data;
            while (j >= gap) {
                Node* jNode = getNode(head, j - gap);
                if (!jNode) continue;

                bool change = false;

                if (jNode->data.Arrival > tempData.Arrival) {
                    change = true;
                } else if (jNode->data.Arrival == tempData.Arrival && jNode->data.OID > tempData.OID) {
                    change = true;
                }

                if (!change) {
                    break;
                }
                Node* jNode2 = getNode(head, j);
                jNode2->data = jNode->data;  
                j -= gap;
            }

            Node* target = getNode(head, j);
            target->data = tempData;
        }
        gap = gap / 2;
    }
}

bool Queue::LoadFromFile(std::string &filename) {
    std::ifstream fin(filename); //fin 型別是 ifstream，全名是「input file stream」，意思是輸入檔案串流，類似cin有buffer
    if (!fin.is_open()) {
        std::cout << "\n" << filename << " does not exist!\n\n";
        return false;
    }

    clear();
    std::string header;
    if (!getline(fin, header)) {
        fin.close();
        return false;
    }

    int data[4] = {0};
    while(fin >> data[0]) {
        fin >> data[1] >> data[2] >> data[3];
        Order order(data[0] , data[1] , data[2] , data[3]);
        enqueue(order);
    }

    fin.close();
    return true;
}

void Queue::Print() {
    std::cout << "\tOID\tArrival\tDuration\tTimeOut" << std::endl;
    Node* temp = head;
    for (int i = 0 ; i < size ; i++) {
        std::cout << "(" << i + 1 << ")";
        std::cout << "\t" << temp->data.OID << "\t" << temp->data.Arrival;
        std::cout << "\t" << temp->data.Duration << "\t" << temp->data.Timeout;
        std::cout << std::endl;
        temp = temp->next;
    }
    std::cout << std::endl;
}

bool Queue::ToFile(std::string outname) {
    std::ofstream fout(outname);
    if (!fout.is_open()) {
        std::cout << "Cannot open file: " << outname << "\n";
        return false;
    }

    fout << "OID\tArrival\tDuration\tTimeOut\n";
    Node* cur = head;
    while (cur != nullptr) {
        fout << cur->data.OID << "\t" << cur->data.Arrival << "\t" << cur->data.Duration << "\t" << cur->data.Timeout << "\n";
        cur = cur->next;
    }

    fout.close();
    return true;
}
