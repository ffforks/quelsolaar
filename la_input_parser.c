#include "la_includes.h"

#include "la_geometry_undo.h"
#include "la_projection.h"
#include "la_tool.h"
#include "la_pop_up.h"
#include "la_draw_overlay.h"
#include "la_particle_fx.h"
 
#define VERTEX_SNAPP_DISTANCE 0.0003
#define MAX_POPUP_TIME 100

typedef enum { 
	PIM_IDLE,
	PIM_DRAW,
	PIM_DRAW_SELECT,
	PIM_RESHAPE,
	PIM_SPLIT,
	PIM_DRAG_MANIPULATOR,
	PIM_DRAG_ONE_VERTEX,
	PIM_SHOW_EMPTY_MENY,
	PIM_SHOW_VERTEX_MENY,
	PIM_SHOW_EDGE_MENY,
	PIM_SHOW_MANIPULATOR_MENY,
	PIM_SHOW_POLY_MENY,
	PIM_CHANGE_VIEW
}ParceInputMode;

struct{
	ParceInputMode	mode;
	uint			start_vertex;
	uint			closest_vertex;
	double			depth[3];
	uint			click_time;
}ParceInputData;

boolean draw_view_cage(void)
{
	return ParceInputData.mode != PIM_CHANGE_VIEW;
}

