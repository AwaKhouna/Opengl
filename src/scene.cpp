#include "scene.hpp"

#include "terrain.hpp"
#include "lac.hpp"
#include "interpolation.hpp"

using namespace cgp;

void scene_structure::update_camera()
{
	inputs_keyboard_parameters const& keyboard = inputs.keyboard;
	camera_head& camera = environment.camera;

	float const dt = time.update();
	vec3 const forward_displacement = gui.speed * 0.1f * dt * camera.front();
	camera.position_camera += forward_displacement;

	float pitch = 0.7f; 
	float roll  = 0.9f;
	
	if (keyboard.up)
		camera.manipulator_rotate_roll_pitch_yaw(0, pitch * dt, 0); 
	if (keyboard.down)
		camera.manipulator_rotate_roll_pitch_yaw(0,  -pitch * dt, 0); 
	if (keyboard.right)
		camera.manipulator_rotate_roll_pitch_yaw( roll * dt, 0, 0);
	if (keyboard.left)
		camera.manipulator_rotate_roll_pitch_yaw(-roll * dt, 0, 0);
	if (keyboard.z)
		gui.speed += 2;
	if (keyboard.s)
		gui.speed -= 2;
}

void scene_structure::initialize()
{
	skybox.initialize("assets/skybox/"); 

	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	// environment.camera.axis = camera_spherical_coordinates_axis::z;
	// environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });

	environment.camera.position_camera = { 0.0f, -10.0f, 18.0f };
	environment.camera.manipulator_rotate_roll_pitch_yaw(0, 0, 0);

	int N_terrain_samples = 1000;
	float terrain_length = 100;
	terrain_mesh = create_terrain_mesh(N_terrain_samples, terrain_length, parameters);
	terrain.initialize(terrain_mesh, "terrain");
	update_terrain(terrain_mesh, terrain);

	lac_mesh = create_lac_mesh(N_terrain_samples/5, terrain_length);
	lac.initialize(lac_mesh, "lac");
	update_lac(lac_mesh, lac, parameters);
	// lac.shader = opengl_load_shader("shaders/transparency/vert.glsl","shaders/transparency/frag.glsl");              

	rectangle_mesh1 = cgp::mesh_primitive_quadrangle({-terrain_length/4 + 0.1f,-terrain_length/2,0},{-terrain_length/4,-terrain_length/2,10.0f},{-terrain_length/4,0,10.0f},{-terrain_length/4 + 0.1f,0,0});
	rectangle_mesh2 = cgp::mesh_primitive_quadrangle({-terrain_length/4 + 0.1f,6.0f,0},{-terrain_length/4,6.0f,10.0f},{-terrain_length/4,terrain_length/2,10.0f},{-terrain_length/4 + 0.1f,terrain_length/2,0});
	for(int i = 0; i < rectangle_mesh1.uv.size(); i++){
		rectangle_mesh1.uv[i][1] *= 10; 
	}
	rectangle1.initialize(rectangle_mesh1,"Rectangle1");
	for(int i = 0; i < rectangle_mesh2.uv.size(); i++){
		rectangle_mesh2.uv[i][1] *= 10; 
	}
	rectangle2.initialize(rectangle_mesh2,"Rectangle2");
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
	//foliage.shader = opengl_load_shader("shaders/transparency/vert.glsl","shaders/transparency/frag.glsl");
 

	lac.texture = opengl_load_texture_image("assets/eau.jpg",
		GL_REPEAT,
		GL_REPEAT);
	
	rectangle1.texture = opengl_load_texture_image("assets/mountain1.jpg",
		GL_REPEAT,
		GL_REPEAT);
	rectangle2.texture = opengl_load_texture_image("assets/mountain1.jpg",
		GL_REPEAT,
		GL_REPEAT);



	////  PinGouin

	mesh_drawable base;
	mesh_drawable head;
	mesh_drawable eye1;
	mesh_drawable eye2;
	mesh_drawable nose;
	mesh_drawable aileG;
	mesh_drawable aileD;
	mesh_drawable genouG;
	mesh_drawable genouD;
	mesh_drawable piedG;
	mesh_drawable piedD;


	base.initialize(mesh_primitive_ellipsoid(), "Base"); 
	base.anisotropic_scale = vec3{0.7,1.0,0.5};
	head.initialize(mesh_primitive_sphere(0.15f), "Head");
	eye1.initialize(mesh_primitive_sphere(0.02f), "Eye 1");
	eye2.initialize(mesh_primitive_sphere(0.02f), "Eye 2");
	nose.initialize(mesh_primitive_cone(0.04f,0.09f), "Nose");
	aileG.initialize(mesh_primitive_quadrangle(vec3{0,0,0},vec3{0.8f,0.3f,0},vec3{0.8f,0.7f,0},vec3{0,1,0}), "Aile G");
	aileD.initialize(mesh_primitive_quadrangle(vec3{0,0,0},vec3{0.8f,0.3f,0},vec3{0.8f,0.7f,0},vec3{0,1,0}), "Aile D");
	genouG.initialize(mesh_primitive_cylinder(0.06f, vec3{0.1f,0,0},vec3{0.1f,-0.2f,0}), "Genou G");
	genouD.initialize(mesh_primitive_cylinder(0.06f, vec3{-0.1f,0,0},vec3{-0.1f,-0.2f,0}), "Genou D");
	piedG.initialize(mesh_primitive_quadrangle(vec3{0.1f,0,-0.15f},vec3{-0.1f,0,-0.15f},vec3{-0.05f,0,0},vec3{0.05f,0,0}), "Pied G");
	piedD.initialize(mesh_primitive_quadrangle(vec3{0.1f,0,-0.15f},vec3{-0.1f,0,-0.15f},vec3{-0.05f,0,0},vec3{0.05f,0,0}), "Pied D");

	base.transform.scaling = 0.5f;
	aileG.transform.scaling = 0.5f;
	aileD.transform.scaling = 0.5f;
	nose.shading.color = { 0.0f, 1.0f, 0.0f };
	head.shading.color = { 0.0f, 0.0f, 0.0f };
	genouG.shading.color = { 0.0f, 0.0f, 0.0f };
	genouD.shading.color = { 0.0f, 0.0f, 0.0f };
	aileG.shading.color = { 0.0f, 0.0f, 0.0f };
	aileD.shading.color = { 0.0f, 0.0f, 0.0f };
	piedG.shading.color = { 1.0f, 1.0f, 0.0f };
	piedD.shading.color = { 1.0f, 1.0f, 0.0f };
	
	hierarchy.add(base);
	hierarchy.add(head, "Base", { 0, 0.5f,-0.1f }); 
	hierarchy.add(eye1, "Head", { 0.06f,0.13f,0 }); 
	hierarchy.add(eye2, "Head", { -0.06f,0.13f,0 }); 
	hierarchy.add(nose, "Head", { 0,0.13f,-0.04f }); 
	hierarchy.add(aileG, "Base", { 0.2f,-0.25f,0 }); 
	hierarchy.add(aileD, "Base", { -0.2f,-0.25f,0 }); 
	hierarchy.add(genouG, "Base", { 0,-0.4f,0 }); 
	hierarchy.add(genouD, "Base", { 0,-0.4f,0 }); 
	hierarchy.add(piedG, "Genou G", { 0.1,-0.2,0 }); 
	hierarchy.add(piedD, "Genou D", { -0.1,-0.2,0 }); 


	// Interpolation 

	buffer<vec3> key_positions;
	//{ {-20,20,1}, {-20,20,1}, {1,1,2}, {10,-10,2}, {-10,-10,0}, {2,2,2}, {15,10,2}, {15,-10,2}, {25,-10,2}, {5,10,2}, {-20,20,2}, {-20,20,2} };
	
	buffer<float> key_times; 
	buffer<vec3> key_positions1;
	buffer<float> key_times1;
	float x0 = 0; 
	float thet = 1.0f;
	float pas = 3.14f/thet;
	float imax = (terrain_length/2 - x0)/pas;
	key_positions.push_back({15,15, 8});
	key_positions1.push_back({0,0, -1.2f});
	key_times.push_back(0);
	key_times1.push_back(0);
	for(int i = 0; i < imax + 1; i++){
		float x = 15 - 17*i/imax;
		float y = 5*x/16 + 165/16.0f;
		key_positions.push_back({x, y, evaluate_terrain_height(x,y) + 0.9f});
		key_positions1.push_back({x0 + i*pas, - 2.0f*sin(3.14f*i/imax), 0.1*cos(thet*(x0 + i*pas)) - 1.2f});
		key_times1.push_back(i*pas);
		key_times.push_back(i*pas);
	}
	int i_fin = (int) imax;
	key_positions1.push_back({0,0,-1.2f});
	key_positions.push_back({15 - 17*i_fin/imax, 5*(15 - 17*i_fin/imax)/16 + 165/16.0f, evaluate_terrain_height(15 - 17*i_fin/imax, 5*(15 - 17*i_fin/imax)/16 + 165/16.0f) + 0.9f});
	key_times.push_back(i_fin*pas + 20);
	key_times1.push_back(i_fin*pas + 20);

	// int i_fin = (int) imax;
	// key_positions1.push_back({x0 + i_fin*pas, - 2.0f*sin(3.14f*i_fin/imax), cos(thet*(x0 + i_fin*pas))});
	// key_times1.push_back(i_fin*pas);
	

	// Key times (time at which the position must pass in the corresponding position)
	// { 0.0f, 1.0f, 4.0f, 8.0f, 12.0f, 16.5f, 20.75f, 24.5f, 29.0f, 36.0f, 47.0f, 47.0f };

	// Initialize the helping structure to display/interact with these positions
	keyframe.initialize(key_positions, key_times);
	keyframe1.initialize(key_positions1, key_times1);

	int N = key_times1.size();
	time.t_min = key_times1[1];
	time.t_max = key_times1[N - 2];
	time.t = time.t_min;
}





