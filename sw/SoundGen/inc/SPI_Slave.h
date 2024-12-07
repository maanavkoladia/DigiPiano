#ifndef SPI_SLAVE_H
#define SPI_SLAVE_H

void SPI_SlaveInit(void);
extern struct note {
	int number;
	int velocity;
	int press; // if press=1, the note is being pressed, if 0 it is in release
	           // phase- update volume accordingly
} note;

typedef enum {
	KEY_IDLE,
	KEY_PLAYING,
	KEY_PRESSED,
	KEY_RELEASED,
	KEY_RELEASING
} KeyStatus;

// maximum number of notes the queue can hold at a time
#define MAX_QUEUE_SIZE 8

// Queue structure using a fixed-size array
struct Queue {
	struct note data[MAX_QUEUE_SIZE];
	int front;
	int rear;
	int size;
};

extern struct Queue notesQueue; // our queue of all playing notes

// Function to initialize the queue
void initQueue(struct Queue *q);
// Function to check if the queue is full
int isFull(struct Queue *q);
// Function to check if the queue is empty
int isEmpty(struct Queue *q);
// Function to push a note into the queue
int push(struct Queue *q, struct note note);
// Function to pop a note from the queue
struct note pop(struct Queue *q);
// Function to get the current size of the queue
int getCurrentSize(struct Queue *q);

int putInQueue(int, int);

#endif /* SPI_SLAVE_H */