void la_parse_input(BInputState *input)
{
	double output[3]; 
	static double snap[3], distance, selected_distance;
	static int closest, select_closest, edge[2], polygon;

	if(input->mode == BAM_EVENT)
	{
		distance = 1E100;
		selected_distance = 1E100;
		p_find_closest_vertex(&closest, &select_closest, &distance, &selected_distance, input->pointer_x, input->pointer_y);
		if(selected_distance > VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE) && ParceInputData.mode != PIM_SHOW_EDGE_MENY)
			p_find_closest_edge(edge, input->pointer_x, input->pointer_y);
	}else
		la_pfx_select_vertex();
	switch(ParceInputData.mode)
	{
		case PIM_IDLE :
			if(input->mode == BAM_EVENT)
			{
				if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				{
					ParceInputData.click_time = 0;
					if(la_t_tm_grabb(input))
					{
						ParceInputData.mode = PIM_DRAG_MANIPULATOR;
						return;
					}
					if(selected_distance < VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE))
					{
						udg_get_vertex_pos(ParceInputData.depth, select_closest);
						ParceInputData.start_vertex = select_closest;
					}
					else
					{
						if(edge[0] != -1 && edge[1] != -1)
						{
							if(udg_get_select(edge[0]) > 0.01 && udg_get_select(edge[1]) > 0.01)
							{
								ParceInputData.mode = PIM_RESHAPE;								
								la_t_reshape_hull_start(input, edge);
							}else
							{
								ParceInputData.mode = PIM_SPLIT;
								la_t_edge_spliter_start(input, edge);
							}
							return;
						}
						p_get_projection_vertex(ParceInputData.depth, ParceInputData.depth, input->pointer_x, input->pointer_y);
						ParceInputData.start_vertex = -1;
					}
					if(ParceInputData.start_vertex != -1 && udg_get_select(ParceInputData.start_vertex) > 0.5)
					{
                    	ParceInputData.mode = PIM_DRAG_ONE_VERTEX;
                        grab_one_vertex(input, ParceInputData.start_vertex, ParceInputData.depth);
					}
                    else
						ParceInputData.mode = PIM_DRAW;
					la_t_new_draw_line();
				}else if(input->mouse_button[2] == TRUE && input->last_mouse_button[2] == FALSE)
				{
					ParceInputData.start_vertex = select_closest;
					if(la_t_tm_test_center(input))
						ParceInputData.mode = PIM_SHOW_MANIPULATOR_MENY;
					else if(selected_distance < VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE))
					{
						ParceInputData.mode = PIM_SHOW_VERTEX_MENY;
						la_pu_vertex(input, ParceInputData.start_vertex);
					}
					else if(edge[0] != -1 && edge[1] != -1)
						ParceInputData.mode = PIM_SHOW_EDGE_MENY;
					else if((polygon = la_t_poly_test(input)) != -1)
						ParceInputData.mode = PIM_SHOW_POLY_MENY;
					else
						ParceInputData.mode = PIM_SHOW_EMPTY_MENY;
				}
				if(input->mouse_button[1] == TRUE && input->last_mouse_button[1] == FALSE)
				{
					if(la_t_tm_test_center(input))
						la_t_tm_view_center();
					else
					{
						ParceInputData.mode = PIM_CHANGE_VIEW;
						p_view_change_start(input);
					}
				}
			}else
			{
				if(la_t_tm_test_center(input) == FALSE)
				{
					if((ParceInputData.mode == PIM_IDLE && selected_distance < VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE)) || (ParceInputData.mode == PIM_DRAW || ParceInputData.mode == PIM_DRAG_MANIPULATOR || ParceInputData.mode == PIM_DRAG_ONE_VERTEX) && input->mouse_button[1] == TRUE)
					{
						double closest[3];
						udg_get_vertex_pos(closest, select_closest);
						la_do_active_vertex(closest, 0.01 < udg_get_select(select_closest));
					}else
						la_do_draw_closest_edge(edge, input->pointer_x, input->pointer_y, FALSE);
	
				}
					la_t_tm_draw(input, ParceInputData.mode == PIM_DRAG_MANIPULATOR);
			}
			if(ParceInputData.mode != PIM_IDLE)
				la_parse_input(input);
		break;
		case PIM_DRAW :
			udg_get_vertex_pos(snap, select_closest);
			glDisable(GL_DEPTH_TEST);
			if(selected_distance > VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE) && ParceInputData.start_vertex == -1 && input->mode == BAM_DRAW)
				la_t_draw_line_draw_delete_overlay();
			glEnable(GL_DEPTH_TEST);
			p_get_projection_vertex_with_axis(output, ParceInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap);
			if(ParceInputData.start_vertex == -1)
				if(la_t_draw_select_meny_test())
					ParceInputData.mode = PIM_DRAW_SELECT;
			if(input->mode == BAM_EVENT)
			{			
				if((input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE) || (input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE))
				{
					double pos[3];
					uint vertex[2];
					if(selected_distance < VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE) && input->mouse_button[1] != TRUE)
					{
						vertex[1] = select_closest;
						if(ParceInputData.start_vertex == select_closest)
						{
							udg_set_select(select_closest, 1);
							break;
						}else if(ParceInputData.start_vertex != -1)
						{
							vertex[0] = ParceInputData.start_vertex;
							if(la_t_edge_connector(vertex) == TRUE)
								udg_create_edge(ParceInputData.start_vertex, vertex[1]);
							ParceInputData.start_vertex = vertex[1];
							udg_get_vertex_pos(ParceInputData.depth, ParceInputData.start_vertex);
							break;
						}
					}else
					{
						if(ParceInputData.start_vertex == -1 && input->mouse_button[1] != TRUE && input->last_mouse_button[2] != TRUE)
						{
							if(la_t_draw_line_test_delete() || la_t_draw_line_test_select(SM_SELECT))
								break;
							if(0.01 > (input->pointer_x - input->click_pointer_x) * (input->pointer_x - input->click_pointer_x) + (input->pointer_y - input->click_pointer_y) * (input->pointer_y - input->click_pointer_y))
							{
								polygon = la_t_poly_test(input);
								if(polygon == -1)
								{
									la_pfx_create_dust_selected_vertexes(ParceInputData.depth);
									udg_clear_select(0);
								}								
								else
									la_t_poly_select(polygon);
								break;
							}
						}
						p_get_projection_vertex_with_axis(pos, ParceInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap);
						vertex[1] = udg_find_empty_slot_vertex();
						udg_vertex_set(vertex[1], NULL, pos[0], pos[1], pos[2]);
						ParceInputData.depth[0] = pos[0];
						ParceInputData.depth[1] = pos[1];
						ParceInputData.depth[2] = pos[2];
					}
					if(ParceInputData.start_vertex == -1)
					{
						p_get_projection_vertex_with_axis(pos, ParceInputData.depth, input->click_pointer_x, input->click_pointer_y, FALSE, snap);
						ParceInputData.start_vertex = udg_find_empty_slot_vertex();
						udg_vertex_set(ParceInputData.start_vertex, NULL, pos[0], pos[1], pos[2]);				
					}
					udg_create_edge(ParceInputData.start_vertex, vertex[1]);
					ParceInputData.start_vertex = vertex[1];
					if(input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE)
						undo_event_done();
				}
			}else
			{
				la_do_draw(ParceInputData.depth, output, input->mouse_button[1], snap);
				if(ParceInputData.start_vertex == -1)
					la_t_draw_line_add(input->pointer_x, input->pointer_y, TRUE);
				if(selected_distance < VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE) || input->mouse_button[1])
				{
					double closest[3];
					udg_get_vertex_pos(closest, select_closest);
					la_do_active_vertex(closest, FALSE);
				}
			}
		break;
		case PIM_RESHAPE :
			if(input->mode == BAM_EVENT)
				la_t_reshape_hull(input);
			else
				la_t_reshape_hull_draw();
		break;
		case PIM_SPLIT :
			if(input->mode == BAM_EVENT)
				la_t_edge_spliter(input);
		break;
		case PIM_DRAG_MANIPULATOR :
			udg_get_vertex_pos(snap, select_closest);
			if(input->mode == BAM_EVENT)
				la_t_tm_manipulate(input, snap);
			else
				la_t_tm_draw(input, ParceInputData.mode == PIM_DRAG_MANIPULATOR);
		break;
		case PIM_DRAG_ONE_VERTEX :
			{
				double vertex[3];
				static uint colapse = 0;
				udg_get_vertex_pos(snap, select_closest);

				if(input->mode == BAM_EVENT)
				{
                    p_get_projection_vertex_with_axis(vertex, ParceInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap);
                    grab_one_vertex(input, ParceInputData.start_vertex, vertex);
//                  udg_vertex_move(ParceInputData.start_vertex, vertex[0], vertex[1], vertex[2]);
                    if(input->mouse_button[0] == FALSE)
						ParceInputData.mode = PIM_IDLE;
                    
/*					if(input->last_mouse_button[0] != TRUE)
						colapse = ParceInputData.start_vertex;
					if(input->mouse_button[2] != TRUE)
						p_get_projection_vertex_with_axis(vertex, ParceInputData.depth, input->pointer_x, input->pointer_y, input->mouse_button[1], snap);
					else
					{
						if(closest != ParceInputData.start_vertex)
							colapse = select_closest;
						vertex[0] = snap[0];
						vertex[1] = snap[1];
						vertex[2] = snap[2];
					}                    
					if(input->mouse_button[0] == FALSE)
					{
						if(input->mouse_button[2] != TRUE)
							udg_vertex_set(ParceInputData.start_vertex, ParceInputData.depth, vertex[0], vertex[1], vertex[2]);
						else
							la_t_colapse_two_vertexes(ParceInputData.start_vertex, select_closest);
						ParceInputData.mode = PIM_IDLE;
					}else
						udg_vertex_move(ParceInputData.start_vertex, vertex[0], vertex[1], vertex[2]);*/
				}else
				{
					la_do_xyz_lines(ParceInputData.depth, input->mouse_button[1]);
					if(select_closest != ParceInputData.start_vertex && selected_distance < VERTEX_SNAPP_DISTANCE - (0.1 * VERTEX_SNAPP_DISTANCE) && input->mouse_button[1] == TRUE)
						la_do_active_vertex(snap, FALSE);
				}
			}
		break;
		case PIM_DRAW_SELECT :
			{
				uint i;
				i = la_pu_select(input);
				if(input->mode == BAM_EVENT)
				{
					if(i != -1)
					la_t_draw_line_test_select(i);
				}else
					la_t_draw_line_add(input->pointer_x, input->pointer_y, FALSE);
			}
		break;
		case PIM_SHOW_EMPTY_MENY :
			if(la_pu_empty(input))
			{
				ParceInputData.depth[0] = 0;
				ParceInputData.depth[1] = 0;
				ParceInputData.depth[2] = 0;
			}
			if((input->mouse_button[2] == FALSE && input->last_mouse_button[2] == TRUE) || (input->mouse_button[0] == FALSE && input->last_mouse_button[0] == TRUE))
				ParceInputData.mode = PIM_IDLE;
		break;
		case PIM_SHOW_VERTEX_MENY :
			la_pu_vertex(input, ParceInputData.start_vertex);
		break;
		case PIM_SHOW_EDGE_MENY :
			la_pu_edge(input, edge);
		break;
		case PIM_SHOW_MANIPULATOR_MENY :
			la_pu_manipulator(input);
		break;
		case PIM_SHOW_POLY_MENY :
			la_pu_polygon(input, polygon);
		break;
		case PIM_CHANGE_VIEW :
			if(input->mode != BAM_EVENT)
				la_t_tm_draw(input, FALSE);
			else
				p_view_change(input);
		break;
	}
	if(input->mode != BAM_EVENT)