void scene_structure::display()
{
	draw(skybox, environment);
	// Basic elements of the scene
	for(int i = 0; i < tree_position.size(); i++){
		trunk.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.5f);// pour transformer au cours du temps ajouter time.t ...
		branches.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.5f);// pour transformer au cours du temps ajouter time.t ...
		foliage.transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.5f);// pour transformer au cours du temps ajouter time.t ...
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

	draw(rectangle1, environment);
	draw(rectangle2, environment);
	


	draw(terrain, environment);


	if (gui.display_wireframe){
		draw_wireframe(terrain, environment);
		draw_wireframe(lac, environment);
		draw_wireframe(hierarchy, environment);
	}
	time.update();

	float t2 = time.t; 
	update_lac_time(lac_mesh, lac,t2);
	dynamic_update(lac_mesh,lac , t2);
	lac.clear();
	lac.initialize(lac_mesh, "lac");
	lac.texture = opengl_load_texture_image("assets/eau.jpg",
	GL_REPEAT,
	GL_REPEAT);
	draw(lac, environment);
	hierarchy["Base"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.57f);
	hierarchy["Head"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, -1.07f + 0.2*std::cos(time.t));
	hierarchy["Aile G"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.57f);
	hierarchy["Aile G"].transform.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, 0.6*std::cos(4*time.t));
	hierarchy["Aile D"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 1.57f);
	hierarchy["Aile D"].transform.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, 3.14 - 0.6*std::cos(4*time.t));
	hierarchy["Nose"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, -2);
	hierarchy["Genou G"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, -0.2*std::cos(4*time.t));
	hierarchy["Genou D"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 0.2*std::cos(4*time.t));

	// This function must be called before the drawing in order to propagate the deformations through the hierarchy
	// Draw the hierarchy as a single mesh

	

	// Update the current time
	float t = time.t;


	// Compute the interpolated position
	//  This is this function that you need to complete
	vec3 p = interpolation(t, keyframe.key_positions, keyframe.key_times);
	vec3 p1 = interpolation(t, keyframe1.key_positions, keyframe1.key_times);

	// Display the interpolated position (and its trajectory)
	// keyframe.display_current_position(p, environment);
	// TO DO
	int k=0;
    while( keyframe.key_times[k+1]<t )
        ++k;
	float theta = -1.57f;
	float theta1 = std::atan((keyframe1.key_positions[k+1].y-keyframe1.key_positions[k].y)/(keyframe1.key_positions[k+1].z-keyframe1.key_positions[k].z));
	hierarchy["Base"].transform.rotation = rotation_transform::from_matrix({cos(theta), 0, -sin(theta), -sin(theta), 0, -cos(theta), 0, 1, 0}); // TO DO 
	
	hierarchy["Base"].transform.translation = p;
	hierarchy.update_local_to_global_coordinates();
	draw(hierarchy, environment);


	hierarchy["Base"].transform.rotation = rotation_transform::from_matrix({0, cos(theta1 ), sin(theta1 ),  -1, 0, 0, 0, -sin(theta1 ), cos(theta1)}); // TO DO 
	hierarchy["Base"].transform.translation = p1;
	hierarchy.update_local_to_global_coordinates();
	draw(hierarchy, environment);

	// pour arreter le mouvement des pieds
	hierarchy["Genou G"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 0);
	hierarchy["Genou D"].transform.rotation = rotation_transform::from_axis_angle({ 1,0,0 }, 0);

	for(int i = 1; i < 10; i ++){
		float O = -2*3.14*i/10 - 1.58f;
		hierarchy["Base"].transform.rotation = rotation_transform::from_matrix({cos(O), 0, -sin(O), -sin(O), 0 ,-cos(O), 0, 1, 0}); // TO DO 
		hierarchy["Base"].transform.translation = {3*cos(2*3.14*i/10) - 4.0f,10 + 3*sin(2*3.14*i/10),0.7f};
		hierarchy.update_local_to_global_coordinates();
		draw(hierarchy, environment);
	}

	

}





void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
	
	// ImGui::SliderFloat("Time", &time.t, time.t_min, time.t_max);
	// ImGui::SliderFloat("Time scale", &time.scale, 0.0f, 2.0f);

	ImGui::SliderFloat("Speed", &gui.speed, 0.0f, 10.0f);
	// Display the GUI associated to the key position
	//keyframe.display_gui();
	// keyframe1.display_gui();

}
// void scene_structure::mouse_move()
// {
// 	// Handle the picking (displacement of the position using mouse drag)
// 	keyframe.update_picking(inputs, environment);
// 	keyframe1.update_picking(inputs, environment);
// }



