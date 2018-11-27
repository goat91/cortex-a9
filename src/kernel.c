#include <stdint.h>
#include <stdio.h>
#include "pl011.h"
#include "pl050.h"
#include "pl111.h"
#include "sp804.h"
#include "interrupt.h"
#include "cursor.h"

UG_WINDOW wnd, dialog;
UG_OBJECT dialog_objs[3];

UG_BUTTON ok_btn;
UG_TEXTBOX label;

static void dialog_callback(UG_MESSAGE *msg)
{
	if (msg->type != MSG_TYPE_OBJECT)
		return;

	UG_OBJECT *obj = *((UG_OBJECT **)msg->src);
	switch (msg->sub_id) {
	case BTN_ID_0:
		UG_WindowHide(&dialog);
		UG_WindowDelete(&dialog);
		UG_Update();
		break;
	default:
		break;
	}
}

static void popup() {
	UG_WindowCreate(&dialog, dialog_objs, 3, dialog_callback);
	UG_WindowResize(&dialog, 250, 150, 650, 350);
	UG_WindowSetTitleTextFont(&dialog, &FONT_8X14);
	UG_WindowSetTitleHeight(&dialog, 30);
	UG_WindowSetTitleText(&dialog, "Information");

	UG_ButtonCreate(&dialog, &ok_btn, BTN_ID_0, 150, 120, 250, 150 );
	UG_ButtonSetFont(&dialog, BTN_ID_0, &FONT_8X14);
	UG_ButtonSetText(&dialog, BTN_ID_0, "Close");

	UG_TextboxCreate(&dialog, &label, TXB_ID_0, 50, 20, 250, 100, 0);
	UG_TextboxSetAlignment(&dialog, TXB_ID_0, ALIGN_CENTER_LEFT);
	UG_TextboxSetForeColor(&dialog, TXB_ID_0, C_BLACK);
	UG_TextboxSetFont(&dialog, TXB_ID_0, &FONT_8X14);
	UG_TextboxSetText(&dialog, TXB_ID_0, "Login Successfully!");

	UG_WindowShow(&dialog);
	UG_Update();
}

static void window_callback(UG_MESSAGE *msg)
{
	if (msg->type != MSG_TYPE_OBJECT)
		return;

	UG_OBJECT *obj = *((UG_OBJECT **)msg->src);
	switch (msg->sub_id) {
	case BTN_ID_0:
		if (obj->event == OBJ_EVENT_CLICKED) {
			printf("login button clicked\n");
			printf("name is '%s'\n", UG_TextboxGetText(&wnd, TXB_ID_1));
			printf("passwd is '%s'\n", UG_TextboxGetText(&wnd, TXB_ID_3));
			popup();
		}
		break;
	case BTN_ID_1:
		if (obj->event == OBJ_EVENT_CLICKED) {
			printf("cancel button clicked\n");
			if (UG_WindowHide(&wnd) == UG_RESULT_FAIL)
				printf("failed to hide window\n");

			UG_Update();
			/*
			if (UG_WindowDelete(&wnd) == UG_RESULT_FAIL)
				printf("failed to delete window\n");
			*/
		}
		break;
/*
	case TXB_ID_1:
		if (obj->event == OBJ_EVENT_FOCUSED_IN)
			printf("input focused in\n");
		if (obj->event == OBJ_EVENT_FOCUSED_OUT)
			printf("input focused out\n");
		break;
		*/
	default:
		break;
	}
}

int main(void){
 	interrupt_init();
	clcd_init();
 	kmi0_init();
	kmi1_init();
 	timer_init();

 	UG_FontSelect(&FONT_16X26);
 	UG_SetBackcolor(C_BLACK);
	UG_SetForecolor(C_YELLOW);
#if 0
	//UG_FillFrame( 100, 200, 200, 300, 0xe6341a);

	/*UG_FillCircle( 300, 200, 50, C_GREEN);
	UG_FillCircle( 500, 200, 50, C_BLUE);
	*/
	UG_DrawBMP(100, 100, &cursor_pixmap);
	UG_Update();
#else
	UG_OBJECT objects[8];
	UG_BUTTON btn, btn2;
	UG_TEXTBOX name_field, name_label, passwd_label, passwd_field;

	UG_WindowCreate(&wnd, objects, 8, window_callback);
	UG_WindowResize(&wnd, 200, 100, 600, 400);
	UG_WindowSetTitleTextFont(&wnd, &FONT_8X14);
	UG_WindowSetTitleTextAlignment(&wnd, ALIGN_CENTER);
	UG_WindowSetTitleText(&wnd, "Login Form");
	UG_WindowSetTitleHeight(&wnd, 30);
	UG_ButtonCreate(&wnd, &btn, BTN_ID_0, 100, 200, 200, 230);
	UG_ButtonSetFont(&wnd, BTN_ID_0, &FONT_8X14);
	UG_ButtonSetText(&wnd, BTN_ID_0, "Login");

	UG_ButtonCreate(&wnd, &btn2, BTN_ID_1, 220, 200, 320, 230);
	UG_ButtonSetFont(&wnd, BTN_ID_1, &FONT_8X14);
	UG_ButtonSetText(&wnd, BTN_ID_1, "Cancel");

	UG_TextboxCreate(&wnd, &name_label, TXB_ID_0, 10, 40, 120, 70, 0);
	UG_TextboxSetAlignment(&wnd, TXB_ID_0, ALIGN_CENTER_LEFT);
	UG_TextboxSetForeColor(&wnd, TXB_ID_0, C_BLACK);
	UG_TextboxSetFont(&wnd, TXB_ID_0, &FONT_8X14);
	UG_TextboxSetText(&wnd, TXB_ID_0, "Username: ");


	UG_TextboxCreate(&wnd, &name_field, TXB_ID_1, 130, 40, 350, 70, 1);
	UG_TextboxSetAlignment(&wnd, TXB_ID_1, ALIGN_CENTER_LEFT);
	UG_TextboxSetBackColor(&wnd, TXB_ID_1, C_WHITE);
	UG_TextboxSetFont(&wnd, TXB_ID_1, &FONT_8X14);
	UG_TextboxSetEditable(&wnd, TXB_ID_1, 1);
	
	UG_TextboxCreate(&wnd, &passwd_label, TXB_ID_2, 10, 100, 120, 130, 0);
	UG_TextboxSetAlignment(&wnd, TXB_ID_2, ALIGN_CENTER_LEFT);
	UG_TextboxSetFont(&wnd, TXB_ID_2, &FONT_8X14);
	UG_TextboxSetText(&wnd, TXB_ID_2, "Password: ");

	UG_TextboxCreate(&wnd, &passwd_field, TXB_ID_3, 130, 100, 350, 130, 1);
	UG_TextboxSetAlignment(&wnd, TXB_ID_3, ALIGN_CENTER_LEFT);
	UG_TextboxSetBackColor(&wnd, TXB_ID_3, C_WHITE);
	UG_TextboxSetFont(&wnd, TXB_ID_3, &FONT_8X14);
	UG_TextboxSetEchoMode(&wnd, TXB_ID_3, TXB_ECHO_PASSWD);
	UG_TextboxSetEditable(&wnd, TXB_ID_3, 1);

	UG_WindowShow(&wnd);
	UG_Update();
	UG_CursorInit(&cursor_pixmap);
#endif
 	asm volatile("SVC 0x05");
 	for(;;);
	return 0;
}