/*	{
		la_t_tm_draw(input, ParceInputData.mode == PIM_DRAG_MANIPULATOR);
		la_pu_type(input);
	}*/
	if(input->mouse_button[0] == FALSE && input->last_mouse_button[0] == FALSE && input->mouse_button[1] == FALSE && input->last_mouse_button[1] == FALSE && input->mouse_button[2] == FALSE && input->last_mouse_button[2] == FALSE)
	{
		undo_event_done();
		ParceInputData.mode = PIM_IDLE;
		ParceInputData.click_time = 0;
	}
//	ParceInputData.mode = PIM_CHANGE_VIEW;
}

extern void la_pu_browser(BInputState *input, uint node);

void la_edit_func(BInputState *input, void *user)
{
	if(input->mode == BAM_MAIN)
	{
		verse_callback_update(0);
		return;
	}
	if(input->mode == BAM_DRAW)
	{
		glClearColor(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		p_view_set();
		la_do_owerlay();
	}
	if(udg_update_geometry())
	{
		la_parse_input(input);
	}else
	{
		ENode *node;
		if(input->mode == BAM_DRAW)
		{
			glPopMatrix();
			glPushMatrix();
			glTranslatef(0, 0, -1);
			sui_draw_text(-0.5 * sui_compute_text_length(SUI_T_SIZE * 2, 3, "NO GEOMETRY"), 0, SUI_T_SIZE * 2, 3, "NO GEOMETRY", 1, 1, 1);
			sui_draw_text(-0.5 * sui_compute_text_length(SUI_T_SIZE, SUI_T_SPACE, "Click to create a geomtry node"), -0.1, SUI_T_SIZE, SUI_T_SPACE, "Click to create a geomtry node", 1, 1, 1);
		}else
		{
			if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
				udg_create_new_modeling_node();
			node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
			if(node != NULL)
				udg_set_modeling_node(e_ns_get_node_id(node));
		}
	}
	if(input->mode == BAM_DRAW)
	{
		glPopMatrix();
		la_pfx_draw(FALSE);
	}
}