#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct
{
	unsigned int dwMsgId;
	unsigned int dwParam1;
	unsigned int dwParam2;
}MSG_INFO;

#define LOOP_SNAPSHOT_INTERVAL	50	//milli seconds.

extern "C" int send_hid_msg(void* buf, int len, int display_id);
extern void create_thread(unsigned long* thread_id, void* attr, void *(*start_routine) (void *), void* arg);
extern void thread_sleep(unsigned int milli_seconds);
extern int get_std_input(char *buffer, int size);
extern int snap_shot(int display);

static const char* s_tip_help =
"-------------------- Help ------------------------\n"
"ss: 	Snapshot all screens in snapshot_N.bmp.\n"
"sslp: 	Snapshot in loop\n"
"aaN: 	Screen N flip left		N: 0-8\n"
"ddN: 	Screen N flip right		N: 0-8\n"
"exit: 	Exit the App.\n"
"--------------------------------------------------\n";

static bool s_is_loop_snapshot = false;
static void* loop_snapshot(void* param)
{
	int display_cnt = *(int*)(param);

	printf("Loop snapshot start...\n");
	printf("Snapshot per 50 millisecond\n>");
	fflush(stdout);
	while (s_is_loop_snapshot)
	{
		for (int i = 0; i < display_cnt; i++)
		{
			snap_shot(i);
		}
		thread_sleep(LOOP_SNAPSHOT_INTERVAL);
	}
	printf("Loop snapshot exit\n>");
	fflush(stdout);
	return NULL;
}

static void press_down(int x, int y, int display_id)
{
	MSG_INFO msg;
	msg.dwMsgId = 0x4700;
	msg.dwParam1 = x | (y << 16);
	send_hid_msg(&msg, sizeof(msg), display_id);
}

static void press_release(int x, int y, int display_id)
{
	MSG_INFO msg;
	msg.dwMsgId = 0x4600;
	msg.dwParam1 = x | (y << 16);
	send_hid_msg(&msg, sizeof(msg), display_id);
}

static void left_flip(int display_id)
{
	press_down(0, 100, display_id);
	press_release(200, 100, display_id);
	printf("Screen %d left flip done.", display_id);
}

static void right_flip(int display_id)
{
	press_down(200, 100, display_id);
	press_release(0, 100, display_id);
	printf("Screen %d right flip done.", display_id);
}

static void* stdin_thread(void* param)
{
	int display_cnt = *(int*)(param);
	char buffer[64];

	while (true)
	{
		memset(buffer, 0, sizeof(buffer));
		printf("\n>");
		fflush(stdout);

		if (0 >= get_std_input(buffer, sizeof(buffer)))
		{
			continue;
		}

		if (strcmp(buffer, "exit") == 0)
		{
			exit(-1);
		}
		else if (strcmp(buffer, "ss") == 0)
		{
			for (int i = 0; i < display_cnt; i++)
			{
				snap_shot(i);
			}
			printf("snapshot done.");
		}
		else if (strcmp(buffer, "sslp") == 0)
		{
			s_is_loop_snapshot = (s_is_loop_snapshot) ? false : true;
			if (s_is_loop_snapshot)
			{
				unsigned long pid;
				create_thread(&pid, NULL, loop_snapshot, param);
			}
		}
		else if (strstr(buffer, "aa") == buffer)
		{
			int display_id = atoi(&buffer[2]);
			if (display_id < 0 || display_id > 8)
			{
				printf("Bad screen ID, should be in 0~8");
				continue;
			}
			left_flip(display_id);
		}
		else if (strstr(buffer, "dd") == buffer)
		{
			int display_id = atoi(&buffer[2]);
			if (display_id < 0 || display_id > 8)
			{
				printf("Bad screen ID, should be in 0~8");
				continue;
			}
			right_flip(display_id);
		}
		else if (strstr(buffer, "press") == buffer)
		{
			int x, y;
			sscanf(buffer, "%*[a-z|(]%d,%d", &x, &y);
			press_down(x, y, 0);
			printf(buffer);
		}
		else if (strstr(buffer, "release") == buffer)
		{
			int x, y;
			sscanf(buffer, "%*[a-z|(]%d,%d", &x, &y);
			press_release(x, y, 0);
			printf(buffer);
		}
		else
		{
			printf("%s -- unknow command!", buffer);
			printf(s_tip_help);
		}
	}
}

void init_std_io(int display_cnt)
{
	unsigned long pid;
	static int s_display_cnt = display_cnt;
	create_thread(&pid, NULL, stdin_thread, &s_display_cnt);
}