
#include <stdio.h>
#include <stdlib.h>

#include "support/primitives.h"
#include <brsh/brsh.h>

//#include <drw/drw.h>

#include "brsh_demo_common.h"

#include "demo.h"

extern BrshDemo hello;

#define WIDTH 512
#define HEIGHT 512

//#include "demos/svg.c"

GLFWwindow* window = NULL;
static void switch_demo(int i);

static void reset_current_demo(void)
{
	if (!current_demo)
		return;

	current_demo->deinit();
	current_demo->init();
}

static void calculate_retina_scale(void)
{
	dpi = frame_w / window_w;

	printf("dpi updated? is %f\n", dpi);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	frame_w = height;
	frame_h = width;

	drw_setup(frame_w, frame_w);

	calculate_retina_scale();
}

static void window_pos_callback(GLFWwindow* window, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
	if (current_demo)
		current_demo->mouse_move(x, y);
}

static void window_size_callback(GLFWwindow* window, int width, int height)
{

	window_h = height;
	window_w = width;

	drw_setup(window_w, window_h);
	reset_current_demo();
	calculate_retina_scale();
}

// static void draw_joysticks(void);

static void scroll_callback(GLFWwindow* window, double x, double y)
{
}

static void start_faking_it(void)
{
	printf("Starting to fake it.\n");
	faking_it = true;
}

static void stop_faking_it(double x, double y)
{
	//recorder_end_line(x, y);

	faking_it = false;
	printf("No longer faking it.\n");
}

static void normalize_coordinates(double* x, double* y)
{
	*x *= dpi;
	*y *= dpi;

	//*y *= -1;
	//	wacoms upside down? wooo
	mouse_y = (window_h - mouse_y) + window_h;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	mouse_down = action;
	//(button > 0)
	//	return;

	static int once = 0;
	if (once == 0)
	{
		once = 1;
#ifdef WSH_DEMO_ENABLE_WCM
		wcm_init(window_w, window_h);
#endif
	}

	if (action == 1)
	{
		if (!faking_it)
		{
			if (!wash_demo_has_pressure_sensitivity_of_any_kind)
			{
				start_faking_it();
			};
		}
	}
	else
	{
		if (faking_it)
		{
			stop_faking_it(mouse_x, mouse_y);
			//simulator_reset_sims();
		}
	}

	if (current_demo)
		current_demo->mouse_click(button, action, mods);

	//if
}

static void cursor_enter_callback(GLFWwindow* window, int entered)
{
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	mouse_x = xpos;
	mouse_y = ypos;

	normalize_coordinates(&mouse_x, &mouse_y);

	if (faking_it)
	{
		//WPoint p = simulator_simulate_point(mouse_x, mouse_y);
		//recorder_record_manual_point(p);
	}
}

static void key_callback(GLFWwindow* window, int key, int scan, int action, int mods)
{
	if (action == 0)
	{
		return;
	}

	if (key >= GLFW_KEY_1 && key < GLFW_KEY_9)
	{
		int v = key - GLFW_KEY_1;
		if (v != current_demo_index)
		{
			switch_demo(v);
			return;
		}
	}

	if (current_demo)
	{
		current_demo->key(key, action, mods);
	}
}

static void drop_callback(GLFWwindow* window, int num, const char** paths)
{
	const char* first = paths[0];
	if (!strstr(first, ".wash"))
	{
		printf("Error, file dragged was not a .wash file!\n");
		return;
	}

	//wsh_demo_load_document(first);
	if (!current_demo)
		return;

	if (!current_demo->drop)
		return;

	current_demo->drop(num, paths);
}

void my_tablet_prox(int v)
{
	printf("got tablet prox? %d\n", v);
}

void my_tablet_up(double x, double y, int button, double p, double r, double tx, double ty, double altitude, double azimuth, double idk)
{

	mouse_x = x;
	mouse_y = y;

	normalize_coordinates(&mouse_x, &mouse_y);

	printf("got rich up? %f %f %f %f %f %f\n", mouse_x, mouse_y, p, r, tx, ty);
	//recorder_end_line(mouse_x, mouse_y);
}

static void have_pressure(void)
{

	wash_demo_has_pressure_sensitivity_of_any_kind = true;
	printf("Have pressure! Should probably tell the user that we're disabling the mouse...\n");
}

