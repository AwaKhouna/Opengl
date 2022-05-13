
#include "lac.hpp"
#include <cmath>

using namespace cgp;

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_height(float x, float y)
{
    
    float z = 0;
   
    return z;
}

mesh create_terrain_mesh(int N, float terrain_length)
{

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N*N);
    terrain.uv.resize(N*N);

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
            
            
                
                    z = - 2.0f*cos(2*pi/6.0f);
                    float ct=0.1;
                     z = z+0.02*cos(6*y-ct/2)+0.07*noise_perlin({x,y,z+ct/10},3,0.5,6);
                    
               
          

            // Store vertex coordinates
            terrain.position[kv+N*ku] = {x,y,z};
            
            vec2 uv = {u*20,v*20};
            terrain.uv[kv+N*ku] = uv;
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


	


