#include "scene.hpp"

#include "terrain.hpp"
#include "tree.hpp"
#include "lac.hpp"

using namespace cgp;



void scene_structure::initialize()
{
	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });

	int N_terrain_samples = 100;
	float terrain_length = 60;
	terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");
	update_terrain(terrain_mesh, terrain, parameters);

	lac_mesh = create_lac_mesh(N_terrain_samples/10, terrain_length);
	lac.initialize(lac_mesh, "lac");
	update_lac(lac_mesh, lac, parameters);

	mesh const tree_mesh = create_tree();
	tree.initialize(tree_mesh, "tree");
	terrain.shading.phong.specular = 0.0f; 
	tree_position = generate_positions_on_terrain( 30, 30);
	
	// Load an image from a file, and send the result to the GPU
	// return its identifier texture_image_id
	GLuint const texture_image_id = opengl_load_texture_image("assets/texture_grass.jpg",
		GL_REPEAT,
		GL_REPEAT);
	// Associate the texture_image_id to the image texture used when displaying visual
	terrain.texture = texture_image_id;

	GLuint const texture_eau = opengl_load_texture_image("assets/eau.jpg",
		GL_REPEAT,
		GL_REPEAT);
	// Associate the texture_image_id to the image texture used when displaying visual
	lac.texture = texture_eau;

}





void scene_structure::display()
{
	// Basic elements of the scene
	for(int i = 0; i < tree_position.size(); i++){
		tree.transform.translation = tree_position[i];
		draw(tree, environment);
	}
	
	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);





	draw(lac, environment);
	draw(terrain, environment);
	if (gui.display_wireframe){
		draw_wireframe(tree, environment);
		draw_wireframe(terrain, environment);
		draw_wireframe(lac, environment);
	}

}





void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	bool update = false;
	update |= ImGui::SliderFloat("Persistance", &parameters.persistency, 0.1f, 0.6f);
	update |= ImGui::SliderFloat("Frequency gain", &parameters.frequency_gain, 1.0f, 3.5f);
	update |= ImGui::SliderInt("Octave", &parameters.octave, 1, 10);
	update |= ImGui::SliderFloat("Height", &parameters.terrain_height, 0.1f, 10.0f);

	if (update)// if any slider has been changed - then update the terrain
		update_terrain(terrain_mesh, terrain, parameters);
}


