/* la_tool_cloapse.c */

extern void		la_t_collapse_two_vertexes(uint vertex_a, uint vertex_b);
extern void		la_t_collapse_selected_vertexes(void);
extern void		la_t_weld_selected_vertexes(void);

/* la_tool_draw.c */


extern void		la_t_init_draw_line(void);
extern void		la_t_new_draw_line(void);
extern void		la_t_draw_line_add(float x, float y, boolean add);
extern boolean  la_t_draw_select_meny_test(void);

typedef enum{
	SM_SELECT,
	SM_DESELECT,
	SM_SUB,
	SM_ADD
}SelectionMode;

extern boolean la_t_draw_line_test_delete(void);
extern boolean la_t_draw_line_draw_delete_overlay(void);
extern boolean la_t_draw_line_test_select(SelectionMode mode);


/* la_tool_edge_connector.c */

extern void		la_t_init_edge_connector(void);
extern boolean la_t_edge_connector(uint *edge);

/* la_tool_manipulator.c */

extern void		la_t_tm_init(void);
extern void		la_t_tm_place(double x, double y, double z);
extern void		la_t_tm_get_pos(double *pos);
extern void		la_t_tm_get_vector(double *vector);
extern void		la_t_tm_hide(boolean hide);
extern void		la_t_tm_draw(BInputState *input, boolean active);
extern boolean	la_t_tm_grabb(BInputState *input);
extern void		la_t_tm_view_center(void);
extern boolean	la_t_tm_test_center(BInputState *input);
extern void		la_t_tm_manipulate(BInputState *input, double *snap);
extern void 	grab_one_vertex(BInputState *input, uint id, double *pos);

/* la_tool_poly_select.c */

extern void		la_t_face_vector(double *origo, double *vector, uint v0, uint v1, uint v2);

extern void		la_t_extrude(uint vertex_count, void (*func)(double *output, uint vertex_id, void *data), void *data);
extern void		la_t_detach_selected_polygons(void);
extern void		la_t_duplicate_selected_polygons(void);
extern void		la_t_flip_selected_polygons(void);
extern void		la_t_mirror(double *pos, double *vector);
extern void		la_t_flatten(double *origo, double *vector);
extern void		la_t_delete_selection(void);

/* la_tool_reshape.c */

extern void 	la_t_reshape_hull_start(BInputState *input, uint *edge);
extern void 	la_t_reshape_hull(BInputState *input);
extern void 	la_t_reshape_hull_draw(void);
extern void		la_t_select_hull(uint *edge);

/* la_tool_revolve.c */

extern void 	la_t_revolve(uint *start_edge, uint revolve);
extern void		la_t_tube(uint *start_edge, uint revolve);
extern void 	la_t_select_open_edge(void);
extern void 	la_t_crease_selected(uint32 crease_value);

/* la_tool_select.c */
extern uint		la_t_poly_test(BInputState *input);
extern void		la_t_poly_select(uint polygon);
extern void		la_t_smooth_select(void);

/* la_tool_split.c */

extern void 	la_t_edge_spliter_start(BInputState *input, uint *edge);
extern void 	la_t_edge_spliter(BInputState *input);

/* la_tool_deply.c */

extern void 	la_t_deploy(uint poly);
extern void		la_t_polygon_select_fill(uint poly);
/* la_tool_slice.c */

extern void		la_t_slice(double *pos, double *vector, boolean del);


