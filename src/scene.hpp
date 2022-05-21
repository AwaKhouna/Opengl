#pragma once

#include "cgp/cgp.hpp"
#include "terrain.hpp"

// The element of the GUI that are not already stored in other structures
struct gui_parameters {
	bool display_frame      = false;
	bool display_wireframe  = false;
};



// The structure of the custom scene
struct scene_structure {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::mesh_drawable global_frame;          // The standard global frame
	cgp::scene_environment_basic_camera_spherical_coords environment; // Standard environment controler
	cgp::inputs_interaction_parameters inputs; // Storage for inputs status (mouse, keyboard, window dimension)

	cgp::timer_basic timer;
	gui_parameters gui;                       // Standard GUI element storage
	

	cgp::hierarchy_mesh_drawable hierarchy;
	cgp::mesh rectangle_mesh1;
	cgp::mesh rectangle_mesh2;
	cgp::mesh terrain_mesh;
	cgp::mesh lac_mesh;
	cgp::mesh_drawable rectangle1;
	cgp::mesh_drawable rectangle2;
	cgp::mesh_drawable terrain;
	cgp::mesh_drawable lac;
	cgp::mesh_drawable trunk;
	cgp::mesh_drawable branches;
	cgp::mesh_drawable foliage;
	std::vector<cgp::vec3> tree_position;
	perlin_noise_parameters parameters;

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop


};





