//11327104 林采寧 11327111 林方晴

#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <chrono> // 計時
#include <iomanip>
#include <sstream>


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
    TimeoutData *next = nullptr;
};

struct Node {
    Order data;
    Node* next;
    Node(Order& ord) : data(ord), next(nullptr) {} // Order& ord???
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
        size = 0;
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

    bool getFront(Order& out) {
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
    Queue* Copy();
};

class Cook {
  private:
    Queue *queue = new Queue(); 
    int idle_time = 0; //據說這是閒置時刻的英文
    int number = 0;
  public:
    Cook() {}
    Cook(int num) {
        number = num;
    }
    void SetNumber(int num) {
        number = num;
    }
    Queue* GetQueue() {
        return queue;
    }
    int GetIdleTime() {
        return idle_time;
    }
    void SetIdleTime(int num) {
        idle_time = num;
    }

    bool OrderOKK(Order &todo, AbortData* &abort_list, TimeoutData* &timeout_list) {//判斷
        if (todo.Timeout < idle_time) {
            RemoveOrder(todo, abort_list , false);
            return false;
        }
        if (idle_time > todo.Arrival) {
            idle_time = idle_time + todo.Duration;// 做訂單
        } else {
            idle_time = todo.Arrival + todo.Duration;
        }
        if (todo.Timeout < idle_time) { // 做完發現逾時
            PutToTimeout(todo, timeout_list);
        }
        return true;
    }

    void PutToTimeout(Order &todo, TimeoutData* &timeout_list) {
        TimeoutData *to_put_timeout = new TimeoutData();
        to_put_timeout->OID = todo.OID; // 單號
        to_put_timeout->Departure = idle_time;
        to_put_timeout->Delay = (idle_time - todo.Duration) - todo.Arrival; // idle_time - todo.Duration取出時的閒置時刻
        to_put_timeout->CID = number; // 廚師編號
        if (!timeout_list) {
            timeout_list = to_put_timeout;
            return;
        }

        TimeoutData* cur = timeout_list;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = to_put_timeout;
    }

    void RemoveOrder(Order &todo, AbortData* &abort_list , bool is_full) {
        AbortData *to_delete = new AbortData();
        to_delete->OID = todo.OID; // 單號
        if (is_full) {
            to_delete->CID = 0;
            to_delete->Abort = todo.Arrival;
            to_delete->Delay = 0;
        } else {
            to_delete->CID = number;
            to_delete->Abort = idle_time;
            to_delete->Delay = to_delete->Abort - todo.Arrival;// 延誤時間為取消時刻減去該訂單的下單時刻
        }
        if (!abort_list) {
            abort_list = to_delete;
            return;
        }

        AbortData* cur = abort_list;
        while (cur->next) cur = cur->next;
        cur->next = to_delete;
    }
};

class Kitchen {
  private:
    Cook *cook = nullptr;
    int number = 1;
    Queue *allorder = new Queue();
    AbortData* abort_list = nullptr;
    TimeoutData* timeout_list = nullptr;
    int total_order = -1;

