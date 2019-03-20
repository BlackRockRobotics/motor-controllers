#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#define FD_UNDEFINED -3
#define DISPLAY_STRING 1

#define FALSE 0
#define TRUE 1

typedef void(*ReadHandler)(const char const*, const int);
//typedef void(*ReadHandler)();
static int fd = FD_UNDEFINED;
static int wlen;
static int READ_LOOP_QUIT = FALSE;
static pthread_t read_thread;
static pthread_mutex_t rw_lock;

static int set_interface_attribs(int fd, int speed)
{
  struct termios tty;

  if (tcgetattr(fd, &tty) < 0) {
    printf("Error from tcgetattr: %s\n", strerror(errno));
    return -1;
  }

  cfsetospeed(&tty, (speed_t)speed);
  cfsetispeed(&tty, (speed_t)speed);

  tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;         /* 8-bit characters */
  tty.c_cflag &= ~PARENB;     /* no parity bit */
  tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
  tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

  /* setup for non-canonical mode */
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tty.c_oflag &= ~OPOST;

  /* fetch bytes as they become available */
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 1;

  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    printf("Error from tcsetattr: %s\n", strerror(errno));
    return -1;
  }
  return 0;
}

static void set_mincount(int fd, int mcount)
{
  struct termios tty;

  if (tcgetattr(fd, &tty) < 0) {
    printf("Error tcgetattr: %s\n", strerror(errno));
    return;
  }

  tty.c_cc[VMIN] = mcount ? 1 : 0;
  tty.c_cc[VTIME] = 5;        /* half second timer */

  if (tcsetattr(fd, TCSANOW, &tty) < 0)
    printf("Error tcsetattr: %s\n", strerror(errno));
}

static void *pthread_readwriteloop(void *func) {
  /* simple noncanonical input */
  ReadHandler handler = (ReadHandler)func;
  do {
    unsigned char buf[80];
    int rdlen;

    pthread_mutex_lock(&rw_lock);
    rdlen = read(fd, buf, sizeof(buf) - 1);
    if (rdlen > 0) {
      buf[rdlen] = 0;
      (*handler)(buf, rdlen);
    } else if (rdlen < 0) {
      //printf("Error from read: %d: %s\n", rdlen, strerror(errno));
    } else {  /* rdlen == 0 */
      printf("Timeout from read\n");
    }
    pthread_mutex_unlock(&rw_lock);
    sleep(1);

    /* repeat read to get full message */
  } while (READ_LOOP_QUIT == FALSE);
  return NULL;
}

int init(const char portname[], ReadHandler func)
{
  //char *portname = "/dev/ttyS4";
  if(fd != FD_UNDEFINED)
    return 0;

  fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC | O_NONBLOCK);
  if (fd < 0) {
    printf("Error opening %s: %s\n", portname, strerror(errno));
    return -1;
  }

  /*baudrate 115200, 8 bits, no parity, 1 stop bit */
  set_interface_attribs(fd, B9600);
  //set_mincount(fd, 0);                /* set to pure timed read */

  /* simple output */
  // wlen = write(fd, "Hello!\n", 7);
  //if (wlen != 7) {
  //    printf("Error from write: %d, %d\n", wlen, errno);
  //}
  tcdrain(fd);    /* delay for output */

  if (pthread_mutex_init(&rw_lock, NULL) != 0)
    {
      printf("\n mutex init has failed\n");
      return 1;
    }
  printf("Pthread create\n");
  if(pthread_create(&read_thread, NULL, pthread_readwriteloop, func)) {
    fprintf(stderr, "PThread create didn't work\n");
    return -1;
  }
}

int close_mcb() {
  if(fd < 0)
    return 0;
  else {
    READ_LOOP_QUIT = TRUE;
    pthread_join(read_thread, NULL);
    pthread_mutex_destroy(&rw_lock);
    return 0;
  }
  return 0;
}


int writeToSerial(const char * const toWrite) {
  if(fd == FD_UNDEFINED) {
    return -1;
  }

  int writelen = strlen(toWrite);
  pthread_mutex_lock(&rw_lock);
  //printf("Writing str: %s\n", toWrite);
  int wlen = write(fd, toWrite, writelen);

  if (wlen != writelen) {
    fprintf(stderr, "Error from write: %d, %d\n", wlen, errno);
  }
  tcdrain(fd);    /* delay for output */
  pthread_mutex_unlock(&rw_lock);
  return 0;
}

void printRead(const char const* readString, int rdlen) {
  //void printRead() {
#ifdef DISPLAY_STRING
  printf("Read %d: \"%s\"\n", rdlen, readString);
#else /* display hex */
  unsigned char   *p;
  printf("Read %d:", rdlen);
  for (p = buf; rdlen-- > 0; p++)
    printf(" 0x%x", *p);
  printf("\n");
#endif

}

int main(char *buff[]) {
  init("/dev/ttyS4", &printRead);
  sleep(5);
  writeToSerial("40\n");
  sleep(5);
  writeToSerial("20\n");
  sleep(5);
  writeToSerial("0\n");
  sleep(5);
  close_mcb();
  printf("Done closing down\n");
  return 0;
}
