#include "circlebuf_x.h"

void initializeBuffer(CircularBuffer *cBuffer, char *pbuf, int size) {
    cBuffer->buffer = pbuf;
    cBuffer->size = size;
    cBuffer->head = 0;
    cBuffer->tail = 0;
#ifdef USE_OS
    cBuffer->readMutex = xSemaphoreCreateMutex();
    cBuffer->writeMutex = xSemaphoreCreateMutex();
#endif
}

int writeBufferMutex(CircularBuffer *cBuffer, char data) {
#ifdef USE_OS
    if (xSemaphoreTake(cBuffer->writeMutex, portMAX_DELAY) != pdTRUE) {
        // 无法获取写互斥锁，写入失败
        return 0;
    }
#endif

    if ((cBuffer->tail + 1) % cBuffer->size == cBuffer->head) {
        // 缓冲区已满
#ifdef USE_OS
        xSemaphoreGive(cBuffer->writeMutex);
#endif
        return 0;
    } else {
        cBuffer->buffer[cBuffer->tail] = data;
        cBuffer->tail = (cBuffer->tail + 1) % cBuffer->size;

#ifdef USE_OS
        xSemaphoreGive(cBuffer->writeMutex);
#endif

        return 1; // 写入成功
    }
}

int writeBufferFromISR(CircularBuffer *cBuffer, char data) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    int writeSuccess = 0;

    if (xSemaphoreTakeFromISR(cBuffer->writeMutex, &xHigherPriorityTaskWoken) == pdTRUE) {
        if ((cBuffer->tail + 1) % cBuffer->size != cBuffer->head) {
            cBuffer->buffer[cBuffer->tail] = data;
            cBuffer->tail = (cBuffer->tail + 1) % cBuffer->size;
            writeSuccess = 1; // 写入成功
        }

        xSemaphoreGiveFromISR(cBuffer->writeMutex, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return writeSuccess;
}

/**
 * 写入多个数据到循环缓冲区（带互斥锁版本）
 * @param cBuffer 循环缓冲区结构指针
 * @param data 要写入的数据指针
 * @param length 要写入的数据长度
 * @return 成功写入的数据数量，若写入失败则返回 0
 */
int writeBufferMultipleMutex(CircularBuffer *cBuffer, char *data, int length) {
#ifdef USE_OS
    // 尝试获取写互斥锁
    if (xSemaphoreTake(cBuffer->writeMutex, portMAX_DELAY) != pdTRUE) {
        // 无法获取写互斥锁，写入失败
        return 0;
    }
#endif

    int successCount = 0;
    // 获取可写容量
    int capacity = getWritableCapacityNoMutex(cBuffer);

    // 如果可写容量小于待写入数据长度，直接返回写入失败
    if (capacity < length) {
        return 0;
    }

    // 循环写入数据到缓冲区
    while (successCount < length) {
        cBuffer->buffer[cBuffer->tail] = data[successCount];
        cBuffer->tail = (cBuffer->tail + 1) % cBuffer->size;
        successCount++;
    }

#ifdef USE_OS
    // 释放写互斥锁
    xSemaphoreGive(cBuffer->writeMutex);
#endif

    return successCount; // 返回成功写入的数据数量
}


int readBufferMutex(CircularBuffer *cBuffer, char *data) {
#ifdef USE_OS
    if (xSemaphoreTake(cBuffer->readMutex, portMAX_DELAY) != pdTRUE) {
        // 无法获取读互斥锁，读取失败
        return 0;
    }
#endif

    if (cBuffer->head == cBuffer->tail) {
        // 缓冲区为空
#ifdef USE_OS
        xSemaphoreGive(cBuffer->readMutex);
#endif
        return 0;
    } else {
        *data = cBuffer->buffer[cBuffer->head];
        cBuffer->head = (cBuffer->head + 1) % cBuffer->size;

#ifdef USE_OS
        xSemaphoreGive(cBuffer->readMutex);
#endif

        return 1; // 读取成功
    }
}

int readBufferMultipleMutex(CircularBuffer *cBuffer, char *data, int length) {
#ifdef USE_OS
    if (xSemaphoreTake(cBuffer->readMutex, portMAX_DELAY) != pdTRUE) {
        // 无法获取读互斥锁，读取失败
        return 0;
    }
#endif

    int successCount = 0;

    for (int i = 0; i < length; ++i) {
        if (cBuffer->head == cBuffer->tail) {
            // 缓冲区为空
            break; // 读取失败，跳出循环
        } else {
            data[i] = cBuffer->buffer[cBuffer->head];
            cBuffer->head = (cBuffer->head + 1) % cBuffer->size;
            successCount++;
        }
    }

#ifdef USE_OS
    xSemaphoreGive(cBuffer->readMutex);
#endif

    return successCount;
}

// 以下是不使用互斥锁的读写接口

int writeBufferNoMutex(CircularBuffer *cBuffer, char data) {
    if ((cBuffer->tail + 1) % cBuffer->size == cBuffer->head) {
        // 缓冲区已满
        return 0;
    } else {
        cBuffer->buffer[cBuffer->tail] = data;
        cBuffer->tail = (cBuffer->tail + 1) % cBuffer->size;
        return 1; // 写入成功
    }
}

int readBufferNoMutex(CircularBuffer *cBuffer, char *data) {
    if (cBuffer->head == cBuffer->tail) {
        // 缓冲区为空
        return 0;
    } else {
        *data = cBuffer->buffer[cBuffer->head];
        cBuffer->head = (cBuffer->head + 1) % cBuffer->size;
        return 1; // 读取成功
    }
}

int readBufferMultipleNoMutex(CircularBuffer *cBuffer, char *data, int length) {
    int successCount = 0;

    for (int i = 0; i < length; ++i) {
        if (cBuffer->head == cBuffer->tail) {
            // 缓冲区为空
            break; // 读取失败，跳出循环
        } else {
            data[i] = cBuffer->buffer[cBuffer->head];
            cBuffer->head = (cBuffer->head + 1) % cBuffer->size;
            successCount++;
        }
    }

    return successCount;
}

int writeBufferMultipleNoMutex(CircularBuffer *cBuffer, char *data, int length) {
    int successCount = 0;

    for (int i = 0; i < length; ++i) {
        if ((cBuffer->tail + 1) % cBuffer->size == cBuffer->head) {
            // 缓冲区已满
            break; // 写入失败，跳出循环
        } else {
            cBuffer->buffer[cBuffer->tail] = data[i];
            cBuffer->tail = (cBuffer->tail + 1) % cBuffer->size;
            successCount++;
        }
    }

    return successCount;
}

int readBufferMultipleAndClearNoMutex(CircularBuffer *cBuffer, char *data, int length) {
    int successCount = 0;

    for (int i = 0; i < length; ++i) {
        if (cBuffer->head == cBuffer->tail) {
            // 缓冲区为空
            break; // 读取失败，跳出循环
        } else {
            data[i] = cBuffer->buffer[cBuffer->head];
            cBuffer->head = (cBuffer->head + 1) % cBuffer->size;
            successCount++;
        }
    }

    // 清空缓冲区
    cBuffer->head = cBuffer->tail;

    return successCount;
}

int getRemainingCountNoMutex(CircularBuffer *cBuffer) {
    int count;

    if (cBuffer->tail >= cBuffer->head) {
        count = cBuffer->tail - cBuffer->head;
    } else {
        count = cBuffer->size - (cBuffer->head - cBuffer->tail);
    }

    return count;
}

int getWritableCapacityNoMutex(CircularBuffer *cBuffer) {
    int capacity;

    if (cBuffer->head <= cBuffer->tail) {
        capacity = cBuffer->size - (cBuffer->tail - cBuffer->head) - 1;
    } else {
        capacity = cBuffer->head - cBuffer->tail - 1;
    }

    return capacity;
}

void clearBufferNoMutex(CircularBuffer *cBuffer) {
    cBuffer->head = cBuffer->tail;
}