  public:
    ~Kitchen() {
        if (total_order == -1) {
            return;
        }
        clear();
    }
    void clear();
    void SetKitchen(int num_of_cook);
    bool SetAllorder(std::string &filename);
    int GetTotalOrder();
    void Work();
    bool ToFile(std::string outname);
    int ChoseCook(Order &to_work);
    int ChoseCook();
};

int main() {
    std::string garbage;
    int verb = -1;
    Kitchen k;
    std::string file;
    while(true) {
        PrintMenu();
        if (!(std::cin >> verb)) {
            break;
        }
        getline(std::cin , garbage);
        if (verb == 0) break;
        else if (verb == 1) {
            k.clear();
            std::cout << "\nInput a file number (e.g., 401, 402, 403, ...): \n";            
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

            std::chrono::time_point<std::chrono::high_resolution_clock> start;
            std::chrono::time_point<std::chrono::high_resolution_clock> end;
            std::chrono::microseconds read_time;
            std::chrono::microseconds print_time;
            std::chrono::microseconds sort_time;

            start = std::chrono::high_resolution_clock::now();
            if (!(queue.LoadFromFile(filename))) {
                continue;
            }
            end = std::chrono::high_resolution_clock::now();
            read_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            start = std::chrono::high_resolution_clock::now();
            queue.Print();
            end = std::chrono::high_resolution_clock::now();
            print_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            start = std::chrono::high_resolution_clock::now();
            queue.sort();
            end = std::chrono::high_resolution_clock::now();
            sort_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            filename = "sorted" + num + ".txt";
            queue.ToFile(filename);

            std::cout << "Reading data: " << read_time.count() << " us.\n\n";
            std::cout << "Sorting data: " << sort_time.count() << " us.\n\n";
            std::cout << "Writing data: " << print_time.count() << " us.\n\n";

        } else if (verb == 2) {
            std::cout << "\nInput a file number (e.g., 401, 402, 403, ...): \n";
            garbage = RemoveSpace(garbage);
            if (garbage != "") {
                file = garbage;
            } else {
                getline(std::cin , file);
                file = RemoveSpace(file);
            }
            std::string filename = "sorted" + file + ".txt"; 

            k.clear();
            if (!(k.SetAllorder(filename))) {
                continue;
            }
            k.SetKitchen(1);
            k.Work();

            filename = "one" + file + ".txt";
            k.ToFile(filename);
        } else if (verb == 3) {
            if (k.GetTotalOrder() == -1) {
                std::cout << "\n### Execute command 2 first! ###\n\n";
                continue;
            }
            k.SetKitchen(2);
            k.Work();

            std::string filename = "two" + file + ".txt";
            k.ToFile(filename);
            std::cout << "\n";
        } else if (verb == 4) {
            if (k.GetTotalOrder() == -1) {
                std::cout << "\n### Execute command 2 first! ###\n\n";
                continue;
            }
            int num = 0;
            std::cout <<"\nInput the number of queues: ";
            while (true) {
                while (!(std::cin >> num)) {
                    std::cin.clear(); // 清掉錯誤
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 丟掉錯誤輸入
                    std::cout <<"\nInput the number of queues: ";
                }

                if (num > 0) {
                    break;
                }
                std::cin.clear(); // 清掉錯誤
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 丟掉錯誤輸入
                std::cout <<"\nInput the number of queues: ";

            }
            std::cout << "\n";
            k.SetKitchen(num);
            k.Work();

            std::string filename = "any" + file + ".txt";
            if (num == 1) {
                filename = "one" + file + ".txt";
            } else if (num == 2) {
                filename = "two" + file + ".txt";
            }
            k.ToFile(filename);
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
    if(num.size() == 0) {
        return false;
    }
  for (int i = 0; i < num.size(); i++) {
    if (i == 0 && (num[i] == '+' || num[i] == '-')) {
        continue;
    }
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

std::string RemoveDotTab(std::string target) {
    std::string to_return;
    for (int i = 0 ; i < target.size() ; i++) {
        if (target[i] == '.' || target[i] == '\t') {
            break;
        }
        to_return = to_return + target[i];
    }
    return to_return;
}

Node* getNode(Node* start, int step) {
    Node* cur = start;
    for (int i = 0; i < step && cur != nullptr; i++)
        cur = cur->next;
    return cur;
}

//----------------------------------------------------------//

void Queue::sort() {
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
        std::cout << "### " << filename << " does not exist! ###\n\n";
        return false;
    }

    clear();
    std::string header;
    if (!getline(fin, header)) {
        fin.close();
        std::cout << "\n### Get nothing from " << filename<<" ! ###\n\n";
        return false;
    }

    std::string a[4];
    int data[4] = {0};
    while (getline(fin, a[0], '\t') && getline(fin, a[1], '\t') &&getline(fin, a[2], '\t') &&getline(fin, a[3])) {

        a[0] = RemoveDotTab(a[0]);
        a[1] = RemoveDotTab(a[1]);
        a[2] = RemoveDotTab(a[2]);
        a[3] = RemoveDotTab(a[3]);
        
        a[0] = RemoveSpace(a[0]);
        a[1] = RemoveSpace(a[1]);
        a[2] = RemoveSpace(a[2]);
        a[3] = RemoveSpace(a[3]);


        for (int i = 0; i < 4; i++) {
            if (IsInt(a[i])) {
                data[i] = stoi(a[i]);
            }

            else {
                data[i] = 0;
            }
        }
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
        std::cout << "(" << i + 1 << ") ";
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

Queue* Queue::Copy() {
    Queue* copy = new Queue();

    Node* cur = this->head;
    while (cur != nullptr) {
        copy->enqueue(cur->data);
        cur = cur->next;
    }

    return copy;
}

//----------------------------------------------------------//

void Kitchen::clear() {
    if (!cook) return;
    delete []cook;
    cook = nullptr;
    allorder->clear();
    number = 0;
    while (abort_list) {
        AbortData *temp = abort_list -> next;
        delete abort_list;
        abort_list = temp;
    }
    while (timeout_list) {
        TimeoutData *temp = timeout_list -> next;
        delete timeout_list;
        timeout_list = temp;
    }
    total_order = -1;
}

bool Kitchen::ToFile(std::string outname) {
    std::ofstream fout(outname);
    if (!fout.is_open()) {
        std::cout << "Cannot open file: " << outname << "\n";
        return false;
    }

    fout << "\t[Abort List]\n";
    fout << "\tOID\tCID\tDelay\tAbort\n";

    int total_delay = 0;//秒數
    int total_delay_order = 0;
    float failure_per = 0;

    AbortData* cur_abort = abort_list;
    while (cur_abort) {
        fout << "[" << total_delay_order + 1 << "]\t";
        fout << cur_abort->OID << "\t" << cur_abort->CID << "\t" << cur_abort->Delay << "\t" << cur_abort->Abort << "\n";
        total_delay = total_delay + cur_abort ->Delay;
        cur_abort = cur_abort -> next;
        total_delay_order++;
    }

    fout << "\t[Timeout List]\n";
    fout << "\tOID\tCID\tDelay\tDeparture\n";
    int i = 1;
    TimeoutData* cur_timeout = timeout_list;
    while (cur_timeout) {
        fout << "[" << i << "]\t";
        i++;
        fout << cur_timeout->OID << "\t" << cur_timeout->CID << "\t" << cur_timeout->Delay << "\t" << cur_timeout->Departure << "\n";
        total_delay = total_delay + cur_timeout ->Delay;
        cur_timeout = cur_timeout -> next;
        total_delay_order++;
    }

    failure_per = ((float)total_delay_order/(float)total_order) * 100; //失敗率

    fout << "[Total Delay]\n" << total_delay <<" min.\n";
    fout << "[Failure Percentage]\n" << std::fixed << std::setprecision(2) <<failure_per <<" %\n";
    
    fout.close();
    return true;
}

void Kitchen::SetKitchen(int num_of_cook) {
    number = num_of_cook;
    if (cook) delete []cook;
    cook = new Cook[number + 1];
    for (int i = 1 ; i <= number ; i++) {
        cook[i].SetNumber(i);
    }
    while (abort_list) {
        AbortData *temp = abort_list -> next;
        delete abort_list;
        abort_list = temp;
    }
    while (timeout_list) {
        TimeoutData *temp = timeout_list -> next;
        delete timeout_list;
        timeout_list = temp;
    }
    total_order = allorder->GetSize();
}

bool Kitchen::SetAllorder(std::string &filename) {
    if (!(allorder->LoadFromFile(filename))) {
        return false;
    }
    allorder->Print();
    total_order = allorder->GetSize();
    return true;
}

int Kitchen::GetTotalOrder() {
    return total_order;
}

int Kitchen::ChoseCook(Order &to_work) {
    int freecook = 0;
    int queue_short = 3;
    for (int i = 1 ; i <= number ; i++) {
        if (!(cook[i].GetQueue() -> is_full()) && cook[i].GetQueue()->GetSize() < queue_short) {
            queue_short = cook[i].GetQueue()->GetSize();
            freecook = i;
        }
    }
    return freecook;
}

int Kitchen::ChoseCook() {
    int cooker = 0;
    int short_idle = 1e9;
    for (int i = 1 ; i <= number ; i++) {
        if (!(cook[i].GetQueue() -> is_empty()) && cook[i].GetIdleTime() < short_idle) {
            short_idle = cook[i].GetIdleTime();
            cooker = i;
        }
    }
    return cooker;
}

void Kitchen::Work() {
    int all_size = allorder -> GetSize();
    Queue* all = allorder->Copy();
    int j = 0;
    Order to_work(0, 0, 0, 0);
    while (j <= all_size) {
        bool put = false;
        if (all && !(all -> dequeue(to_work))) {
            int cooker = ChoseCook();
            while (cooker != 0) {
                Order to_do(0, 0, 0, 0);
                cook[cooker].GetQueue() -> dequeue(to_do);
                cook[cooker].OrderOKK(to_do, abort_list, timeout_list);
                cooker = ChoseCook();
            }
            return;
        }
        j++;
        if (to_work.Arrival < 0 || to_work.Duration < 0 || to_work.OID < 0 || to_work.Timeout < 0) {//非法輸入，忽略
            total_order--;
            continue;
        } else if(to_work.Arrival + to_work.Duration > to_work.Timeout) {
            total_order--;
            continue;
        }
        
        for (int i = 1 ; i <= number ; i++) {
            if (to_work.Arrival >= cook[i].GetIdleTime()) {
                Order to_do(0, 0, 0, 0);
                while (cook[i].GetQueue() -> dequeue(to_do)){
                    if (cook[i].OrderOKK(to_do, abort_list, timeout_list) && 
                        cook[i].GetIdleTime() > to_work.Arrival) {
                            break;
                    }
                }
            }
        }

        bool ok = false;
        for (int i = 1 ; i <= number ; i++) {
            if (cook[i].GetQueue() -> is_empty() && to_work.Arrival >= cook[i].GetIdleTime()) {
                cook[i].SetIdleTime(to_work.Duration + to_work.Arrival);
                ok = true;
                break;
            } 
        }
        if (ok) {
            continue;
        }


        int freecook = ChoseCook(to_work);
        if (freecook != 0) {
            cook[freecook].GetQueue() -> enqueue(to_work);
            put = true;
        }

        if (!put) {
            if (j > all_size) {
                break;
            }
            cook[0].RemoveOrder(to_work, abort_list , true);
        }
    }
}
