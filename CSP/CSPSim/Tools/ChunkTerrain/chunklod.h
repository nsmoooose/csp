// chunklod.h	-- tu@tulrich.com Copyright 2001 by Thatcher Ulrich

// Header declaring data structures for chunk LOD rendering.

#ifndef CHUNKLOD_H
#define CHUNKLOD_H


#include <engine/geometry.h>
#include <engine/view_state.h>
#include <engine/container.h>

class tqt;
struct lod_chunk;
class chunk_tree_loader;


struct render_options {
	bool	show_box;
	bool	show_geometry;
	bool	morph;

	render_options()
		:
		show_box(false),
		show_geometry(true),
		morph(true)
	{
	}
};


class lod_chunk_tree {
// Use this class as the UI to a chunked-LOD object.
// !!! turn this into an interface class and get the data into the .cpp file !!!
public:
	lod_chunk_tree(SDL_RWops* src, const tqt* texture_quadtree);
	~lod_chunk_tree();

	// External interface.
	void	set_parameters(float max_pixel_error, float max_texel_size, float screen_width, float horizontal_FOV_degrees);
	void	update(const vec3& viewpoint);
	int	render(const view_state& v, render_options opt);

	void	get_bounding_box(vec3* box_center, vec3* box_extent);

	// Internal interfaces, used by our contained chunks.
	Uint16	compute_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const;
	int	compute_texture_lod(const vec3& center, const vec3& extent, const vec3& viewpoint) const;

	// Call this to enable/disable loading in a background thread.
	void	set_use_loader_thread(bool use);

//data:
	lod_chunk*	m_chunks;
	int	m_chunks_allocated;
	int	m_tree_depth;	// from chunk data.
	float	m_error_LODmax;	// from chunk data.
	float	m_distance_LODmax;	// computed from chunk data params and set_parameters() inputs --> controls displayed LOD level.
	float	m_texture_distance_LODmax;	// computed from texture quadtree params and set_parameters() --> controls when textures kick in.
	float	m_vertical_scale;	// from chunk data; displayed_height = y_data * m_vertical_scale.
	float	m_base_chunk_dimension;	// x/z size of highest LOD chunks.
	int	m_chunk_count;
	lod_chunk**	m_chunk_table;
	const tqt*	m_texture_quadtree;
	chunk_tree_loader*	m_loader;
};


#endif // CHUNKLOD_H


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:
