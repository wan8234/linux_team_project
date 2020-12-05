#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/time.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/delay.h>

typedef struct _Node{ // define node.
    int data;
    struct _Node *prev;
    struct _Node *next;
} Node;

typedef struct _List{ // define list.
    Node *head;
    Node *tail;
} List;

void init_list(List *list){ // initalize list.
    list->head = NULL;
    list->tail = NULL;
}

void insert_node_front(List *list, List *garbage, int data){ // insert new node at front.
    Node *new;
    if (garbage->head == NULL){ // if there is no garbage list, make new node.
        new = kmalloc(sizeof(Node), GFP_KERNEL);
        //printk("Make New\n");
    }
    else { // if there is garbage list, use garbage list.
        new = garbage->head;
        if (garbage->head->next != NULL)
            garbage->head->next->prev = NULL;
        garbage->head = garbage->head->next;
        //printk("Use garbage\n");
    }
    
    new->data = data;    
    new->prev = NULL;
    new->next = NULL;    

    if (list->head == NULL){ // first insert.
        list->head = new;
        list->tail = new;
    }
    else {
        list->head->prev = new;
        new->next = list->head;
        list->head = new;
    }
}

Node *search_node(List *list, int data){ // search the node.
    Node *temp = list->head;

    if (temp == NULL)
        return NULL;

    while (temp->data != data){
        temp = temp->next;
        if (temp == NULL)
            return NULL;
    }
    return temp;
}

void return_node(List *list, List *garbage){ // return node to garbage list. no free work.
    if (garbage->head == NULL){ // no garbage list exists before.
        garbage->head = list->head;
        garbage->tail = list->tail;
        list->head = NULL;
        list->tail = NULL;
    }
    else {
    	garbage->tail->next = list->head;
    	list->head->prev = garbage->tail;
    	garbage->tail = list->tail;
    }
}

int delete_node(List *list, int data){ // delete node with free.
    Node *temp = search_node(list, data);
    int value = temp->data;

    if (temp->next != NULL){ 
        temp->prev->next = temp->next;
        temp->next->prev = temp->prev;
    }
    else if (list->head == temp){ // only one node.
        list->head = NULL;
        // list->tail = NULL;
    }
    else { // last node.
        temp->prev->next = NULL;
        temp->prev = NULL;
        temp->next = NULL;
    }    
    kfree(temp);

    return value;
}

void delete_list(List *list){
    Node *temp;
    while (list->head != NULL){
        temp = list->head;
        list->head = list->head->next;
        kfree(temp);
    }
}

void struct_example(void){
	List list;
	List garbage;
	int i;
	init_list(&list);
	init_list(&garbage);

	ktime_t start, end;
	__s64 actual_time;
	
	///////////////////////////////////// 1000
	start = ktime_get(); // insert 1000
	for(i = 0; i < 1000; i++){
		insert_node_front(&list, &garbage, i);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Insert 1000 element in list: %lld ns\n", (long long)actual_time);
	
	Node *current_node;
	
	start = ktime_get(); // search 1000
	for(i = 0; i < 1000; i++){
		current_node = search_node(&list, i);
		//printk("current value: %d\n", current_node->data);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Search 1000 element in list: %lld ns\n", (long long)actual_time);	
	
	start = ktime_get(); // delete 1000
	return_node(&list, &garbage);
	end = ktime_get();	
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Return 1000 element in list: %lld ns\n", (long long)actual_time);	
	
	start = ktime_get(); // insert 1000
	for(i = 0; i < 1000; i++){
		insert_node_front(&list, &garbage, i);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Insert 1000 element in list: %lld ns\n", (long long)actual_time);
	
	start = ktime_get(); // delete 1000
	delete_list(&list);
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("delete list: %lld ns\n", (long long)actual_time);
	
	start = ktime_get(); // delete 1000
	delete_list(&garbage);
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("delete garbage: %lld ns\n", (long long)actual_time);
	
	start = ktime_get(); // insert 1000
	for(i = 0; i < 1000; i++){
		insert_node_front(&list, &garbage, i);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Insert 1000 element in list: %lld ns\n", (long long)actual_time);
	
	/*
	///////////////////////////////////// 10000
	start = ktime_get(); // insert 10000
	for(i = 0; i < 10000; i++){
		insert_node_front(&list, &garbage, i);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Insert 10000 element in list: %lld ns\n", (long long)actual_time);	
	
	start = ktime_get(); // search 10000
	for(i = 0; i < 10000; i++){
		current_node = search_node(&list, i);
		//printk("current value: %d\n", current_node->data);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Search 10000 element in list: %lld ns\n", (long long)actual_time);
		
	start = ktime_get(); // delete 10000
	for(i = 0; i < 10000; i++){
		tmp = return_node(&list, &garbage, i);
		//printk("return data: %d", tmp);
	}
	end = ktime_get();	
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Delete 10000 element in list: %lld ns\n", (long long)actual_time);	
	*/
	/*
	///////////////////////////////////// 100000
	start = ktime_get(); // insert 100000
	for(i = 0; i < 100000; i++){
		insert_node_front(&list, &garbage, i);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Insert 100000 element in list: %lld ns\n", (long long)actual_time);	
	
	start = ktime_get(); // search 100000
	for(i = 0; i < 100000; i++){
		current_node = search_node(&list, i);
		//printk("current value: %d\n", current_node->data);
	}
	end = ktime_get();
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Search 100000 element in list: %lld ns\n", (long long)actual_time);
		
	start = ktime_get(); // delete 100000
	for(i = 0; i < 100000; i++){
		tmp = return_node(&list, &garbage, i);
		//printk("return data: %d", tmp);
	}
	end = ktime_get();	
	
	actual_time = ktime_to_ns(ktime_sub(end, start));
	printk("Delete 100000 element in list: %lld ns\n", (long long)actual_time);	*/
	
}

int __init hello_module_init(void){
	printk(KERN_EMERG "Simple Module! \n");
	struct_example();	
	return 0;
}

void __exit hello_module_cleanup(void){
	printk("Bye Module! \n");
}
module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
