
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
    pthread_mutex_t mutex;
} Stack;

void stack_init(Stack *stack) {
    stack->top = -1;
    pthread_mutex_init(&stack->mutex, NULL);
}

int stack_is_empty(Stack *stack) {
    return (stack->top == -1);
}

int stack_is_full(Stack *stack) {
    return (stack->top == (MAX_SIZE - 1));
}

void stack_push(Stack *stack, int value) {
    pthread_mutex_lock(&stack->mutex);
    if (stack_is_full(stack)) {
        printf("Stack is full. Cannot push.\n");
        pthread_mutex_unlock(&stack->mutex);
        return;
    }
    stack->data[++stack->top] = value;
    printf("Pushed %d onto the stack\n", value);
    pthread_mutex_unlock(&stack->mutex);
}

int stack_pop(Stack *stack) {
    pthread_mutex_lock(&stack->mutex);
    if (stack_is_empty(stack)) {
        printf("Stack is empty. Cannot pop.\n");
        pthread_mutex_unlock(&stack->mutex);
        return -1;
    }
    int value = stack->data[stack->top--];
    printf("Popped %d from the stack\n", value);
    pthread_mutex_unlock(&stack->mutex);
    return value;
}

void* push_thread(void *arg) {
    Stack *stack = arg;
    for (int i = 0; i < 5; i++) {
        stack_push(stack, i);
        sleep(1);
    }
    return NULL;
}

void* pop_thread(void *arg) {
    Stack *stack = arg;
    for (int i = 0; i < 5; i++) {
        int value = stack_pop(stack);
        if (value != -1) {
            sleep(1);
        }
    }
    return NULL;
}

int main() {
    pthread_t push_tid, pop_tid;
    Stack stack;
    stack_init(&stack);

    pthread_create(&push_tid, NULL, push_thread, &stack);
    pthread_create(&pop_tid, NULL, pop_thread, &stack);

    pthread_join(push_tid, NULL);
    pthread_join(pop_tid, NULL);

    return 0;
}
