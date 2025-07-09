typedef struct node {
    int data;
    struct node* next;
    node(int d): data(d), next(nullptr){}
}node;

void reverse(node* head) {
    if (head == nil) {
        return;
    }
    node* pleft = nullptr, pcurrent = head, pright = head->next;
    while (pright != nullptr) {
        pcurrrent->next = pleft;
        node* tmp = pright->next;
        pright->next = pcurrent;
        pleft = pcurrent;
        pcurrent = pright;
        pright = tmp;
    }

    while (pcurrent != nullptr) {
        cout << pcurrent->data << '\t';
        pcurrent = pcurrent->next;
    }
}