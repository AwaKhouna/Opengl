
#include "lac.hpp"
#include <cmath>




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
            float pi = std::atan(1)*4;
            float x = (u - 0.5f) * lac_length;
            float y = (v - 0.5f) * 8.0f+ 2.5f * sin(2*pi*u);//*noise_perlin({u,u}, 1, 0.9f, 5.0f); 

            // Compute the surface height function at the given sampled coordinate
            float z = -1.0f; // voir terrain.php pour modifier lac_height
            if( x < -lac_length/4 - 0.5f)
                z += 10.0f;
            // Store vertex coordinates
            lac.position[kv+N*ku] = {x,y,z};
            
            vec2 uv = {u*10,v};
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
    parameters.persistency = rand_interval(0.0f,1.0f);
	parameters.frequency_gain = rand_interval(1.0,10.0);
	parameters.octave = 3;
	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			
			// Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

			int const idx = ku*N+kv;

			// Compute the Perlin noise
			float const noise = noise_perlin({10*u, v}, parameters.octave, parameters.persistency, 2*parameters.frequency_gain);

			// use the noise as height value
			lac.position[idx].z += noise/2;

			// use also the noise as color value
			lac.color[idx] = 0.2f*vec3(0,0.5f,0)+0.8f*noise*vec3(1,1,1);
		}
	}

	// Update the normal of the mesh structure
	lac.compute_normal();

	// Update step: Allows to update a mesh_drawable without creating a new one
	lac_visual.update_position(lac.position);
	lac_visual.update_normal(lac.normal);
	lac_visual.update_color(lac.color);
	
    
}
void update_lac_time(mesh& lac, mesh_drawable& lac_visual,float t)
{

 int const N = std::sqrt(lac.position.size());

 for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			
			// Compute local parametric coordinates (u,v) \in [0,1]
            float u = ku/(N-1.0f);
             float v = kv/(N-1.0f);

			int const idx = ku*N+kv;
            // use the noise as height value
			lac.position[idx].z += 0.02*std::cos(4*t);//noise;
            float H=100.0f;
            float nx= - std::abs(std::sin(0.01*t)*std::cos(0.1*t)); // std::cos(4*t)/(std::sqrt(2));
            float ny=0;
            float nz=std::abs(std::cos(0.1*t));
            u=lac.position[idx].x+(H-lac.position[idx].z)*(nx/nz);
            v=lac.position[idx].y+(H-lac.position[idx].z)*(ny/nz);


            vec2 uv = {u,v};
            lac.uv[kv+N*ku] = uv;

			// use also the noise as color value
			//lac.color[idx] 
		}
        
	}
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


// Update the normal of the mesh structure
	lac.compute_normal();

	// Update step: Allows to update a mesh_drawable without creating a new one
	lac_visual.update_position(lac.position);
	lac_visual.update_normal(lac.normal);
	lac_visual.update_color(lac.color);

     

}


	


