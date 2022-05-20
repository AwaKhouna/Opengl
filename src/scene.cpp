#include "scene.hpp"

#include "terrain.hpp"
#include "lac.hpp"

using namespace cgp;



void scene_structure::initialize()
{
	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });

	int N_terrain_samples = 500;
	float terrain_length = 60;
	terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");
	update_terrain(terrain_mesh, terrain, parameters);

	lac_mesh = create_lac_mesh(N_terrain_samples/10, terrain_length);
	lac.initialize(lac_mesh, "lac");
	update_lac(lac_mesh, lac, parameters);
	// lac.shader = opengl_load_shader("shaders/deformation/vert.glsl", "shaders/deformation/frag.glsl");                 

	rectangle_mesh = cgp::mesh_primitive_quadrangle({-terrain_length/4 + 0.1f,-terrain_length/2,0},{-terrain_length/4,-terrain_length/2,10.0f},{-terrain_length/4,terrain_length/2,10.0f},{-terrain_length/4 + 0.1f,terrain_length/2,0});
	for(int i = 0; i < rectangle_mesh.uv.size(); i++){
		rectangle_mesh.uv[i][1] *= 10; 
	}
	rectangle.initialize(rectangle_mesh,"Rectangle");
	// Tree creation
	// GLuint const shader_with_transparency = opengl_load_shader("shaders/transparency/vert.glsl","shaders/transparency/frag.glsl");
	tree_position = generate_positions_on_terrain(100, terrain_length/2);
	trunk.initialize(mesh_load_file_obj("assets/trunk.obj"), "Trunk");
	trunk.texture = opengl_load_texture_image("assets/trunk.png");

	branches.initialize(mesh_load_file_obj("assets/branches.obj"), "Branches");
	branches.shading.color = { 0.65f, 0.61f, 0.54f };

	foliage.initialize(mesh_load_file_obj("assets/foliage.obj"), "Foliage");
	foliage.texture = opengl_load_texture_image("assets/pine.png");
	foliage.shading.phong = { 0.4f, 0.6f, 0, 0.5 }; 
	foliage.shading.color/=2; // changer la couleur du foliage 
	foliage.shader = opengl_load_shader("shaders/transparency/vert.glsl","shaders/transparency/frag.glsl");

	
	terrain.texture = opengl_load_texture_image("assets/texture_grass.jpg",
		GL_REPEAT,
		GL_REPEAT);

	lac.texture = opengl_load_texture_image("assets/eau.jpg",
		GL_REPEAT,
		GL_REPEAT);
	
	rectangle.texture = opengl_load_texture_image("assets/mountain1.jpg",
		GL_REPEAT,
		GL_REPEAT);

}





void scene_structure::display()
{
	// Basic elements of the scene
	for(int i = 0; i < tree_position.size(); i++){
		trunk.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.5f);// pour transformer au cours du temps ajouter timer.t ...
		branches.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.5f);// pour transformer au cours du temps ajouter timer.t ...
		foliage.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.5f);// pour transformer au cours du temps ajouter timer.t ...
		trunk.transform.translation = tree_position[i];
		branches.transform.translation = tree_position[i];
		foliage.transform.translation = tree_position[i];
		draw(trunk, environment);
		draw(branches, environment);
		draw(foliage, environment);
	}
	

	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);

	draw(rectangle, environment);
	draw(terrain, environment);
	if (gui.display_wireframe){
		draw_wireframe(terrain, environment);
		draw_wireframe(lac, environment);
	}
	// timer.update();
	// float t = timer.t;
	// // environment1.update(lac,lac_mesh, t); à voir 
	draw(lac, environment);


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


