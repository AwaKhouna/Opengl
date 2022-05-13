
#include "terrain.hpp"
#include <cmath>

using namespace cgp;

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_height(float x, float y)
{
    vec2 p_i[6] = { {15,15} };
    float h_i[6] = { 8.0f};
    float sigma_i[6] = { 6.0f};
    float z = 0;
    for(int i = 0; i < 1; i++){
        float d = norm(vec2(x, y) - p_i[i]) / sigma_i[i];
        z += h_i[i] * std::exp(-d * d);
    }
    return z;
}

mesh create_terrain_mesh(int N, float terrain_length)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);

    // Fill terrain geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the terrain 
            float x = (u - 0.5f) * terrain_length;
            float y = (v - 0.5f) * terrain_length;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_terrain_height(x,y);
            float pi = std::atan(1)*4;
            for i(o,)
            if(-3.0f <= y && y <= 3.0f){
                z = - 2.0f*cos(y*pi/6.0f);
            }

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(int ku=0; ku<N-1; ++ku)
    {
        for(int kv=0; kv<N-1; ++kv)
        {
            unsigned int idx = kv + N*ku; // current vertex offset

            uint3 triangle_1 = {idx, idx+1+N, idx+1};
            uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	terrain.fill_empty_field(); 

    return terrain;
}

void update_terrain(mesh& terrain, mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
{
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(terrain.position.size()/4);

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			
			// Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

			int const idx = ku*N+kv;

			// Compute the Perlin noise
			float const noise = noise_perlin({u, v}, parameters.octave, parameters.persistency, parameters.frequency_gain);

			// use the noise as height value
			terrain.position[idx].z = parameters.terrain_height*noise;

			// use also the noise as color value
			terrain.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*noise*vec3(1,1,1);
		}
	}

	// Update the normal of the mesh structure
	terrain.compute_normal();
	
	// Update step: Allows to update a mesh_drawable without creating a new one
	terrain_visual.update_position(terrain.position);
	terrain_visual.update_normal(terrain.normal);
	terrain_visual.update_color(terrain.color);
	
}

std::vector<cgp::vec3> generate_positions_on_terrain(int N, float terrain_length){
    std::vector<cgp::vec3> pos;
    for(int i = 0; i < N/2; i++){
        float x =  rand_interval(-terrain_length/4,terrain_length);
        float y =  rand_interval(-terrain_length, -3.0f);
        float z = evaluate_terrain_height(x,y);
        pos.push_back(vec3({x,y,z}));
    }
    for(int i = 0; i < N/2; i++){
        float x =  rand_interval(-terrain_length/4,terrain_length);
        float y =  rand_interval( 3.0f ,terrain_length);
        float z = evaluate_terrain_height(x,y);
        pos.push_back(vec3({x,y,z}));
    }
    return pos;
}

