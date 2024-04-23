#include <stdio.h>
#include <windows.h>

#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];
    int top;
    CRITICAL_SECTION mutex;
} Stack;

void stack_init(Stack *stack) {
    stack->top = -1;
    InitializeCriticalSection(&stack->mutex);
}

int stack_is_empty(Stack *stack) {
    return (stack->top == -1);
}

int stack_is_full(Stack *stack) {
    return (stack->top == (MAX_SIZE - 1));
}

void stack_push(Stack *stack, int value) {
    EnterCriticalSection(&stack->mutex);
    if (stack_is_full(stack)) {
        printf("Stack is full. Cannot push.\n");
        LeaveCriticalSection(&stack->mutex);
        return;
    }
    stack->data[++stack->top] = value;
    printf("Pushed %d onto the stack\n", value);
    LeaveCriticalSection(&stack->mutex);
}

int stack_pop(Stack *stack) {
    EnterCriticalSection(&stack->mutex);
    if (stack_is_empty(stack)) {
        printf("Stack is empty. Cannot pop.\n");
        LeaveCriticalSection(&stack->mutex);
        return -1;
    }
    int value = stack->data[stack->top--];
    printf("Popped %d from the stack\n", value);
    LeaveCriticalSection(&stack->mutex);
    return value;
}

DWORD WINAPI push_thread(LPVOID arg) {
    Stack *stack = (Stack *)arg;
    for (int i = 0; i < 5; i++) {
        stack_push(stack, i);
        Sleep(1000);
    }
    return 0;
}

DWORD WINAPI pop_thread(LPVOID arg) {
    Stack *stack = (Stack *)arg;
    for (int i = 0; i < 5; i++) {
        int value = stack_pop(stack);
        if (value != -1) {
            Sleep(1000);
        }
    }
    return 0;
}

int main() {
    HANDLE push_thread_handle, pop_thread_handle;
    Stack stack;
    stack_init(&stack);

    push_thread_handle = CreateThread(NULL, 0, push_thread, &stack, 0, NULL);
    pop_thread_handle = CreateThread(NULL, 0, pop_thread, &stack, 0, NULL);

    WaitForSingleObject(push_thread_handle, INFINITE);
    WaitForSingleObject(pop_thread_handle, INFINITE);

    CloseHandle(push_thread_handle);
    CloseHandle(pop_thread_handle);

    return 0;
}
