
#include "st_matrix_operations.h"
#include "enough.h"
#include "seduce.h"
#include "persuade.h"

//#include "co_func_repository.h"
//#include "co_storage.h"

/* Disable hijacking of main() by SDL. */
#if defined _WIN32 && defined BETRAY_SDL_SYSTEM_WRAPPER
#undef main
#endif

void connect_type_in_func(void *user, char *text)
{
	e_vc_connect(text, "unknown", "none", NULL);
}

extern void qs_input_handler(BInputState *input, void *user);
extern void qs_intro_draw(void);
extern void g_set_ship_camera(BInputState *input, float delta_time);


extern void sds_2_test(void);
void draw_table_debuging(void);
void p_status_print(void);

extern void callback_send_g_vertex_set_real_xyz(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 vertex_id, egreal x, egreal y, egreal z);


extern void		qs_camera_init();
extern float	*qs_get_cam_matrix(void);
extern float	*qs_get_cam_pos(void);
extern void		qs_set_camera();
extern void		qs_compute_camera(BInputState *input, float delta_time);

void qs_draw_handler(BInputState *input, void *user)
{

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(30);
		p_task_compute(8);
		return;
	}
/*	{
		static float time = 0;
		ENode *node;
		time += 0.01;
		node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
		if(node != NULL)
			callback_send_g_vertex_set_real_xyz(NULL, e_ns_get_node_id(node), 0, 0, sin(time), sin(time * 0.87), sin(time * 0.27));
	}
*/	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.2, 0.2, 0.2, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPushMatrix();

	//	glTranslatef(0, 0, -1);
	//	p_status_print();
	//	draw_table_debuging();
		
		qs_set_camera();
	//	p_lod_set_wiev_pos(double view_cam);
	//	p_draw_scene();

	//	glScalef(12, 12, 12);
	/*	{
			static float rot_x = 0, rot_y = 0, delta_x = 0, delta_y = 0;
			if(input->mouse_button[0])
			{
				delta_x = delta_x * 0.9 + (input->pointer_x - input->click_pointer_x) * 0.1;
				delta_y = delta_y * 0.9 + (input->pointer_y - input->click_pointer_y) * 0.1;
			}
			rot_x += delta_x * 360 * betray_get_delta_time();
			rot_y += delta_y * 360 * betray_get_delta_time();

			glRotatef(rot_x, 0, 1, 0);
			glRotatef(rot_y, 1, 0, 0);
		}*/
		p_draw_scene();
//		sds_2_test();
		glPopMatrix();
	}else
		qs_compute_camera(input, betray_get_delta_time());
}


void qs_intro_handler(BInputState *input, void *user)
{
	static boolean active = FALSE;
	static char connect_type_in[48];
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		betray_set_action_func(qs_draw_handler, NULL);
		return;
	}

	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(30000);
		return;
	}

	if(input->mode == BAM_DRAW)
	{
		glClearColor(1, 1, 1, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glPopMatrix();
		glPopMatrix();
		glPushMatrix();

		glTranslatef(0, 0, -1);
		glDisable(GL_DEPTH_TEST);
	}
	qs_intro_draw();
	if((input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE) || (input->mouse_button[1] == FALSE && input->last_mouse_button[1] == TRUE) || (input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE))
		active = TRUE;

	if(active)
	{
		if(e_vc_check_connected_slot(0))
		{
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECTING...") * -0.5, -0.3, 0.02, 2, "CONNECTING...", 0, 0, 0);    
		}
		else
		{
			static boolean text_init = FALSE;
			static char address[64] = {255}, name[64] = {255}, pass[64] = {255};
			if(!text_init)
			{
				char *t;
				uint i;
				t = sui_get_setting_text("address", "localhost");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					address[i] = t[i];
				address[i] = 0;
				t = sui_get_setting_text("name", "name");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					name[i] = t[i];
				name[i] = 0;
				t = sui_get_setting_text("pass", "pass");
				for(i = 0; i < 64 && t[i] != 0 ; i++)
					pass[i] = t[i];
				pass[i] = 0;
				text_init = TRUE;
			}
			if(input->mode == BAM_DRAW)
				sui_draw_text(sui_compute_text_length(0.02, 2, "CONNECT") * -0.5, 0.35, 0.02, 2, "CONNECT", 0, 0, 0);

			if(sw_text_button(input, -0.2, 0.25 - 2 * 0.02, 0, 0.02, 0.3, "Localhost", 0, 0, 0))
				e_vc_connect("localhost", "somepass", "somepass", NULL);
			if(sw_text_button(input, 0.2, 0.25 - 2 * 0.02, 1, 0.02, 0.3, "Exit", 0, 0, 0))
			{
				sui_save_settings("qs_config.cfg");
				exit(0);
			}
			
			sui_type_in(input, -0.2, -0.2, 0.4, SUI_T_SIZE, address, 64, NULL, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.2, 0.2, -0.2, 0, 0, 0);
			sui_type_in(input, -0.2, -0.25, 0.4, SUI_T_SIZE, name, 64, NULL, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.25, 0.2, -0.25, 0, 0, 0);
			sui_type_in(input, -0.2, -0.3, 0.4, SUI_T_SIZE, pass, 64, NULL, NULL, 0, 0, 0);
			sui_draw_2d_line_gl(-0.2, -0.3, 0.2, -0.3, 0, 0, 0);


			if(sw_text_button(input, -0.2, -0.325 - 2 * 0.02, 0, 0.02, 0.3, "OK", 0, 0, 0))
			{
				sui_set_setting_text("address", address);
				sui_set_setting_text("name", name);
				sui_set_setting_text("pass", pass);
				sui_save_settings("qs_config.cfg");
				e_vc_connect(address, name, pass, NULL);
			}
			if(sw_text_button(input, 0.2, -0.325 - 2 * 0.02, 1, 0.02, 0.3, "Cancel", 0, 0, 0))
			{
				address[0] = 0;
				name[0] = 0;
				pass[0] = 0;
			}
		}
	}

	if(input->mode == BAM_DRAW)
	{
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();
		glPushMatrix();
		glPushMatrix();
	}
}


extern void *se_symbol_editor_func(BInputState *input, void *user_pointer);
extern void *se_font_editor_func(BInputState *input, void *user_pointer);
extern void qs_intro_init();


int main(int argc, char **argv)
{
	betray_init(argc, argv, 1280, 1024, FALSE, "Quel Solaar");
	sui_load_settings("qs_config.cfg");
	sui_init();
	qs_camera_init();
	glClearColor(1, 1, 1, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	enough_init();					/* initializing the Enough Lib, setting the max subdivision level to 3*/
#ifdef PERSUADE_H
	persuade_init(3, betray_get_gl_proc_address());
	p_geo_set_sds_level(2);
#endif
	qs_intro_init();
//	betray_set_mouse_warp(TRUE);

	betray_set_action_func(qs_intro_handler, NULL);
//	betray_set_action_func(sui_symbol_editor_func, NULL);
	betray_launch_main_loop();
	return 0;
}
