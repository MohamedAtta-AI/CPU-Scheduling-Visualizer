#include <iostream>
using namespace std;

class Process {
public:
    int id;
    int arrival;
    int burst;
    int completed;
    int waiting;
    int turnaround;
    Process() { id = arrival = burst = completed = waiting = turnaround = 0; }
};

class Gantt {
public:
    int start;
    int end;
    Gantt() { start = end = 0; }
};


template<class T>
class Node {
public:
    T data;
    Node<T>* next;
    Node(){
        next = 0;
    }
    Node(const T &s, Node<T>* const n = 0){
        data = s;
        next = n;
    }
};

template<class T>
class TSortedList: private Node<T> {
public:
    TSortedList(){
        head = tail = 0;
        size = 0;
    }
    void addtohead(const T &data){
        head = new Node<T>(data, head);
        size++;
    }
    void addToTail(const T &data)
    {
        if (!tail)
            head = tail = new Node<T>(data);
        else {
            tail->next = new Node<T>(data);
            tail = tail->next;
        }
        size++;
    }
    void removeFromHead() {
        size--;
        if (head == NULL) {
            return;
        } else if (head == tail) {
            delete head;
            head = tail = NULL;
        } else {
            Node<T>* ptr = head;
            head = head->next;
            delete ptr;
        }
    }
    void removeFromTail() {
        Node<T>* ptr = head;
        if (head == tail) {
            delete head;
            delete tail;
            head = tail = NULL;
        } else {
            while (ptr->next != tail)
                ptr = ptr->next;
            tail = ptr;
            delete tail->next;
            tail->next = NULL;
        }
        size--;
    }
    T& at(const int &pos) {
        Node<T>* ptr = head;
        for (int i = 0; i < pos; i++) ptr = ptr->next;
        return ptr->data;
    }
    void insertAt(const T &data, const int &pos) {
        Node<T>* ptr = head;
        for (int i = 0; i < pos; i++) ptr = ptr->next;
        Node<T> newNode = new Node<T>(data, ptr->next);
        ptr->next = newNode;
    }
    void InsertionSort(){
        Node<T>* ptr1 = head->next;
        while (ptr1){
            Node<T>* ptr2 = head;
            while (ptr2 != ptr1){
                if (ptr2->data.arrival > ptr1->data.arrival){
                    T temp = ptr1->data;
                    ptr1->data = ptr2->data;
                    ptr2->data = temp;
                }
                else
                    ptr2 = ptr2->next;
            }
            ptr1 = ptr1->next;
        }
    }
    int getSize() {
        return size;
    }
    void print() const{
        if (head == 0)
            cout << "List is empty" << endl;
        else{
            Node<T>* temp = head;
            while (temp != 0){
                cout << "ID: " << temp->data.id << " "
                     << "Arrival: " << temp->data.arrival << " "
                     << "Burst: " << temp->data.burst << " "
                     << "->";
                temp = temp->next;
            }
            cout << "NULL" << endl;
        }
    }

    Node<T> *head, *tail;
    int size;
};