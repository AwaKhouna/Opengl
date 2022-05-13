
#include "lac.hpp"
#include <cmath>


// Evaluate 3D position of the lac for any (u,v) \in [0,1]
float evaluate_lac_height(float x, float y)
{
    // if(x < 2500)
    //     float z = 10.0; TO DO
    float z = - 0.5f; // To modify later
    // float ct=0.1;
    // z = z+0.1*cos(6*y-ct/2)+0.07*noise_perlin({x,y,z+ct/10},3,0.5,6);
                    
   
    return z;
}

mesh create_lac_mesh(int N, float lac_length)
{

    mesh lac; // temporary lac storage (CPU only)
    lac.position.resize(N*N);
    lac.uv.resize(N*N);

    // Fill lac geometry
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
            float v = kv/(N-1.0f);

            // Compute the real coordinates (x,y) of the lac 
            float x = (u - 0.5f) * lac_length;
            float y = (v - 0.5f) * 5.0f;

            // Compute the surface height function at the given sampled coordinate
            float z = evaluate_lac_height(x,y);
            // float pi = std::atan(1)*4;
            
            
               
          

            // Store vertex coordinates
            lac.position[kv+N*ku] = {x,y,z};
            
            vec2 uv = {u*20,v};
            lac.uv[kv+N*ku] = uv;
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

            lac.connectivity.push_back(triangle_1);
            lac.connectivity.push_back(triangle_2);
        }
    }

    // need to call this function to fill the other buffer with default values (normal, color, etc)
	lac.fill_empty_field(); 

    return lac;
}

void update_lac(mesh& lac, mesh_drawable& lac_visual, perlin_noise_parameters & parameters)
{
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(lac.position.size());
    parameters.persistency = 0.5f;
	parameters.frequency_gain = 6.0f;
	parameters.octave = 3;
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
			lac.position[idx].z = parameters.lac_height*noise;
            

			// use also the noise as color value
			lac.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*noise*vec3(1,1,1);
		}
	}

	// Update the normal of the mesh structure
	lac.compute_normal();
	
	// Update step: Allows to update a mesh_drawable without creating a new one
	lac_visual.update_position(lac.position);
	lac_visual.update_normal(lac.normal);
	lac_visual.update_color(lac.color);
	
}


	


