#include "../rw.h"

/*
    读写锁有三种状态：1. 读模式下加锁 2. 写模式下加锁 3. 不加锁状态
    * 写加锁状态时，被解锁之前，其他对这个锁加锁的操作都会被阻塞。
    * 读加锁状态时，其他线程读加锁获得访问权，但是写加锁被阻塞，直到释放读加锁
    * 读加锁状态时，再来一个写加锁会阻塞随后的读加锁，防止读模式锁长期占用，写模式得不到满足
    读写锁非常适合数据 读远大于写 的场景
*/

struct job 
{
    struct job* j_next;
    struct job* j_prev;
    pthread_t j_id;
};

struct queue
{
    struct job* q_head;
    struct job* q_tail;
    pthread_rwlock_t q_lock;
};

int queue_init(struct queue* qp)
{
    int err;

    qp->q_head = NULL;
    qp->q_tail = NULL;
    err = pthread_rwlock_init(&qp->q_lock, NULL);
    if (err != 0) return err;
    return 0;
}

void job_init(struct queue* qp, struct job* jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = qp->q_head;
    jp->j_prev = NULL;
    if (qp->q_head != NULL)
        qp->q_head->j_prev = jp;
    else
        qp->q_tail = jp;
    qp->q_head = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

void job_append(struct queue* qp, struct job* jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    jp->j_next = NULL;
    jp->j_prev = qp->q_tail;
    if (qp->q_tail != NULL)
        qp->q_tail->j_next = jp;
    else
        qp->q_head = jp;
    qp->q_tail = jp;
    pthread_rwlock_unlock(&qp->q_lock);
}

void job_remove(struct queue* qp, struct job* jp)
{
    pthread_rwlock_wrlock(&qp->q_lock);
    if (jp == qp->q_head)
    {
        qp->q_head = jp->j_next;
        if (qp->q_tail == jp)
            qp->q_tail == NULL;
        else
            jp->j_next->j_prev = jp->j_next;
    }
    else if (jp == qp->q_tail) 
    {
        qp->q_tail == jp->j_prev;
        jp->j_prev->j_next = jp->j_next;
    }
    else
    {
        jp->j_prev->j_next = jp->j_next;
        jp->j_next->j_prev = jp->j_prev;
    }
    pthread_rwlock_unlock(&qp->q_lock);
}

struct job* job_find(struct queue* qp, pthread_t id)
{
    struct job* jp;

    if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
        return NULL;
    for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
        if (pthread_equal(jp->j_id, id))
            break;
    pthread_rwlock_unlock(&qp->q_lock);
    return jp;
}