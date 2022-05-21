
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

mesh create_terrain_mesh(int N, float terrain_length, perlin_noise_parameters & parameters)
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
            float y0 = sin(2*pi*u);//*noise_perlin({u,u}, 1, 0.9f, 5.0f) ;
            
            if(-3.0f + 3.0f*y0 <= y && y <= 3.0f + 3.0f*y0 ){
                z = -4.0f*cos((y - 3.0*y0)*pi/6.0f);
                if(-2.0f + 2.0f*y0 <= y && y <= 2.0f + 2.0f*y0 ){
                    z = - 6.0f*cos(2*pi/6.0f);
                }
            }

            // Store vertex coordinates
            float const noise = noise_perlin({5*u, 5*v}, parameters.octave, parameters.persistency, 2*parameters.frequency_gain);
            z += noise/4; 
            terrain.position[kv+N*ku] = {x,y,z};
            //terrain.color[kv+N*ku] = 0.2f*vec3(0,0.5f,0)+0.8f*noise*vec3(1,1,1);

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

void update_terrain(mesh& terrain, mesh_drawable& terrain_visual) // ,perlin_noise_parameters const& parameters)
{
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(terrain.position.size()/4);

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {

			int const idx = ku*N+kv;

			// // Compute the Perlin noise
            if(terrain.position[idx].y > 0.0f && terrain.position[idx].y < 6.0f){
                if(ku>2*N/3)
                    terrain.position[idx].z -= 10.0f;
            }
			terrain.position[idx].z += 10.0f; //( (float) 3.0f*(N/3 - (ku - 2*N/3))/N )*terrain.position[idx-N-1].z;
            // }

			// // use also the noise as color value
			// terrain.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*noise*vec3(1,1,1);
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
        float x =  rand_interval(-terrain_length,terrain_length);
        float y =  rand_interval(-terrain_length, -5.0f);
        vec2 xy = {x,y};
        while (! separated(xy ,pos) && x + terrain_length/4 + 5.0f < 0.1f && x + terrain_length/4 + 5.0f > - 0.1f)
        {
            x =  rand_interval(-terrain_length,terrain_length);
            y =  rand_interval(-terrain_length, -5.0f);
            xy = {x,y};
        }
        float z = evaluate_terrain_height(x,y);
        if(x < -terrain_length/4 - 8.0f)
            z = 10.0f;
        pos.push_back(vec3({x,y,z}));
    }
    for(int i = 0; i < N/2; i++){
        float x =  rand_interval(-terrain_length,terrain_length);
        float y =  rand_interval( 2.0f ,terrain_length);
        vec2 xy = {x,y};
        while (! separated(xy ,pos) && x + terrain_length/4 + 5.0f < 0.1f && x + terrain_length/4 + 5.0f > - 0.1f)
        {
            x =  rand_interval(-terrain_length,terrain_length);
            y =  rand_interval(-terrain_length, -5.0f);
            xy = {x,y};
        }
        float z = evaluate_terrain_height(x,y);
        if(x < -terrain_length/4 - 8.0f)
            z = 10.0f;
        pos.push_back(vec3({x,y,z}));
        
    }
    return pos;
}
double norm(vec2 a){
    double sum = 0;
    for(int i = 0; i < 2; i++){
        sum += a[i]*a[i];
    }
    return std::sqrt(sum);
}
bool separated(vec2 a, std::vector<cgp::vec3> pos){
    for(int i = 0; i < pos.size(); i++){
            if((pos[i][0]-a[0])*(pos[i][0]-a[0]) < 0.01 && (pos[i][1] - a[1])*(pos[i][1] - a[1]) < 0.01)
                return false;
        }
        return true;
}

