#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_SUBSCRIBERS 10
#define MAX_TOPICS 5
#define MAX_MSG_LEN 256
#define QUEUE_CAPACITY 20

// ================== 1. 基础数据结构定义 ==================

typedef struct {
	char content[MAX_MSG_LEN];
	char topic_name[32];
} Message;

// 订阅者模块：封装名称与回调函数
typedef struct Subscriber Subscriber;
typedef void (*SubscriberCallback)(Message msg);

struct Subscriber {
	char name[32];
	SubscriberCallback callback;
};

// 主题模块：封装主题名与该主题下的订阅者列表
typedef struct {
	char name[32];
	Subscriber *subscribers[MAX_SUBSCRIBERS]; // 指针 * 紧贴变量名
	int subscriber_count;
} Topic;

// 消息队列模块：封装环形队列及线程同步工具
typedef struct {
	Message buffer[QUEUE_CAPACITY];
	int in;      // 生产者写入位置
	int out;     // 消费者读取位置
	int size;    // 当前队列中的消息数量
	pthread_mutex_t mutex;
	pthread_cond_t not_empty;
	pthread_cond_t not_full;
} MessageQueue;

// ================== 2. 全局对象实例化 ==================

MessageQueue g_queue;
Topic g_topics[MAX_TOPICS];
int g_topic_count = 0;
pthread_mutex_t g_topic_mutex = PTHREAD_MUTEX_INITIALIZER;

// ================== 3. 消息队列模块的方法 ==================

void queue_init(MessageQueue *q)
{
	q->in = 0;
	q->out = 0;
	q->size = 0;
	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->not_empty, NULL);
	pthread_cond_init(&q->not_full, NULL);
}

void queue_push(MessageQueue *q, Message *msg)
{
	pthread_mutex_lock(&q->mutex);
	while (q->size == QUEUE_CAPACITY) {
		printf("⚠️ 队列已满，生产者等待...\n");
		pthread_cond_wait(&q->not_full, &q->mutex);
	}

	// 指针进行解引用
	q->buffer[q->in] = *msg;
	q->in = (q->in + 1) % QUEUE_CAPACITY;
	q->size++;

	printf("📢 生产者发布消息到 [%s]: \"%s\"\n", msg->topic_name, msg->content);

	pthread_cond_signal(&q->not_empty);
	pthread_mutex_unlock(&q->mutex);
}

int queue_pop(MessageQueue *q, Message *msg)
{
	pthread_mutex_lock(&q->mutex);
	while (q->size == 0) {
		pthread_cond_wait(&q->not_empty, &q->mutex);
	}

	*msg = q->buffer[q->out];
	q->out = (q->out + 1) % QUEUE_CAPACITY;
	q->size--;

	pthread_cond_signal(&q->not_full);
	pthread_mutex_unlock(&q->mutex);
	return 1;
}

// ================== 4. 主题管理模块的方法 ==================

Topic *get_or_create_topic(const char *topic_name)
{
	for (int i = 0; i < g_topic_count; i++) {
		if (strcmp(g_topics[i].name, topic_name) == 0) {
		return &g_topics[i];
		}
	}
	if (g_topic_count >= MAX_TOPICS) {
		return NULL;
	}
	Topic *new_topic = &g_topics[g_topic_count++];
	strncpy(new_topic->name, topic_name, sizeof(new_topic->name) - 1);
	new_topic->subscriber_count = 0;
	return new_topic;
}

void subscribe(const char *topic_name, const char *subscriber_name, SubscriberCallback callback)
{
	pthread_mutex_lock(&g_topic_mutex);
	Topic *topic = get_or_create_topic(topic_name);
	if (!topic) {
		pthread_mutex_unlock(&g_topic_mutex);
		printf("❌ 主题创建失败\n");
		return;
	}
	if (topic->subscriber_count >= MAX_SUBSCRIBERS) {
		pthread_mutex_unlock(&g_topic_mutex);
		printf("❌ 主题 [%s] 订阅者已满！\n", topic_name);
		return;
	}
	Subscriber *sub = (Subscriber *)malloc(sizeof(Subscriber));
	strncpy(sub->name, subscriber_name, sizeof(sub->name) - 1);
	sub->callback = callback;
	topic->subscribers[topic->subscriber_count++] = sub;
	pthread_mutex_unlock(&g_topic_mutex);

	printf("🔗 订阅者 [%s] 成功订阅主题 [%s]\n", subscriber_name, topic_name);
}

// ================== 5. 消费者线程逻辑 ==================

void *consumer_thread(void *arg)
{
	(void)arg;
	Message msg;
	while (1) {
		// 从队列模块中取出消息
		queue_pop(&g_queue, &msg);

		// 分发逻辑：根据消息的主题名找到对应的 Topic 模块并触发回调
		pthread_mutex_lock(&g_topic_mutex);
		for (int i = 0; i < g_topic_count; i++) {
		if (strcmp(g_topics[i].name, msg.topic_name) == 0) {
			Topic *topic = &g_topics[i];
			for (int j = 0; j < topic->subscriber_count; j++) {
			topic->subscribers[j]->callback(msg);
			}
			break;
		}
		}
		pthread_mutex_unlock(&g_topic_mutex);
	}
	return NULL;
}

// ================== 6. 发布者接口（供主程序调用） ==================

void publish(const char *topic_name, const char *content)
{
	Message msg;
	strncpy(msg.topic_name, topic_name, sizeof(msg.topic_name) - 1);
	strncpy(msg.content, content, sizeof(msg.content) - 1);
	queue_push(&g_queue, &msg);
}

// ================== 7. 测试用例 ==================

void handle_news(Message msg)
{
	printf("  -> [新闻订阅者] 收到推送: %s\n", msg.content);
}

void handle_sports(Message msg)
{
	printf("  -> [体育订阅者] 收到推送: %s\n", msg.content);
}

int main()
{
	// 初始化队列模块
	queue_init(&g_queue);

	// 启动独立的消费者线程
	pthread_t consumer_tid;
	pthread_create(&consumer_tid, NULL, consumer_thread, NULL);

	// 订阅阶段
	subscribe("news", "Reader_A", handle_news);
	subscribe("sports", "Reader_B", handle_sports);

	// 模拟生产者快速发布多条消息
	publish("news", "C语言多线程模块化教程发布了！");
	publish("sports", "湖人队赢得了总冠军！");
	publish("news", "今天天气真不错！");

	sleep(2);
	printf("✅ 主程序结束\n");
	return 0;
}