void my_tablet_down(double x, double y, int button, double p, double r, double tx, double ty, double altitude, double azimuth, double idk)
{
	if (!wash_demo_has_pressure_sensitivity_of_any_kind)
	{
		have_pressure();
	}

	mouse_x = x;
	mouse_y = y;
	normalize_coordinates(&mouse_x, &mouse_y);

	printf("got rich down? %f %f %f %f %f %f\n", x, y, p, r, tx, ty);
	//recorder_record_point(mouse_x, mouse_y, button, p, r, tx, ty, altitude, azimuth, idk);
}

void my_tablet_motion(double x, double y, int button, double p, double r, double tx, double ty, double altitude, double azimuth, double idk)
{
	mouse_x = x;
	mouse_y = y;
	normalize_coordinates(&mouse_x, &mouse_y);

	printf("got rich motion? %f %f %f %f %f %f\n", x, y, p, r, tx, ty);
}

void my_tablet_drag(double x, double y, int button, double p, double r, double tx, double ty, double altitude, double azimuth, double idk)
{
	printf("got rich drag? %f %f %f %f %f %f\n", mouse_x, mouse_y, p, r, tx, ty);

	mouse_x = x;
	mouse_y = y;
	normalize_coordinates(&mouse_x, &mouse_y);

	//recorder_record_point(mouse_x, mouse_y, button, p, r, tx, ty, altitude, azimuth, idk);
}

static void setup_callbacks()
{
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetWindowPosCallback(window, window_pos_callback);

	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwSetScrollCallback(window, scroll_callback);
	glfwSetDropCallback(window, drop_callback);
}

static void update(void)
{
	if (current_demo)
	{
		current_demo->update();
	}
}

static void draw(void)
{
	drw_clear();
	drw_color(0, 0, 0, 1);

	if (!document.src)
	{
		return;
	}

	WSequence* seq = document.src->sequence.src;

	WObject* fr = seq->frames[0];

	if (!fr)
		return;

	if (simple_brush)
	{
	}

	drw_color(0, 0, 0, .25);
	drw_wobject(fr);

	int n = num_brushes;
	for (int i = 0; i < n; i++)
	{
		BBrush* b = brushes[i];
		drw_wline(b->stroke);
	}
}

static void switch_demo(int i)
{
	printf("Switching demo: %d\n", i);

	if (current_demo)
	{
		current_demo->deinit();
	}

	current_demo_index = i;
	if (current_demo_index >= NUM_DEMOS)
	{
		printf("Asked for a demo beyond our range.\n");
		return;
	}
	current_demo = demos[current_demo_index];

	if (!current_demo)
	{
		printf("Error, got a NULL demo at position %d\n", i);
		return;
	}

	current_demo->init();

	printf("Loaded demo: %s\n", current_demo->name);
}

int main(int argc, const char* argv[])
{

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	window_w = WIDTH;
	window_h = HEIGHT;
	frame_w  = WIDTH;
	frame_h  = HEIGHT;
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(window_w, window_h, "Wsh Demos4", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	setup_callbacks();
#ifdef WSH_DEMO_ENABLE_WCM
	wcm_set_tablet_proximity_func(my_tablet_prox);
	wcm_set_tablet_up_func(my_tablet_up);
	wcm_set_tablet_down_func(my_tablet_down);
	wcm_set_tablet_motion_func(my_tablet_motion);
	wcm_set_tablet_drag_func(my_tablet_drag);

	wcm_init(window_w, window_h);
#endif

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	int ww, wh;
	glfwGetWindowSize(window, &ww, &wh);
	int fw, fh;
	glfwGetFramebufferSize(window, &fw, &fh);

	dpi = (double)fw / ww;

	//	recorder_init();

	drw_set_dpiscale(dpi);
	printf("dpi: %f\n", dpi);

	//document.src = NULL;
	//document.src = wsh_serial_document_unserialize_file("data/wash/squares-anim.wash");

	drw_setup(window_w, window_h);

	brsh_demo_load_wash("/Users/vs/art/barn/src/art/wash/dump/drawing-ios-2018_06_13-20_09_42.wash");

	brsh_plugins_set_search("/Users/vs/Library/Application Support/wash/plugins");
	brsh_plugins_init();

	simple_brush = brsh_plugins_query("space.ruminant.b_simple");
	if (simple_brush)
	{
		printf("Huzzah!\n");
	}

	update_brushes();
	drw_color_clear(1, 1, 1, 1);
	drw_color(0, 0, 0, 1);

	switch_demo(0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		update();
		/* Render here */
		draw();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	if (current_demo)
	{
		current_demo->deinit();
	}

	//recorder_deinit();
	//wash_demo_text_deinit();

#ifdef WSH_DEMO_ENABLE_WCM
	wcm_deinit();
#endif

	return 0;
}
