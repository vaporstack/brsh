//
//  brsh_brush.h
//  brsh
//
//  Created by vs on 9/20/17.
//  Copyright © 2017 vs. All rights reserved.
//

#ifndef brsh_brush_h
#define brsh_brush_h

#include <brsh/brsh.h>

#ifdef BRSH_DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <stdbool.h>
#include <wsh/src/geo/wsh_line.h>

enum
{
	B_BRUSH_TYPE_NONE,
	B_BRUSH_TYPE_FULL,
	B_BRUSH_TYPE_DISTANCE,
	B_BRUSH_TYPE_RIDIC
};

typedef struct BBrush
{

	int	   brush_type;
	bool	  needs_update;
	double	width;
	WLineHndConst* hnd;
	//	ostensibly, this is wasteful and should be an rline
	//	but that's a library dep I don't need I guess
	struct WLine* stroke;
	double*       data;
	void*	 extra;
	void*	 tess;
	void*	 tristrip;
	void*	 paint;
	unsigned long tristripnum;
	unsigned long num;
	void*	 update_func;
	unsigned long long seed;
} BBrush;

typedef struct BBrushHnd
{
	BBrush* src;
} BBrushHnd;

typedef void (*brush_update_func)(BBrush*);

unsigned long brsh_request_seed(void);
void	  brsh_brush_update_custom(BBrush* brush, brush_update_func func);

struct BBrush* brsh_brush_create(WLineHndConst hnd, double width);
struct BBrush* brsh_brush_create_ptr(const WLineHndConst* hnd_ptr);

struct BBrush* brsh_brush_copy(void* wlinehnddata_old, void* wlinehnd);

void brsh_brush_offset(struct BBrush* brush);

void brsh_brush_destroy(struct BBrush*);
void brsh_brush_update(BBrush* brush, brush_update_func func);
void brsh_brush_draw(BBrush*);

#endif /* brsh_brush_h */
