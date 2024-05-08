// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

template <typename T>
struct TPriorityQueueItem
{
    T Item;
    int Priority;

    TPriorityQueueItem() { }
    TPriorityQueueItem(T item, int priority) { Item = item; Priority = priority; }

    bool operator<(const TPriorityQueueItem other) const { return Priority < other.Priority; }
};

/**
 * A queue with priorities, where the item with the lowest priority value is the next to be popped.
 */
template <typename T>
class GRAYZONE_API TPriorityQueue
{
public:

	TPriorityQueue() { m_array.Heapify(); }

    T Pop()
    {
        if (this->IsEmpty())
        {
            UE_LOG(LogTemp, Fatal, TEXT("You are trying to pop an item from an empty TPriorityQueue."));
            return nullptr;
        }

        TPriorityQueueItem<T> queueItem;
        this->m_array.HeapPop(queueItem);

        return queueItem.Item;
    }

    void Enqueue(T item, int priority) { this->m_array.HeapPush(TPriorityQueueItem<T>(item, priority)); }
    bool IsEmpty() const               { return this->m_array.IsEmpty(); }

private:

    TArray<TPriorityQueueItem<T>> m_array;
};
